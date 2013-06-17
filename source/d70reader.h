/************************************************************************/
/*	$Id: d70reader.h,v 1.1 2005/03/04 16:00:02 joman Exp joman $	*/
/*									*/
/*      Include file name:   d70reader.h 				*/
/*                                                                      */
/************************************************************************/
/*	Revisions:							*/
/*									*/
/*	Version 0.01 - Fri Mar  4 10:53:34 EST 2005:			*/
/*		First coding.						*/
/************************************************************************/

struct xmp_data
{
	unsigned short tag;
	char *description;
	char *value;
	struct xmp_data *next;
};

