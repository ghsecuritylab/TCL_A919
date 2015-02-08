
#ifndef _WPRINTF_PARSE_H
#define _WPRINTF_PARSE_H

#include "printf-args.h"


/* Flags */
#define FLAG_GROUP	 1	/* ' flag */
#define FLAG_LEFT	 2	/* - flag */
#define FLAG_SHOWSIGN	 4	/* + flag */
#define FLAG_SPACE	 8	/* space flag */
#define FLAG_ALT	16	/* # flag */
#define FLAG_ZERO	32

/* arg_index value indicating that no argument is consumed.  */
#define ARG_NONE	(~(size_t)0)

/* A parsed directive.  */
typedef struct
{
  const wchar_t* dir_start;
  const wchar_t* dir_end;
  int flags;
  const wchar_t* width_start;
  const wchar_t* width_end;
  size_t width_arg_index;
  const wchar_t* precision_start;
  const wchar_t* precision_end;
  size_t precision_arg_index;
  wchar_t conversion; /* d i o u x X f e E g G c s p n U % but not C S */
  size_t arg_index;
}
wchar_t_directive;

/* A parsed format string.  */
typedef struct
{
  size_t count;
  wchar_t_directive *dir;
  size_t max_width_length;
  size_t max_precision_length;
}
wchar_t_directives;


#ifdef STATIC
STATIC
#else
extern
#endif
int wprintf_parse (const wchar_t *format, wchar_t_directives *d, arguments *a);

#endif /* _WPRINTF_PARSE_H */
