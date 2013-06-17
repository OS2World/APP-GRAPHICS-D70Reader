/************************************************************************/
/*	$Id: d70config.h,v 1.1 2005/02/15 02:04:32 joman Exp joman $	*/
/*									*/
/*      Include file name:   d70config.h 				*/
/*                                                                      */
/************************************************************************/
/*	Revisions:							*/
/*									*/
/*	Version 0.01 - Mon Feb 14 20:57:55 EST 2005:			*/
/*		First coding.						*/
/************************************************************************/

/*****************************************************************************
 *	To over-ride the program defaults, change the following variable, to
 *	the path where you want the config file, before compiling the source.
 *	
 *	Note:  The path you choose, must already exist.  It will not be created.
 *	
 *	Example for Windows (notice the double slashes!):
 *	static char *file_sys_dir_ptr = "C:\\Documents and Settings\\Peter Boyle";
 *	
 *	Example for OS/2 (notice the double slashes!):
 *	static char *file_sys_dir_ptr = "C:\\OS2\\APPS";
 *	
 *	Example for Linux/Unix:
 *	static char *file_sys_dir_ptr = "/home/user_name";
 *****************************************************************************/

static char *file_sys_dir_ptr = NULL;
