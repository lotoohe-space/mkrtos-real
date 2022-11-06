#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef WANT_STRTOD_WITHOUT_LONG_DOUBLE
#define ldbltype long double
#else
#define ldbltype double
#endif

double strtod(const char* s, char** endptr) {
    register const char*  p     = s;
    register ldbltype     value = 0.;
    int                   sign  = +1;
    ldbltype              factor;
    unsigned int          expo;

    while ( isspace(*p) )
        p++;

    switch (*p) {
    case '-': sign = -1;	/* fall through */
    case '+': p++;
    default : break;
    }

    while ( (unsigned int)(*p - '0') < 10u )
        value = value*10 + (*p++ - '0');

    if ( *p == '.' ) {
        factor = 1.;

        p++;
        while ( (unsigned int)(*p - '0') < 10u ) {
            factor *= 0.1;
            value  += (*p++ - '0') * factor;
        }
    }

    if ( (*p | 32) == 'e' ) {
        expo   = 0;
        factor = 10.;

        switch (*++p) {                 // ja hier weiß ich nicht, was mindestens nach einem 'E' folgenden MUSS.
        case '-': factor = 0.1;	/* fall through */
        case '+': p++;
                  break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                  break;
        default : value = 0.;
                  p     = s;
                  goto done;
        }

        while ( (unsigned int)(*p - '0') < 10u )
            expo = 10 * expo + (*p++ - '0');

        while ( 1 ) {
            if ( expo & 1 )
                value *= factor;
            if ( (expo >>= 1) == 0 )
                break;
            factor *= factor;
        }
    }

done:
    if ( endptr != NULL )
        *endptr = (char*)p;

    return value * sign;
}
