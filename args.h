
//#include <iostream>
#include <string>
// bool isPrefix_(string const& s1, string const&s2);
#include <stdio.h>
#include <string.h>

enum PARAM_TYP {
    STRING,
    INT,
    BOOL,
    ENUM,
};

static const char* STRING_ = "string";
static const char* INT_ = "int";
static const char* BOOL_ = "bool";
static const char* ENUM_ = "enum";

extern void utest ();

extern int isPrefix(char const * p, char const * q);

extern int isContain (char const *p, char c);

// return 0 if not found; return 1 if found 
extern int getBool (char const * name, int argc, const char * const argv[], char const * description);

// return 0 if not found; return 1 if found; value in  res_value
extern const char * getString (char const * param_name, int argc, const char * argv[], char const * description);

// returns -1 if not found; return value if found
extern int getInteger (char const * param_name, int argc, const char * const argv[], char const * description);

extern const char* argsGetEnum (const char* name, int argc, const char* const args[],
        int enum_count, const char * enumerator[],  const char * enumHelp[], char const * description);



extern int split (char const * name_value, char * name, char * value);

extern void args_report ();

extern const char * verify (int argc, const char * const argv[]);

int getCPUCount();

int formatInt (char* str, long n);

void formatIntUtest (void);