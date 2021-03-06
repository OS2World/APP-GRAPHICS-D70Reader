/************************************************************************/
/*      $Id: d70utils.c,v 1.1 2005/03/21 23:07:04 joman Exp joman $    */
/*                                                                      */
/*      Source file name:   d70utils.c                                  */
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Mon Mar 21 17:16:56 EST 2005:                    */
/*              First coding.                                           */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>

#ifndef __APPLE__
#ifndef __FreeBSD__
#include <malloc.h>
#endif
#endif

#ifdef WIN32
#include <direct.h>
#endif

#include "d70reader.h"
#include "d70config.h"
#include "d70inc2.h"

#ifdef MPATROL_DEBUG
#include "mpatrol.h"
#endif

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void *my_malloc(char * str_ptr, size_t size)
{
	void *ptr = NULL;

	malloc_count++;

	if(iMemTrace)
		printf("\nmalloc: %s", str_ptr);

	ptr = malloc(size);

	if(ptr == NULL)
	{
		printf("\nmalloc failed");
		exit(1);
	}

	return(ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void my_free(char * str_ptr, void *memblock)
{
	free_count++;

	if(iMemTrace)
		printf("\nfree: %s", str_ptr);

	free(memblock);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void *my_simple_malloc(size_t size)
{
	malloc_count++;
	return(malloc(size));
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void my_simple_free(void *memblock)
{
	free_count++;
	free(memblock);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int my_mkdir(char *str_ptr)
{
	int ret_code = 0;

#ifdef WIN32
	ret_code = _mkdir(str_ptr);
#else
	ret_code = mkdir(str_ptr, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif

	return(ret_code);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int my_rmdir(char *str_ptr)
{
	int ret_code = 0;

#ifdef WIN32
	ret_code = _rmdir(str_ptr);
#else
	ret_code = rmdir(str_ptr);
#endif

	return(ret_code);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void my_error(FILE *rfd)
{
	if(ferror(rfd))
	{
		printf("Error number: %d", fileno(rfd));
		exit(1);
	}
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void my_fseek(FILE *rfd, long pos, int offset)
{
	long new_pos;

	trace("\n+my_fseek");

	if((new_pos = fseek(rfd, pos, offset)) != -1)
	{
		trace("\n-my_fseek");
		return;
	}
	else
	{
		my_error(rfd);
		exit(1);
	}

	trace("\n-my_fseek");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void my_exit(char *text_ptr)
{
	printf("\n\n%s\n", text_ptr);
	printf("\n%s\n\n", "File is not a JPG/JPEG file");
	exit(1);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void my_jpg_exit(char *text_ptr)
{
	printf("\n\n%s\n", text_ptr);
	printf("\n%s\n\n", "File is not a NEF/RAW file");
	exit(1);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void trace(char *str)
{
	if(iTrace)
		printf("%s", str);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void trace_formatted(char *str)
{
	if(iTrace)
		printf("\n%s", str);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void mtrace(char *str)
{
	if(iMemTrace)
		printf("%s", str);
}

