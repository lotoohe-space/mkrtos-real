/* bin2c.c -- dump binary file in hex format
   Copyright (C) 2007-2019 Free Software Foundation, Inc.

   This file is part of GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include <stdio.h>
#include <stdlib.h>
#include "binary-io.h"

#ifdef MKRTOS

#define HEAP_SIZE 1*1024 * 1024
#define STACK_SIZE (8 * 1024)

#if defined(__CC_ARM)
#define HEAP_ATTR SECTION("HEAP") __attribute__((zero_init))
#define STACK_ATTR SECTION("STACK") __attribute__((zero_init))
#elif defined(__GNUC__)
#define HEAP_ATTR __attribute__((__section__(".bss.heap")))
#define STACK_ATTR __attribute__((__section__(".bss.stack")))
#elif defined(__IAR_SYSTEMS_ICC__)
#define HEAP_ATTR
#define STACK_ATTR
#endif

__attribute__((used)) HEAP_ATTR static char _____heap_____[HEAP_SIZE];
__attribute__((used)) STACK_ATTR static char _____stack_____[STACK_SIZE];

#endif

int
main (int argc, char *argv[])
{
  int c;
  int i;

  if (argc != 1)
    {
      int ishelp = 0;
      FILE *stream;

      if (argc == 2 && argv[1][0] == '-')
	{
	  const char *opt = &argv[1][1];
	  if (*opt == '-')
	    ++opt;
	  ishelp = *opt == 'h' || *opt == 'H';
	}

      stream = ishelp ? stdout : stderr;
      fprintf (stream, "Usage: %s < input_file > output_file\n", argv[0]);
      fprintf (stream, "Prints bytes from stdin in hex format.\n");
      exit (!ishelp);
    }

  SET_BINARY (fileno (stdin));

  i = 0;
  while ((c = getc (stdin)) != EOF)
    {
      printf ("0x%02x,", c);
      if (++i == 16)
	{
	  printf ("\n");
	  i = 0;
	}
    }
  if (i != 0)
    printf ("\n");

  exit (0);
}
