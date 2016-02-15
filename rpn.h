// Reverse Polish Notation Calculator
//
// Copyright (C) 2007
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Wed Mar 14 13:04:20 CDT 2007

#ifndef RPN_H
#define RPN_H

#include "version.h"
#include <bitset>
#include <cmath>
#include <iostream>
#include <map>
#include <limits>
#include <locale>
#include <string>
#include <vector>

namespace jsp
{

// A Stack is a normal stack, except that you can Pop() and Top() an
// Empty() stack, in which case 0.0 is returned.
//
// It also has a temporary register that you can use to store stuff.
class Stack
{
    public:
    size_t Size () const { return stack.size (); }
    bool Empty () const { return stack.size () == 0; }
    void Push (double x) { stack.push_back (x); }
    double Pop ()
    {
        double x = 0.0;
        if (!Empty ())
        {
            x = stack.back ();
            stack.pop_back ();
        }
        return x;
    }
    double Top () const
    {
        double x = 0.0;
        if (!Empty ())
            x = Get (Size () - 1);
        return x;
    }
    void Clear () { stack.clear (); }
    double Get (size_t i) const
    {
        if (i >= Size ())
            throw std::runtime_error ("Invalid stack index");
        return stack[i];
    }
    double GetReg () const { return reg; }
    void SetReg (double x) { reg = x; }
    private:
    std::vector<double> stack;
    double reg;
};

// A Display contains properties associated with an RPN calculator
// display.
class Display
{
    public:
    Display () :
        hex (false),
        bin (false),
        thousands (false)
    {
    }
    void Prec ()
    {
        std::streamsize p;
        std::cerr << "Enter the precision: ";
        std::cerr.flush ();
        std::cin >> p;
        std::cerr.precision (p);
    };
    void Hex ()
    {
        hex = !hex;
        std::cerr << "hex " << (hex ? "on" : "off") << std::endl;
    };
    void Bin ()
    {
        bin = !bin;
        std::cerr << "binary " << (bin ? "on" : "off") << std::endl;
    };
    void Thousands ()
    {
        thousands = !thousands;
        std::cerr << "thousands separator " << (thousands ? "on" : "off") << std::endl;
    };
    void Show (double x)
    {
        ShowDec (x);
        // Show optional columns
        if (hex)
            ShowHex (x);
        if (bin)
            ShowBinary (x);
        std::cerr << std::endl;
    }
    private:
    void ShowDecThousands (double x)
    {
        // By default, no thousands separator is used.  However, if
        // you specify the locale, the separator for that locale will
        // be used.
        std::stringstream ss;
        ss.imbue (std::locale ("en_US"));
        ss << std::fixed << x;
        std::cerr << ss.str ();
    }
    void ShowDec (double x)
    {
        if (thousands)
            ShowDecThousands (x);
        else
            std::cerr << std::fixed << x;
    }
    void ShowHex (double x)
    {
        std::cerr << "\t" << std::hex << std::uppercase << static_cast<size_t> (x);
    }
    void ShowBinary (double x)
    {
        const size_t N = std::numeric_limits<long>::digits;
        std::cerr << "\t" << std::bitset<N> (static_cast<size_t> (x));
    }
    bool hex;
    bool bin;
    bool thousands;
};

template<typename Ty>
class Op
{
    public:
    virtual void operator() (Ty &t) = 0;
    virtual std::string Help () const = 0;
};

// This helper ensures that you are not relying on function argument
// ordering.
//
// This is very bad:
//
//      s.Push(pow(s.Pop(),s.Pop()); // WRONG! x^y or y^x?
//
class BinaryStackOp : public Op<Stack>
{
    public:
    void operator() (Stack &s)
    {
        double y = s.Pop ();
        double x = s.Pop ();
        s.Push (F (x, y));
    }
    virtual double F (double x, double y) const = 0;
};

class UnaryStackOp : public Op<Stack>
{
    public:
    void operator() (Stack &s)
    {
        double x = s.Pop ();
        s.Push (F (x));
    }
    virtual double F (double x) const = 0;
};

class RPNCalc
{
    public:
    virtual ~RPNCalc () { }
    virtual std::string Version () const
    {
        std::stringstream ss;
        ss << MAJOR_VERSION << "." << MINOR_VERSION;
        return ss.str ();
    }
    void Add (const std::string &name, Op<Stack> *stack_op)
    {
        stack_ops[name] = stack_op;
    }
    void Add (const std::string &name, Op<Display> *display_op)
    {
        display_ops[name] = display_op;
    }
    bool Lookup (const std::string &str) const
    {
        if (stack_ops.find (str) != stack_ops.end ())
            return true;
        else if (display_ops.find (str) != display_ops.end ())
            return true;
        else
            return false;
    }
    void Exec (const std::string &str, Stack &stack, Display &display)
    {
        if (stack_ops.find (str) != stack_ops.end ())
            (*stack_ops[str]) (stack);
        else if (display_ops.find (str) != display_ops.end ())
            (*display_ops[str]) (display);
        else
            throw std::runtime_error ("Invalid operator");
    }
    typedef std::map<std::string,Op<Stack> *> StackOps;
    typedef std::map<std::string,Op<Display> *> DisplayOps;
    StackOps::iterator StackBegin () { return stack_ops.begin (); }
    StackOps::iterator StackEnd () { return stack_ops.end (); }
    DisplayOps::iterator DisplayBegin () { return display_ops.begin (); }
    DisplayOps::iterator DisplayEnd () { return display_ops.end (); }
    private:
    StackOps stack_ops;
    DisplayOps display_ops;
};

const double PI = 2.0 * std::asin (1.0);

class BasicCalc : public RPNCalc
{
    public:
    BasicCalc ()
    {
        // Add the Stack operations
        Add ("-", &minus);
        Add ("+", &plus);
        Add ("*", &times);
        Add ("/", &divides);
        Add ("pi", &pi);
        // Add the Display operations
        Add ("hex", &hex);
        Add ("bin", &bin);
        Add ("prec", &prec);
    }
    private:
    struct PlusOp : public BinaryStackOp {
        double F (double x, double y) const { return x + y; }
        std::string Help () const { return "x+y"; }
    } plus;
    struct MinusOp : public BinaryStackOp {
        double F (double x, double y) const { return x - y; }
        std::string Help () const { return "x-y"; }
    } minus;
    struct TimesOp : public BinaryStackOp {
        double F (double x, double y) const { return x * y; }
        std::string Help () const { return "x*y"; }
    } times;
    struct DividesOp : public BinaryStackOp {
        double F (double x, double y) const { return x / y; }
        std::string Help () const { return "x/y"; }
    } divides;
    struct PiOp : public Op<Stack> {
        void operator() (Stack &s) { s.Push (2.0 * std::asin (1.0)); }
        std::string Help () const { return "pi"; }
    } pi;
    struct HexOp : public Op<Display> {
        void operator() (Display &d) { d.Hex (); }
        std::string Help () const { return "toggle hexadecimal display"; }
    } hex;
    struct BinOp : public Op<Display> {
        void operator() (Display &d) { d.Bin (); }
        std::string Help () const { return "toggle binary display"; }
    } bin;
    struct PrecOp : public Op<Display> {
        void operator() (Display &d) { d.Prec (); }
        std::string Help () const { return "change the display precision"; }
    } prec;
};

class HP35 : public BasicCalc
{
    public:
    HP35 () :
        BasicCalc ()
    {
        Add ("pow", &pow);
        Add ("log", &log10);
        Add ("ln", &log);
        Add ("exp", &exp);
        Add ("clr", &clear);
        Add ("sqrt", &sqrt);
        Add ("sin", &sin);
        Add ("asin", &asin);
        Add ("cos", &cos);
        Add ("acos", &acos);
        Add ("tan", &tan);
        Add ("atan", &atan);
        Add ("inv", &inv);
        Add ("swap", &swap);
        Add ("sto", &store);
        Add ("rcl", &recall);
        Add ("dup", &dup);
        Add ("chs", &chs);
        Add ("clx", &clx);
    }
    private:
    struct PowOp : public BinaryStackOp {
        double F (double x, double y) const { return std::pow (y, x); }
        std::string Help () const { return "x^y"; }
    } pow;
    struct Log10Op : public UnaryStackOp {
        double F (double x) const { return std::log10 (x); }
        std::string Help () const { return "log base 10 of x"; }
    } log10;
    struct LogOp : public UnaryStackOp {
        double F (double x) const { return std::log (x); }
        std::string Help () const { return "natural log of x"; }
    } log;
    struct ExpOp : public UnaryStackOp {
        double F (double x) const { return std::exp (x); }
        std::string Help () const { return "e^x"; }
    } exp;
    struct ClearOp : public Op<Stack> {
        void operator() (Stack &s) { s.Clear (); }
        std::string Help () const { return "clear the stack"; }
    } clear;
    struct SqrtOp : public UnaryStackOp {
        double F (double x) const { return std::sqrt (x); }
        std::string Help () const { return "square root of x"; }
    } sqrt;
    struct SinOp : public UnaryStackOp {
        double F (double x) const { return std::sin (x * PI / 180.0); }
        std::string Help () const { return "sine of x"; }
    } sin;
    struct ArcSinOp : public UnaryStackOp {
        double F (double x) const { return std::asin (x) * 180.0 / PI; }
        std::string Help () const { return "arcsine of x"; }
    } asin;
    struct CosOp : public UnaryStackOp {
        double F (double x) const { return std::cos (x * PI / 180.0); }
        std::string Help () const { return "cosine of x"; }
    } cos;
    struct ArcCosOp : public UnaryStackOp {
        double F (double x) const { return std::acos (x) * 180.0 / PI; }
        std::string Help () const { return "arccosine of x"; }
    } acos;
    struct TanOp : public UnaryStackOp {
        double F (double x) const { return std::tan (x * PI / 180.0); }
        std::string Help () const { return "tangent of x"; }
    } tan;
    struct ArcTanOp : public UnaryStackOp {
        double F (double x) const { return std::atan (x) * 180.0 / PI; }
        std::string Help () const { return "arctangent of x"; }
    } atan;
    struct InvOp : public UnaryStackOp {
        double F (double x) const { return 1.0 / x; }
        std::string Help () const { return "1/x"; }
    } inv;
    struct SwapOp : public Op<Stack> {
        void operator() (Stack &s)
        {
            double y = s.Pop ();
            double x = s.Pop ();
            s.Push (y);
            s.Push (x);
        }
        std::string Help () const { return "swap x and y"; }
    } swap;
    struct StoreOp : public Op<Stack> {
        void operator() (Stack &s)
        {
            s.SetReg (s.Top ());
        }
        std::string Help () const { return "store x (see rcl)"; }
    } store;
    struct RecallOp : public Op<Stack> {
        void operator() (Stack &s)
        {
            s.Push (s.GetReg ());
        }
        std::string Help () const { return "recall x (see sto)"; }
    } recall;
    struct DupOp : public Op<Stack> {
        void operator() (Stack &s) { s.Push (s.Top ()); }
        std::string Help () const { return "duplicate x"; }
    } dup;
    struct ChsOp : public UnaryStackOp {
        double F (double x) const { return -x; }
        std::string Help () const { return "change sign of x"; }
    } chs;
    struct ClxOp : public Op<Stack> {
        void operator() (Stack &s) { s.Pop (); }
        std::string Help () const { return "clear x"; }
    } clx;
};

class SuperCalc : public HP35
{
    public:
    SuperCalc () :
        HP35 ()
    {
        Add ("lg", &lg);
        Add ("noop", &noop);
        Add ("sum", &sum);
        Add ("deg", &deg);
        Add ("rad", &rad);
        Add (",", &thousands);
    }
    private:
    struct LgOp : public Op<Stack> {
        void operator() (Stack &s)
        {
            const double LOG2 = std::log10 (2.0);
            s.Push (std::log10 (s.Pop ()) / LOG2);
        }
        std::string Help () const { return "log base 2 of x"; }
    } lg;
    struct NoOp : public Op<Stack> {
        void operator() (Stack &) { }
        std::string Help () const { return "do nothing"; }
    } noop;
    struct SumOp : public Op<Stack> {
        void operator() (Stack &s)
        {
            double sum = 0.0;
            const size_t N = s.Size ();
            for (size_t i = 0; i < N; ++i)
                sum += s.Pop ();
            s.Push (sum);
        }
        std::string Help () const { return "sum all numbers on the stack"; }
    } sum;
    struct DegOp : public UnaryStackOp {
        double F (double x) const { return x * 180.0 / PI; }
        std::string Help () const { return "change x to degrees from radians"; }
    } deg;
    struct RadOp : public UnaryStackOp {
        double F (double x) const { return x * PI / 180; }
        std::string Help () const { return "change x to radians from degrees"; }
    } rad;
    struct ThousandsOp : public Op<Display> {
        void operator() (Display &d) { d.Thousands (); }
        std::string Help () const { return "toggle dislay of thousands separator"; }
    } thousands;
};

} // namespace jsp

#endif // RPN_H
