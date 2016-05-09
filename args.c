

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



#include <iostream>
#include "args.h"
#include <string>
#include <map>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//#include <string.h>

using namespace std;

//static char * s_argv[];

const int STR_MAX = 100;
const int ARGS_MAX = 20;

const char * paramNameArray[ARGS_MAX];
PARAM_TYP paramTypeArray[ARGS_MAX];
const char * param_type_name[ARGS_MAX];
const char * paramDescriptionArray[ARGS_MAX];
const char * * s_enumHelp [ARGS_MAX];

int paramCount = 0;
int s_enumHelpCount;

int isPrefix(char const * prefix, char const * str)
{
    int p_len = strlen (prefix);
    int q_len = strlen (str);
    
    if (p_len > q_len)
        return 0;
                
    while (*prefix && *str) {
        if (*prefix++ != *str++)
            return 0;
    }
    return 1;
}

int isContain (char const *p, char c) {

    while (*p) {
        if (*p++ == c)
            return 1;
    }
    return 0;    
}

int isNumeric (const char* str) {
    
    for (int n = 0; n < sizeof(str) - 1; n++) {
        if (str[n] == 0)
            return 1;
        if (! isdigit (str[n])) {
            fprintf (stderr, "\nbad numeric value=%s %s", str, "\n");            
            exit (1);
        }
    }
    return 1;
}

int getBool (char const * name, int argc, const char * const argv[], char const * description) {
    paramNameArray[paramCount] = name;
    paramTypeArray[paramCount] = BOOL;
    param_type_name[paramCount] = "bool";
    paramDescriptionArray[paramCount] = description;    
    paramCount ++;
    for (int n = 1; n < argc; n++) {
        if (isContain(argv[n],'='))
            continue;    
        if (isPrefix (argv[n], name)) {           
            return 1;
        }
    }
    return 0;
}

int split (char const * name_value, char * param_name, char * value) {
    int len = strlen (name_value);
    for (int n = 0; n < len; n++) {
        if (*(name_value + n) == '=') {
            if (len - n - 1 > 0) {
                memcpy (param_name, name_value, n);
                memcpy (value, name_value + n + 1, len - n - 1);
            }
            else {
                param_name[0] = 0;
                value[0] = 0;
            }
                
            return 1;
        }
    }
    return 0;    
    
}

const char * getString (char const * param_name, int argc, const char * const argv[], char const * description) {
    paramNameArray[paramCount] = param_name;
    paramTypeArray[paramCount] = STRING;
    param_type_name[paramCount] = "string";
    paramDescriptionArray[paramCount] = description; 
    paramCount ++;   
    for (int n = 0; n < argc; n++) {
        const char * e = strchr(argv[n], '=');
        if (e == NULL)
            continue;
        e++; // skip =
        if (e - argv[n] >= strlen (argv[n]))
            return NULL;
        {
            char name [STR_MAX] = {0};
            char value [STR_MAX] = {0};    

            int res = split (argv[n], name, value);
            if (res = 0)
                continue;

            int nameLen = strlen(name);
            int valueLen = strlen (value);

            if (nameLen == 0 || valueLen == 0)
                continue;

            if ( ! isPrefix(name, param_name))
                continue;
            //fprintf (stderr, "\nsearch_value=%s\n", value);            
            return e;
        }
    }
    return NULL;
}

int getInteger (char const * param_name, int argc, const char * const argv[], char const * description) {
    
    const char * res = getString (param_name, argc, argv, description);
    paramCount --;
    paramNameArray[paramCount] = param_name;
    paramTypeArray[paramCount] = INT;
    param_type_name[paramCount] = "int";    
    paramDescriptionArray[paramCount] = description;      
    paramCount ++;
    
    if (res == NULL)
        return -1;

    if (! isNumeric(res)) {
        //fprintf (stderr, "\nbad numeric value \n");
        exit (1);
    }
    int value = atoi(res);    

    return value;
}

const char* argsGetEnum (const char* name, int argc, const char * const args[], int enum_count,
        const char * enumerator[], const char * enumHelp[], char const * description) {
    s_enumHelp[paramCount] = enumHelp;
    s_enumHelpCount = enum_count;
    int ptr = 0;
    char descr[STR_MAX] = {0};
    ptr += sprintf (descr, "%senum alternatives: ", description);
    for (int n = 0; n < enum_count; n++) {
        ptr += sprintf (descr + ptr, "%s", enumerator[n]);
        if (n < enum_count - 1)
            ptr += sprintf (descr + ptr, "|");
    }

    const char * selection = getString (name, argc, args, descr);
    paramCount --;
    paramNameArray[paramCount] = name;
    paramTypeArray[paramCount] = ENUM;
    param_type_name[paramCount] = "enum";    
    paramDescriptionArray[paramCount] = description;   
    paramCount ++;    
    if (selection == NULL)
         return NULL;
  
     
     const char * result1;
     int count = 0;
     char alternatines[STR_MAX] = {0};
     ptr = 0;
     for (int n = 0; n < enum_count; n++) {
         if (isPrefix(selection, enumerator[n])) {
             count++;
             result1 = enumerator[n];
             ptr += sprintf (alternatines + ptr, "%s,", enumerator[n]);
         }
     }
     switch (count) {
        case 1:
            return result1;
        case 0:
             return NULL;
        default:
            fprintf (stderr, "\nmultiple match: %s\n", alternatines);
        return NULL;
     }
 }


