// Reverse Polish Notation Calculator
//
// Copyright (C) 2007
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Wed Mar 14 13:07:41 CDT 2007

#include "argv.h"
#include "rpn.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace jsp;

int main (int argc, char *argv[])
{
    try
    {
        bool help = false;
        bool basic = false;
        bool hp35 = false;
        bool super = false;
        string fn;

        jsp::CommandLine cl;
        cl.AddSpec ("help",     'h',    help,   "Show help");
        cl.AddSpec ("basic",    'b',    basic,  "Basic mode");
        cl.AddSpec ("hp35",     '3',    hp35,   "HP35 mode");
        cl.AddSpec ("super",    's',    super,  "Super mode (default)");

        cl.GroupArgs (argc, argv, 1);
        cl.ExtractBegin ();
        cl.Extract (help);
        cl.Extract (basic);
        cl.Extract (hp35);
        cl.Extract (super);
        cl.ExtractEnd ();

        if (!cl.GetLeftOverArgs ().empty ())
            throw runtime_error ("usage: rpn " + cl.Usage () + "\n");

        // A Reverse Polish Notation Calculator
        auto_ptr<BasicCalc> calc;

        if (basic)
            calc = auto_ptr<BasicCalc> (new BasicCalc);
        else if (hp35)
            calc = auto_ptr<HP35> (new HP35);
        else
            calc = auto_ptr<SuperCalc> (new SuperCalc);

        cerr << "RPN calculator, version "
            << calc->Version ()
            << endl;
        cerr << "Copyright (C) 2007 Jeff Perry"
            << endl;

        if (help)
        {
            cerr << endl;
            cerr << cl.Help ();
            cerr << endl;
            cerr << "Type 'help' from the calculator prompt for more help" << endl;
            return 0;
        }

        // The calculator operates on a stack and a display
        Stack stack;
        Display display;

        // Loop until 'quit' or eof
        while (true)
        {
            // Check for more input
            if (!cin)
                break;

            // Show a prompt
            cerr << "> "; cerr.flush ();

            // Get a string
            string str;
            cin >> str;

            // Does it contain anything?
            if (str.empty ())
                continue;

            // Save it in a stringstream
            stringstream tmp (str);

            // Try to get it into a double
            double x;
            tmp >> x;

            // If it's a double, push it onto the stack
            if (!tmp.fail ())
            {
                stack.Push (x);
                // ... then show the stack
                for (size_t i = 0; i < stack.Size (); ++i)
                {
                    double x  = stack.Get (i);
                    display.Show (x);
                }
            }
            // If it's a program command, do the command
            else if (str == "quit")
            {
                break;
            }
            else if (str == "help")
            {
                cerr << "commands:" << endl;
                // Program commands
                cerr << "help\tdisplay this help screen" << endl;
                cerr << "quit\tpop the stack and exit" << endl;
                // Display commands
                for (RPNCalc::DisplayOps::iterator i = calc->DisplayBegin ();
                    i != calc->DisplayEnd (); ++i)
                {
                    cerr << i->first << "\t";
                    cerr << i->second->Help () << endl;
                }
                // Calculator commands
                for (RPNCalc::StackOps::iterator i = calc->StackBegin ();
                    i != calc->StackEnd (); ++i)
                {
                    cerr << i->first << "\t";
                    cerr << i->second->Help () << endl;
                }
            }
            // If it's a calculator op, do the op
            else if (calc->Lookup (str))
            {
                calc->Exec (str, stack, display);
                // ... then show the stack
                for (size_t i = 0; i < stack.Size (); ++i)
                {
                    double x  = stack.Get (i);
                    display.Show (x);
                }
            }
            else
                cerr << str << "?" << endl;
        }

        // Print the top of the stack and exit
        cout << stack.Top () << endl;

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
