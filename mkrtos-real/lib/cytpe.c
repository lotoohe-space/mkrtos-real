/*
 * cytpe.c
 *
 *  Created on: 2022Äê9ÔÂ13ÈÕ
 *      Author: Administrator
 */

int mkrtos_isupper ( int ch )
{
    return (unsigned int)(ch - 'A') < 26u;
}
int mkrtos_islower( int ch ) {
    return (unsigned int) (ch - 'a') < 26u;
}
int mkrtos_tolower(int ch) {
  if ( (unsigned int)(ch - 'A') < 26u )
    ch += 'a' - 'A';
  return ch;
}
int mkrtos_toupper(int ch) {
  if ( (unsigned int)(ch - 'a') < 26u )
    ch += 'A' - 'a';
  return ch;
}
int mkrtos_iscntrl ( int ch ) {
    return (unsigned int)ch < 32u  ||  ch == 127;
}
