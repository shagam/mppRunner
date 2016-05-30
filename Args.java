/*

The MIT License (MIT)

Copyright (c) <2012-2016> eli.shagam@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished 
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

package mpprunner;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Map;
import java.util.TreeMap;

/**
 * Jewel Store Copyright (C) 2011-2014  TechoPhil Ltd
 *
 * @author Eli Shagam
 *
 *  retrieve invocation parameters
 * 

 *   usage
 * 
 *      See utest()  below
 *
 * 
 */
public class Args {
    
    static final int ARGS_MAX = 20;
    static ArrayList <String> s_searchedParams = new ArrayList<> ();
    static Map <String, String> s_descriptionMap = new TreeMap <> ();
    static ArrayList <ArgType> s_argType = new ArrayList<> ();
    static String [] s_args;
    public static String s_err = null;
    static String [] [] s_enumHelp;
    
    enum ArgType {
        bool,
        string,
        integer,
        enumer,
    }
    

    
    // search one formalParam in actual args
    public static String getString (String name, String [] args, String description) {
        if (s_args == null)
            s_args = args;
        String param = extractParam (name);        
        checkDuplicatesAndAddToList(param);
        s_descriptionMap.put (name, description);
        s_argType.add (ArgType.string);
        for (int n = 0; n < args.length; n++) {
            // remove leading dashes
            String arg = args[n];
            if (arg.startsWith("--"))
                arg = arg.substring(2);            
            if (arg.contains("=")) {
                String [] array = arg.split("=");
                if (array.length != 2)
                    continue;
                if (! matchPrefix (name, array[0]))
                    continue;
                return array[1];
            }
            if (matchPrefix (name, args[n]))
                s_err = "miss_value " + args[n];
                    
        }
        return null;
    }
      
    public static int getInteger (String name, String [] args, String description) {
        String strValue = getString (name, args, description);
        s_argType.remove(s_argType.size() - 1);
        s_argType.add (ArgType.integer);
        s_descriptionMap.put (name, description);
        if (strValue == null)
            return Integer.MAX_VALUE;
        
        int val;
        try {
            val = Integer.parseInt(strValue, 10);
            return val;
        }
        catch (NumberFormatException e) {
            s_err = name + " invalid int";
            System.err.print(name + " invalid int" );
            return Integer.MAX_VALUE;
        }
    }

    public static boolean getBool (String name, String [] args, String description) {
        if (s_args == null)
            s_args = args;

        checkDuplicatesAndAddToList (name);
        s_descriptionMap.put (name, description);
        s_argType.add (ArgType.bool);
        for (int n = 0; n < args.length; n++) {
            // remove leading dashes
            String arg = args[n];
            if (arg.startsWith("--"))
                arg = arg.substring(2);
            
            if (matchPrefix(arg, name)) {
                if (arg.contains("="))
                    s_err = "misMatchType " + arg;
                return true;       
            }
        }
        return false;
    }
    
    public static String getEnum (String name, String [] args, String[] enumerator, String [] enumHelp, String grossDescription) {
        if (s_args == null)
            s_args = args;
        //checkDuplicatesAndAddToList (name);

        grossDescription += " enum alternatives: ";
        for (int n = 0; n < enumerator.length; n++) {
            grossDescription += enumerator[n];
            if (n < enumerator.length - 1)
                grossDescription += "|";
        }
        
        String selection = getString (name, args, grossDescription);
        s_argType.remove(s_argType.size() - 1);
        s_argType.add (ArgType.enumer);          

        
        if (s_enumHelp == null) {
            s_enumHelp = new String [ARGS_MAX][];
        }
        assert s_searchedParams.size() < ARGS_MAX;
        s_enumHelp [s_searchedParams.size() - 1] = enumHelp;
        
        if (selection == null)
            return null;
                 
        assert enumerator.length == enumHelp.length;
         String result = null;
         int count = 0;
         String alternatines = "";
         for (int n = 0; n < enumerator.length; n++) {
             if (enumerator[n].startsWith(selection)) {
                 count++;
                 result = enumerator[n];
                 alternatines += enumerator[n] + ",";
             }
         }
         switch (count) {
            case 1:
                return result;
            case 0:
                 return null;
            default:
                System.err.print("\nmultiple match: " + alternatines + "\n");
            return null;
         }
     }

    
    static void checkDuplicatesAndAddToList (String name) {
        String param = extractParam (name);
        if (param == null) {
            s_err = " invalid " + name;
            return;
        }
        for (int n = 0; n < s_searchedParams.size(); n++) {
            if (matchPrefix(param, s_searchedParams.get(n))) {
                s_err = " duplicate=" + param;
                return;
            }
        }
        s_searchedParams.add(name);         
    }

