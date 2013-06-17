/************************************************************************/
/*      $Id: d70image.h,v 1.1 2005/03/21 23:03:02 joman Exp joman $    */
/*                                                                      */
/*      Include file name:   d70image.h                                 */
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Mon Mar 21 17:16:56 EST 2005:                    */
/*              First coding.                                           */
/************************************************************************/

#ifndef __D70IMAGE_H__
#define __D70IMAGE_H__

#include "d70types.h"
#include "d70color.h"

#define MAX_JPEG_FILE_SIZE 10000000
#define JPEG_QUALITY 255

typedef uint32 pix;

typedef struct {
  int32 width,height;
  pix    *buf;
} image;

#endif /* __D70IMAGE_H__ */
