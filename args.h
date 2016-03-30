
//#include <iostream>
#include <string>
// bool isPrefix_(string const& s1, string const&s2);
#include <stdio.h>
#include <string.h>

static const char* STRING = "string";
static const char* INT = "int";
static const char* BOOL = "bool";

extern void utest ();

extern int isPrefix(char const * p, char const * q);

extern int isContain (char const *p, char c);

// return 0 if not found; return 1 if found 
extern int getBool (char const * name, int argc, const char * const argv[], char const * description);

// return 0 if not found; return 1 if found; value in  res_value
extern int getString (char const * param_name, int argc, char * argv[], char * res_value, char const * description) ;

// returns -1 if not found; return value if found
extern int getInteger (char const * param_name, int argc, const char * const argv[], char const * description);

extern int split (char const * name_value, char * name, char * value);

extern void args_report ();

int getCPUCount();

int formatInt (char* str, long n);

void formatIntUtest (void);