    static String extractParam (String nameValue) {
        if (! nameValue.contains("="))
            return nameValue;
        String [] array = nameValue.split("=");
        if (array.length == 2)
            return array[0];
        else
            return null;
    }
    
    static boolean matchPrefix (String a, String b) {
        int len = min (a.length(), b.length());
        a = a.substring(0, len);
        b = b.substring(0, len);
        return a.matches(b);
    }
   
    public static void showAndVerify (boolean help) {
        assert Args.s_err == null : s_err; 
        String txt = "";

        if (help) {
            txt += optionalParams();
        }
        
        //txt += actualArgs();

        System.err.print (txt); // print optional parameters        

        String err =  Args.verifyArgs();
        assert err == null : optionalParams() + err;
        verifyUnique ();        
        assert Args.s_err == null : s_err;
    }
    
    public static String actualArgs () {
        if (s_args.length == 0)
            return "";
        String txt = "\nactual args:  ["; 
        for (int n = 0; n < s_args.length; n++) {
            txt += s_args[n] + ", ";
        }
        txt += "]\n\n";
        return txt;
    }

    public static String optionalParams () {
        final int TXT_LEN = 30;        
        String txt = "";
        txt += "optional args:   ";
        for (int n = 0; n < s_searchedParams.size(); n++) {
            String txt1 = "\n";
            String arg = s_searchedParams.get(n);
            txt1 += arg;
            if (s_argType.get(n) == ArgType.bool)
                txt1 += "(" + s_argType.get(n) + ")  ";
            else
                txt1 += "=<" + s_argType.get(n) + ">  ";

            if (txt1.length() < TXT_LEN)
                txt1 += "                                            ".substring(0, TXT_LEN - txt1.length());
            int len = txt1.length();

            String descr = s_descriptionMap.get(arg);
            assert descr != null;
            txt1 += "        " + descr;
            
            if (s_argType.get(n) == ArgType.enumer) {
                for (String help :s_enumHelp[n]) {
                    txt1 += "\n       " + help;
                }                
            }
            txt += txt1;
        }
        return txt;
    }
        
    // verify all args appear in search list
    public static String verifyArgs () {
        for (int nArg = 0; nArg < s_args.length; nArg++) {
            boolean found = false;
            String arg = extractParam(s_args[nArg]);
            if (arg.startsWith("--"))
                arg = arg.substring(2);
            for (int param = 0; param < s_searchedParams.size(); param ++) {
                String parameter = s_searchedParams.get(param);
                if (matchPrefix (parameter, arg)) {
                    found = true;
                    break;
                }
            }
            if (! found)
                return "  not found param=" + arg;
        }
        return null;
    }
    
    public static void exit (String str, int err) {
        System.err.format("\n" + str + "\n");
        System.exit (err);        
    }
   
    // check if actual arg matches multiple params
    public static void verifyUnique () {
        for (int nArg = 0; nArg < s_args.length; nArg++) {
            String arg = s_args[nArg];
            if (arg.startsWith("--"))
                arg = arg.substring(2);
            String argName;
            if (arg.contains("=")) {
                String [] array = arg.split("=");
                argName = array[0];
            }
            else
                argName = arg;
            
            Collection <String> matches = new ArrayList<> ();
            for (int n = 0; n < s_searchedParams.size(); n++) {
                String param = s_searchedParams.get(n);

                if (matchPrefix (param, argName))
                    matches.add (param);
                assert matches.size() <= 1 : "duplicate matches of arg \"" + argName + "\" params: " + matches;
            }
        }
        
    }

    public static int min (int a, int b) {
        if (a <= b)
            return a;
        else
            return b;
    }
    
    // utest can be used as a demo
    public static void utest () {
        String [] args = {"th=7", "test=copy", "file=/var/js/blabla", "verbose"};

        // Args.getBool 
        boolean verbose = Args.getBool ("verbose", args, "");
        assert verbose;
        
        // Args.getString
        String file = Args.getString ("file", args, "file name");
        assert file != null;
        
        // Args.getInteger
        int threadCount = -1;
        //Args.search_int_value("thredd", args, "number of concurrent threads");        
        int threads = Args.getInteger("threds", args, "number of concurrent threads");
        if (threads != Integer.MAX_VALUE)
            threadCount = threads;
        else
            threadCount = Runtime.getRuntime().availableProcessors();
        assert threadCount == 7;

        // Args.getEnum 
        String [] args1 = {"test=tre"};        
        String [] tests = {"tree","hash","queue","alloc","prime","queue","copy","count","noLock"};
        String test = Args.getEnum ("test", args, tests, tests, "test selection ");
        
        // verify and print
        Args.showAndVerify (true);
        System.exit(0);
    }
}
