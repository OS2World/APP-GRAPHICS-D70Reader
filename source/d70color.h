/************************************************************************/
/*      $Id: d70color.h,v 1.1 2005/03/21 23:02:43 joman Exp joman $    */
/*                                                                      */
/*      Include file name:  d70color.h                                  */
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Mon Mar 21 17:16:56 EST 2005:                    */
/*              First coding.                                           */
/************************************************************************/

#ifndef __D70COLOR_H__
#define __D70COLOR_H__

#include "d70image.h"

#define COL(red,green,blue) (((red)<<24)|((green)<<16)|((blue)<<8)|0xFF)
#define COL_FULL(red,green,blue,alpha) (((red)<<24)|((green)<<16)|((blue)<<8)|(alpha))
#define SET_COL(col,red,green,blue) {col=(red)<<24|(green)<<16|(blue)<<8|0xFF;}
#define SET_COL_RED(col,red)     {col=((col)&0x00FFFFFF)|((red)<<24);  }
#define SET_COL_GREEN(col,green) {col=((col)&0xFF00FFFF)|((green)<<16);}
#define SET_COL_BLUE(col,blue)   {col=((col)&0xFFFF00FF)|((blue)<<8);  }
#define SET_COL_ALPHA(col,alpha) {col=((col)&0xFFFFFF00)|(alpha);      }
#define COL_RED(col)   (col>>24)
#define COL_GREEN(col) ((col>>16)&0xFF)
#define COL_BLUE(col)  ((col>>8)&0xFF)
#define COL_ALPHA(col) (col&0xFF)

#define RED_MASK   0xFF000000
#define GREEN_MASK 0x00FF0000
#define BLUE_MASK  0x0000FF00
#define ALPHA_MASK 0x000000FF

#define RED_SHL   24
#define GREEN_SHL 16
#define BLUE_SHL  8
#define ALPHA_SHL 0

#define WHITE  0xFFFFFFFF
#define BLACK  0x000000FF
#define BLUE   0x0000FFFF
#define GREEN  0x00FF00FF
#define RED    0xFF0000FF
#define ORANGE 0xF08020FF
#define YELLOW 0xFFFF00FF

#endif /* __D70COLOR_H__ */
