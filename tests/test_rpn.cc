// Reverse Polish Notation Calculator
//
// Copyright (C) 2007
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Wed Mar 14 13:05:32 CDT 2007

#include "verify.h"
#include "rpn.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace jsp;

void test0 ()
{
    Stack s;
    VERIFY (s.Size () == 0);
    VERIFY (s.Empty ());
    VERIFY (s.Top () == 0.0);
    VERIFY (s.Pop () == 0.0);
    s.Push (1.0);
    VERIFY (s.Top () == 1.0);
    VERIFY (s.Get (0) == 1.0);
    VERIFY (s.Pop () == 1.0);
    VERIFY (s.Empty ());
}

template<typename Ty>
bool AboutEqual (Ty a, Ty b)
{
    const double PREC = 1000000.0;
    return round (a * PREC) == round (b * PREC);
}

void test1 ()
{
    Stack s;
    Display d;
    BasicCalc c;

    s.Push (1.0);
    s.Push (2.0);
    c.Exec ("+", s, d);
    VERIFY (AboutEqual(s.Top (), 3.0));
    s.Push (1.0);
    s.Push (3.0);
    c.Exec ("-", s, d);
    VERIFY (AboutEqual(s.Top (), -2.0));
    s.Push (2.0);
    s.Push (3.0);
    c.Exec ("*", s, d);
    VERIFY (s.Top () == 6.0);
    s.Push (4.0);
    s.Push (7.0);
    c.Exec ("/", s, d);
    VERIFY (s.Top () == 4.0 / 7.0);
}

void test2 ()
{
    Stack s;
    Display d;
    HP35 c;

    s.Push (3.0);
    s.Push (4.0);
    c.Exec ("pow", s, d);
    VERIFY (AboutEqual(s.Top (), 64.0));
    s.Push (1000.0);
    c.Exec ("log", s, d);
    VERIFY (AboutEqual(s.Top (), 3.0));
    const double e = 2.718281828;
    s.Push (e * e * e * e);
    c.Exec ("ln", s, d);
    VERIFY (AboutEqual(s.Top (), 4.0));
    s.Push (1.0);
    c.Exec ("exp", s, d);
    VERIFY (AboutEqual(s.Top (), e));
    c.Exec ("clr", s, d);
    VERIFY (s.Size () == 0.0);

    s.Push (16.0);
    c.Exec ("sqrt", s, d);
    VERIFY (AboutEqual(s.Top (), 4.0));
    s.Push (30.0);
    c.Exec ("sin", s, d);
    VERIFY (AboutEqual(s.Top (), 0.5));
    c.Exec ("asin", s, d);
    VERIFY (AboutEqual(s.Top (), 30.0));
    s.Push (60.0);
    c.Exec ("cos", s, d);
    VERIFY (AboutEqual(s.Top (), 0.5));
    c.Exec ("acos", s, d);
    VERIFY (AboutEqual(s.Top (), 60.0));
    s.Push (135.0);
    c.Exec ("tan", s, d);
    VERIFY (AboutEqual(s.Top (), -1.0));
    c.Exec ("atan", s, d);
    VERIFY (AboutEqual(s.Top (), -45.0));

    s.Push (100.0);
    c.Exec ("inv", s, d);
    // In some cases, the results should be exactly equal
    VERIFY (s.Top () == 1.0/100.0);
    s.Push (1.0);
    s.Push (2.0);
    c.Exec ("swap", s, d);
    VERIFY (AboutEqual(s.Pop (), 1.0));
    VERIFY (AboutEqual(s.Pop (), 2.0));
    s.Clear ();
    s.Push (1.0);
    c.Exec ("sto", s, d);
    s.Clear ();
    VERIFY (AboutEqual(s.Top (), 0.0));
    c.Exec ("rcl", s, d);
    VERIFY (AboutEqual(s.Top (), 1.0));

    s.Clear ();
    s.Push (1.0);
    c.Exec ("dup", s, d);
    VERIFY (s.Pop () == 1.0);
    VERIFY (s.Pop () == 1.0);
    VERIFY (s.Pop () == 0.0);
    s.Push (1.0);
    c.Exec ("chs", s, d);
    VERIFY (s.Pop () == -1.0);
    s.Clear ();
    s.Push (1.0);
    c.Exec ("clx", s, d);
    VERIFY (s.Pop () == 0.0);
}

void test3 ()
{
    Stack s;
    Display d;
    SuperCalc c;

    s.Push (3.0);
    c.Exec ("lg", s, d);
    VERIFY (AboutEqual(s.Top (), 1.584962501));
    s.Push (4.0);
    c.Exec ("noop", s, d);
    VERIFY (s.Top () == 4.0);
    c.Exec ("clr", s, d);
    s.Push (1.0);
    s.Push (2.0);
    s.Push (3.0);
    s.Push (4.0);
    s.Push (5.0);
    c.Exec ("sum", s, d);
    VERIFY (s.Pop () == 15.0);
    s.Push (45.0);
    c.Exec ("rad", s, d);
    VERIFY (AboutEqual(s.Pop (), PI / 4.0));
    s.Push (180.0);
    c.Exec ("rad", s, d);
    VERIFY (AboutEqual(s.Pop (), PI));
    s.Push (PI * 2.0);
    c.Exec ("deg", s, d);
    VERIFY (AboutEqual(s.Pop (), 360.0));
    s.Push (PI / 2.0);
    c.Exec ("deg", s, d);
    VERIFY (AboutEqual(s.Pop (), 90.0));
}

int main (int argc, char *argv[])
{
    try
    {
        test0 ();
        test1 ();
        test2 ();
        test3 ();

        cerr << "Success" << endl;
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
