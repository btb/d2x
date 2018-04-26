/*
 *
 * Library functions for printing to mono card.
 *
 */


#include <stdio.h>
#include <stdarg.h>


void mprintf(short n, char *format, ...)
{
   va_list args;

   va_start(args, format);
   vfprintf(stderr, format, args);
   va_end(args);
}

void mprintf_at( short n, short row, short col, char * format, ... )
{
   va_list args;

   va_start(args, format);
   vfprintf(stderr, format, args);
   va_end(args);
}

void mclose(short n)
{
}

void mopen(short n, short row, short col, short width, short height, char *title)
{
}

int minit()
{
   return -1; // everything ok
}

