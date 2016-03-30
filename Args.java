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
 *  int main (String [] args) {   
 *     int threads = ArgsParse.search_int_value(THREADS, args);
 *     if (threads != Integer.MAX_VALUE)
 *            ....
 *     System.out.print(ArgsParse.show() );
 * 
 *     if (ArgsParse.s_err != null)
 *         ArgsParse.exit (ArgsParse.s_err, 1);
 *
 *     String err =  ArgsParse.verifyArgs();
 *     if (err != null)
 *         ArgsParse.exit (err, 1);
 * 
 */
public class Args {
    

    static ArrayList <String> s_searchedParams = new ArrayList<> ();
    static Map <String, String> s_descriptionMap = new TreeMap <> ();
    static ArrayList <String> s_argType = new ArrayList<> ();
    static String [] s_args;
    public static String s_err = null;
    enum ArgType {
        bool,
        string,
        integer,
    }
    
    public static void clear () {
        s_searchedParams.clear();
        s_err = null;
        s_args = null;
    }

    
    // search one formalParam in actual args
    public static String getString (String name, String [] args, String description) {
        if (s_args == null)
            s_args = args;
        String param = extractParam (name);        
        checkDuplicatesAndAddToList(param);
        s_descriptionMap.put (name, description);
        s_argType.add (ArgType.string.toString());
        for (int n = 0; n < args.length; n++) {
            if (args[n].contains("=")) {
                String [] array = args[n].split("=");
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
        s_argType.add (ArgType.integer.toString());
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
        s_argType.add (ArgType.bool.toString());
        for (int n = 0; n < args.length; n++) {
            if (matchPrefix(args[n], name)) {
                if (args[n].contains("="))
                    s_err = "misMatchType " + args[n];
                return true;       
            }
        }
        return false;
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
   
    public static int min (int a, int b) {
        if (a <= b)
            return a;
        else
            return b;
    }

    public static void showAndVerify (boolean description) {
        final int TXT_LEN = 20; 
        String txt = "";
        txt += "optional args:   ";

        if (description)
            txt += "\n";
        for (int n = 0; n < s_searchedParams.size(); n++) {
            String txt1 = "";
            String arg = s_searchedParams.get(n);
            txt1 += arg;
            if (s_argType.get(n).matches(ArgType.bool.toString()))
                txt1 += "(" + s_argType.get(n) + ")  ";
            else
                txt1 += "=" + s_argType.get(n) + "  ";
            
            if (txt1.length() < TXT_LEN)
                txt1 += "                               ".substring(0, TXT_LEN - txt1.length());
            int len = txt1.length();
            if (description) {
                String descr = s_descriptionMap.get(arg);
                assert descr != null;
                txt1 += "        " + descr + "\n";
            }

            txt += txt1;
        }
        txt += "\nactual args:  ["; 
        for (int n = 0; n < s_args.length; n++) {
            txt += s_args[n] + ", ";
        }
        txt += "]\n\n";

        System.err.print (txt); // print optional parameters        

        String err =  Args.verifyArgs();
        assert err == null : err;
        verifyUnique ();        
        assert Args.s_err == null : s_err;
    }

    // verify all args appear in search list
    public static String verifyArgs () {
        for (int nArg = 0; nArg < s_args.length; nArg++) {
            boolean found = false;
            for (int param = 0; param < s_searchedParams.size(); param ++) {
                String arg = extractParam(s_args[nArg]);
                String parameter = s_searchedParams.get(param);
                if (matchPrefix (parameter, arg)) {
                    found = true;
                    break;
                }
            }
            if (! found)
                return "  invalid param=" + s_args[nArg];
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
    
    // utest can be used as a demo
    public static void utest () {
        String [] args = {"th=7", "copy", "file=/var/js/blabla"};

        boolean copy = Args.getBool ("copy", args, "test==copy_memory (measure bandwidth)");
        assert copy;
        String file = Args.getString ("file", args, "file name");
        assert file != null;
        
        int threadCount = -1;
        //Args.search_int_value("thredd", args, "number of concurrent threads");        
        int threads = Args.getInteger("threds", args, "number of concurrent threads");
        if (threads != Integer.MAX_VALUE)
            threadCount = threads;
        else
            threadCount = Runtime.getRuntime().availableProcessors();
        assert threadCount == 7;
        
        // verify and print
        Args.showAndVerify (true);
        System.exit(0);
    }
}