void args_report () {
    char report [1000] = {0};
    int ptr = 0;
    std::cerr << "\nparams:  ";
    for (int n = 0; n < paramCount; n++) {
    //for(auto elem : param_map) {
        const int GAP = 20;
        int len;
     
        len = strlen (paramNameArray[n]);
        switch (paramTypeArray[n]) {
            case  BOOL:
            printf ("\n%s(%s)  ", paramNameArray[n], "bool");
            len += 2;
            break;
            
            case STRING:
            printf ("\n%s=<%s>", paramNameArray[n], "string");
            break;

            case INT:
            printf ("\n%s=<%s>", paramNameArray[n], "int");
            break;
            
            case ENUM:
            printf ("\n%s=<%s>", paramNameArray[n], "enum");
            break;
        }        
        if (len < GAP) {
            if (paramTypeArray[n] == STRING)
                len += 3;
            for (int n = 0; n < GAP - len; n++)
                printf (" ");
        }
        
        printf ("      %s", paramDescriptionArray [n]);
        
        if (paramTypeArray[n] == ENUM) {

            for (int i = 0; i < s_enumHelpCount; i++) {
                printf ("\n        %s", s_enumHelp[n][i]);                
            }
        }
        
    }
    std::cerr << "\n";
}

int formatInt (char* str, long n) {
        const int BILION = 1000000000L; 
        const int MILION = 1000000L; 
        const int KILO = 1000L; 
        int fill = 0;
        char* str_save = str;

        if (n > BILION) {
            str += sprintf(str, "%ld,", n / BILION);
            n -= n / BILION * BILION;
            fill++;
        }
        if (n > MILION) {
            if (fill)
                str += sprintf(str, "%03ld,", n / MILION);
            else
                str += sprintf(str, "%ld,", n / MILION);
            n -= n / MILION * MILION;
            fill++;
        }        
        if (n > KILO) {

            if (fill)
                str += sprintf(str, "%03ld,", n / KILO);
            else
                str += sprintf(str, "%ld,", n / KILO);
            n -= n / KILO * KILO;
            fill++;          
        }
        if (fill)
            str += sprintf(str, "%03ld", n);
        else
            str += sprintf(str, "%ld", n);
        return str - str_save;
    }
    
void formatIntUtest (void) {
    char str1 [1000] = {0};
    char* str = str1;
    str += sprintf (str, "\n");
    str += formatInt(str, 3333034567089L);
    str += sprintf (str, "\n");
    str += formatInt(str, 4567089);
    str += sprintf (str, "\n");
    str += formatInt(str, 7089);
    str += sprintf (str, "\n");
    str += formatInt(str, 89);
    str += sprintf (str, "\n");
    printf ("%s", str1);
    
}

// verify valid args 
const char * verify (int argc, const char * const argv[]) {
    for (int nArg = 1; nArg < argc; nArg++) {
        const char * nameValue = argv[nArg];
        if (nameValue == NULL)
            return NULL;       
        char argName [STR_MAX] = {0};
        char value[STR_MAX] = {0};
        const char * e = strchr(nameValue, '=');
        if (e == NULL) {
            memcpy (argName, nameValue, strlen (nameValue));
        }
        else
            int res = split (nameValue, argName, value);
        int found = 0;
        
        for (int param = 0; param < paramCount ; param ++) {
            const char * parameter = paramNameArray[param];

            if (isPrefix (argName, parameter)) {
                found = 1;
                break;
            }
        }
        if (! found)
            return /* "  invalid param=" + */ argv[nArg];
    }
    return NULL;
  
}

void utest () {
       
    static const char *const argv[] = {"testNam=allo","delay=4", "help"};
    static const int argc = 3;

    // get string param
    const char * status = getString ("testName", argc, argv, "description: type of test");
    if (status == NULL) { // means param testName is missing 
        fprintf (stderr, "\n  stat=%s\n", status);
    }

    // get int param
    int delay = getInteger ("delay", argc, argv,"description: duration of test");
    if (delay == -1) {  //-1  means delay arg missing
        fprintf (stderr, "\nvalue_int=%d\n", delay);
    }
    
    const char * tests[] = {"tree","hash","queue","alloc","prime","copy","count","noLock"};
    const char * testHelp[] = {"tree retrieve", "hash retrieve", "queue get/enque", "alloc new / delete (With variable arraySize)",
            "prime compute intensive", "copy mem (Bandwidth)", "common counter increment by many threads", "tree retrieve without lock"};    
    
    const char * test = argsGetEnum ("testName", argc, argv, 8, tests, testHelp, "test selection ");
    
    // get boolean param 0/1
    int help = getBool ("help", argc, argv,"description: print optional params");
    if (help)
        args_report(); // if help exist print all allowed arguments;  arg_report after all args collected
    
    // verify no bogus arguments
    const char * err = verify (argc, argv);
    if (err != NULL) {
        fprintf (stderr, "invalid param=%s  \n", err);
        exit (3);
    }
}

// test for sub routines
int utest_sub () {
   int res = isContain ("aaa=bbbb", '=');
//    if (res)
//        fprintf (stderr, "contain\n");
    
    res = isPrefix ("aaa", "aaattt\n");
    if (res)
        fprintf (stderr, "prefix\n");
    
    
    
    //const int SIZ = 20;
    char name_value [] = "param=value";
    char name [STR_MAX];
    char value [STR_MAX];    
    
    res = split (name_value, name, value);
    int nameLen = strlen(name);
    int valueLen = strlen (value);
    
    if (res && valueLen > 0) {
        fprintf (stderr, "\nSplit   in:%s  name=%s %d   value=%s %d\n", name_value, name, nameLen, value, valueLen);
    }
    
    static const char *const days[] = { "mon", "tue", "wed", "thur",
                                       "fri", "sat", "sun" };
    
}


int getCPUCount() {
 cpu_set_t cs;
 CPU_ZERO(&cs);
 sched_getaffinity(0, sizeof(cs), &cs);

 int count = 0;
 for (int i = 0; i < 20; i++)
 {
  if (CPU_ISSET(i, &cs))
   count++;
 }
 return count;
}