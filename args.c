

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

std::map<string,string> param_map;
std::map<string,string> description_map;

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

int isNumeric (char* str) {
    
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
//        if (s_args == null)
//            s_args = args;
    param_map.insert(std::pair<string,string>(name,BOOL));
    description_map.insert(std::pair<string,string>(name,description));    
    for (int n = 1; n < argc; n++) {
        if (isContain(argv[n],'='))
            continue;    
        if (isPrefix (argv[n], name)) {           
            return 1;
        }
    }
    return 0;
}

int split (char const * name_value, char * name, char * value) {
    int len = strlen (name_value);
    for (int n = 0; n < len; n++) {
        if (*(name_value + n) == '=') {
            if (len - n - 1 > 0) {
                memcpy (name, name_value, n);
                memcpy (value, name_value + n + 1, len - n - 1);
            }
            else {
                name[0] = 0;
                value[0] = 0;
            }
                
            return 1;
        }
    }
    return 0;    
    
}

int getString (char const * param_name, int argc, const char * const argv[], char * res_value, char const * description) {
    param_map.insert(std::pair<string,string>(param_name,STRING));
    description_map.insert(std::pair<string,string>(param_name,description));     
    for (int n = 1; n < argc; n++) {
        if (isContain (argv[n], '=')) {
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
            strcpy (res_value, value);            
            return 1;
        }
    }
    return 0;
}

int getInteger (char const * param_name, int argc, const char * const argv[], char const * description) {
    
    char res_value [STR_MAX] = {0};
    int res = getString (param_name, argc, argv, res_value, description);
    param_map.erase (param_name);
    param_map.insert (std::pair<string,string>(param_name,INT));
    description_map.erase (param_name);
    description_map.insert(std::pair<string,string>(param_name,description));     
    
    if (res == 0)
        return -1;

    if (! isNumeric(res_value)) {
        //fprintf (stderr, "\nbad numeric value \n");
        exit (1);
    }
    int value = atoi(res_value);    
    return value;
}

void args_report () {
    char report [1000] = {0};
    int ptr = 0;
    std::cerr << "\nparams:  ";
    for(auto elem : param_map) {
        const int GAP = 15;
        int len;
     
        len = strlen (elem.first.c_str());
        if (elem.second == BOOL) {
            printf ("\n%s(%s)  ", elem.first.c_str(), elem.second.c_str());
            len += 2;
        }
        else {
            printf ("\n%s=<%s>", elem.first.c_str(), elem.second.c_str());
        }
        if (len < GAP) {
            for (int n = 0; n < GAP - len; n++)
                printf (" ");
        }
        
        auto search = description_map.find (elem.first);
        printf ("      %s", search->second.c_str());
        
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
        char value[STR_MAX];
        const char * e = strchr(nameValue, '=');
        if (e == NULL) {
            memcpy (argName, nameValue, strlen (nameValue));
        }
        else
            int res = split (nameValue, argName, value);
        int found = 0;
        for(auto elem : param_map) {
            const char * parameter = elem.first.c_str();

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
      
    static const char *const argv[] = {"nolock","pppp=4","cccc=8","param=123"};
    char str_result [STR_MAX] = {0};

    int argc = 1;
    res = getString ("ppppp", sizeof(argv), argv, str_result, "");
    fprintf (stderr, "\nvalue_str=%s   stat=%d\n", str_result, res);
    res = getInteger ("ppppp", sizeof(argv), argv,"");
    fprintf (stderr, "\nvalue_int=%d\n", res);

    if (getBool ("nolock", sizeof(argv), argv,""))
        fprintf (stderr, "\nnolock__\n");        
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