/************************************************************************/
/*      $Id: d70types.h,v 1.1 2005/03/21 23:03:57 joman Exp joman $    */
/*                                                                      */
/*      Include file name:   d70types.h                                 */
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Mon Mar 21 17:16:56 EST 2005:                    */
/*              First coding.                                           */
/************************************************************************/

#ifndef __D70TYPES_H__
#define __D70TYPES_H__

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  !FALSE
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef int8
#define int8  char
#endif
#ifndef int16
#define int16 short
#endif
#ifndef int32
#define int32 int
#endif

#ifndef uint8
#define uint8  unsigned int8
#endif
#ifndef uint16
#define uint16 unsigned int16
#endif
#ifndef uint32
#define uint32 unsigned int32
#endif

#ifndef pointer
#define pointer void *
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN3
#define MIN3(a,b,c) MIN(MIN(a,b),MIN(b,c))
#endif
#ifndef MAX3
#define MAX3(a,b,c) MAX(MAX(a,b),MAX(b,c))
#endif

#ifndef ABS
#define ABS(a)   ((a >= 0)?(a):-(a))
#endif

#endif /* __D70TYPES_H__ */
