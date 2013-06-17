/************************************************************************/
/*	$Id: d70reader.c,v 1.31 2005/04/14 17:38:53 joman Exp joman $	*/
/*									*/
/*      Program name:   d70reader.c                                     */
/*                                                                      */
/*      This program will output image file EXIF data to the screen.    */
/*      This program was extended from my nefexif.c program. 		*/
/*      This may be compiled under Linux GCC like this:  cc d70reader.c */
/*      This can also be compiled within a Visual C++ project.          */
/************************************************************************/
/*	Revisions:							*/
/*									*/
/*	Version 0.01 - Sun Dec 12 12:11:21 EST 2004:			*/
/*		First coding to combine nefexif and jpgexif.		*/
/*	Version 0.02 - Thu Dec 23 14:13:18 EST 2004:			*/
/*		Added wildcard support.					*/
/*	Version 0.03 - Sun Dec 26 11:19:15 EST 2004:			*/
/*		Added edits for Format 7 tags.  Added the -v and -h	*/
/*		options.  Fixed a couple of Format 3 edits.		*/
/*	Version 0.04 - Mon Dec 27 16:19:25 EST 2004:			*/
/*		Added LensInfo.  Reformatted raw lens data to be	*/
/*		more presentable.  Truncated decimal part of 		*/
/*		focal length display.  Added to RCS revision system.	*/
/*	Version 0.05 - Wed Dec 29 16:01:24 EST 2004:			*/
/*		Added -w option.  This option will create an HTML	*/
/*		table of the EXIF data.  This can be then inserted	*/
/*		into an HTML document.					*/
/*	Version 0.06 - Thu Dec 30 16:20:39 EST 2004:			*/
/*		Added -x option.  This option outputs XML data.		*/
/*	Version 0.07 - Sat Jan  1 12:05:28 EST 2005:			*/
/*		Fixed UserComment.  Correct misspelling of 'seconds'.	*/
/*		Handled 'unknown' tag for D2H.				*/
/*	Version 0.08 - Mon Jan  3 23:03:38 EST 2005:			*/
/*		Added tag 8827 (ISO).  Modified to use right shift 	*/
/*		operators for selected formats that are less than 	*/
/*		4 bytes long.						*/
/*	Version 0.09 - Mon Jan 10 16:32:23 EST 2005:			*/
/*		Added -j option to extract JPG thumbnail image from	*/
/*		a NEF image.  Additional code cleanup.			*/
/*	Version 0.10 - Wed Jan 12 20:49:51 EST 2005:			*/
/*		Added -c option to generate CSV data.			*/
/*	Version 0.11 - Thu Jan 13 12:03:17 EST 2005:			*/
/*		Corrected a problem with the -c option.  Forgot to	*/
/*		include the image file name in the output! Also added	*/
/*		double quotes around the data elements.			*/
/*	Version 0.12 - Fri Jan 21 09:34:51 EST 2005:			*/
/*		When using option -j, to extract the JPG image from a	*/
/*		NEF image, write EXIF data to the new JPG file.  Also	*/
/*		modified the XML output to convert the tags to		*/
/*		lowercase.						*/
/*	Version 0.13 - Sat Jan 22 15:57:10 EST 2005:			*/
/*		Fixed a problem with wildcard processing, where the	*/
/*		program would stop on first error, instead of going	*/
/*		on to the next file in the list.  Also fixed, was the	*/
/*		JPEG marker scan logic.  Finally, changed the 'ifdef'	*/
/*		logic for the FreeBSD system compile.			*/
/*	Version 0.14 - Fri Jan 28 12:17:09 EST 2005:			*/
/*		Added logic to rotate the JPG image when the -j option	*/
/*		is used.  The source now uses the IJG JPEG library.	*/
/*		A configuration file called 'd70reader.conf' can be	*/
/*		used to select the tags that will be displayed.		*/
/*	Version 0.15 - Mon Feb 14 22:52:53 EST 2005:			*/
/*		Changed the default location for the config file.	*/
/*		The default is to create the file in the user space,	*/
/*		not the system space. (Not in /etc, or c:\windows)	*/
/*	Version 0.16 - Sat Feb 19 14:01:53 EST 2005:			*/
/*		Added 'assert' logic to prevent bad data processing.	*/
/*		Fixed a buffer overflow exception with format #1.	*/
/*		Cleaned out dead code.  Better control of iSystemType.	*/
/*		Only process MakerNotes for Nikon cameras.  Calculate	*/
/*		shutter speed, instead of if/else logic.  Added 'G-VR'	*/
/*		and 'D-VR' lens types.					*/
/*	Version 0.17 - Mon Feb 21 15:47:19 EST 2005:			*/
/*		JPG extraction now works for the D100 and D2H.  Added	*/
/*		WhiteBalance and Sharpening tags.  Fixed (hopefully)	*/
/*		the last buffer overflow problem.			*/
/*	Version 0.18 - Fri Mar  4 10:53:34 EST 2005:			*/
/*		Added option -js, to generate JavaScript arrays of the	*/
/*		EXIF data.  Added place holders for the missing tags	*/
/*		for the -c option.  Added the Adobe XMP SDK logic to	*/
/*		capture more EXIF data for Adobe modified files.  Fixed	*/
/*		a problem with the lens display, where the decimal	*/
/*		part was being truncated.  Allow the *.* wildcard,	*/
/*		ignoring non-image files.  Added support for the D2X	*/
/*		camera.							*/
/*	Version 0.19 - Mon Mar 21 17:38:33 EST 2005:			*/
/*		Added ImageWidth and ImageLength tags.  These are 	*/
/*		embedded in a NEF file.  Added a space before the 'mm'	*/
/*		output for focal length and lens.  Truncated FNumber to	*/
/*		two decimal places.  Added the -web option.  This 	*/
/*		allows the generation of image galleries that can be	*/
/*		copied as is to a web site for display.			*/
/*	Version 0.20 - Wed Mar 23 22:54:24 EST 2005:			*/
/*		Corrected a problem, where the program was not		*/
/*		rotating a JPG image with the -web option.		*/
/*	Version 0.21 - Thu Apr 14 13:17:57 EDT 2005:			*/
/*		Added the -rm option to allow complete removal of the	*/
/*		EXIF data from a JPG image.  Fixed a problem with the	*/
/*		-web option, where vertical images were not always	*/
/*		displayed properly.  Handle a situation where the image	*/
/*		file does not contain the orientation tag 0x112.	*/
/*		Fixed a problem where the -j option would fail with	*/
/*		certain Nikon Capture created files.  When using the	*/
/*		-j option, the EXIF data is now correctly formatted	*/
/*		into the proper buckets, instead of having it all at	*/
/*		the lowest level.  This means that other applications	*/
/*		will now correctly read it.  The original -j logic	*/
/*		from version 0.20 is kept as -jold.  There are still	*/
/*		some image processing software created images that	*/
/*		can not be handled correctly with the newer -j option.	*/
/*		The -jold option does work with some of these images.	*/
/*									*/
/************************************************************************/
#ifdef __OS2__
#define _TRUNCATE_DECLARED
#endif

#ifdef __FreeBSD__
#define _TRUNCATE_DECLARED
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>

#ifdef WIN32
#include <direct.h>
#endif

#include "d70reader.h"
#include "d70config.h"
#include "d70inc1.h"
#include "d70inc2.h"

#ifndef __APPLE__
#ifndef __FreeBSD__
#include <malloc.h>
#endif
#endif

#ifdef MPATROL_DEBUG
#include "mpatrol.h"
#endif

#define D70_VERSION "0.21"
#define D70_DATE "Thu Apr 14 17:18:08 UTC 2005"
static char rcsid[] = "$Id: d70reader.c,v 1.31 2005/04/14 17:38:53 joman Exp joman $";

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int main(int argc, char *argv[], char* env[])
{
	int ctr1 = 0;
	int ctr2 = 0;
	int ctr3 = 0;
	int argn = 0;
	char *arg = NULL;
	struct stat buf;
	char *filename = NULL;
	char *temp_ptr = NULL;

#ifdef __OS2__
	_wildcard(&argc, &argv);
#endif

	if(argc < 2)
	{
		instruct();
		exit(1);
	}

	for(argn = 1; argn < argc; argn++)
	{
		arg = argv[argn];

		if(arg[0] != '-') 
			break; // Filenames have started.

		if(!strcmp(arg,"-v"))
		{
			iVerbos = 1;
		}
		else if(!strcmp(arg,"-w"))
		{
			iHtml = 1;
		}
		else if(!strcmp(arg,"-c"))
		{
			iCsv = 1;
		}
		else if(!strcmp(arg,"-web"))
		{
			iWeb = 1;
		}
		else if(!strcmp(arg,"-js"))
		{
			iJS = 1;
		}
		else if(!strcmp(arg,"-x"))
		{
			iXml = 1;
		}
		else if(!strcmp(arg,"-j"))
		{
			iThumbnail = 1;
		}
		else if(!strcmp(arg,"-jold"))
		{
			iThumbnailOld = 1;
		}
		else if(!strcmp(arg,"-rm"))
		{
			iRemoveEXIF = 1;
		}
		else if(!strcmp(arg,"-t"))
		{
			iTrace = 1;
		}
		else if(!strcmp(arg,"-h"))
		{
			instruct();
			exit(1);
		}
		else
		{
			instruct();
			exit(1);
		}

		ctr1++;
	}

	if(argc == argn)
	{
		instruct();
		exit(1);
	}

	do
	{
		temp_ptr = (char *) malloc(strlen(env[ctr3]) + 1);
		memset(temp_ptr, (int) NULL, strlen(env[ctr3]) + 1);
		memcpy(temp_ptr, env[ctr3], strlen(env[ctr3]));
		temp_ptr = upper_case(temp_ptr);

		if(iTrace)
			printf("\n%s", temp_ptr);

		if(strlen(temp_ptr) >= 5)
		{
			if(!memcmp(temp_ptr, "PATH=", 5))
			{
				if(memchr(temp_ptr,'\\', 20))
					iSystemType = 1;
				else
					iSystemType = 0;
			}
		}

		if(file_sys_dir_ptr == NULL)
		{
			if(strlen(temp_ptr) >= 12)
			{
				// if found, this should be windows:
				if(!memcmp(temp_ptr, "USERPROFILE=", 12))
				{
					file_sys_dir_ptr = (char *) my_malloc("file_sys_dir_ptr", strlen(temp_ptr) + 1);
					memset(file_sys_dir_ptr, (int) NULL, strlen(temp_ptr) + 1);
					memcpy(file_sys_dir_ptr, &env[ctr3][12], strlen(temp_ptr) - 12);
	
					if(iTrace)
						printf("\nfile_sys_dir_ptr: %s\n", file_sys_dir_ptr);
				}
			}

			if(strlen(temp_ptr) >= 5)
			{
				// if found, this should be linux/unix:
				if(!memcmp(temp_ptr, "HOME=", 5))
				{
					file_sys_dir_ptr = (char *) my_malloc("file_sys_dir_ptr", strlen(temp_ptr) + 1);
					memset(file_sys_dir_ptr, (int) NULL, strlen(temp_ptr) + 1);
					memcpy(file_sys_dir_ptr, &env[ctr3][5], strlen(temp_ptr) - 5);
	
					if(iTrace)
						printf("\nfile_sys_dir_ptr: %s\n", file_sys_dir_ptr);
				}
			}

			if(strlen(temp_ptr) >= 10)
			{
				// if found, this should be OS/2:
				if(!memcmp(temp_ptr, "OS2_SHELL=", 10))
				{
					file_sys_dir_ptr = (char *) my_malloc("file_sys_dir_ptr", strlen(temp_ptr) + 10);
					memset(file_sys_dir_ptr, (int) NULL, strlen(temp_ptr) + 10);
					memcpy(file_sys_dir_ptr, &env[ctr3][10], 2);
					memcpy(&file_sys_dir_ptr[2], "\\OS2\\APPS", 9);
	
					if(iTrace)
						printf("\nfile_sys_dir_ptr: %s\n", file_sys_dir_ptr);
				}
			}
		}

		ctr3++;
	}
	while(env[ctr3] != NULL);

	// Suggestion by BRN, in case nothing is set
	if(file_sys_dir_ptr == NULL)
	{
		file_sys_dir_ptr = (char *) my_malloc("file_sys_dir_ptr", 5);
		memset(file_sys_dir_ptr, (int) NULL, 5);
		//put the config file in the current directory
		strcpy (file_sys_dir_ptr, ".");
	}

	check_for_config();

	for(ctr2 = 0; ctr2 < ctr1; ctr2++)
		argv++;

	while(filename = *++argv)
	{
		if(iTrace)
			printf("\nfilename: %s---->", filename);

		if(stat(filename, &buf) == (-1))
		{
			perror(filename);
		}
		else
		{
			lFileSize = buf.st_size;

			if(iThumbnail || iThumbnailOld)
				ProcessFileJPG(argc, filename);
			else
				ProcessFile(argc, filename);

			ulGlobalOrientation = 0l;
			ulGlobalPosition = 0l;

			if(iTrace)
				printf("\nulGlobalPosition: %x", ulGlobalPosition);
		}
	}

	if(iJS)
		printf("var intExifDataArrayCount=%d\n", (js_ctr1 - 1));

	if(iWeb)
	{
		if(gallery_ofd != NULL)
		{
			fprintf(gallery_ofd, "var intExifDataArrayCount=%d\n", (iWeb_Image_count - 1));
			fclose(gallery_ofd);

			fprintf(root_ofd, "var intExifDataArrayCount=%d\n", (iRoot_Web_Image_count - 1));
			fclose(root_ofd);

			if(str_root_temp_dir != NULL)
			{
				my_rmdir(str_root_temp_dir);
				my_free("str_root_temp_dir", str_root_temp_dir);
			}
		}
	}

	if(file_sys_dir_ptr != NULL)
		my_free("file_sys_dir_ptr", file_sys_dir_ptr);

	if(iMemTrace)
	{
		printf("\nmalloc_count: %d", malloc_count);
		printf("\nfree_count: %d", free_count);
	}

	return(0);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void ProcessFile(int argc, char *filename)
{
    FILE *rfd = NULL;
	struct xmp_data *xmp_data_ptr = NULL;
	struct xmp_data *start_xmp_data_ptr = NULL;
	struct xmp_data *free_xmp_data_ptr = NULL;
	struct xmp_packet *xmp_packet_ptr = NULL;
	char buffer[256];
	char *temp_ptr = NULL;
	char *file_name_ptr = NULL;
	unsigned long offset_ifd = 0l;
	long old_position = 0l;
	int ctr1 = 0;
	struct data_header_ptr *temp_header_start_ptr = NULL;
	struct data_header_ptr *free_header_start_ptr = NULL;
	struct html_header *free_html_header_start_ptr = NULL;

	trace("\n+ProcessFile");
	mtrace("\n+ProcessFile");

	temp_ptr = get_extension(filename);

	memset(trace_buffer, (int) NULL, 256);
	sprintf(trace_buffer, "\nFile Extension: %s", temp_ptr);
	trace(trace_buffer);

	if(!strncmp(upper_case(temp_ptr), "JPG", 3))
	{
		iFile_Type = 0;
	}
	else
	{
		if(!strncmp(upper_case(temp_ptr), "NEF", 3))
		{
			iJPG_Type = 99;
			iFile_Type = 1;
		}
		else
		{
			if((!iHtml) && (!iXml) && (!iCsv) && (!iJS))
				printf("\nFile not readable: %s\n\n", filename);

			return;
		}
	}

	if((rfd = fopen(filename, "rb")) == NULL)
	{
		printf("\nFile not opened: %s\n\n", filename);
		return;
	}

	if(iRemoveEXIF)
	{
		if(!iFile_Type)
			remove_exif(filename, rfd);
		else
			printf("\nCan't process NEF files\n\n");

		return;
	}

	if((!iHtml) && (!iXml) && (!iCsv) && (!iJS))
	{
		printf("\nD70ReaderProgramVersion    : %s", D70_VERSION);
		printf("\nFilename                   : %s", filename);
	}

	if(!iFile_Type)
	{
		if(!check_for_exif_data(rfd))
		{
			if((!iHtml) && (!iXml) && (!iCsv) && (!iJS))
				printf("\nNo EXIF data found\n");

			trace("\n-ProcessFile");
			mtrace("\n-ProcessFile");
			return;
		}
	}

	if((iHtml) || (iXml) || (iCsv) || (iJS) || (iWeb))
	{
		if(iWeb)
		{
			if(iFile_Type)
			{
				web_file_name_src_ptr = strdup(filename);

				file_name_ptr = (char *) my_malloc("file_name_ptr", (strlen(filename) + strlen(".JPG") + 1));
				memset(file_name_ptr, (int) NULL, (strlen(filename) + strlen(".JPG") + 1));
				sprintf(file_name_ptr, "%s.JPG", filename);

				web_file_name_tmp_ptr = strdup(file_name_ptr);
			}
			else
			{
				web_file_name_tmp_ptr = strdup(filename);

				file_name_ptr = (char *) my_malloc("file_name_ptr", (strlen(filename) + 1));
				memset(file_name_ptr, (int) NULL, (strlen(filename) + 1));
				sprintf(file_name_ptr, "%s", filename);
			}
		}
		else
		{
			file_name_ptr = (char *) my_malloc("file_name_ptr", (strlen(filename) + 1));
			memset(file_name_ptr, (int) NULL, (strlen(filename) + 1));
			sprintf(file_name_ptr, "%s", filename);
		}

		if(iTrace)
		{
			debug_printf("%x %s %s", 0xFFFD, "D70ReaderProgramVersion    ", D70_VERSION);
			debug_printf("%x %s %s", 0xFFFE, "Filename                   ", file_name_ptr);
		}
		else
		{
			my_printf("%s %s", 0xFFFD, "D70ReaderProgramVersion    ", D70_VERSION);
			my_printf("%s %s", 0xFFFE, "Filename                   ", file_name_ptr);
		}

	 	my_free("file_name_ptr", file_name_ptr);
	}

	memset(buffer, (int) NULL, 256);
	sprintf(buffer, "IFD%d", ctr1++);

	offset_ifd = inspect_image_file_header(rfd, iFile_Type);

	if(dump_image_file_directory(rfd, offset_ifd, jpg_type_offset(), get_new_data_header_ptr(), buffer))
	{
		free_html_header_start_ptr = html_header_start_ptr;

		free_memory(free_header_start_ptr, free_html_header_start_ptr);
		return;
	}

	old_position = ftell(rfd);

	fread(&offset_ifd, sizeof(unsigned long), 1, rfd);
	offset_ifd = Get32u(&offset_ifd);

#ifdef XMP_SDK
	if(jpg_type() == 0)
	{
		if((xmp_packet_ptr = chk_get_xmp_data(rfd)) != NULL)
		{
			xmp_data_ptr = xmp_display(filename, xmp_packet_ptr->length, xmp_packet_ptr->value, iVerbos);

			start_xmp_data_ptr = xmp_data_ptr;
			free_xmp_data_ptr = xmp_data_ptr;

			do
			{
				if(iTrace)
					printf("\n%d - \"%s\" - \"%s\"", xmp_data_ptr->tag, xmp_data_ptr->description, xmp_data_ptr->value);

				xmp_data_ptr = xmp_data_ptr->next;
			}
			while(xmp_data_ptr != NULL); 
		}
	}
#endif

	temp_header_start_ptr = main_header_start_ptr;
	free_header_start_ptr = main_header_start_ptr;

	if(iTrace || iVerbos)
	{
		printf("\n--------------START OF RAW DATA---------------\n");

		ctr1 = 0;

		while(main_header_start_ptr->next != NULL)
		{
			header_ptr = main_header_start_ptr->header_ptr;

			while(header_ptr->next != NULL)
			{
				if(header_ptr->Tag != 0)
					printf("\n(%d) - %s - FMT: %d - NUM: %ld - TAG: %#x: %s: %s", ctr1++, main_header_start_ptr->data_type, header_ptr->field_type, header_ptr->count, header_ptr->Tag, header_ptr->Desc, header_ptr->Value);
	
				header_ptr = header_ptr->next;
			}

			main_header_start_ptr = main_header_start_ptr->next;
		}

		header_ptr = main_header_start_ptr->header_ptr;

		//special case for XMP data
		if(header_ptr->Tag == 0xFFFF)
			printf("\n(%d) - %s - FMT: %d - NUM: %ld - TAG: %#x: %s: %s", ctr1++, main_header_start_ptr->data_type, header_ptr->field_type, header_ptr->count, header_ptr->Tag, header_ptr->Desc, header_ptr->Value);

		while(header_ptr->next != NULL)
		{
			if(header_ptr->Tag != 0)
					printf("\n(%d) - %s - FMT: %d - NUM: %ld - TAG: %#x: %s: %s", ctr1++, main_header_start_ptr->data_type, header_ptr->field_type, header_ptr->count, header_ptr->Tag, header_ptr->Desc, header_ptr->Value);

			header_ptr = header_ptr->next;
		}

		printf("\n--------------END OF RAW DATA---------------\n");
	}

	GlobalOrientationPtr = (char *) my_malloc("GlobalOrientationPtr", (strlen("xxx") + 1));
	memset(GlobalOrientationPtr, (int) NULL, (strlen("xxx") + 1));
	sprintf(GlobalOrientationPtr, "%s", "X");

	if(iTrace)
		printf("\n--------------START NEW DATA DISPLAY---------------\n");

	ctr1 = 0;

	while(temp_header_start_ptr->next != NULL)
	{
		display_data(temp_header_start_ptr->header_ptr, temp_header_start_ptr->data_type);

		temp_header_start_ptr = temp_header_start_ptr->next;
	}

	display_data(temp_header_start_ptr->header_ptr, temp_header_start_ptr->data_type);

	if(iTrace)
		printf("\n\n--------------END NEW DATA DISPLAY---------------\n");

#ifdef XMP_SDK
	if(jpg_type() == 0)
	{
		if(start_xmp_data_ptr != NULL)
		{
			do
			{
				if(iTrace)
					printf("\n%x - \"%s\" - \"%s\"", start_xmp_data_ptr->tag, get_description_label(start_xmp_data_ptr->tag, 0), start_xmp_data_ptr->value);

				if(iTrace)
					debug_printf("\n%#x: %s: %s seconds", start_xmp_data_ptr->tag, get_description_label(start_xmp_data_ptr->tag, 0), start_xmp_data_ptr->value);
				else
					my_printf("\n%s: %s", start_xmp_data_ptr->tag, get_description_label(start_xmp_data_ptr->tag, 0), start_xmp_data_ptr->value);

				start_xmp_data_ptr = start_xmp_data_ptr->next;
			}
			while(start_xmp_data_ptr != NULL); 
		}
	}
#endif

	free_html_header_start_ptr = html_header_start_ptr;

	if(iWeb)
		get_sof0_data(rfd);

	fclose(rfd);

	if(iHtml)
		output_html();

	if(iXml)
		output_xml();

	if(iCsv)
		output_csv();

	if(iJS)
	{
		if(iJS_First_Time)
		{
			iJS_First_Time = 0;
			output_js(stdout, 1, js_ctr1++);
		}
		else
			output_js(stdout, 0, js_ctr1++);
	}

	if(ALLOW_ROTATE)
	{
		if(iWeb)
			output_web();
	}

	if((!iHtml) && (!iXml) && (!iCsv) && (!iJS) && (!iWeb))
		printf("\n\n");

#ifdef XMP_SDK
	if(jpg_type() == 0)
		if(free_xmp_data_ptr != NULL)
			free_xmp_data_header_list(free_xmp_data_ptr);
#endif

	free_memory(free_header_start_ptr, free_html_header_start_ptr);

	trace("\n-ProcessFile");
	mtrace("\n-ProcessFile");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void output_xml()
{
	struct html_header *local_header_start_ptr = NULL;

	trace("\n+output_xml");
	//mtrace("\n+output_xml");

	local_header_start_ptr = html_header_start_ptr;

	printf("\n\n<?xml version=\"1.0\" standalone=\"yes\"?>\n");
	printf("<data>\n");

	while(local_header_start_ptr->next != NULL)
	{
		printf("<%s>%s</%s>\n", lower_case(strip(local_header_start_ptr->Desc)), local_header_start_ptr->Value, lower_case(strip(local_header_start_ptr->Desc)));

		local_header_start_ptr = local_header_start_ptr->next;
	}

	printf("<%s>%s</%s>\n", lower_case(strip(local_header_start_ptr->Desc)), local_header_start_ptr->Value, lower_case(strip(local_header_start_ptr->Desc)));
	printf("</data>\n");

	trace("\n-output_xml");
	//mtrace("\n-output_xml");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void output_csv()
{
	int ctr1 = 0;
	struct all_wanted_tags *local_header_all_ptr = NULL;
	struct html_header *local_header_start_ptr = NULL;

	trace("\n+output_csv");
	//mtrace("\n+output_csv");

	if(iCsv_First_Time)
	{
		printf("\"D70ReaderProgramVersion\",\"Filename\"");

		local_header_all_ptr = start_all_wanted_tags_ptr;

		do
		{
			if(is_tag_printed(local_header_all_ptr->tag))
			{
				if(iTrace)
					printf("\n%d - %x - \"%s\",", ctr1++, local_header_all_ptr->tag, get_description_label(local_header_all_ptr->tag, 1));

				printf(",\"%s\"", get_description_label(local_header_all_ptr->tag, 1));
			}

			local_header_all_ptr = local_header_all_ptr->next;
		}
		while(local_header_all_ptr != NULL); 

		printf("\n");

		iCsv_First_Time = 0;
	}

	printf("\"%s\",\"%s\"", D70_VERSION, get_description_value(0xFFFE));

	local_header_all_ptr = start_all_wanted_tags_ptr;

	do
	{
		if(is_tag_printed(local_header_all_ptr->tag))
		{
			if(iTrace)
				printf("\n%d - %x - \"%s\",", ctr1++, local_header_all_ptr->tag, get_description_value(local_header_all_ptr->tag));

			printf(",\"%s\"", get_description_value(local_header_all_ptr->tag));
		}

		local_header_all_ptr = local_header_all_ptr->next;
	}
	while(local_header_all_ptr != NULL); 

	printf("\n");

	trace("\n-output_csv");
	//mtrace("\n-output_csv");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
char *strip(char *str_input)
{
	char *temp_ptr = NULL;
	char *token = NULL;

	//trace("\n+strip");
	//mtrace("\n+strip");

	temp_ptr = strdup(str_input);

	token = strtok(temp_ptr, " ");

	//trace("\n-strip");
	//mtrace("\n-strip");
	return(token);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void output_html()
{
	struct html_header *local_header_start_ptr = NULL;

	trace("\n+output_html");
	//mtrace("\n+output_html");

	local_header_start_ptr = html_header_start_ptr;

	printf("\n\n<table border=\"1\" width=\"450\" cellpadding=\"1\" align=\"center\">\n");

	while(local_header_start_ptr->next != NULL)
	{
		printf("<tr><td>%s</td><td>%s</td></tr>\n", local_header_start_ptr->Desc, local_header_start_ptr->Value);

		local_header_start_ptr = local_header_start_ptr->next;
	}

	printf("<tr><td>%s</td><td>%s</td></tr>\n", local_header_start_ptr->Desc, local_header_start_ptr->Value);

	printf("</table>\n");

	trace("\n-output_html");
	//mtrace("\n-output_html");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void free_memory(struct data_header_ptr *free_header_start_ptr, struct html_header *free_html_header_start_ptr)
{
	struct data_header_ptr *temp_header_start_ptr = NULL;
	struct data_header_ptr *tmp2_header_start_ptr = NULL;

	trace("\n+free_memory");
	mtrace("\n+free_memory");

	temp_header_start_ptr = free_header_start_ptr;

	if(temp_header_start_ptr != NULL)
	{
		while(temp_header_start_ptr->next != NULL)
		{
			tmp2_header_start_ptr = temp_header_start_ptr;
			free_data_header_list(temp_header_start_ptr->header_ptr);
			temp_header_start_ptr = temp_header_start_ptr->next;
			my_free("tmp2_header_start_ptr", tmp2_header_start_ptr);
		}

		my_free("temp_header_start_ptr", temp_header_start_ptr);
	}

	main_header_ptr = NULL;
	main_header_start_ptr = NULL;

	free_html_header_list(free_html_header_start_ptr);
	html_header_ptr = NULL;

	trace("\n-free_memory");
	mtrace("\n-free_memory");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void free_data_header_list(struct data_header *header_ptr)
{
	struct data_header *temp_header_ptr = NULL;

	trace("\n+free_data_header_list");
	mtrace("\n+free_data_header_list");

	while(header_ptr->next != NULL)
	{
		temp_header_ptr = header_ptr;
		header_ptr = header_ptr->next;

		if(temp_header_ptr != NULL)
			my_free("temp_header_ptr", temp_header_ptr);
	}

	if(header_ptr != NULL)
		my_free("header_ptr", header_ptr);

	trace("\n-free_data_header_list");
	mtrace("\n-free_data_header_list");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void free_html_header_list(struct html_header *header_ptr)
{
	struct html_header *temp_header_ptr = NULL;

	trace("\n+free_html_header_list");
	mtrace("\n+free_html_header_list");

	if(header_ptr != NULL)
	{
		while(header_ptr->next != NULL)
		{
			temp_header_ptr = header_ptr;
			header_ptr = header_ptr->next;
			my_free("temp_header_ptr", temp_header_ptr);
		}

		my_free("header_ptr", header_ptr);
	}

	trace("\n-free_html_header_list");
	mtrace("\n-free_html_header_list");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
char *upper_case(char *local_ptr)
{
	int ctr1 = 0;
	int length = 0;
	char *temp_ptr = NULL;
	char *final_ptr = NULL;

	//trace("\n+upper_case");
	//mtrace("\n+upper_case");

	temp_ptr = strdup(local_ptr);
	final_ptr = temp_ptr;

	length = strlen(temp_ptr);

	for(ctr1 = 0; ctr1 < length; ctr1++)
	{
		*temp_ptr = toupper(*temp_ptr);
		temp_ptr++;
	}

	//trace("\n-upper_case");
	//mtrace("\n-upper_case");

	return(final_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
char *lower_case(char *local_ptr)
{
	int ctr1 = 0;
	int length = 0;
	char *temp_ptr = NULL;
	char *final_ptr = NULL;

	//trace("\n+lower_case");
	//mtrace("\n+lower_case");

	temp_ptr = strdup(local_ptr);
	final_ptr = temp_ptr;

	length = strlen(temp_ptr);

	for(ctr1 = 0; ctr1 < length; ctr1++)
	{
		*temp_ptr = tolower(*temp_ptr);
		temp_ptr++;
	}

	//trace("\n-lower_case");
	//mtrace("\n-lower_case");

	return(final_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
char *get_extension(char *string)
{
	char *temp_ptr = NULL;
	char *final_ptr = NULL;
	char *token = NULL;

	trace("\n+get_extension");
	//mtrace("\n+get_extension");

	temp_ptr = strdup(string);

	token = strtok(temp_ptr, ".");

	do
	{
		if(token != NULL)
			final_ptr = token;

		token = strtok(NULL, ".");
	}
	while(token != NULL);

	trace("\n-get_extension");
	//mtrace("\n-get_extension");
	return(final_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void get_xmp_data(FILE *rfd, struct data_header *local_ptr)
{
	int ctr1 = 0;
	int iFoundHTTP = 0;
	unsigned char *data_ptr = NULL;

	unsigned short length;
	long position;
	int oldMotorolaOrder;

	struct marker
	{
		unsigned char pad;
		unsigned char marker;
		unsigned short length;
		char test_data[6];
	};

	struct marker *temp_ptr;

	oldMotorolaOrder = MotorolaOrder;
	MotorolaOrder = 1;

	temp_ptr = (struct marker *) my_malloc("temp_ptr", sizeof(struct marker));
	memset(temp_ptr, (int) NULL, sizeof(struct marker));

	fseek(rfd, 2, 0);

	do
	{
		ctr1++;
		position = ftell(rfd);

		memset(temp_ptr, (int) NULL, sizeof(struct marker));
		fread(temp_ptr, sizeof(struct marker), 1, rfd);

		length = Get16u(&temp_ptr->length);

		if(!strncmp(temp_ptr->test_data, "http:", strlen("http:")))
		{
			iFoundHTTP = 1;
			break;
		}
		else
			fseek(rfd, Get16u(&temp_ptr->length) - sizeof(struct marker) + 2, 1);

		if(temp_ptr->marker == 0xda)
			break;
	}
	while(strncmp(temp_ptr->test_data, "http:", strlen("http:")));	

	if(iFoundHTTP)
	{
		data_ptr = (unsigned char *) my_malloc("data_ptr", (Get16u(&temp_ptr->length) + 1));
		memset(data_ptr, (int) NULL, (Get16u(&temp_ptr->length) + 1));

		fseek(rfd, position + 4, 0);
		fread(data_ptr, Get16u(&temp_ptr->length), 1, rfd);

		while(*data_ptr != (unsigned char) NULL)
			data_ptr++;

		data_ptr++;

		local_ptr->Tag = 0xFFFF;
		local_ptr->field_type = 2;
		local_ptr->count = Get16u(&temp_ptr->length);
		local_ptr->Desc = "XMP Data";
		local_ptr->Value = data_ptr;
		
		main_header_ptr->next = get_new_main_header_ptr();
		main_header_ptr->prev =  main_header_ptr;
		main_header_ptr =  main_header_ptr->next;
		main_header_ptr->header_ptr = local_ptr;
		main_header_ptr->data_type = (char *) my_malloc("main_header_ptr->data_type", (strlen("XMP") + 1));
		memset(main_header_ptr->data_type, (int) NULL, (strlen("XMP") + 1));
		strncpy(main_header_ptr->data_type, "XMP", strlen("XMP"));
	}

	MotorolaOrder = oldMotorolaOrder;

	my_free("temp_ptr", temp_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
unsigned char * pad_field(unsigned char *str_ptr, int length, char pad_char, int iPrePend)
{
	int ctr1 = 0;
	unsigned char *temp_ptr = NULL;
	unsigned char buffer[2];

	trace("\n+pad_field");
	mtrace("\n+pad_field");

	memset(buffer, (int) NULL, 2);
	buffer[0] = pad_char;

	temp_ptr = (unsigned char *) my_malloc("temp_ptr", (strlen(str_ptr) + length + 1));
	memset(temp_ptr, (int) NULL, (strlen(str_ptr) + length + 1));

	if(iPrePend)
	{
		for(ctr1 = 0; ctr1 < (length - (int) strlen(str_ptr)); ctr1++)
		{
			strcat(temp_ptr, buffer);
		}

		strcat(temp_ptr, str_ptr);
	}
	else
	{
		strcat(temp_ptr, str_ptr);

		for(ctr1 = 0; ctr1 < (length - (int) strlen(str_ptr)); ctr1++)
		{
			strcat(temp_ptr, buffer);
		}
	}

	trace("\n-pad_field");
	mtrace("\n-pad_field");

	return temp_ptr;
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void print_8bit_hex(unsigned char *byte_ptr, int ByteCount, struct data_header *local_ptr)
{
	int ctr1 = 0;
	char buffer[256];

	trace("\n+print_8bit_hex");
	mtrace("\n+print_8bit_hex");

	assert(ByteCount < 256);

	memset(buffer, (int) NULL, 256);

	for(ctr1 = 0; ctr1 < ByteCount; ctr1++)
	{
		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "%x ", *byte_ptr);
		strcat(buffer, trace_buffer);
		byte_ptr++;
	}

	memset(trace_buffer, (int) NULL, 256);
	sprintf(trace_buffer, "%s ", buffer);
	trace_formatted(trace_buffer);

	local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(trace_buffer) + 1));
	memset(local_ptr->Value, (int) NULL, (strlen(trace_buffer) + 1));
	strcpy(local_ptr->Value, trace_buffer);

	trace("\n-print_8bit_hex");
	mtrace("\n-print_8bit_hex");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void print_8bit_bytes(unsigned char *byte_ptr, int ByteCount, struct data_header *local_ptr)
{
	trace("\n+print_8bit_bytes");
	mtrace("\n+print_8bit_bytes");

	if(iTrace)
		printf("byte_ptr: %s\n", byte_ptr);

	local_ptr->Value = (char *) my_malloc("local_ptr->Value", (ByteCount + 1));
	memset(local_ptr->Value, (int) NULL, (ByteCount + 1));
	memcpy(local_ptr->Value, byte_ptr, ByteCount);

	trace("\n-print_8bit_bytes");
	mtrace("\n-print_8bit_bytes");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void print_16bit_hex(unsigned char *byte_ptr, int components, int ByteCount, struct data_header *local_ptr)
{
	int ctr1 = 0;
	char * temp_1_ptr = NULL;
	char * temp_2_ptr = NULL;
	unsigned char *buffer_ptr = NULL;

	trace("\n+print_16bit_hex");
	mtrace("\n+print_16bit_hex");

	temp_1_ptr = strdup(byte_ptr);

	buffer_ptr = (unsigned char*) my_malloc("buffer_ptr", ((2 * components * ByteCount) + 1));
	memset(buffer_ptr, (int) NULL, ((2 * components * ByteCount) + 1));
	//memset(buffer_ptr, (int) ' ', (2 * components * ByteCount));
	temp_2_ptr = buffer_ptr;

	for(ctr1 = 0; ctr1 < components; ctr1++)
	{
		strncpy(temp_2_ptr, temp_1_ptr, ByteCount);
		strcat(temp_2_ptr, " ");

		temp_1_ptr += ByteCount;
		temp_2_ptr += (ByteCount + 1);
	}

	trace_formatted(buffer_ptr);

	local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(buffer_ptr) + 1));
	memset(local_ptr->Value, (int) NULL, (strlen(buffer_ptr) + 1));
	strcpy(local_ptr->Value, buffer_ptr);

	my_free("buffer_ptr", buffer_ptr);

	trace("\n-print_16bit_hex");
	mtrace("\n-print_16bit_hex");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void print_16bit_bytes(unsigned char *byte_ptr, int components, int ByteCount, struct data_header *local_ptr)
{
	int ctr1 = 0;
	char * temp_ptr = NULL;
	char buffer[256];

	trace("\n+print_16bit_bytes");
	mtrace("\n+print_16bit_bytes");

	assert(components < 256);

	temp_ptr = strdup(byte_ptr);

	memset(buffer, (int) NULL, 256);

	for(ctr1 = 0; ctr1 < components; ctr1++)
	{
		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "%s ", temp_ptr);
		strcat(buffer, trace_buffer);

		temp_ptr += 2;
	}

	memset(trace_buffer, (int) NULL, 256);
	sprintf(trace_buffer, "%s ", buffer); 

	local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(trace_buffer) + 1));
	memset(local_ptr->Value, (int) NULL, (strlen(trace_buffer) + 1));
	strcpy(local_ptr->Value, trace_buffer);

	trace("\n-print_16bit_bytes");
	mtrace("\n-print_16bit_bytes");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void  instruct()                                /*give instructions for use     */
{
    printf("\nD70Reader - Version %s - %s\n", D70_VERSION, D70_DATE); 
    printf("\nThis is a Freeware program, use at your own risk.\n");

	printf("\nThis software makes use of \"The Independent JPEG Group's JPEG Software\"");
	printf("\nSpecifically, it uses their image rotation and resizing logic.");
	printf("\nPlease read their copyright information, found in the jpeglib/README file.\n");

	printf("\nThis software makes use of \"The Adobe XMP SDK Software\"");
	printf("\nPlease read their copyright information, found in the ");
	printf("\nXMP_SDK/Documentation/LICENSE.TXT file.\n");

    printf("\nThis program only works with Nikon D70/D100/D2H/D2X image files.");
    printf("\n(It will work with NEF/RAW and JPG files.)\n");

    printf("\nCheck here for latest version:");
    printf("\nhttp://www.gogebic-pc.com/d70/index.html\n");

    printf("\nFormat:");
    printf("\n\td70reader [-h|-v|-w|-x|-j|-rm|-c|-js|-web] image_file_name\n");
    printf("\n\td70reader [-h|-v|-w|-x|-j|-rm|-c|-js|-web] *.NEF *.JPG\n");
    printf("\n\t-h    = this screen");
    printf("\n\t-v    = verbose (raw data)");
    printf("\n\t-w    = generate HTML table");
    printf("\n\t-x    = generate XML tree");
    printf("\n\t-j    = extract JPG thumbnail image from NEF");
    printf("\n\t-jold = extract JPG thumbnail image from NEF (old)");
    printf("\n\t-rm   = remove all EXIF data from JPG file");
    printf("\n\t-c    = generate Comma Separated Values (CSV) data");
    printf("\n\t-js   = generate JavaScript Array data");
    printf("\n\t-web  = generate Photo Website\n\n");

/*
	printf("Press <Enter> to Continue\n\n");
	getchar();

	printf("I am trying to make sense out of the following EXIF tags:\n\n");

	printf("AF-S DX Zoom-Nikkor ED G IF:\n\n");
	printf("LensType : 6\n");
	printf("Lens : 18-70mm f/3.5-f/4.5\n");
	printf("LensInfo : 40 01 0c 00\n\n");

	printf("AF Zoom-Nikkor D:\n\n");
	printf("LensType : 2\n");
	printf("Lens : 70-300mm f/4.0-f/5.6\n");
	printf("LensInfo : 48 01 0c 00\n\n");

	printf("AF Nikkor D:\n\n");
	printf("LensType : 2\n");
	printf("Lens : 50mm f/1.8\n");
	printf("LensInfo : 58 01 0c 00\n\n");

	printf("AF Micro-Nikkor D:\n\n");
	printf("LensType : 2\n");
	printf("Lens : 60mm f/2.8\n");
	printf("LensInfo : 54 01 0c 00 \n\n");

	printf("I have identified the tag information for the above four lenses.\n");
	printf("If you would like to help me decode the lens information into\n");
	printf("something that makes sense (like my above lens descriptions),\n");
	printf("please send me the above three EXIF data tags for your lenses,\n");
	printf("to the following email address:\n\n");
*/
	printf("joman@gogebic-pc.com\n\n");
}

//--------------------------------------------------------------------------
// Convert a 16 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
static void Put16u(void * Short, unsigned short PutValue)
{
	if (MotorolaOrder)
	{
		((uchar *)Short)[0] = (uchar)(PutValue>>8);
		((uchar *)Short)[1] = (uchar)PutValue;
	}
	else
	{
		((uchar *)Short)[0] = (uchar)PutValue;
		((uchar *)Short)[1] = (uchar)(PutValue>>8);
	}
}

//--------------------------------------------------------------------------
// Convert a 16 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
int Get16u(void * Short)
{
	if (MotorolaOrder)
	{
		return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
	}
	else
	{
		return (((uchar *)Short)[1] << 8) | ((uchar *)Short)[0];
	}
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
unsigned long rotate_right(unsigned long Long)
{

	unsigned long mByte0 = 0l;

	mByte0 = Long >> 16;

	return(mByte0);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
//static int Get32s(void * Long)
int Get32s(void * Long)
{
	if (MotorolaOrder)
	{
		return  ((( char *)Long)[0] << 24) | (((uchar *)Long)[1] << 16)
			| (((uchar *)Long)[2] << 8 ) | (((uchar *)Long)[3] << 0 );
	}
	else
	{
		return  ((( char *)Long)[3] << 24) | (((uchar *)Long)[2] << 16)
			| (((uchar *)Long)[1] << 8 ) | (((uchar *)Long)[0] << 0 );
	}
}

//--------------------------------------------------------------------------
// Convert a 32 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
unsigned Get32u(void * Long)
{
	return (unsigned)Get32s(Long) & 0xffffffff;
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int dump_image_file_directory(FILE *rfd, unsigned long offset_ifd, unsigned long offset_base, struct data_header *local_ptr, char *type_ptr)
{
	char *buff_ptr = NULL;
	char buffer[256];
	char type_buffer[256];
	struct image_file_directory_entry *ifde_ptr = NULL;
	unsigned short count = 0;
	unsigned short num_ifd = 0;
	int a = 0;
	unsigned long ctr1 = 0l;
	unsigned long ctr2 = 0l;
	unsigned long ctr3 = 0l;
	long old_position = 0l;
	long temp_position = 0l;
	int ByteCount = 0;
	unsigned long components = 0l;      		//count
	unsigned long value_offset = 0l;     	//value offset
	unsigned char *byte_ptr = NULL;
	unsigned short *short_byte_ptr;
	unsigned long *long_byte_ptr = 0l;

	unsigned long numirator = 0l;
	unsigned long denominator = 0l;
	unsigned long numirator_cnvtd = 0l;
	unsigned long denominator_cnvtd = 0l;

	long snumirator = 0l;
	long sdenominator = 0l;
	long snumirator_cnvtd = 0l;
	long sdenominator_cnvtd = 0l;

	double num = 0.0;
	double den = 0.0;

	unsigned long maker_note_header_offset = 0l;
	unsigned long ulLocalPosition = 0l;
	struct html_header *local_header_start_ptr = NULL;

	trace("\n+dump_image_file_directory");
	mtrace("\n+dump_image_file_directory");

	ifde_ptr = (struct image_file_directory_entry *) my_malloc("ifde_ptr", sizeof(struct image_file_directory_entry));
	memset(ifde_ptr, (int) NULL, sizeof(struct image_file_directory_entry));

	if(fseek(rfd, offset_ifd, 0) != -1)
	{
		fread(&count, sizeof(unsigned short), 1, rfd);
		num_ifd = Get16u(&count);

		//assert(num_ifd != 0x0000);
		if(num_ifd == 0x0000)
		{
			if(!iFile_Type)
			{
				if((!iHtml) && (!iXml) && (!iCsv) && (!iJS))
				{
					printf("\n\nIf the above file was created with the -j option,");
					printf("\ntry using the -jold option to re-create this JPG.\n");
					printf("\nIf this file was not created with the -j option, ");
					printf("\nthe EXIF data is corrupted.\n\n");
				}
			}
			else
			{
				if((!iHtml) && (!iXml) && (!iCsv) && (!iJS))
					printf("\nThe EXIF data for this NEF/RAW file is corrupted.\n\n");
			}

			if(iTrace)
			{
				local_header_start_ptr = html_header_start_ptr;

				do
				{
					printf("\nDesc: %s - Value: %s", local_header_start_ptr->Desc, local_header_start_ptr->Value);

					local_header_start_ptr = local_header_start_ptr->next;
				}
				while(local_header_start_ptr != NULL);
			}

			return(1);
		}

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nNumber of Image File Directories: %d", num_ifd);
		trace(trace_buffer);
	}
	else
	{
		my_error(rfd);
	}

	if(main_header_ptr == NULL)
	{
		main_header_ptr = get_new_main_header_ptr();
		main_header_ptr->header_ptr = local_ptr;
		main_header_ptr->data_type = (char *) my_malloc("main_header_ptr->data_type", (strlen(type_ptr) + 1));
		memset(main_header_ptr->data_type, (int) NULL, (strlen(type_ptr) + 1));
		strncpy(main_header_ptr->data_type, type_ptr, strlen(type_ptr));
		main_header_start_ptr = main_header_ptr;
	}
	else
	{
		main_header_ptr->next = get_new_main_header_ptr();
		main_header_ptr->prev =  main_header_ptr;
		main_header_ptr =  main_header_ptr->next;
		main_header_ptr->header_ptr = local_ptr;
		main_header_ptr->data_type = (char *) my_malloc("main_header_ptr->data_type", (strlen(type_ptr) + 1));
		memset(main_header_ptr->data_type, (int) NULL, (strlen(type_ptr) + 1));
		strncpy(main_header_ptr->data_type, type_ptr, strlen(type_ptr));
	}

	for(ctr1 = 0; ctr1 < num_ifd; ctr1++)
	{
		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\n\nctr1: %d", ctr1);
		trace(trace_buffer);

		memset(ifde_ptr, (int) NULL, sizeof(struct image_file_directory_entry));
		fread(ifde_ptr, sizeof(struct image_file_directory_entry), 1, rfd);

		ulLocalPosition = ftell(rfd);

		if(ulLocalPosition > ulGlobalPosition)
			ulGlobalPosition = ulLocalPosition;

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nTag: %x", Get16u(&ifde_ptr->tag));
		trace(trace_buffer);

		local_ptr->Tag = Get16u(&ifde_ptr->tag);
		local_ptr->field_type = Get16u(&ifde_ptr->field_type);
		local_ptr->count = Get32u(&ifde_ptr->count);

		for (a = 0; ;a++)
		{
			if (a >= SizeTagTable )
			{
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "    Unknown Tag %04x Value = ", Get16u(&ifde_ptr->tag));
				trace(trace_buffer);

				local_ptr->Desc = (char *) my_malloc("local_ptr->Desc", (strlen(trace_buffer) + 1));
				memset(local_ptr->Desc, (int) NULL, (strlen(trace_buffer) + 1));
				strcpy(local_ptr->Desc, trace_buffer);
				break;
			}

			if (TagTable[a].Tag == Get16u(&ifde_ptr->tag))
			{
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "%s",TagTable[a].Desc); 

				local_ptr->Desc = (char *) my_malloc("local_ptr->Desc", (strlen(trace_buffer) + 1));
				memset(local_ptr->Desc, (int) NULL, (strlen(trace_buffer) + 1));
				strcpy(local_ptr->Desc, trace_buffer);

				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\n%s",TagTable[a].Desc); 
				trace(trace_buffer);
				break;
			}
		}

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nField Type: %x", Get16u(&ifde_ptr->field_type));
		trace(trace_buffer);

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nNumber of Values: %x", Get32u(&ifde_ptr->count));
		trace(trace_buffer);

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nValue Offset: %x", Get32u(&ifde_ptr->value_offset));
		trace(trace_buffer);

		components = Get32u(&ifde_ptr->count);
		value_offset = Get32u(&ifde_ptr->value_offset) + offset_base;

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\ncomponents: %d", components);
		trace(trace_buffer);

		old_position = ftell(rfd);

		switch(Get16u(&ifde_ptr->field_type))
		{
			case 1:
				ByteCount = components * 1;
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);

				byte_ptr = (unsigned char *) my_malloc("byte_ptr", (ByteCount + 1));
				memset(byte_ptr, (int) NULL, (ByteCount + 1));

				if (ByteCount > 4)
				{
					fseek(rfd, Get32u(&ifde_ptr->value_offset), 0);
					fread(byte_ptr, ByteCount, 1, rfd);
				}
				else
				{
					extract_and_format_data(ifde_ptr, local_ptr, 0);
					break;
				}

				print_8bit_bytes(byte_ptr, ByteCount, local_ptr);
				my_free("byte_ptr", byte_ptr);

				break;
			case 2:
				ByteCount = components * 1;

				if (ByteCount > 4)
				{
					fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);
					
					buff_ptr = (char *) my_malloc("buff_ptr", (components + 1));
					memset(buff_ptr, (int) NULL, (components + 1));

					fread(buff_ptr, components, 1, rfd);
					trace_formatted(buff_ptr);

					local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(buff_ptr) + 1));
					memset(local_ptr->Value, (int) NULL, (strlen(buff_ptr) + 1));
					strcpy(local_ptr->Value, buff_ptr);

					if(Get16u(&ifde_ptr->tag) == 0x10F)
					{
						make_ptr = (char *) my_malloc("make_ptr", (components + 1));
						memset(make_ptr, (int) NULL, (components + 1));
						strcpy(make_ptr, buff_ptr);
					}

					if(Get16u(&ifde_ptr->tag) == 0x110)
					{
						model_ptr = (char *) my_malloc("model_ptr", (components + 1));
						memset(model_ptr, (int) NULL, (components + 1));
						strcpy(model_ptr, buff_ptr);
					}

					if(Get16u(&ifde_ptr->tag) == 0x131)
					{
						software_ptr = (char *) my_malloc("software_ptr", (components + 1));
						memset(software_ptr, (int) NULL, (components + 1));
						strcpy(software_ptr, buff_ptr);
					}

					my_free("buff_ptr", buff_ptr);
				}
				else
				{
					extract_and_format_data(ifde_ptr, local_ptr, 0);
				}
				break;
			case 3:
			case 8:
				ByteCount = components * 2;
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);

				if (ByteCount > 4)
				{
					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nValue Offset: %x", Get32u(&ifde_ptr->value_offset));
					trace(trace_buffer);

					my_fseek(rfd, Get32u(&ifde_ptr->value_offset), 0);
					temp_position = ftell(rfd);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\ntemp_position: %x", temp_position);
					trace(trace_buffer);

					for(ctr3 = 0; ctr3 < components; ctr3++)
					{
						short_byte_ptr = (unsigned short *) my_malloc("short_byte_ptr", sizeof(unsigned short));
						*short_byte_ptr = 0;
						memset(buffer, (int) NULL, 256);
						fread(short_byte_ptr, sizeof(unsigned short), 1, rfd);
						sprintf(buffer, "%-02.2x", Get16u(short_byte_ptr));

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nget16u_short_byte_ptr: %x", Get16u(short_byte_ptr));
						trace(trace_buffer);
	
						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nbuffer: %s", buffer);
						trace(trace_buffer);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nshort_byte_ptr: %x", *short_byte_ptr);
						trace(trace_buffer);

						print_16bit_bytes(buffer, 1, 1, local_ptr);
						my_free("short_byte_ptr", short_byte_ptr);
					}
				}
				else
				{
					extract_and_format_data(ifde_ptr, local_ptr, 0);
				}
				break;
			case 4:
				ByteCount = components * 4;

				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);

				if (ByteCount > 4)
				{
					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nValue Offset: %x", Get32u(&ifde_ptr->value_offset));
					trace(trace_buffer);

					my_fseek(rfd, Get32u(&ifde_ptr->value_offset), 0);

					temp_position = ftell(rfd);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\ntemp_position: %x", temp_position);
					trace(trace_buffer);

					for(ctr3 = 0; ctr3 < components; ctr3++)
					{
						long_byte_ptr = (unsigned long *) my_malloc("long_byte_ptr", sizeof(unsigned long));
						*long_byte_ptr = 0;
						memset(buffer, (int) NULL, 256);
						fread(long_byte_ptr, sizeof(unsigned long), 1, rfd);
						sprintf(buffer, "%-04.4x", Get32u(long_byte_ptr));

						if(Get16u(&ifde_ptr->tag) == TAG_SUBIFD_S)
						{
							trace("\n------Start SubIFD------");

							//save file ptr
							temp_position = ftell(rfd);
		
							memset(trace_buffer, (int) NULL, 256);
							sprintf(trace_buffer, "\ntemp_position: %x", temp_position);
							trace(trace_buffer);

							memset(trace_buffer, (int) NULL, 256);
							sprintf(trace_buffer, "\noffset to count: %x", (Get32u(long_byte_ptr)));
							trace(trace_buffer);

							memset(type_buffer, (int) NULL, 256);
							sprintf(type_buffer, "SUB_IFD%d", ctr3);

							if(dump_image_file_directory(rfd, (Get32u(long_byte_ptr)), 0, get_new_data_header_ptr(), type_buffer))
								return(1);
		
							//restore file ptr
							fseek(rfd, temp_position, 0);

							trace("\n------End SubIFD------");
						}

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nget33u_long_byte_ptr: %x", Get32u(long_byte_ptr));
						trace(trace_buffer);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nbuffer: %s", buffer);
						trace(trace_buffer);
	
						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nlong_byte_ptr: %x", *long_byte_ptr);
						trace(trace_buffer);

						print_16bit_bytes(buffer, 1, 1, local_ptr);
						my_free("long_byte_ptr", long_byte_ptr);
					}
				}
				else
				{
					extract_and_format_data(ifde_ptr, local_ptr, 0);

					if(Get16u(&ifde_ptr->tag) == TAG_THUMBNAIL_IFD)
					{
						if(!strncmp(upper_case(model_ptr), "NIKON D100", strlen("NIKON D100")) ||
						   !strncmp(upper_case(model_ptr), "NIKON D2H", strlen("NIKON D2H")))
						{
							trace("\n------Start TAG_THUMBNAIL_IFD------");
							//save file ptr
							temp_position = ftell(rfd);

							memset(trace_buffer, (int) NULL, 256);
							sprintf(trace_buffer, "\ntemp_position: %x", temp_position);
							trace(trace_buffer);

							memset(type_buffer, (int) NULL, 256);
							sprintf(type_buffer, "THUMBNAIL");

							//call dump_.... with offset
							if(dump_image_file_directory(rfd, Get32u(&ifde_ptr->value_offset) + calc_maker_note_header_offset, calc_maker_note_header_offset, get_new_data_header_ptr(), type_buffer))
								return(1);

							//restore file ptr
							fseek(rfd, temp_position, 0);
							trace("\n------End TAG_THUMBNAIL_IFD------");
						}
					}

					if(Get16u(&ifde_ptr->tag) == TAG_THUMBNAIL_OFFSET)
					{
						lThumbnailOffset = Get32u(&ifde_ptr->value_offset);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nlThumbnailOffset: %ld", lThumbnailOffset);
						trace(trace_buffer);
					}

					if(Get16u(&ifde_ptr->tag) == TAG_THUMBNAIL_LENGTH)
					{
						lThumbnailLength = Get32u(&ifde_ptr->value_offset);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nlThumbnailLength: %ld", lThumbnailLength);
						trace(trace_buffer);
					}

					if(Get16u(&ifde_ptr->tag) == TAG_EXIF_OFFSET)
					{
						trace("\n------Start EXIF------");
						//save file ptr
						temp_position = ftell(rfd);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\ntemp_position: %x", temp_position);
						trace(trace_buffer);

						memset(type_buffer, (int) NULL, 256);
						sprintf(type_buffer, "EXIF");

						//call dump_.... with offset
						if(dump_image_file_directory(rfd, Get32u(&ifde_ptr->value_offset) + jpg_type_offset(), jpg_type_offset(), get_new_data_header_ptr(), type_buffer))
							return(1);

						//restore file ptr
						fseek(rfd, temp_position, 0);
						trace("\n------End EXIF------");
					}
				}
				break;
			case 5:
				ByteCount = components * 8;

				memset(buffer, (int) NULL, 256);

				fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);

				for(ctr2 = 0; ctr2 < components; ctr2++)
				{
					fread(&numirator, sizeof(unsigned long), 1, rfd);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nnumirator: %x", numirator);
					trace(trace_buffer);

					fread(&denominator, sizeof(unsigned long), 1, rfd);
	
					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\ndenominator: %x", denominator);
					trace(trace_buffer);
				
					numirator_cnvtd = Get32s(&numirator);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "%ld,0", numirator_cnvtd);
					num = atof(trace_buffer);
				
					denominator_cnvtd = Get32s(&denominator);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "%ld.0", denominator_cnvtd);
					den = atof(trace_buffer);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "%f ", num/den); 
					trace_formatted(trace_buffer);
					strcat(buffer, trace_buffer);
				}

				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "%s", buffer); 

				local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(trace_buffer) + 1));
				memset(local_ptr->Value, (int) NULL, (strlen(trace_buffer) + 1));
				strcpy(local_ptr->Value, trace_buffer);

				break;
			case 10:
				ByteCount = components * 8;

				memset(buffer, (int) NULL, 256);

				fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);

				for(ctr2 = 0; ctr2 < components; ctr2++)
				{
					fread(&snumirator, sizeof(long), 1, rfd);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nsnumirator: %x", snumirator);
					trace(trace_buffer);

					fread(&sdenominator, sizeof(long), 1, rfd);
	
					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nsdenominator: %x", sdenominator);
					trace(trace_buffer);
				
					snumirator_cnvtd = Get32s(&snumirator);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "%ld,0", snumirator_cnvtd);
					num = atof(trace_buffer);
				
					sdenominator_cnvtd = Get32s(&sdenominator);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "%ld.0", sdenominator_cnvtd);
					den = atof(trace_buffer);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "%f ", num/den); 
					trace_formatted(trace_buffer);
					strcat(buffer, trace_buffer);
				}

				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "%s", buffer); 

				local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(trace_buffer) + 1));
				memset(local_ptr->Value, (int) NULL, (strlen(trace_buffer) + 1));
				strcpy(local_ptr->Value, trace_buffer);

				break;
			case 7:
				if(Get16u(&ifde_ptr->tag) == 0xA300 || 
                                   Get16u(&ifde_ptr->tag) == 0xA301)
				{
					trace("\n------Start FileSource/SceneType------");

					memset(buffer, (int) NULL, 256);
					sprintf(buffer, "%x", Get32u(&ifde_ptr->value_offset)); 

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nvalue_offset: %x", Get32u(&ifde_ptr->value_offset)); 
					trace(trace_buffer);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\ncount: %d", Get32u(&ifde_ptr->count)); 
					trace(trace_buffer);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "%x", Get32u(&ifde_ptr->value_offset)); 
					trace(trace_buffer);

					byte_ptr = &buffer[0];

					print_8bit_bytes(byte_ptr, Get32u(&ifde_ptr->count), local_ptr);
					trace("\n------End FileSource/SceneType------");

					break;
				}

				if(Get16u(&ifde_ptr->tag) == 0x000D ||
                                   Get16u(&ifde_ptr->tag) == 0x000E ||
                                   Get16u(&ifde_ptr->tag) == 0x0012 ||
                                   Get16u(&ifde_ptr->tag) == 0x0017 ||
                                   Get16u(&ifde_ptr->tag) == 0x0018 ||
                                   Get16u(&ifde_ptr->tag) == 0x0088 ||
                                   Get16u(&ifde_ptr->tag) == 0x008B ||
                                   Get16u(&ifde_ptr->tag) == 0x00A4 ||
                                   Get16u(&ifde_ptr->tag) == 0x9000 ||
                                   Get16u(&ifde_ptr->tag) == 0x9101 ||
                                   Get16u(&ifde_ptr->tag) == 0xA000 ||
                                   Get16u(&ifde_ptr->tag) == 0x0001)
				{
					trace("\n------Start print_16bit_hex Tags------");

					memset(buffer, (int) NULL, 256);
					sprintf(buffer, "%x", Get32u(&ifde_ptr->value_offset)); 

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nvalue_offset: %x", Get32u(&ifde_ptr->value_offset)); 
					trace(trace_buffer);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\ncount: %d", Get32u(&ifde_ptr->count)); 
					trace(trace_buffer);

					byte_ptr = &buffer[0];

					print_16bit_hex(pad_field(byte_ptr, 2 * Get32u(&ifde_ptr->count), '0', 1),Get32u(&ifde_ptr->count), 2, local_ptr);
					trace("\n------End print_16bit_hex Tags------");

					break;
				}

				if(Get16u(&ifde_ptr->tag) == 0xA302 ||
                                   Get16u(&ifde_ptr->tag) == 0x008C ||
                                   Get16u(&ifde_ptr->tag) == 0x0091 ||
                                   Get16u(&ifde_ptr->tag) == 0x0096 ||
                                   Get16u(&ifde_ptr->tag) == 0x0097 ||
                                   Get16u(&ifde_ptr->tag) == 0x00A8 ||
                                   Get16u(&ifde_ptr->tag) == 0x0098)
				{
					trace("\n------Start print_8bit_hex Tags------");

					fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);
					
					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nvalue_offset: %x", Get32u(&ifde_ptr->value_offset)); 
					trace(trace_buffer);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\noffset_base: %x", offset_base); 
					trace(trace_buffer);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\ncount: %d", Get32u(&ifde_ptr->count)); 
					trace(trace_buffer);

					byte_ptr = (char *) my_malloc("byte_ptr", (Get32u(&ifde_ptr->count) + 1));
					memset(byte_ptr, (int) NULL, (Get32u(&ifde_ptr->count) + 1));

					fread(byte_ptr, Get32u(&ifde_ptr->count), 1, rfd);

					if(Get32u(&ifde_ptr->count) > 10)
						print_8bit_hex(byte_ptr, 10, local_ptr);
					else
						print_8bit_hex(byte_ptr, Get32u(&ifde_ptr->count), local_ptr);

					my_free("byte_ptr", byte_ptr);

					trace("\n------End print_8bit_hex Tags------");

					break;
				}

				if(Get16u(&ifde_ptr->tag) == 0x9286)
				{
					trace("\n------Start UserComment------");

					fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base + ASCII_LABEL_SKIP_LENGTH, 0);
					
					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nvalue_offset: %x", Get32u(&ifde_ptr->value_offset)); 
					trace(trace_buffer);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\noffset_base: %x", offset_base); 
					trace(trace_buffer);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\ncount: %d", Get32u(&ifde_ptr->count)); 
					trace(trace_buffer);

					byte_ptr = (char *) my_malloc("byte_ptr", (Get32u(&ifde_ptr->count) + 1));
					memset(byte_ptr, (int) NULL, (Get32u(&ifde_ptr->count) + 1));

					fread(byte_ptr, Get32u(&ifde_ptr->count), 1, rfd);

					if(iTrace)
						printf("\nbyte_ptr: %s\n", byte_ptr);

					local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(byte_ptr) + 1));
					memset(local_ptr->Value, (int) NULL, (strlen(byte_ptr) + 1));
					strcpy(local_ptr->Value, byte_ptr);

					my_free("byte_ptr", byte_ptr);

					trace("\n------End UserComment------");

					break;
				}

				if(Get16u(&ifde_ptr->tag) == TAG_MAKER_NOTE)
				{
					if(!strncmp(upper_case(make_ptr), "NIKON", 5))
					{
						trace("\n------Start Maker Notes------");

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "%x", Get32u(&ifde_ptr->value_offset));
						local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(trace_buffer) + 1));
						memset(local_ptr->Value, (int) NULL, (strlen(trace_buffer) + 1));
						strcpy(local_ptr->Value, trace_buffer);

						//save file ptr
						temp_position = ftell(rfd);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nold_file_position: %x", temp_position);
						trace(trace_buffer);

						old_byte_order = MotorolaOrder;

						maker_note_header_offset = Get32u(&ifde_ptr->value_offset) + MAKER_NOTE_PRE_HEADER_LENGTH + jpg_type_offset();
						calc_maker_note_header_offset = maker_note_header_offset;

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nmaker_note_header_offset: %x", maker_note_header_offset);
						trace(trace_buffer);

						fseek(rfd, maker_note_header_offset, 0);

						memset(type_buffer, (int) NULL, 256);
						sprintf(type_buffer, "MAKER_NOTES");

						//call dump_.... with offset
						if(dump_image_file_directory(rfd, (maker_note_header_offset + inspect_image_file_header(rfd, 1)), maker_note_header_offset, get_new_data_header_ptr(), type_buffer))
							return(1);

						MotorolaOrder = old_byte_order;

						//restore file ptr
						fseek(rfd, temp_position, 0);
						trace("\n------End Maker Notes------");
					}

#ifdef CANON
					if(!strncmp(upper_case(make_ptr), "CANON", 5))
					{
						trace("\n------Start Maker Notes------");

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "%x", Get32u(&ifde_ptr->value_offset));
						local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(trace_buffer) + 1));
						memset(local_ptr->Value, (int) NULL, (strlen(trace_buffer) + 1));
						strcpy(local_ptr->Value, trace_buffer);

						//save file ptr
						temp_position = ftell(rfd);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nold_file_position: %x", temp_position);
						trace(trace_buffer);

						old_byte_order = MotorolaOrder;

						maker_note_header_offset = Get32u(&ifde_ptr->value_offset) + MAKER_NOTE_PRE_HEADER_LENGTH + 2;

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nmaker_note_header_offset: %x", maker_note_header_offset);
						trace(trace_buffer);

						fseek(rfd, maker_note_header_offset, 0);

						memset(type_buffer, (int) NULL, 256);
						sprintf(type_buffer, "MAKER_NOTES");

						//call dump_.... with offset
						if(dump_image_file_directory(rfd, (maker_note_header_offset /*+ inspect_image_file_header(rfd, 1)*/), maker_note_header_offset, get_new_data_header_ptr(), type_buffer))
							return(1);

						MotorolaOrder = old_byte_order;

						//restore file ptr
						fseek(rfd, temp_position, 0);
						trace("\n------End Maker Notes------");
					}
#endif // CANON
				}

				break;
			default:
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nUnknown Format: %x", Get16u(&ifde_ptr->field_type));
				trace(trace_buffer);
				break;
		}

		local_ptr->next = (struct data_header *) my_malloc("local_ptr->next", sizeof(struct data_header));
		memset(local_ptr->next, (int) NULL, sizeof(struct data_header));
		local_ptr = local_ptr->next;
		memset(local_ptr, (int) NULL, sizeof(struct data_header));

		fseek(rfd, old_position, 0);
	}

	trace("\n-dump_image_file_directory");
	mtrace("\n-dump_image_file_directory");

	return(0);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
struct data_header_ptr * get_new_main_header_ptr()
{
	struct data_header_ptr *local_main_header_ptr = NULL;
	local_main_header_ptr = (struct data_header_ptr *) my_malloc("local_main_header_ptr", sizeof(struct data_header_ptr));
	memset(local_main_header_ptr, (int) NULL, sizeof(struct data_header_ptr));
	return(local_main_header_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
struct data_header * get_new_data_header_ptr()
{
	struct data_header *local_header_ptr = NULL;
	local_header_ptr = (struct data_header *) my_malloc("local_header_ptr", sizeof(struct data_header));
	memset(local_header_ptr, (int) NULL, sizeof(struct data_header));
	return(local_header_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
struct html_header * get_new_html_header_ptr()
{
	struct html_header *local_header_ptr = NULL;
	local_header_ptr = (struct html_header *) my_malloc("local_header_ptr", sizeof(struct html_header));
	memset(local_header_ptr, (int) NULL, sizeof(struct html_header));
	return(local_header_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
unsigned long inspect_image_file_header(FILE *rfd, int iType)
{
        char buffer[256];
        static unsigned char ExifHeader[] = "Exif\0\0";
	struct image_file_header *ifh_ptr = NULL;
	unsigned long offset_ifd = 0;

	struct jpg_indicator_header
	{
		unsigned char start;
		unsigned char soi;
		unsigned char pad;
		unsigned char exif_ind;
	};

	struct jpg_indicator_header *jpg_ind_hdr_ptr = NULL;

	trace("\n+inspect_image_file_header");
	mtrace("\n+inspect_image_file_header");

	if(iType)
	{
		ifh_ptr = (struct image_file_header *) my_malloc("ifh_ptr", sizeof(struct image_file_header));
		memset(ifh_ptr, (int) NULL, sizeof(struct image_file_header));

		if((fread(ifh_ptr, sizeof(struct image_file_header), 1, rfd)) != 0)
		{
			memset(buffer, (int) NULL, 256);
			memcpy(buffer, ifh_ptr->byte_order, 2);
	
			if(!strncmp(buffer, "MM", 2))
				MotorolaOrder = 1;
			else
				MotorolaOrder = 0;
	
			offset_ifd = Get32u(&ifh_ptr->offset_to_first_ifd);
	
			memset(trace_buffer, (int) NULL, 256);
			sprintf(trace_buffer, "\nByte Order: %s", buffer);
			trace(trace_buffer);
			memset(trace_buffer, (int) NULL, 256);
			sprintf(trace_buffer, "\nImage File Type: %x", Get16u(&ifh_ptr->image_file_type));
			trace(trace_buffer);
			memset(trace_buffer, (int) NULL, 256);
			sprintf(trace_buffer, "\nOffset To First IFD: %x", offset_ifd);
			trace(trace_buffer);
	
			if((!strncmp(buffer, "MM", 2)) && (Get16u(&ifh_ptr->image_file_type) == 42))
			{
			}
			else if((!strncmp(buffer, "II", 2)) && (Get16u(&ifh_ptr->image_file_type) == 42))
			{
			}
			else
			{
				printf("\nFile type is not NEF/Nikon RAW\n\n");
				exit(1);
			}
	
		}
		else
		{
			my_error(rfd);
		}

		my_free("ifh_ptr", ifh_ptr);
	}
	else
	{
		jpg_ind_hdr_ptr = (struct jpg_indicator_header *) my_malloc("jpg_ind_hdr_ptr", sizeof(struct jpg_indicator_header));
		memset(jpg_ind_hdr_ptr, (int) NULL, sizeof(struct jpg_indicator_header));

		if((fread(jpg_ind_hdr_ptr, sizeof(struct jpg_indicator_header), 1, rfd)) != 0)
		{
			if(jpg_ind_hdr_ptr->start != 0xff)
			{
				my_exit("First byte must be 0xFF");
			}

			if(jpg_ind_hdr_ptr->soi != 0xd8)
			{
				my_exit("Second byte must be 0xD8");
			}

			if(jpg_ind_hdr_ptr->pad != 0xff)
			{
				my_exit("Third byte must be 0xFF");
			}

			fseek(rfd, 0, 0);

			switch(jpg_ind_hdr_ptr->exif_ind)
			{
				case 0xe0:
					iJPG_Type = 0;
					fseek(rfd, 18, 0);
					offset_ifd = get_jpg_type_and_offset(rfd, jpg_type_offset());
					break;
				case 0xe1:
					iJPG_Type = 1;
					offset_ifd = get_jpg_type_and_offset(rfd, jpg_type_offset());
					break;
				default:
					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "Fourth byte is unknown: %x", jpg_ind_hdr_ptr->exif_ind);
					my_exit(trace_buffer);
					break;
			}
		}

		my_free("jpg_ind_hdr_ptr", jpg_ind_hdr_ptr);
	}

	trace("\n-inspect_image_file_header");
	mtrace("\n-inspect_image_file_header");
	return(offset_ifd);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
unsigned long get_jpg_type_and_offset(FILE *rfd, unsigned long offset)
{
	char buffer[256];
	static unsigned char ExifHeader[] = "Exif\0\0";
	unsigned long offset_ifd = 0l;
	struct jpg_image_file_header *jpg_ifh_ptr = NULL;

	trace("\n+get_jpg_type_and_offset");
	mtrace("\n+get_jpg_type_and_offset");

	jpg_ifh_ptr = (struct jpg_image_file_header *) my_malloc("jpg_ifh_ptr", sizeof(struct jpg_image_file_header));
	memset(jpg_ifh_ptr, (int) NULL, sizeof(struct jpg_image_file_header));

	if((fread(jpg_ifh_ptr, sizeof(struct jpg_image_file_header), 1, rfd)) != 0)
	{
	        if (strncmp(jpg_ifh_ptr->exif_header_text, ExifHeader, 6))
		{
			my_exit("EXIF header text incorrect");
		}
			
		memset(buffer, (int) NULL, 256);
		memcpy(buffer, jpg_ifh_ptr->byte_order, 2);
				
		if(!strncmp(buffer, "MM", 2))
			MotorolaOrder = 1;
		else
			MotorolaOrder = 0;
				
		offset_ifd = Get32u(&jpg_ifh_ptr->offset_to_first_ifd) + offset;
				
		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nByte Order: %s", buffer);
		trace(trace_buffer);
		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nImage File Type: %x", Get16u(&jpg_ifh_ptr->image_file_type));
		trace(trace_buffer);
		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nOffset To First IFD: %x", offset_ifd);
		trace(trace_buffer);

		if(iTrace)
		{
			printf("\nstart: %x", jpg_ifh_ptr->start);
			printf("\nsoi: %x", jpg_ifh_ptr->soi);
			printf("\npad: %x", jpg_ifh_ptr->pad);
			printf("\nexif_ind: %x", jpg_ifh_ptr->exif_ind);
			printf("\nlength: %x", jpg_ifh_ptr->length);
			printf("\nexif_header_text: %s", jpg_ifh_ptr->exif_header_text);
			printf("\nbyte_order: %s", jpg_ifh_ptr->byte_order);
			printf("\nimage_file_type: %x", jpg_ifh_ptr->image_file_type);
			printf("\noffset: %x\n", jpg_ifh_ptr->offset_to_first_ifd);
		}

		if((!strncmp(buffer, "MM", 2)) && (Get16u(&jpg_ifh_ptr->image_file_type) == 42))
		{
		}
		else if((!strncmp(buffer, "II", 2)) && (Get16u(&jpg_ifh_ptr->image_file_type) == 42))
		{
		}
		else
		{
			printf("\nFile type is not Nikon JPG\n\n");
			exit(1);
		}
				
	}
	else
	{
		my_error(rfd);
	}

	my_free("jpg_ifh_ptr", jpg_ifh_ptr);

	trace("\n-get_jpg_type_and_offset");
	mtrace("\n-get_jpg_type_and_offset");
	return(offset_ifd);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int jpg_type()
{
	return(iJPG_Type);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int jpg_type_offset()
{
	int iType = 0;

	switch(iJPG_Type)
	{
		case 0:
			iType = 30;
			break;
		case 1:
			iType = 12;
			break;
		case 99:
			iType = 0;
			break;
		default:
			iType = -1;
			break;
	}

	return(iType);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void display_data(struct data_header *local_ptr, char *data_type)
{
	int height = LARGE_IMAGE_HEIGHT;
	int width = LARGE_IMAGE_WIDTH;
	char buffer[256];

	while(local_ptr->next != NULL)
	{
		if(iTrace)
		{
			if(is_tag_printed((unsigned short) local_ptr->Tag))
			{
				switch(local_ptr->Tag)
				{
					case 0x829a:		// shutter speed

						if(atof(local_ptr->Value) <= 0.5)
							sprintf(buffer, "1/%d", (int) (0.5 + 1/atof(local_ptr->Value)));
						else
							sprintf(buffer, "%6.4f", (double) atof(local_ptr->Value));

						debug_printf("\n%#x: %s: %s seconds", local_ptr->Tag, local_ptr->Desc, buffer);
						break;
					case 0x9209:

						if(!strncmp(local_ptr->Value, "0", strlen("0")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "No Flash");
						}
						else if((strlen(local_ptr->Value) == 1) && !strncmp(local_ptr->Value, "1", strlen("1")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Fired");
						}
						else if((strlen(local_ptr->Value) == 1) && !strncmp(local_ptr->Value, "5", strlen("5")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Return not detected");
						}
						else if((strlen(local_ptr->Value) == 1) && !strncmp(local_ptr->Value, "7", strlen("7")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Return detected");
						}
						else if((strlen(local_ptr->Value) == 1) && !strncmp(local_ptr->Value, "9", strlen("9")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "On");
						}
						else if(!strncmp(local_ptr->Value, "13", strlen("13")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "On, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "15", strlen("15")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "On, Return detected");
						}
						else if(!strncmp(local_ptr->Value, "16", strlen("16")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Off");
						}
						else if(!strncmp(local_ptr->Value, "24", strlen("24")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Did not fire");
						}
						else if(!strncmp(local_ptr->Value, "25", strlen("25")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired");
						}
						else if(!strncmp(local_ptr->Value, "29", strlen("29")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "31", strlen("31")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Return detected");
						}
						else if(!strncmp(local_ptr->Value, "32", strlen("32")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "No flash function");
						}
						else if(!strncmp(local_ptr->Value, "65", strlen("65")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Red-eye reduction");
						}
						else if(!strncmp(local_ptr->Value, "69", strlen("69")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Red-eye reduction, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "71", strlen("71")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Red-eye reduction, Return detected");
						}
						else if(!strncmp(local_ptr->Value, "73", strlen("73")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "On, Red-eye reduction");
						}
						else if(!strncmp(local_ptr->Value, "77", strlen("77")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "On, Red-eye reduction, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "79", strlen("79")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "On, Red-eye reduction, Return detected");
						}
						else if(!strncmp(local_ptr->Value, "89", strlen("89")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Red-eye reduction");
						}
						else if(!strncmp(local_ptr->Value, "93", strlen("93")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Red-eye reduction, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "95", strlen("95")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Red-eye reduction, Return detected");
						}
						else
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0x8822:
						if(!strncmp(local_ptr->Value, "2", strlen("2")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Program AE");
						}
						else if(!strncmp(local_ptr->Value, "0", strlen("0")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Auto");
						}
						else if(!strncmp(local_ptr->Value, "4", strlen("4")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Shutter speed priority AE");
						}
						else if(!strncmp(local_ptr->Value, "3", strlen("3")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Aperture-priority AE");
						}
						else if(!strncmp(local_ptr->Value, "1", strlen("1")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Manual");
						}
						else
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0x9207:
						if(!strncmp(local_ptr->Value, "5", strlen("5")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Multi-segment");
						}
						else if(!strncmp(local_ptr->Value, "3", strlen("3")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Spot");
						}
						else if(!strncmp(local_ptr->Value, "2", strlen("2")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Center-weighted average");
						}
						else
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0xa002:
						debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0xa003:
						debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x0100:
						if(!strncmp(data_type, "IFD0", strlen("IFD0")))
							break;

						debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x0101:
						if(!strncmp(data_type, "IFD0", strlen("IFD0")))
							break;

						debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x0112:
//						if(!strncmp(data_type, "SUB_IFD1", strlen("SUB_IFD1")))
//						if(!strncmp(data_type, "IFD0", strlen("IFD0")))
						if(ulGlobalOrientation != 0)
							break;

						if(GlobalOrientationPtr != NULL)
							my_free("GlobalOrientationPtr", GlobalOrientationPtr);

						GlobalOrientationPtr = (char *) my_malloc("GlobalOrientationPtr", (strlen(local_ptr->Value) + 1));
						memset(GlobalOrientationPtr, (int) NULL, (strlen(local_ptr->Value) + 1));
						sprintf(GlobalOrientationPtr, "%s", local_ptr->Value);

						ulGlobalOrientation = (unsigned long) atoi(local_ptr->Value);

						if(!strncmp(local_ptr->Value, "1", strlen("1")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Horizontal (normal)");
						}
						else if(!strncmp(local_ptr->Value, "2", strlen("2")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Mirrored horizontal");
						}
						else if(!strncmp(local_ptr->Value, "3", strlen("3")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Rotated 180");
						}
						else if(!strncmp(local_ptr->Value, "4", strlen("4")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Mirrored vertical");
						}
						else if(!strncmp(local_ptr->Value, "5", strlen("5")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Mirrored horizontal then rotated 90 CCW");
						}
						else if(!strncmp(local_ptr->Value, "6", strlen("6")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Rotated 90 CW");
						}
						else if(!strncmp(local_ptr->Value, "7", strlen("7")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Mirrored horizontal then rotated 90 CW");
						}
						else if(!strncmp(local_ptr->Value, "8", strlen("8")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "Rotated 90 CCW");
						}
						else
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0x0084:		// lens
						debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, convert_lens(local_ptr->Value));
						break;
					case 0x920a:		// FocalLength
						debug_printf("\n%#x: %s: %s mm", local_ptr->Tag, local_ptr->Desc, truncate(local_ptr->Value, precision(local_ptr->Value)));
						break;
					case 0xa405:		// FocalLengthIn35mmFilm
						debug_printf("\n%#x: %s: %s mm", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x0083:
						if(!strncmp(local_ptr->Value, "2", strlen("2")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "D");
						}
						else if(!strncmp(local_ptr->Value, "6", strlen("6")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "G");
						}
						else if(!strncmp(local_ptr->Value, "e", strlen("e")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "G-VR");
						}
						else if(!strncmp(local_ptr->Value, "a", strlen("a")))
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, "D-VR");
						}
						else
						{
							debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0x829d:
						debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, truncate(local_ptr->Value, 3));
						break;
					case 0x0131:
						//software_ptr = (char *) my_malloc("software_ptr", (strlen(local_ptr->Value) + 1));
						//memset(software_ptr, (int) NULL, (strlen(local_ptr->Value) + 1));
						//strcpy(software_ptr, local_ptr->Value);

						debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x8827:
					case 0x010f:
					case 0x0110:
					case 0x9004:
					case 0x0002:
					case 0x0013:
					case 0x0004:
					case 0x0005:
					case 0x0006:
					case 0x00a7:
					case 0x0007:
					case 0x0008:
					case 0x0009:
					case 0x0081:
					case 0x008b:
					case 0x008d:
					case 0x0090:
					case 0x0095:
		//			case 0x00a0:
					case 0x00a9:
					case 0x00aa:
					case 0x00ab:
					case 0x9204:
					case 0x9286:
		//			case 0x010E:
					case 0xffff:
						debug_printf("\n%#x: %s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					default:
						break;
				}	// end switch
			}		// end is_tag_printed
		}	// end if(iTrace)
		else	// not debug
		{
			if(is_tag_printed((unsigned short) local_ptr->Tag))
			{
				switch(local_ptr->Tag)
				{
					case 0x829a:		// shutter speed

						if(atof(local_ptr->Value) <= 0.5)
							sprintf(buffer, "1/%d", (int) (0.5 + 1/atof(local_ptr->Value)));
						else
							sprintf(buffer, "%6.4f", (double) atof(local_ptr->Value));

						my_printf("\n%s: %s seconds", local_ptr->Tag, local_ptr->Desc, buffer);
						break;
					case 0x9209:
						if(!strncmp(local_ptr->Value, "0", strlen("0")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "No Flash");
						}
						else if((strlen(local_ptr->Value) == 1) && !strncmp(local_ptr->Value, "1", strlen("1")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Fired");
						}
						else if((strlen(local_ptr->Value) == 1) && !strncmp(local_ptr->Value, "5", strlen("5")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Return not detected");
						}
						else if((strlen(local_ptr->Value) == 1) && !strncmp(local_ptr->Value, "7", strlen("7")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Return detected");
						}
						else if((strlen(local_ptr->Value) == 1) && !strncmp(local_ptr->Value, "9", strlen("9")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "On");
						}
						else if(!strncmp(local_ptr->Value, "13", strlen("13")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "On, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "15", strlen("15")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "On, Return detected");
						}
						else if(!strncmp(local_ptr->Value, "16", strlen("16")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Off");
						}
						else if(!strncmp(local_ptr->Value, "24", strlen("24")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Did not fire");
						}
						else if(!strncmp(local_ptr->Value, "25", strlen("25")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired");
						}
						else if(!strncmp(local_ptr->Value, "29", strlen("29")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "31", strlen("31")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Return detected");
						}
						else if(!strncmp(local_ptr->Value, "32", strlen("32")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "No flash function");
						}
						else if(!strncmp(local_ptr->Value, "65", strlen("65")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Red-eye reduction");
						}
						else if(!strncmp(local_ptr->Value, "69", strlen("69")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Red-eye reduction, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "71", strlen("71")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Fired, Red-eye reduction, Return detected");
						}
						else if(!strncmp(local_ptr->Value, "73", strlen("73")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "On, Red-eye reduction");
						}
						else if(!strncmp(local_ptr->Value, "77", strlen("77")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "On, Red-eye reduction, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "79", strlen("79")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "On, Red-eye reduction, Return detected");
						}
						else if(!strncmp(local_ptr->Value, "89", strlen("89")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Red-eye reduction");
						}
						else if(!strncmp(local_ptr->Value, "93", strlen("93")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Red-eye reduction, Return not detected");
						}
						else if(!strncmp(local_ptr->Value, "95", strlen("95")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Auto, Fired, Red-eye reduction, Return detected");
						}
						else
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0x8822:
						if(!strncmp(local_ptr->Value, "2", strlen("2")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Program AE");
						}
						else if(!strncmp(local_ptr->Value, "0", strlen("0")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Auto");
						}
						else if(!strncmp(local_ptr->Value, "4", strlen("4")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Shutter speed priority AE");
						}
						else if(!strncmp(local_ptr->Value, "3", strlen("3")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Aperture-priority AE");
						}
						else if(!strncmp(local_ptr->Value, "1", strlen("1")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Manual");
						}
						else
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0x9207:
						if(!strncmp(local_ptr->Value, "5", strlen("5")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Multi-segment");
						}
						else if(!strncmp(local_ptr->Value, "3", strlen("3")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Spot");
						}
						else if(!strncmp(local_ptr->Value, "2", strlen("2")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Center-weighted average");
						}
						else
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0xa002:
						my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0xa003:
						my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x0100:
						if(!strncmp(data_type, "IFD0", strlen("IFD0")))
							break;

						my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x0101:
						if(!strncmp(data_type, "IFD0", strlen("IFD0")))
							break;

						my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x0112:
//						if(!strncmp(data_type, "SUB_IFD1", strlen("SUB_IFD1")))
//						if(!strncmp(data_type, "IFD0", strlen("IFD0")))
						if(ulGlobalOrientation != 0)
							break;

						if(iTrace)
							printf("\nlocal_ptr->Value: %s", local_ptr->Value);

						if(GlobalOrientationPtr != NULL)
							my_free("GlobalOrientationPtr", GlobalOrientationPtr);

						GlobalOrientationPtr = (char *) my_malloc("GlobalOrientationPtr", (strlen(local_ptr->Value) + 1));
						memset(GlobalOrientationPtr, (int) NULL, (strlen(local_ptr->Value) + 1));
						sprintf(GlobalOrientationPtr, "%s", local_ptr->Value);

						ulGlobalOrientation = (unsigned long) atoi(local_ptr->Value);

						if(!strncmp(local_ptr->Value, "1", strlen("1")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Horizontal (normal)");
						}
						else if(!strncmp(local_ptr->Value, "2", strlen("2")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Mirrored horizontal");
						}
						else if(!strncmp(local_ptr->Value, "3", strlen("3")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Rotated 180");
						}
						else if(!strncmp(local_ptr->Value, "4", strlen("4")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Mirrored vertical");
						}
						else if(!strncmp(local_ptr->Value, "5", strlen("5")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Mirrored horizontal then rotated 90 CCW");
						}
						else if(!strncmp(local_ptr->Value, "6", strlen("6")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Rotated 90 CW");
						}
						else if(!strncmp(local_ptr->Value, "7", strlen("7")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Mirrored horizontal then rotated 90 CW");
						}
						else if(!strncmp(local_ptr->Value, "8", strlen("8")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "Rotated 90 CCW");
						}
						else
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0x0084:		// lens
						my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, convert_lens(local_ptr->Value));
						break;
					case 0x920a:		// FocalLength
						my_printf("\n%s: %s mm", local_ptr->Tag, local_ptr->Desc, truncate(local_ptr->Value, precision(local_ptr->Value)));
						break;
					case 0xa405:		// FocalLengthIn35mmFilm
						my_printf("\n%s: %s mm", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x0083:
						if(!strncmp(local_ptr->Value, "2", strlen("2")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "D");
						}
						else if(!strncmp(local_ptr->Value, "6", strlen("6")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "G");
						}
						else if(!strncmp(local_ptr->Value, "e", strlen("e")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "G-VR");
						}
						else if(!strncmp(local_ptr->Value, "a", strlen("a")))
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, "D-VR");
						}
						else
						{
							my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						}
						break;
					case 0x829d:
						my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, truncate(local_ptr->Value, 3));
						break;
					case 0x0131:
						//software_ptr = (char *) my_malloc("software_ptr", (strlen(local_ptr->Value) + 1));
						//memset(software_ptr, (int) NULL, (strlen(local_ptr->Value) + 1));
						//strcpy(software_ptr, local_ptr->Value);

						my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					case 0x8827:
					case 0x010f:
					case 0x0110:
					case 0x9004:
					case 0x0002:
					case 0x0013:
					case 0x0004:
					case 0x0005:
					case 0x0006:
					case 0x00a7:
					case 0x0007:
					case 0x0008:
					case 0x0009:
					case 0x0081:
					case 0x008b:
					case 0x008d:
					case 0x0090:
					case 0x0095:
		//			case 0x00a0:
					case 0x00a9:
					case 0x00aa:
					case 0x00ab:
					case 0x9204:
					case 0x9286:
		//			case 0x010E:
					case 0xffff:
						my_printf("\n%s: %s", local_ptr->Tag, local_ptr->Desc, local_ptr->Value);
						break;
					default:
						break;
				}	// end switch
			}	// end is_tag_printed
		}

		local_ptr = local_ptr->next;
	}
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
char * convert_lens(char *str_lens)
{
	char *temp_ptr = NULL;
	char *final_ptr = "lens";
	char *token1 = NULL;
	char *token2 = NULL;
	char *token3 = NULL;
	char *token4 = NULL;
	char buffer[256];

	trace("\n+convert_lens");
	mtrace("\n+convert_lens");

	memset(buffer, (int) NULL, 256);

	temp_ptr = strdup(str_lens);

	token1 = strtok(temp_ptr, " ");
	token2 = strtok(NULL, " ");
	token3 = strtok(NULL, " ");
	token4 = strtok(NULL, " ");

	if((!strcmp(token1, token2)) && (!strcmp(token3, token4)))
	{
		sprintf(buffer, "%s mm f/%s", truncate(token1, precision(token1)), truncate(token3, 2));
	}
	else
	{
		if(!strcmp(token3, token4))
			sprintf(buffer, "%s-%s mm f/%s", truncate(token1, precision(token1)), truncate(token2, 0), truncate(token3, 2));
		else
			sprintf(buffer, "%s-%s mm f/%s-%s", truncate(token1, precision(token1)), truncate(token2, 0), truncate(token3, 2), truncate(token4, 2));
	}

	temp_ptr = (char *) my_malloc("temp_ptr", (strlen(buffer) + 1));
	memset(temp_ptr, (int) NULL, (strlen(buffer) + 1));
	strcpy(temp_ptr, buffer);

	final_ptr = strdup(temp_ptr);

	my_free("temp_ptr", temp_ptr);

	trace("\n-convert_lens");
	mtrace("\n-convert_lens");
	return(final_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
char *truncate(char *str_token, int dec_len)
{
	char *temp_ptr = NULL;
	char *char_ptr = NULL;

	temp_ptr = strdup(str_token);

	char_ptr = strchr(temp_ptr, '.');

	char_ptr += dec_len;

	*char_ptr = (char) NULL;
	
	return(temp_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void debug_printf(char *str_format, unsigned short us_tag, char *str_desc, char *str_value)
{
	if(html_header_ptr == NULL)
	{
		html_header_ptr = get_new_html_header_ptr();

		html_header_ptr->Desc = (char *) my_malloc("html_header_ptr->Desc", (strlen(str_desc) + 1));
		memset(html_header_ptr->Desc, (int) NULL, (strlen(str_desc) + 1));
		strncpy(html_header_ptr->Desc, str_desc, strlen(str_desc));

		html_header_ptr->Value = (char *) my_malloc("html_header_ptr->Value", (strlen(str_value) + 1));
		memset(html_header_ptr->Value, (int) NULL, (strlen(str_value) + 1));
		strncpy(html_header_ptr->Value, str_value, strlen(str_value));

		html_header_ptr->Tag = us_tag;

		html_header_start_ptr = html_header_ptr;
	}
	else
	{
		html_header_ptr->next = get_new_html_header_ptr();
		html_header_ptr =  html_header_ptr->next;

		html_header_ptr->Desc = (char *) my_malloc("html_header_ptr->Desc", (strlen(str_desc) + 1));
		memset(html_header_ptr->Desc, (int) NULL, (strlen(str_desc) + 1));
		strncpy(html_header_ptr->Desc, str_desc, strlen(str_desc));

		html_header_ptr->Value = (char *) my_malloc("html_header_ptr->Value", (strlen(str_value) + 1));
		memset(html_header_ptr->Value, (int) NULL, (strlen(str_value) + 1));
		strncpy(html_header_ptr->Value, str_value, strlen(str_value));

		html_header_ptr->Tag = us_tag;
	}

	if((!iHtml) && (!iXml) && (!iCsv) && (!iJS) && (!iWeb))
		printf(str_format, us_tag, str_desc, str_value);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void my_printf(char *str_format, unsigned short us_tag, char *str_desc, char *str_value)
{
	if(html_header_ptr == NULL)
	{
		html_header_ptr = get_new_html_header_ptr();

		html_header_ptr->Desc = (char *) my_malloc("html_header_ptr->Desc", (strlen(str_desc) + 1));
		memset(html_header_ptr->Desc, (int) NULL, (strlen(str_desc) + 1));
		strncpy(html_header_ptr->Desc, str_desc, strlen(str_desc));

		html_header_ptr->Value = (char *) my_malloc("html_header_ptr->Value", (strlen(str_value) + 1));
		memset(html_header_ptr->Value, (int) NULL, (strlen(str_value) + 1));
		strncpy(html_header_ptr->Value, str_value, strlen(str_value));

		html_header_ptr->Tag = us_tag;

		html_header_start_ptr = html_header_ptr;
	}
	else
	{
		html_header_ptr->next = get_new_html_header_ptr();
		html_header_ptr =  html_header_ptr->next;

		html_header_ptr->Desc = (char *) my_malloc("html_header_ptr->Desc", (strlen(str_desc) + 1));
		memset(html_header_ptr->Desc, (int) NULL, (strlen(str_desc) + 1));
		strncpy(html_header_ptr->Desc, str_desc, strlen(str_desc));

		html_header_ptr->Value = (char *) my_malloc("html_header_ptr->Value", (strlen(str_value) + 1));
		memset(html_header_ptr->Value, (int) NULL, (strlen(str_value) + 1));
		strncpy(html_header_ptr->Value, str_value, strlen(str_value));

		html_header_ptr->Tag = us_tag;
	}

	if((!iHtml) && (!iXml) && (!iCsv) && (!iJS) && (!iWeb))
		printf(str_format, str_desc, str_value);
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
void extract_and_format_data(struct image_file_directory_entry *ifde_ptr, struct data_header *local_ptr, int iHex)
{
	int ibyte_count = 0;
	unsigned int ubyte1 = 0;
	unsigned int ubyte2 = 0;
	unsigned int ubyte3 = 0;
	unsigned int ubyte4 = 0;
	unsigned int ubyte5 = 0;
	unsigned int ubyte6 = 0;
	unsigned long ulong = 0l;

	trace("\n+extract_and_format_data");
	mtrace("\n+extract_and_format_data");

	ulong = Get32u(&ifde_ptr->value_offset);

	ubyte1 = (ulong & 0xff000000) >> 24;
	ubyte2 = (ulong & 0x00ff0000) >> 16;
	ubyte3 = (ulong & 0x0000ff00) >> 8;
	ubyte4 = (ulong & 0x000000ff) >> 0;
	ubyte5 = (ulong & 0xffff0000) >> 16;
	ubyte6 = (ulong & 0x0000ffff) >> 0;

/*
	if(iTrace)
	{
		if (MotorolaOrder)
			printf("\nMM\n");
		else
			printf("\nII\n");

		printf("\ntag: %x\n", Get16u(&ifde_ptr->tag));
		printf("field_type: %d\n", Get16u(&ifde_ptr->field_type));
		printf("count: %d\n", Get32u(&ifde_ptr->count));
		printf("value_offset: %x\n", Get32u(&ifde_ptr->value_offset));
		printf("value_offset(no-Get32u): %x\n", ifde_ptr->value_offset);

		printf("\nulong: %x\n", ulong);
		printf("ubyte1: %x\n", ubyte1);
		printf("ubyte2: %x\n", ubyte2);
		printf("ubyte3: %x\n", ubyte3);
		printf("ubyte4: %x\n", ubyte4);
		printf("ubyte5: %x\n", ubyte5);
		printf("ubyte6: %x\n\n", ubyte6);
	}
*/

	memset(trace_buffer, (int) NULL, 256);

	switch(Get16u(&ifde_ptr->field_type))
	{
		case 1:
			ibyte_count = Get32u(&ifde_ptr->count) * 1;

			if(MotorolaOrder)
			{
				if(ibyte_count == 1)
				{
					sprintf(trace_buffer, "%x", ubyte1);
				}
				else if(ibyte_count == 2)
				{
					sprintf(trace_buffer, "%x %x", ubyte1, ubyte2);
				}
				else if(ibyte_count == 3)
				{
					sprintf(trace_buffer, "%x %x %x", ubyte1, ubyte2, ubyte3);
				}
				else if(ibyte_count == 4)
				{
					sprintf(trace_buffer, "%x %x %x %x", ubyte1, ubyte2, ubyte3, ubyte4);
				}
			}
			else
			{
				if(ibyte_count == 1)
				{
					sprintf(trace_buffer, "%x", ubyte4);
				}
				else if(ibyte_count == 2)
				{
					sprintf(trace_buffer, "%x %x", ubyte4, ubyte3);
				}
				else if(ibyte_count == 3)
				{
					sprintf(trace_buffer, "%x %x %x", ubyte4, ubyte3, ubyte2);
				}
				else if(ibyte_count == 4)
				{
					sprintf(trace_buffer, "%x %x %x %x", ubyte4, ubyte3, ubyte2, ubyte1);
				}
			}
			break;
		case 2:
			ibyte_count = Get32u(&ifde_ptr->count) * 1;

			if(ibyte_count == 1)
			{
				sprintf(trace_buffer, "%c", ubyte1);
			}
			else if(ibyte_count == 2)
			{
				sprintf(trace_buffer, "%c%c", ubyte1, ubyte2);
			}
			else if(ibyte_count == 3)
			{
				if(MotorolaOrder)
					sprintf(trace_buffer, "%c%c%c", ubyte1, ubyte2, ubyte3);
				else
					sprintf(trace_buffer, "%c%c%c", ubyte4, ubyte3, ubyte2);
			}
			else if(ibyte_count == 4)
			{
				sprintf(trace_buffer, "%c%c%c%c", ubyte1, ubyte2, ubyte3, ubyte4);
			}
			break;
		case 6:
			ibyte_count = Get32u(&ifde_ptr->count) * 1;

			if(ibyte_count == 1)
			{
				sprintf(trace_buffer, "%d", ubyte1);
			}
			else if(ibyte_count == 2)
			{
				sprintf(trace_buffer, "%d %d", ubyte1, ubyte2);
			}
			else if(ibyte_count == 3)
			{
				sprintf(trace_buffer, "%d %d %d", ubyte1, ubyte2, ubyte3);
			}
			else if(ibyte_count == 4)
			{
				sprintf(trace_buffer, "%d %d %d %d", ubyte1, ubyte2, ubyte3, ubyte4);
			}
			break;
		case 3:
		case 8:
			ibyte_count = Get32u(&ifde_ptr->count) * 2;

			if((Get16u(&ifde_ptr->tag) == 0x0002) || (Get16u(&ifde_ptr->tag) == 0x0013))
			{
				if(ubyte6 != 0)
					sprintf(trace_buffer, "%d", ubyte6);
				else
					sprintf(trace_buffer, "%d", ubyte5);
			}
			else
			{
				if(ibyte_count == 2)
				{
					if(MotorolaOrder)
						sprintf(trace_buffer, "%d", ubyte5);
					else
						sprintf(trace_buffer, "%d", ubyte6);
				}
				else if(ibyte_count == 4)
				{
					sprintf(trace_buffer, "%d %d", ubyte5, ubyte6);
				}
			}
			break;
		case 4:
		case 9:
			ibyte_count = Get32u(&ifde_ptr->count) * 4;
			sprintf(trace_buffer, "%ld", ulong);
			break;
		default:
			my_exit("Unknown Field Type");
			break;
	}

    local_ptr->Value = (char *) my_malloc("local_ptr->Value", (strlen(trace_buffer) + 1));
    memset(local_ptr->Value, (int) NULL, (strlen(trace_buffer) + 1));
    strcpy(local_ptr->Value, trace_buffer);

	if(iTrace)
		printf("\nValue Stored: %s", trace_buffer);

	trace("\n-extract_and_format_data");
	mtrace("\n-extract_and_format_data");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void dump_jpg_image_file_directory(FILE *rfd, unsigned long offset_ifd, unsigned long offset_base, char *type_ptr)
{
	char *buff_ptr = NULL;
	char buffer[256];
	char type_buffer[256];
	struct image_file_directory_entry *ifde_ptr = NULL;
	unsigned short count = 0;
	unsigned short num_ifd = 0;
	unsigned int ubyte5 = 0l;
	unsigned long ulong = 0l;
	unsigned long ctr1 = 0l;
	unsigned long ctr3 = 0l;
	long old_position = 0l;
	long temp_position = 0l;
	int ByteCount = 0;
	unsigned long components = 0l;      		//count
	unsigned long value_offset = 0l;     	//value offset
	unsigned char *byte_ptr = NULL;
	unsigned long *long_byte_ptr = NULL;

	unsigned long maker_note_header_offset = 0l;
	unsigned long ulLocalPosition = 0l;

	trace("\n+dump_jpg_image_file_directory");
	mtrace("\n+dump_jpg_image_file_directory");

	if(iTrace)
		printf("\ntype_ptr: %s", type_ptr);

	ifde_ptr = (struct image_file_directory_entry *) my_malloc("ifde_ptr", sizeof(struct image_file_directory_entry));
	memset(ifde_ptr, (int) NULL, sizeof(struct image_file_directory_entry));

	if(fseek(rfd, offset_ifd, 0) != -1)
	{
		fread(&count, sizeof(unsigned short), 1, rfd);
		num_ifd = Get16u(&count);

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nNumber of Image File Directories: %d", num_ifd);
		trace(trace_buffer);
	}
	else
	{
		my_error(rfd);
	}

	for(ctr1 = 0; ctr1 < num_ifd; ctr1++)
	{
		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nctr1: %d", ctr1);
		trace(trace_buffer);

		memset(ifde_ptr, (int) NULL, sizeof(struct image_file_directory_entry));
		fread(ifde_ptr, sizeof(struct image_file_directory_entry), 1, rfd);

		ulLocalPosition = ftell(rfd);

		if(ulLocalPosition > ulGlobalPosition)
			ulGlobalPosition = ulLocalPosition;

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\ntag: %x", Get16u(&ifde_ptr->tag));
		trace(trace_buffer);

		jpg_header_ptr->tag = Get16u(&ifde_ptr->tag);
		jpg_header_ptr->field_type = Get16u(&ifde_ptr->field_type);
		jpg_header_ptr->count = Get32u(&ifde_ptr->count);
		jpg_header_ptr->value_offset = Get32u(&ifde_ptr->value_offset);

		if(!strncmp(type_ptr, "IFD0", 4))
			jpg_header_ptr->record_type = 1;
		else if(!strncmp(type_ptr, "SUB_IFD0", 8))
			jpg_header_ptr->record_type = 3;
		else if(!strncmp(type_ptr, "SUB_IFD1", 8))
			jpg_header_ptr->record_type = 4;
		else if(!strncmp(type_ptr, "EXIF", 4))
			jpg_header_ptr->record_type = 5;
		else if(!strncmp(type_ptr, "MAKER_NOTES", 11))
			jpg_header_ptr->record_type = 6;
		else if(!strncmp(type_ptr, "SUB_IFD2", 8))
			jpg_header_ptr->record_type = 7;
		else
			jpg_header_ptr->record_type = 99;

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nField Type: %x", Get16u(&ifde_ptr->field_type));
		trace(trace_buffer);

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nNumber of Values: %x", Get32u(&ifde_ptr->count));
		trace(trace_buffer);

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\nValue Offset: %x", Get32u(&ifde_ptr->value_offset));
		trace(trace_buffer);

		components = Get32u(&ifde_ptr->count);
		value_offset = Get32u(&ifde_ptr->value_offset) + offset_base;

		memset(trace_buffer, (int) NULL, 256);
		sprintf(trace_buffer, "\ncomponents: %d", components);
		trace(trace_buffer);

		old_position = ftell(rfd);

		switch(Get16u(&ifde_ptr->field_type))
		{
			case 1:
				ByteCount = components * 1;
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);
				jpg_header_ptr->byte_count = ByteCount;


				if (ByteCount > 4)
				{
					byte_ptr = (unsigned char *) my_malloc("byte_ptr", (ByteCount + 1));
					memset(byte_ptr, (int) NULL, (ByteCount + 1));
					fseek(rfd, Get32u(&ifde_ptr->value_offset), 0);
					fread(byte_ptr, ByteCount, 1, rfd);
					jpg_header_ptr->data_at_offset = byte_ptr;
				}

				break;
			case 2:
				ByteCount = components * 1;
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);
				jpg_header_ptr->byte_count = ByteCount;

				if (ByteCount > 4)
				{
					byte_ptr = (unsigned char *) my_malloc("byte_ptr", (ByteCount + 1));
					memset(byte_ptr, (int) NULL, (ByteCount + 1));
					fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);
					fread(byte_ptr, ByteCount, 1, rfd);
					jpg_header_ptr->data_at_offset = byte_ptr;

					if(Get16u(&ifde_ptr->tag) == 0x10F)
					{
						make_ptr = (char *) my_malloc("make_ptr", (components + 1));
						memset(make_ptr, (int) NULL, (components + 1));
						strcpy(make_ptr, byte_ptr);
					}

					if(Get16u(&ifde_ptr->tag) == 0x110)
					{
						model_ptr = (char *) my_malloc("model_ptr", (components + 1));
						memset(model_ptr, (int) NULL, (components + 1));
						strcpy(model_ptr, byte_ptr);
					}

					if(Get16u(&ifde_ptr->tag) == 0x131)
					{
						software_ptr = (char *) my_malloc("software_ptr", (components + 1));
						memset(software_ptr, (int) NULL, (components + 1));
						strcpy(software_ptr, byte_ptr);
					}
				}

				break;
			case 3:
			case 8:
				ByteCount = components * 2;
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);
				jpg_header_ptr->byte_count = ByteCount;

				if(jpg_header_ptr->tag == 0x112)
				{
					ulong = jpg_header_ptr->value_offset;

					if(MotorolaOrder)
						ubyte5 = (ulong & 0xffff0000) >> 16;
					else
						ubyte5 = ulong;

					ulGlobalOrientation = ubyte5;
				}

				if (ByteCount > 4)
				{
					byte_ptr = (unsigned char *) my_malloc("byte_ptr", (ByteCount + 1));
					memset(byte_ptr, (int) NULL, (ByteCount + 1));
					fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);
					fread(byte_ptr, ByteCount, 1, rfd);
					jpg_header_ptr->data_at_offset = byte_ptr;
				}

				break;
			case 4:
				ByteCount = components * 4;
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);
				jpg_header_ptr->byte_count = ByteCount;

				if (ByteCount > 4)
				{
					if(Get16u(&ifde_ptr->tag) == TAG_SUBIFD_S)
					{
						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nValue Offset: %x", Get32u(&ifde_ptr->value_offset));
						trace(trace_buffer);

						my_fseek(rfd, Get32u(&ifde_ptr->value_offset), 0);
						temp_position = ftell(rfd);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\ntemp_position: %x", temp_position);
						trace(trace_buffer);

						for(ctr3 = 0; ctr3 < components; ctr3++)
						{
							long_byte_ptr = (unsigned long *) my_malloc("long_byte_ptr", sizeof(unsigned long));
							*long_byte_ptr = 0;
							memset(buffer, (int) NULL, 256);
							fread(long_byte_ptr, sizeof(unsigned long), 1, rfd);
							sprintf(buffer, "%-04.4x", Get32u(long_byte_ptr));

							trace("\n------Start SubIFD------");

							//save file ptr
							temp_position = ftell(rfd);
		
							memset(trace_buffer, (int) NULL, 256);
							sprintf(trace_buffer, "\ntemp_position: %x", temp_position);
							trace(trace_buffer);

							memset(trace_buffer, (int) NULL, 256);
							sprintf(trace_buffer, "\noffset to count: %x", (Get32u(long_byte_ptr)));
							trace(trace_buffer);

							memset(type_buffer, (int) NULL, 256);
							sprintf(type_buffer, "SUB_IFD%d", ctr3);

							ul_sub_ifd_array[ctr3] = Get32u(long_byte_ptr);

							if(iTrace)
							{
								printf("\nlong_byte_ptr: %x", Get32u(long_byte_ptr));
								printf("\nul_sub_ifd_array[%d]: %x", ctr3, ul_sub_ifd_array[ctr3]);
							}

							dump_jpg_image_file_directory(rfd, (Get32u(long_byte_ptr)), 0, type_buffer);
		
							//restore file ptr
							fseek(rfd, temp_position, 0);
							
							my_free("long_byte_ptr", long_byte_ptr);
							trace("\n------End SubIFD------");
						}

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nget33u_long_byte_ptr: %x", Get32u(long_byte_ptr));
						trace(trace_buffer);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nbuffer: %s", buffer);
						trace(trace_buffer);
	
						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nlong_byte_ptr: %x", *long_byte_ptr);
						trace(trace_buffer);

						//my_free("long_byte_ptr", long_byte_ptr);
					}
					else
					{
						byte_ptr = (unsigned char *) my_malloc("byte_ptr", (ByteCount + 1));
						memset(byte_ptr, (int) NULL, (ByteCount + 1));
						fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);
						fread(byte_ptr, ByteCount, 1, rfd);
						jpg_header_ptr->data_at_offset = byte_ptr;
					}
				}
				else
				{
					if(Get16u(&ifde_ptr->tag) == TAG_THUMBNAIL_IFD)
					{
						tag_thumbnail_ifd_offset = Get32u(&ifde_ptr->value_offset) + calc_maker_note_header_offset;

						if(!strncmp(upper_case(model_ptr), "NIKON D100", strlen("NIKON D100")) ||
						   !strncmp(upper_case(model_ptr), "NIKON D2H", strlen("NIKON D2H")))
						{
							trace("\n------Start TAG_THUMBNAIL_IFD------");
							//save file ptr
							temp_position = ftell(rfd);

							memset(trace_buffer, (int) NULL, 256);
							sprintf(trace_buffer, "\ntemp_position: %x", temp_position);
							trace(trace_buffer);

							memset(type_buffer, (int) NULL, 256);
							sprintf(type_buffer, "THUMBNAIL");

							//call dump_.... with offset - Why not JPG version of this? - JON JON JON
							dump_image_file_directory(rfd, Get32u(&ifde_ptr->value_offset) + calc_maker_note_header_offset, calc_maker_note_header_offset, get_new_data_header_ptr(), type_buffer);

							//restore file ptr
							fseek(rfd, temp_position, 0);
							trace("\n------End TAG_THUMBNAIL_IFD------");
						}
					}

					if(Get16u(&ifde_ptr->tag) == TAG_THUMBNAIL_OFFSET)
					{
						if(!strncmp(type_ptr, "SUB_IFD0", 8))
							tag_thumbnail_ifd_offset = Get32u(&ifde_ptr->value_offset) + calc_maker_note_header_offset; // JON JON JON

						lThumbnailOffset = Get32u(&ifde_ptr->value_offset);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nlThumbnailOffset: %ld", lThumbnailOffset);
						trace(trace_buffer);
					}

					if(Get16u(&ifde_ptr->tag) == TAG_THUMBNAIL_LENGTH)
					{
						lThumbnailLength = Get32u(&ifde_ptr->value_offset);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\nlThumbnailLength: %ld", lThumbnailLength);
						trace(trace_buffer);
					}

					if(Get16u(&ifde_ptr->tag) == TAG_EXIF_OFFSET)
					{
						trace("\n------Start EXIF------");
						//save file ptr
						temp_position = ftell(rfd);

						memset(trace_buffer, (int) NULL, 256);
						sprintf(trace_buffer, "\ntemp_position: %x", temp_position);
						trace(trace_buffer);

						memset(type_buffer, (int) NULL, 256);
						sprintf(type_buffer, "EXIF");

						//call dump_.... with offset
						dump_jpg_image_file_directory(rfd, Get32u(&ifde_ptr->value_offset) + jpg_type_offset(), jpg_type_offset(), type_buffer);

						//restore file ptr
						fseek(rfd, temp_position, 0);
						trace("\n------End EXIF------");
					}
				}
				break;
			case 5:
				ByteCount = components * 8;
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);
				jpg_header_ptr->byte_count = ByteCount;

				byte_ptr = (unsigned char *) my_malloc("byte_ptr", (ByteCount + 1));
				memset(byte_ptr, (int) NULL, (ByteCount + 1));
				memset(byte_ptr, (int) 0xFF, ByteCount);
				fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);
				fread(byte_ptr, ByteCount, 1, rfd);
				jpg_header_ptr->data_at_offset = byte_ptr;

				if(jpg_header_ptr->record_type == 1)
				{
					if(Get16u(&ifde_ptr->tag) == 0x11a)
					{
						ul_global_XResolution_offset = ifde_ptr->value_offset;
					}

					if(Get16u(&ifde_ptr->tag) == 0x11b)
					{
						ul_global_YResolution_offset = ifde_ptr->value_offset;
					}
				}

				break;
			case 10:
				ByteCount = components * 8;
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);
				jpg_header_ptr->byte_count = ByteCount;

				byte_ptr = (unsigned char *) my_malloc("byte_ptr", (ByteCount + 1));
				memset(byte_ptr, (int) NULL, (ByteCount + 1));
				fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);
				fread(byte_ptr, ByteCount, 1, rfd);
				jpg_header_ptr->data_at_offset = byte_ptr;

				break;
			case 7:
				ByteCount = components * 1;
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nByteCount: %d", ByteCount);
				trace(trace_buffer);
				jpg_header_ptr->byte_count = ByteCount;

				byte_ptr = (unsigned char *) my_malloc("byte_ptr", (ByteCount + 1));
				memset(byte_ptr, (int) NULL, (ByteCount + 1));
				fseek(rfd, Get32u(&ifde_ptr->value_offset) + offset_base, 0);
				fread(byte_ptr, ByteCount, 1, rfd);
				jpg_header_ptr->data_at_offset = byte_ptr;

				if(Get16u(&ifde_ptr->tag) == TAG_MAKER_NOTE)
				{
					trace("\n------Start Maker Notes------");

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "%x", Get32u(&ifde_ptr->value_offset));

					//save file ptr
					temp_position = ftell(rfd);

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nold_file_position: %x", temp_position);
					trace(trace_buffer);

					old_byte_order = MotorolaOrder;

					maker_note_header_offset = Get32u(&ifde_ptr->value_offset) + MAKER_NOTE_PRE_HEADER_LENGTH + jpg_type_offset();
					calc_maker_note_header_offset = maker_note_header_offset;

					memset(trace_buffer, (int) NULL, 256);
					sprintf(trace_buffer, "\nmaker_note_header_offset: %x", maker_note_header_offset);
					trace(trace_buffer);

					fseek(rfd, maker_note_header_offset, 0);

					memset(type_buffer, (int) NULL, 256);
					sprintf(type_buffer, "MAKER_NOTES");

					//call dump_.... with offset
					dump_jpg_image_file_directory(rfd, (maker_note_header_offset + inspect_image_file_header(rfd, 1)), maker_note_header_offset, type_buffer);

					MotorolaOrder = old_byte_order;

					//restore file ptr
					fseek(rfd, temp_position, 0);
					trace("\n------End Maker Notes------");
				}

				break;
			default:
				memset(trace_buffer, (int) NULL, 256);
				sprintf(trace_buffer, "\nUnknown Format: %x", Get16u(&ifde_ptr->field_type));
				trace(trace_buffer);
				break;
		}

		jpg_header_ptr->next = (struct jpg_data_header *) my_malloc("jpg_header_ptr->next", sizeof(struct jpg_data_header));
		memset(jpg_header_ptr->next, (int) NULL, sizeof(struct jpg_data_header));
		jpg_header_ptr = jpg_header_ptr->next;
		memset(jpg_header_ptr, (int) NULL, sizeof(struct jpg_data_header));
		iTagCount++;

		fseek(rfd, old_position, 0);
	}

	trace("\n-dump_jpg_image_file_directory");
	mtrace("\n-dump_jpg_image_file_directory");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
struct jpg_data_header * get_new_jpg_data_header_ptr()
{
	struct jpg_data_header *local_header_ptr = NULL;
	local_header_ptr = (struct jpg_data_header *) my_malloc("local_header_ptr", sizeof(struct jpg_data_header));
	memset(local_header_ptr, (int) NULL, sizeof(struct jpg_data_header));
	return(local_header_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
unsigned short count_tags(struct jpg_data_header *local_ptr)
{
	unsigned short tag_count = 0;

	while(local_ptr->next != NULL)
	{
		switch(local_ptr->tag)
		{
			case 0x0100:
			case 0x0101:
			case 0x0112:
				if(local_ptr->record_type == 1)
					break;
			case 0x829a:
			case 0x9209:
			case 0x8822:
			case 0x9207:
			case 0x0084:
			case 0x920a:
			case 0xa405:
			case 0x0083:
			case 0x8827:
			case 0x010f:
			case 0x0110:
			case 0x0131:
			case 0x9004:
			case 0x829d:
			case 0x0002:
			case 0x0013:
			case 0x0004:
			case 0x0005:
			case 0x0006:
			case 0x00a7:
			case 0x0007:
			case 0x0008:
			case 0x0009:
			case 0x0081:
			case 0x008b:
			case 0x008d:
			case 0x0090:
			case 0x0095:
			case 0x00a9:
			case 0x00aa:
			case 0x00ab:
			case 0xa002:
			case 0xa003:
			case 0x9204:
			case 0x9286:
				tag_count++;
				break;
			default:
				break;
		}

		local_ptr = local_ptr->next;
	}

	return(tag_count);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void output_thumbnail_old(char *filename, FILE *rfd)
{
	FILE *ofd = NULL;
	char buffer[256];
	unsigned long size = 0l;
	unsigned char *byte_ptr = NULL;
	unsigned char *file_ptr = NULL;
	unsigned char *file_tmp_ptr = NULL;

	char *local_argv[10];

	struct image_file_directory_entry ifde;

	struct exif_header
	{
		unsigned char ff;
		unsigned char soi;
		unsigned char pad;
		unsigned char marker;
		unsigned short length;
		unsigned char exif[6];
		unsigned char byte_order[2];
		unsigned short flag;
		unsigned int offset;
		unsigned short dir_count;
	};

	struct type5
	{
		unsigned long numerator;
		unsigned long denominator;
	};

	struct type5 *type5_ptr;

	struct lens
	{
		struct type5 field_1;
		struct type5 field_2;
		struct type5 field_3;
		struct type5 field_4;
	};

	struct lens *lens_ptr;

	struct exif_header *exif_header_ptr = NULL;
	struct jpg_data_header *temp_start_ptr = NULL;
	unsigned char *dir_buffer_ptr = NULL;
	unsigned char *temp_dir_buffer_ptr = NULL;
	unsigned char *data_buffer_ptr = NULL;
	unsigned char *temp_data_buffer_ptr = NULL;

	unsigned short dir_count = 0;
	int iStructSize = 0;
	unsigned int data_offset = 0;
	unsigned short short_length = 0;
	unsigned int ubyte1;
	unsigned int ubyte5;
	unsigned long ulong;
	unsigned long ulOrientation;
	int iSpecialSoftware = 0;

	trace("\n+output_thumbnail");
	mtrace("\n+output_thumbnail");

	if(strlen(software_ptr) >= strlen("Nikon Capture"))
	{
		if(!strncmp(software_ptr, "Nikon Capture", strlen("Nikon Capture")))
			iSpecialSoftware = 1;
		else
			iSpecialSoftware = 0;
	}
	else
		iSpecialSoftware = 0;

	if(!iSpecialSoftware)
	{
		exif_header_ptr = (struct exif_header *) my_malloc("exif_header_ptr", sizeof(struct exif_header));
		memset(exif_header_ptr, (int) 0xFF, sizeof(struct exif_header));
		iStructSize = sizeof(struct exif_header);

		exif_header_ptr->ff = 0xFF;
		exif_header_ptr->soi = 0xD8;
		exif_header_ptr->pad = 0xFF;
		exif_header_ptr->marker = 0xE1;
		exif_header_ptr->length = 0x00;		//set this later
		memcpy(exif_header_ptr->exif,"Exif\0\0", 6);
		memcpy(exif_header_ptr->byte_order,"II", 2);
		exif_header_ptr->flag = 0x002A;
		exif_header_ptr->offset = 0x0008;

		temp_start_ptr = start_header_ptr;

		dir_count += count_tags(temp_start_ptr);

		if(iTrace)
		{
			printf("\nfilename: %s\n", filename);
			printf("\ndir_count: %d", dir_count);
			printf("\niTagCount: %d", iTagCount);
		}

		exif_header_ptr->dir_count = dir_count;

		dir_buffer_ptr = (unsigned char *) my_malloc("dir_buffer_ptr", (12 * dir_count) + 4);
		memset(dir_buffer_ptr, (int) NULL, (12 * dir_count) + 4);
		temp_dir_buffer_ptr = dir_buffer_ptr;
		data_buffer_ptr = (unsigned char *) my_malloc("data_buffer_ptr", 1024);
		memset(data_buffer_ptr, (int) NULL, 1024);

		data_offset = sizeof(struct exif_header) + ((12 * dir_count) + 4) - 14;

		memset(dir_buffer_ptr, (int) NULL, ((12 * dir_count) + 4));
		memset(data_buffer_ptr, (int) NULL, 1024);

		short_length = ((12 * dir_count) + 4) + sizeof(struct exif_header) + 1024 - 6;
		exif_header_ptr->length = Get16u(&short_length);

		temp_data_buffer_ptr = data_buffer_ptr;

		temp_start_ptr = start_header_ptr;

		while(temp_start_ptr->next != NULL)
		{
			switch(temp_start_ptr->tag)
			{
				case 0x0100:
				case 0x0101:
				case 0x0112:
					if(temp_start_ptr->record_type == 1)
						break;
				case 0x829a:
				case 0x9209:
				case 0x8822:
				case 0x9207:
				case 0x0084:
				case 0x920a:
				case 0xa405:
				case 0x0083:
				case 0x8827:
				case 0x010f:
				case 0x0110:
				case 0x0131:
				case 0x9004:
				case 0x829d:
				case 0x0002:
				case 0x0013:
				case 0x0004:
				case 0x0005:
				case 0x0006:
				case 0x00a7:
				case 0x0007:
				case 0x0008:
				case 0x0009:
				case 0x0081:
				case 0x008b:
				case 0x008d:
				case 0x0090:
				case 0x0095:
				case 0x00a9:
				case 0x00aa:
				case 0x00ab:
				case 0xa002:
				case 0xa003:
				case 0x9204:
				case 0x9286:
					if(iTrace)
					{
						printf("tag: %x\n", temp_start_ptr->tag);
						printf("Field_type: %d\n", temp_start_ptr->field_type);
						printf("count: %d\n", temp_start_ptr->count);
						printf("value_offset: %x\n", temp_start_ptr->value_offset);
						printf("record_type: %d\n", temp_start_ptr->record_type);
					}

					ifde.tag = temp_start_ptr->tag;
					ifde.field_type = temp_start_ptr->field_type;
					ifde.count = temp_start_ptr->count;

					if(temp_start_ptr->byte_count > 4)
					{
						if(temp_start_ptr->tag == 0x829a ||
							temp_start_ptr->tag == 0x829d ||
							temp_start_ptr->tag == 0x9204 ||
							temp_start_ptr->tag == 0x920a)
						{
							type5_ptr = (struct type5 *) my_malloc("type5_ptr", sizeof(struct type5));
							memset(type5_ptr, (int) NULL, sizeof(struct type5));

							type5_ptr->numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset);
							type5_ptr->denominator = (unsigned long) Get32u((temp_start_ptr->data_at_offset + 4));

							memcpy(temp_data_buffer_ptr, type5_ptr, temp_start_ptr->byte_count);
							my_free("type5_ptr", type5_ptr);
						}
						else if(temp_start_ptr->tag == 0x0084)
						{
							lens_ptr = (struct lens *) my_malloc("lens_ptr", sizeof(struct lens));
							memset(lens_ptr, (int) NULL, sizeof(struct lens));

							lens_ptr->field_1.numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset);
							lens_ptr->field_1.denominator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 4);
							lens_ptr->field_2.numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 8);
							lens_ptr->field_2.denominator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 12);
							lens_ptr->field_3.numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 16);
							lens_ptr->field_3.denominator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 20);
							lens_ptr->field_4.numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 24);
							lens_ptr->field_4.denominator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 28);

							memcpy(temp_data_buffer_ptr, lens_ptr, temp_start_ptr->byte_count);
							my_free("lens_ptr", lens_ptr);
						}
						else
						{
							if(temp_start_ptr->tag == 0x0004)
							{
								memcpy(temp_data_buffer_ptr, "BASIC\0", strlen("BASIC\0"));
								temp_start_ptr->byte_count = strlen("BASIC\0") + 1;
								ifde.count = strlen("BASIC\0") + 1;
							}
							else if(temp_start_ptr->tag == 0x0131)
							{
								memset(buffer, (int) NULL, 256);
								sprintf(buffer, "D70Reader - v%s\0", D70_VERSION);
								memcpy(temp_data_buffer_ptr, buffer, strlen(buffer));
								temp_start_ptr->byte_count = strlen(buffer) + 1;
								ifde.count = strlen(buffer) + 1;
							}
							else
								memcpy(temp_data_buffer_ptr, temp_start_ptr->data_at_offset, temp_start_ptr->byte_count);
						}

						temp_data_buffer_ptr += temp_start_ptr->byte_count;
						ifde.value_offset = data_offset;
						data_offset += temp_start_ptr->byte_count;
					}
					else
					{
						if(temp_start_ptr->tag == 0xa405 || 
							temp_start_ptr->tag == 0x0112 ||
							temp_start_ptr->tag == 0x8822 || 
							temp_start_ptr->tag == 0x9209 || 
							temp_start_ptr->tag == 0x9207)
						{
							ulong = temp_start_ptr->value_offset;

							if(MotorolaOrder)
								ubyte5 = (ulong & 0xffff0000) >> 16;
							else
								ubyte5 = ulong;

							ifde.value_offset = ubyte5;

							if(temp_start_ptr->tag == 0x0112)
							{
								ulOrientation = ubyte5;

								if(ALLOW_ROTATE)
									ifde.value_offset = 1;
							}
						}
						else if(temp_start_ptr->tag == 0x0083)
						{
							ulong = temp_start_ptr->value_offset;

							ubyte1 = (ulong & 0xff000000) >> 24;

							ifde.value_offset = ubyte1;
						}
						else
							ifde.value_offset = temp_start_ptr->value_offset;
					}

					memcpy(temp_dir_buffer_ptr, &ifde, 12);
					temp_dir_buffer_ptr += 12;
					break;
				default:
					break;
			}
		
			temp_start_ptr = temp_start_ptr->next;
		}

		switch(temp_start_ptr->tag)
		{
			case 0x0100:
			case 0x0101:
			case 0x0112:
				if(temp_start_ptr->record_type == 1)
					break;
			case 0x829a:
			case 0x9209:
			case 0x8822:
			case 0x9207:
			case 0x0084:
			case 0x920a:
			case 0xa405:
			case 0x0083:
			case 0x8827:
			case 0x010f:
			case 0x0110:
			case 0x0131:
			case 0x9004:
			case 0x829d:
			case 0x0002:
			case 0x0013:
			case 0x0004:
			case 0x0005:
			case 0x0006:
			case 0x00a7:
			case 0x0007:
			case 0x0008:
			case 0x0009:
			case 0x0081:
			case 0x008b:
			case 0x008d:
			case 0x0090:
			case 0x0095:
			case 0x00a9:
			case 0x00aa:
			case 0x00ab:
			case 0xa002:
			case 0xa003:
			case 0x9204:
			case 0x9286:
				if(iTrace)
				{
					printf("tag: %x\n", temp_start_ptr->tag);
					printf("Field_type: %d\n", temp_start_ptr->field_type);
					printf("count: %d\n", temp_start_ptr->count);
					printf("value_offset: %x\n", temp_start_ptr->value_offset);
					printf("record_type: %d\n", temp_start_ptr->record_type);
				}

				ifde.tag = temp_start_ptr->tag;
				ifde.field_type = temp_start_ptr->field_type;
				ifde.count = temp_start_ptr->count;

				if(temp_start_ptr->byte_count > 4)
				{
					if(temp_start_ptr->tag == 0x829a ||
						temp_start_ptr->tag == 0x829d ||
						temp_start_ptr->tag == 0x9204 ||
						temp_start_ptr->tag == 0x920a)
					{
						type5_ptr = (struct type5 *) my_malloc("type5_ptr", sizeof(struct type5));
						memset(type5_ptr, (int) NULL, sizeof(struct type5));

						type5_ptr->numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset);
						type5_ptr->denominator = (unsigned long) Get32u((temp_start_ptr->data_at_offset + 4));

						memcpy(temp_data_buffer_ptr, type5_ptr, temp_start_ptr->byte_count);
						my_free("type5_ptr", type5_ptr);
					}
					else if(temp_start_ptr->tag == 0x0084)
					{
						lens_ptr = (struct lens *) my_malloc("lens_ptr", sizeof(struct lens));
						memset(lens_ptr, (int) NULL, sizeof(struct lens));

						lens_ptr->field_1.numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset);
						lens_ptr->field_1.denominator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 4);
						lens_ptr->field_2.numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 8);
						lens_ptr->field_2.denominator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 12);
						lens_ptr->field_3.numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 16);
						lens_ptr->field_3.denominator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 20);
						lens_ptr->field_4.numerator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 24);
						lens_ptr->field_4.denominator = (unsigned long) Get32u(temp_start_ptr->data_at_offset + 28);

						memcpy(temp_data_buffer_ptr, lens_ptr, temp_start_ptr->byte_count);
						my_free("lens_ptr", lens_ptr);
					}
					else
					{
						if(temp_start_ptr->tag == 0x0004)
						{
							memcpy(temp_data_buffer_ptr, "BASIC\0", strlen("BASIC\0"));
							temp_start_ptr->byte_count = strlen("BASIC\0") + 1;
							ifde.count = strlen("BASIC\0") + 1;
						}
						else if(temp_start_ptr->tag == 0x0131)
						{
							memset(buffer, (int) NULL, 256);
							sprintf(buffer, "D70Reader - v%s\0", D70_VERSION);
							memcpy(temp_data_buffer_ptr, buffer, strlen(buffer));
							temp_start_ptr->byte_count = strlen(buffer) + 1;
							ifde.count = strlen(buffer) + 1;
						}
						else
							memcpy(temp_data_buffer_ptr, temp_start_ptr->data_at_offset, temp_start_ptr->byte_count);
					}

					temp_data_buffer_ptr += temp_start_ptr->byte_count;
					ifde.value_offset = data_offset;
					data_offset += temp_start_ptr->byte_count;
				}
				else
				{
					if(temp_start_ptr->tag == 0xa405 || 
						temp_start_ptr->tag == 0x0112 ||
						temp_start_ptr->tag == 0x8822 || 
						temp_start_ptr->tag == 0x9209 || 
						temp_start_ptr->tag == 0x9207)
					{
						ulong = temp_start_ptr->value_offset;

						if(MotorolaOrder)
							ubyte5 = (ulong & 0xffff0000) >> 16;
						else
							ubyte5 = ulong;

						ifde.value_offset = ubyte5;

						if(temp_start_ptr->tag == 0x0112)
						{
							ulOrientation = ubyte5;

							if(ALLOW_ROTATE)
								ifde.value_offset = 1;
						}
					}
					else if(temp_start_ptr->tag == 0x0083)
					{
						ulong = temp_start_ptr->value_offset;

						ubyte1 = (ulong & 0xff000000) >> 24;

						ifde.value_offset = ubyte1;
					}
					else
						ifde.value_offset = temp_start_ptr->value_offset;
				}

				memcpy(temp_dir_buffer_ptr, &ifde, 12);
				temp_dir_buffer_ptr += 12;
				break;
			default:
				break;
		}
	}

	if(ALLOW_ROTATE)
	{
		if(ulOrientation == 6 || ulOrientation == 8)
		{
			file_tmp_ptr = (char *) my_malloc("file_tmp_ptr", (strlen(filename) + strlen(".tmp") + strlen(".JPG") + 1));
			memset(file_tmp_ptr, (int) NULL, (strlen(filename) + strlen(".tmp") + strlen(".JPG") + 1));
			strcat(file_tmp_ptr, filename);
			strcat(file_tmp_ptr, ".tmp.JPG");

			if(iTrace)
				printf("\nfile_tmp_ptr: %s\n", file_tmp_ptr);
		}
	}

	file_ptr = (char *) my_malloc("file_ptr", (strlen(filename) + strlen(".JPG") + 1));
	memset(file_ptr, (int) NULL, (strlen(filename) + strlen(".JPG") + 1));
	strcat(file_ptr, filename);
	strcat(file_ptr, ".JPG");

	if(iTrace)
		printf("\nfile_ptr: %s\n", file_ptr);

	byte_ptr = (char *) my_malloc("byte_ptr", lThumbnailLength);
	memset(byte_ptr, (int) NULL, lThumbnailLength);

	if(!strncmp(upper_case(model_ptr), "NIKON D100", strlen("NIKON D100")) ||
	   !strncmp(upper_case(model_ptr), "NIKON D2H", strlen("NIKON D2H")))
	{
		if(!iSpecialSoftware)
			fseek(rfd, lThumbnailOffset + calc_maker_note_header_offset + 2, 0);
		else
			fseek(rfd, lThumbnailOffset + calc_maker_note_header_offset, 0);
	}
	else
	{
		if(!iSpecialSoftware)
			fseek(rfd, lThumbnailOffset + 2, 0);
		else
			fseek(rfd, lThumbnailOffset, 0);
	}

	fread(byte_ptr, lThumbnailLength, 1, rfd);

	if(ulOrientation == 6 || ulOrientation == 8)
	{
		if(ALLOW_ROTATE)
		{
			if((ofd = fopen(file_tmp_ptr, "wb")) == NULL)
			{
				printf("\nFile not created: %s\n\n", file_tmp_ptr);
				exit(1);
			}
		}
		else
		{
			if((ofd = fopen(file_ptr, "wb")) == NULL)
			{
				printf("\nFile not created: %s\n\n", file_ptr);
				exit(1);
			}
		}
	}
	else
	{
		if((ofd = fopen(file_ptr, "wb")) == NULL)
		{
			printf("\nFile not created: %s\n\n", file_ptr);
			exit(1);
		}
	}

	if(!iSpecialSoftware)
	{
		if((size = fwrite(exif_header_ptr, (sizeof(struct exif_header) - 2), 1, ofd)) == 0)
		{
			printf("\nFile not written: %s\n\n", file_ptr);
			exit(1);
		}


		if((size = fwrite(dir_buffer_ptr, ((12 * dir_count) + 4), 1, ofd)) == 0)
		{
			printf("\nFile not written: %s\n\n", file_ptr);
			exit(1);
		}

		if((size = fwrite(data_buffer_ptr, 1024, 1, ofd)) == 0)
		{
			printf("\nFile not written: %s\n\n", file_ptr);
			exit(1);
		}
	}

	if((size = fwrite(byte_ptr, lThumbnailLength, 1, ofd)) == 0)
	{
		printf("\nFile not written: %s\n\n", file_ptr);
		exit(1);
	}

	printf("Created file: %s\n", file_ptr);

	fclose(ofd);

	if(ALLOW_ROTATE)
	{
		if(ulOrientation == 6 || ulOrientation == 8)
		{
			local_argv[0] = "jpgtran";
			local_argv[1] = "-rot";
	
			if(ulOrientation == 6)
				local_argv[2] = "90";
			else
				local_argv[2] = "270";
	
			local_argv[3] = "-copy";
			local_argv[4] = "all";
			local_argv[5] = file_tmp_ptr;
			local_argv[6] = file_ptr;
			local_argv[7] = NULL;

			jpegtran(7, local_argv);

			if(remove(file_tmp_ptr))
				printf("File not deleted: %s\n", file_tmp_ptr);
		}
	}

	my_free("byte_ptr", byte_ptr);
	my_free("file_ptr", file_ptr);

	if(file_tmp_ptr != NULL)
		my_free("file_tmp_ptr", file_tmp_ptr);

	if(exif_header_ptr != NULL)
		my_free("exif_header_ptr", exif_header_ptr);

	if(dir_buffer_ptr != NULL)
		my_free("dir_buffer_ptr", dir_buffer_ptr);

	if(data_buffer_ptr != NULL)
		my_free("data_buffer_ptr", data_buffer_ptr);

	trace("\n-output_thumbnail");
	mtrace("\n-output_thumbnail");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void free_jpg_data_header_list(struct jpg_data_header *header_ptr)
{
	struct jpg_data_header *temp_header_ptr = NULL;

	trace("\n+free_jpg_data_header_list");
	mtrace("\n+free_jpg_data_header_list");

	while(header_ptr->next != NULL)
	{
		temp_header_ptr = header_ptr;
		header_ptr = header_ptr->next;
		my_free("temp_header_ptr", temp_header_ptr);
	}

	my_free("header_ptr", header_ptr);
	trace("\n-free_jpg_data_header_list");
	mtrace("\n-free_jpg_data_header_list");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void ProcessFileJPG(int argc, char *filename)
{
	FILE *rfd = NULL;
	char buffer[256];
	char *temp_ptr = NULL;
	unsigned long offset_ifd = 0l;
	int ctr1 = 0;
	struct data_header_ptr *temp_header_start_ptr = NULL;
	struct data_header_ptr *free_header_start_ptr = NULL;
	struct html_header *free_html_header_start_ptr = NULL;

	trace("\n+ProcessFileJPG");
	//mtrace("\n+ProcessFileJPG");

	temp_ptr = get_extension(filename);

	memset(trace_buffer, (int) NULL, 256);
	sprintf(trace_buffer, "\nFile Extension: %s", temp_ptr);
	trace(trace_buffer);

	if(!strncmp(upper_case(temp_ptr), "JPG", 3))
	{
		iFile_Type = 0;
		my_jpg_exit("Only files of type NEF can be processed using opton -j");
	}
	else
	{
		iJPG_Type = 99;
		iFile_Type = 1;
	}

	if((rfd = fopen(filename, "rb")) == NULL)
	{
		printf("\nFile not opened: %s\n\n", filename);
		return;
	}

	memset(buffer, (int) NULL, 256);
	sprintf(buffer, "IFD%d", ctr1++);

	offset_ifd = inspect_image_file_header(rfd, iFile_Type);
	jpg_header_ptr = get_new_jpg_data_header_ptr();
	start_header_ptr = jpg_header_ptr;
	dump_jpg_image_file_directory(rfd, offset_ifd, jpg_type_offset(), buffer);

	if(iThumbnail || iThumbnailOld)
	{
		if(iFile_Type)
		{
			if(iThumbnail)
				output_thumbnail(filename, rfd);
			else
				output_thumbnail_old(filename, rfd);
		}
	}

	fclose(rfd);

	free_jpg_data_header_list(start_header_ptr);

	trace("\n-ProcessFileJPG");
	//mtrace("\n-ProcessFileJPG");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int check_for_exif_data(FILE *rfd)
{
	int ctr1 = 0;
	int iFoundEXIF = 0;

	long position = 0l;
	long old_position = 0l;
	int oldMotorolaOrder = 0;

	struct marker
	{
		unsigned char pad;
		unsigned char marker;
		unsigned short length;
		char test_data[6];
	};

	struct marker *temp_ptr;

	oldMotorolaOrder = MotorolaOrder;
	MotorolaOrder = 1;

	old_position = ftell(rfd);

	temp_ptr = (struct marker *) my_malloc("temp_ptr", sizeof(struct marker));
	memset(temp_ptr, (int) NULL, sizeof(struct marker));

	fseek(rfd, 2, 0);

	do
	{
		ctr1++;
		position = ftell(rfd);

		memset(temp_ptr, (int) NULL, sizeof(struct marker));
		fread(temp_ptr, sizeof(struct marker), 1, rfd);

		if(!strncmp(temp_ptr->test_data, "Exif\0\0", 6))
		{
			iFoundEXIF = 1;
			break;
		}
		else
			fseek(rfd, Get16u(&temp_ptr->length) - sizeof(struct marker) + 2, 1);

		if(temp_ptr->marker == 0xda)
			break;
	}
	while(strncmp(temp_ptr->test_data, "Exif\0\0", 6));	

	fseek(rfd, old_position, 0);

	MotorolaOrder = oldMotorolaOrder;

	my_free("temp_ptr", temp_ptr);

	return(iFoundEXIF);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void check_for_config()
{
	int ctr1 = 0;
	unsigned long size = 0l;
	struct stat buf;
	char buffer[256];
	char *file_ptr = NULL;
	char *data_buffer_ptr = NULL;
	FILE *ofd = NULL;
	FILE *rfd = NULL;
	struct wanted_tags *local_header_ptr = NULL;
	struct all_wanted_tags *local_header_all_ptr = NULL;

	trace("\n+check_for_config");
	mtrace("\n+check_for_config");

	if(iSystemType)
	{
		file_ptr = (char *) my_malloc("file_ptr", (strlen(file_sys_dir_ptr) + strlen("\\d70reader.conf") + 1));
		memset(file_ptr, (int) NULL, (strlen(file_sys_dir_ptr) + strlen("\\d70reader.conf") + 1));
		strcat(file_ptr, file_sys_dir_ptr);
		strcat(file_ptr, "\\d70reader.conf");
	}
	else
	{
		file_ptr = (char *) my_malloc("file_ptr", (strlen(file_sys_dir_ptr) + strlen("/d70reader.conf") + 1));
		memset(file_ptr, (int) NULL, (strlen(file_sys_dir_ptr) + strlen("/d70reader.conf") + 1));
		strcat(file_ptr, file_sys_dir_ptr);
		strcat(file_ptr, "/d70reader.conf");
	}

	if(stat(file_ptr, &buf) == (-1))
	{
		if((ofd = fopen(file_ptr, "w")) == NULL)
		{
			printf("\nFile not created: %s\n", file_ptr);
			printf("\nThis must be done by the 'root' user.\n\n");
			exit(0);
		}

		//fputs("D70ReaderProgramVersion=yes", ofd);
		//fputs("Filename=yes", ofd);

		for(ctr1 = 0; ctr1 < SizeTagTable; ctr1++)
		{
			switch(TagTable[ctr1].Tag)
			{
				case 0x0112:
				case 0x829a:
				case 0x9209:
				case 0x8822:
				case 0x9207:
				case 0x0084:
				case 0x920a:
				case 0xa405:
				case 0x0083:
				case 0x8827:
				case 0x010f:
				case 0x0110:
				case 0x0131:
				case 0x9004:
				case 0x829d:
				case 0x0002:
				case 0x0013:
				case 0x0004:
				case 0x0005:
				case 0x0006:
				case 0x00a7:
				case 0x0007:
				case 0x0008:
				case 0x0009:
				case 0x0081:
				case 0x008b:
				case 0x008d:
				case 0x0090:
				case 0x0095:
				case 0x00a9:
				case 0x00aa:
				case 0x00ab:
				case 0x0100:
				case 0x0101:
				case 0xa002:
				case 0xa003:
				case 0x9204:
				case 0x9286:

					data_buffer_ptr = (char *) my_malloc("data_buffer_ptr", (strlen(strip(TagTable[ctr1].Desc)) + 6));
					memset(data_buffer_ptr, (int) NULL, (strlen(strip(TagTable[ctr1].Desc)) + 6));
					sprintf(data_buffer_ptr, "%s=yes\n", strip(TagTable[ctr1].Desc));

					if((size = fwrite(data_buffer_ptr, (strlen(strip(TagTable[ctr1].Desc)) + 5), 1, ofd)) == 0)
					{
						printf("\nFile not written: %s\n\n", file_ptr);
						return;
					}

					store_tags(data_buffer_ptr);
					store_all_tags(data_buffer_ptr);
					my_free("data_buffer_ptr", data_buffer_ptr);

					break;
				default:
					break;
			}
		}

		fclose(ofd);

#ifdef __UNIX_ONLY__
		chmod(file_ptr, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#endif
	}
	else
	{
		if((rfd = fopen(file_ptr, "r")) == NULL)
		{
			printf("\nFile not opened: %s\n\n", file_ptr);
			return;
		}

		do
		{
			memset(buffer, (int) NULL, 256);
			data_buffer_ptr = fgets(buffer, 256, rfd);

			if(data_buffer_ptr == NULL)
				break;

			store_tags(buffer);
			store_all_tags(buffer);
		}
		while(data_buffer_ptr != NULL);

		if(iTrace)
		{
			ctr1 = 0;

			printf("\nDumping wanted_tags list\n");

			local_header_ptr = start_wanted_tags_ptr;

			do
			{
				printf("\n%d - Tag: %x", ctr1++, local_header_ptr->tag);
				local_header_ptr = local_header_ptr->next;
			}
			while(local_header_ptr != NULL); 

			ctr1 = 0;

			printf("\nDumping all_wanted_tags list\n");

			local_header_all_ptr = start_all_wanted_tags_ptr;

			do
			{
				printf("\n%d - Tag: %x", ctr1++, local_header_all_ptr->tag);
				local_header_all_ptr = local_header_all_ptr->next;
			}
			while(local_header_all_ptr != NULL); 
		}
	}

	my_free("file_ptr", file_ptr);

	trace("\n-check_for_config");
	mtrace("\n-check_for_config");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void store_tags(char *local_ptr)
{
	int ctr1 = 0;
	char *temp_ptr = NULL;
	
	trace("\n+store_tags");
	//mtrace("\n+store_tags");

	temp_ptr = strdup(local_ptr);

	temp_ptr = get_description(temp_ptr);

	for(ctr1 = 0; ctr1 < SizeTagTable; ctr1++)
	{
		if(!strncmp(TagTable[ctr1].Desc, temp_ptr, strlen(temp_ptr)) && (strlen(strip(TagTable[ctr1].Desc)) == strlen(temp_ptr)))
		{
			if(tag_is_yes(local_ptr))
			{
				if(main_wanted_tags_ptr == NULL)
				{
					main_wanted_tags_ptr = get_new_main_wanted_tags_ptr();
					main_wanted_tags_ptr->next = NULL;
					main_wanted_tags_ptr->tag = TagTable[ctr1].Tag;
					start_wanted_tags_ptr = main_wanted_tags_ptr;
				}
				else
				{
					main_wanted_tags_ptr->next = get_new_main_wanted_tags_ptr();
					main_wanted_tags_ptr =  main_wanted_tags_ptr->next;
					main_wanted_tags_ptr->next = NULL;
					main_wanted_tags_ptr->tag = TagTable[ctr1].Tag;
				}

				trace("\n-store_tags");
				//mtrace("\n-store_tags");
				return;
			}
		}
	}

	trace("\n-store_tags");
	//mtrace("\n-store_tags");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void store_all_tags(char *local_ptr)
{
	int ctr1 = 0;
	char *temp_ptr = NULL;
	
	trace("\n+store_all_tags");
	//mtrace("\n+store_all_tags");

	temp_ptr = strdup(local_ptr);

	temp_ptr = get_description(temp_ptr);

	for(ctr1 = 0; ctr1 < SizeTagTable; ctr1++)
	{
		if(!strncmp(TagTable[ctr1].Desc, temp_ptr, strlen(temp_ptr)) && (strlen(strip(TagTable[ctr1].Desc)) == strlen(temp_ptr)))
		{
			if(main_all_wanted_tags_ptr == NULL)
			{
				main_all_wanted_tags_ptr = get_new_main_all_wanted_tags_ptr();
				main_all_wanted_tags_ptr->next = NULL;
				main_all_wanted_tags_ptr->tag = TagTable[ctr1].Tag;
				start_all_wanted_tags_ptr = main_all_wanted_tags_ptr;
			}
			else
			{
				main_all_wanted_tags_ptr->next = get_new_main_all_wanted_tags_ptr();
				main_all_wanted_tags_ptr =  main_all_wanted_tags_ptr->next;
				main_all_wanted_tags_ptr->next = NULL;
				main_all_wanted_tags_ptr->tag = TagTable[ctr1].Tag;
			}

			trace("\n-store_all_tags");
			//mtrace("\n-store_all_tags");
			return;
		}
	}

	trace("\n-store_all_tags");
	//mtrace("\n-store_all_tags");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
struct all_wanted_tags *get_new_main_all_wanted_tags_ptr()
{
	struct all_wanted_tags *local_header_ptr = NULL;
	local_header_ptr = (struct all_wanted_tags *) my_malloc("local_header_ptr", sizeof(struct all_wanted_tags));
	memset(local_header_ptr, (int) NULL, sizeof(struct all_wanted_tags));
	return(local_header_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
struct wanted_tags *get_new_main_wanted_tags_ptr()
{
	struct wanted_tags *local_header_ptr = NULL;
	local_header_ptr = (struct wanted_tags *) my_malloc("local_header_ptr", sizeof(struct wanted_tags));
	memset(local_header_ptr, (int) NULL, sizeof(struct wanted_tags));
	return(local_header_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int tag_is_yes(char *local_ptr)
{
	int iWanted = 0;
	char *temp_ptr = NULL;
	char *final_ptr = NULL;
	char *token = NULL;

	trace("\n+tag_is_yes");
	//mtrace("\n+tag_is_yes");

	temp_ptr = strdup(local_ptr);

	token = strtok(temp_ptr, "=");

	do
	{
		if(token != NULL)
			final_ptr = token;

		token = strtok(NULL, "=");
	}
	while(token != NULL);

	if(!strncmp(upper_case(final_ptr), "YES", strlen("YES")))
		iWanted = 1;
	else
		iWanted = 0;

	trace("\n-tag_is_yes");
	//mtrace("\n-tag_is_yes");
	return(iWanted);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int is_tag_printed(unsigned short tag)
{
	int iWanted = 0;

	struct wanted_tags *local_header_ptr = NULL;

	trace("\n+is_tag_printed");
	//mtrace("\n+is_tag_printed");

	local_header_ptr = start_wanted_tags_ptr;

	if(iTrace)
		printf("\ntag: %x", tag);

	if(local_header_ptr != NULL)
	{
		do
		{
			if(local_header_ptr->tag == tag)
			{
				iWanted = 1;
				break;
			}
			else
				iWanted = 0;

			local_header_ptr = local_header_ptr->next;
		}
		while(local_header_ptr != NULL); 
	}

	if(iTrace)
		printf("\niWanted: %d", iWanted);

	trace("\n-is_tag_printed");
	//mtrace("\n-is_tag_printed");
	return(iWanted);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
char *get_description(char *str_input)
{
	char *temp_ptr = NULL;
	char *token = NULL;

	trace("\n+get_description");
	//mtrace("\n+get_description");

	temp_ptr = strdup(str_input);

	token = strtok(temp_ptr, "=");

	trace("\n-get_description");
	//mtrace("\n-get_description");
	return(token);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void output_js(FILE *ofd, int iFirstTime, int iIndex)
{
	int ctr1 = 0;
	struct all_wanted_tags *local_header_all_ptr = NULL;
	struct html_header *local_header_start_ptr = NULL;

	trace("\n+output_js");
	//mtrace("\n+output_js");

	if(iFirstTime)
	{
		fprintf(ofd, "aExifData = new Array()");
		fprintf(ofd, "\naExifDataHeader = new Array()");
		fprintf(ofd, "\naExifDataHeader[0] = new Array(\"D70ReaderProgramVersion\",\"Filename\"");

		local_header_all_ptr = start_all_wanted_tags_ptr;

		do
		{
			if(is_tag_printed(local_header_all_ptr->tag))
			{
				if(iTrace)
					fprintf(ofd, "\n%d - %x - \"%s\",", ctr1++, local_header_all_ptr->tag, get_description_label(local_header_all_ptr->tag, 1));

				fprintf(ofd, ",\"%s\"", get_description_label(local_header_all_ptr->tag, 1));
			}

			local_header_all_ptr = local_header_all_ptr->next;
		}
		while(local_header_all_ptr != NULL); 

		fprintf(ofd, ")\n");

		iFirstTime = 0;
	}

	fprintf(ofd, "aExifData[%d] = new Array(\"%s\",\"%s\"", iIndex, D70_VERSION, get_description_value(0xFFFE));

	local_header_all_ptr = start_all_wanted_tags_ptr;

	do
	{
		if(is_tag_printed(local_header_all_ptr->tag))
		{
			if(iTrace)
				fprintf(ofd, "\n%d - %x - \"%s\",", ctr1++, local_header_all_ptr->tag, get_description_value(local_header_all_ptr->tag));

			fprintf(ofd, ",\"%s\"", get_description_value(local_header_all_ptr->tag));
		}

		local_header_all_ptr = local_header_all_ptr->next;
	}
	while(local_header_all_ptr != NULL); 

	fprintf(ofd, ")\n");

	trace("\n-output_js");
	//mtrace("\n-output_js");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
char *get_description_label(unsigned short Tag, int iStrip)
{
	int ctr1 = 0;
	char *local_ptr = NULL;
	char *final_ptr = NULL;

	for (ctr1 = 0; ;ctr1++)
	{
		if (ctr1 >= SizeTagTable )
		{
			memset(trace_buffer, (int) NULL, 256);
			sprintf(trace_buffer, "    Unknown Tag %04x Value = ", Tag);
			trace(trace_buffer);

			local_ptr = (char *) my_malloc("local_ptr", (strlen(trace_buffer) + 1));
			memset(local_ptr, (int) NULL, (strlen(trace_buffer) + 1));
			strcpy(local_ptr, trace_buffer);
			break;
		}

		if (TagTable[ctr1].Tag == Tag)
		{
			memset(trace_buffer, (int) NULL, 256);
			sprintf(trace_buffer, "%s", TagTable[ctr1].Desc); 

			local_ptr = (char *) my_malloc("local_ptr", (strlen(trace_buffer) + 1));
			memset(local_ptr, (int) NULL, (strlen(trace_buffer) + 1));
			strcpy(local_ptr, trace_buffer);

			memset(trace_buffer, (int) NULL, 256);
			sprintf(trace_buffer, "\n%s",TagTable[ctr1].Desc); 
			trace(trace_buffer);
			break;
		}
	}

	if(iStrip)
		final_ptr = strdup(strip(local_ptr));
	else
		final_ptr = strdup(local_ptr);

	my_free("local_ptr", local_ptr);

	return(final_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
char * get_description_value(unsigned short Tag)
{
	char *local_ptr = NULL;
	char *final_ptr = NULL;
	struct html_header *local_header_start_ptr = NULL;

	trace("\n+get_description_value");
	mtrace("\n+get_description_value");

	local_header_start_ptr = html_header_start_ptr;

	while(local_header_start_ptr->next != NULL)
	{
		if(iTrace)
		{
			printf("\nlocal_header_start_ptr Tag: %x", local_header_start_ptr->Tag);
			printf("\nlocal_header_start_ptr Value: %s", local_header_start_ptr->Value);
		}

		if(local_header_start_ptr->Tag == Tag)
		{
			if(local_header_start_ptr->Value == NULL)
			{
				local_ptr = (char *) my_malloc("local_ptr", 3);
				memset(local_ptr, (int) NULL, 3);
				//strcpy(local_ptr, " ");
			}
			else
			{
				local_ptr = (char *) my_malloc("local_ptr", (strlen(local_header_start_ptr->Value) + 1));
				memset(local_ptr, (int) NULL, (strlen(local_header_start_ptr->Value) + 1));
				strcpy(local_ptr, local_header_start_ptr->Value);
			}

			break;
		}

		local_header_start_ptr = local_header_start_ptr->next;
	}

	if(local_header_start_ptr->Tag == Tag)
	{
		if(local_header_start_ptr->Value == NULL)
		{
			local_ptr = (char *) my_malloc("local_ptr", 3);
			memset(local_ptr, (int) NULL, 3);
			//strcpy(local_ptr, " ");
		}
		else
		{
			local_ptr = (char *) my_malloc("local_ptr", (strlen(local_header_start_ptr->Value) + 1));
			memset(local_ptr, (int) NULL, (strlen(local_header_start_ptr->Value) + 1));
			strcpy(local_ptr, local_header_start_ptr->Value);
		}
	}

	if(local_ptr == NULL)
	{
		local_ptr = (char *) my_malloc("local_ptr", 3);
		memset(local_ptr, (int) NULL, 3);
		//strcpy(local_ptr, " ");
	}

	final_ptr = strdup(local_ptr);

	my_free("local_ptr", local_ptr);

	trace("\n-get_description_value");
	mtrace("\n-get_description_value");
	return(final_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int precision(char *str_value)
{
	int iPercision = 0;
	char *temp_ptr = NULL;
	char *token1 = NULL;
	char *token2 = NULL;

	trace("\n+precision");
	//mtrace("\n+precision");

	temp_ptr = strdup(str_value);

	token1 = strtok(temp_ptr, ".");
	token2 = strtok(NULL, ".");

	if(token2[0] == '0')
		iPercision = 0;
	else
		iPercision = 2;

	trace("\n-precision");
	//mtrace("\n-precision");

	return(iPercision);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
struct xmp_packet *chk_get_xmp_data(FILE *rfd)
{
	int ctr1 = 0;
	int iFoundHTTP = 0;
	char * data_ptr = NULL;
	struct xmp_packet *xmp_ptr = NULL;
	
	unsigned short length = 0;
	long position = 0l;
	long old_position = 0l;
	int oldMotorolaOrder = 0;

	struct marker
	{
		unsigned char pad;
		unsigned char marker;
		unsigned short length;
		char test_data[6];
	};

	struct marker *temp_ptr = NULL;

	old_position = ftell(rfd);

	oldMotorolaOrder = MotorolaOrder;
	MotorolaOrder = 1;

	temp_ptr = (struct marker *) my_malloc("temp_ptr", sizeof(struct marker));
	memset(temp_ptr, (int) NULL, sizeof(struct marker));

	fseek(rfd, 2, 0);

	do
	{
		ctr1++;
		position = ftell(rfd);

		memset(temp_ptr, (int) NULL, sizeof(struct marker));
		fread(temp_ptr, sizeof(struct marker), 1, rfd);

		length = Get16u(&temp_ptr->length);

		if(!strncmp(temp_ptr->test_data, "http:", strlen("http:")))
		{
			iFoundHTTP = 1;
			break;
		}
		else
			fseek(rfd, Get16u(&temp_ptr->length) - sizeof(struct marker) + 2, 1);

		if(temp_ptr->marker == 0xda)
			break;
	}
	while(strncmp(temp_ptr->test_data, "http:", strlen("http:")));	

	if(iFoundHTTP)
	{
		data_ptr = (unsigned char *) my_malloc("data_ptr", (Get16u(&temp_ptr->length) + 1));
		memset(data_ptr, (int) NULL, (Get16u(&temp_ptr->length) + 1));

		fseek(rfd, position + 4, 0);
		fread(data_ptr, Get16u(&temp_ptr->length), 1, rfd);

		xmp_ptr = (struct xmp_packet *) my_malloc("xmp_ptr", sizeof(struct xmp_packet));
		memset(xmp_ptr, (int) NULL, sizeof(struct xmp_packet));
		xmp_ptr->length = Get16u(&temp_ptr->length);
		xmp_ptr->value = data_ptr;
	}

	MotorolaOrder = oldMotorolaOrder;
	fseek(rfd, old_position, 0);

	return(xmp_ptr);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void replace_description_value(unsigned short Tag, char *text_ptr, char *val_ptr)
{
	int iFoundIt = 0;
	char *local_ptr = NULL;
	struct html_header *local_header_start_ptr = NULL;

	trace("\n+replace_description_value");
	mtrace("\n+replace_description_value");

	local_header_start_ptr = html_header_start_ptr;

	while(local_header_start_ptr->next != NULL)
	{
		if(iTrace)
		{
			printf("\nlocal_header_start_ptr Tag: %x", local_header_start_ptr->Tag);
			printf("\nlocal_header_start_ptr Value: %s", local_header_start_ptr->Value);
		}

		if(local_header_start_ptr->Tag == Tag)
		{
			if(local_header_start_ptr->Value != NULL)
			{
				my_free("local_header_start_ptr->Value)", local_header_start_ptr->Value);

				local_ptr = (char *) my_malloc("local_ptr", (strlen(val_ptr) + 1));
				memset(local_ptr, (int) NULL, (strlen(val_ptr) + 1));
				strcpy(local_ptr, val_ptr);
				iFoundIt = 1;
			}

			break;
		}

		local_header_start_ptr = local_header_start_ptr->next;
	}

	if(!iFoundIt)
	{
		if(local_header_start_ptr->Tag == Tag)
		{
			if(local_header_start_ptr->Value != NULL)
			{
				my_free("local_header_start_ptr->Value", local_header_start_ptr->Value);

				local_ptr = (char *) my_malloc("local_ptr", (strlen(val_ptr) + 1));
				memset(local_ptr, (int) NULL, (strlen(val_ptr) + 1));
				strcpy(local_ptr, val_ptr);
				iFoundIt = 1;
			}
		}
	}

	if(!iFoundIt)
	{
		if(iTrace)
			debug_printf("%x %s %s", Tag, text_ptr, val_ptr);
		else
			my_printf("%s %s", Tag, text_ptr, val_ptr);
	}

	trace("\n-replace_description_value");
	mtrace("\n-replace_description_value");
	return;
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int get_sof0_data(FILE *rfd)
{
	int ctr1 = 0;
	int iFoundEXIF = 0;

	long position = 0l;
	long old_position = 0l;
	int oldMotorolaOrder = 0;
	int height = LARGE_IMAGE_HEIGHT;
	int width = LARGE_IMAGE_WIDTH;
	char str_width[10];
	char str_height[10];

	unsigned char m_percision;
	unsigned short m_height;
	unsigned short m_width;

	struct marker
	{
		unsigned char pad;
		unsigned char marker;
		unsigned short length;
	};

	struct marker *temp_ptr;

	oldMotorolaOrder = MotorolaOrder;
	MotorolaOrder = 1;

	old_position = ftell(rfd);

	temp_ptr = (struct marker *) my_malloc("temp_ptr", sizeof(struct marker));
	memset(temp_ptr, (int) NULL, sizeof(struct marker));

	if(!iFile_Type)
		fseek(rfd, 2, 0);
	else
	{
		if(!strncmp(upper_case(model_ptr), "NIKON D100", strlen("NIKON D100")) ||
		   !strncmp(upper_case(model_ptr), "NIKON D2H", strlen("NIKON D2H")))
		{
			fseek(rfd, lThumbnailOffset + calc_maker_note_header_offset + 2, 0);
		}
		else
		{
			fseek(rfd, lThumbnailOffset + 2, 0);
		}
	}

	do
	{
		ctr1++;
		position = ftell(rfd);

		memset(temp_ptr, (int) NULL, sizeof(struct marker));
		fread(temp_ptr, sizeof(struct marker), 1, rfd);

		if(temp_ptr->marker == 0xc0)
		{
			fread(&m_percision, sizeof(unsigned char), 1, rfd);
			fread(&m_height, sizeof(unsigned short), 1, rfd);
			fread(&m_width, sizeof(unsigned short), 1, rfd);
			iFoundEXIF = 1;
			break;
		}
		else
			fseek(rfd, Get16u(&temp_ptr->length) - sizeof(struct marker) + 2, 1);

		if(temp_ptr->marker == 0xda)
			break;
	}
	while(temp_ptr->marker != 0xc0);	

	fseek(rfd, old_position, 0);

	if(iFoundEXIF)
	{
		iGlobalImageWidth = Get16u(&m_width);
		iGlobalImageHeight = Get16u(&m_height);

		if(iWeb)
		{
			if((iGlobalImageWidth != 0) && (iGlobalImageHeight != 0))
			{
				if(iTrace)
					printf("\nGlobalOrientationPtr: %s", GlobalOrientationPtr);

				if((!strcmp(GlobalOrientationPtr, "6") || !strcmp(GlobalOrientationPtr, "8")) || 
					(!strcmp(GlobalOrientationPtr, "1") && (iGlobalImageHeight > iGlobalImageWidth)) ||
					(!strcmp(GlobalOrientationPtr, "X") && (iGlobalImageHeight > iGlobalImageWidth)))
				{
					if(iTrace)
						printf("\ndisplay_data - vertical");

					height = LARGE_IMAGE_LENGTH;

					if(!strcmp(GlobalOrientationPtr, "1") || !strcmp(GlobalOrientationPtr, "X"))
						width = (height * iGlobalImageWidth) / iGlobalImageHeight;
					else
					{
						if(iGlobalImageHeight > iGlobalImageWidth)
							width = (height * iGlobalImageWidth) / iGlobalImageHeight;
						else
							width = (height * iGlobalImageHeight) / iGlobalImageWidth;
					}				}
				else
				{
					if(iTrace)
						printf("\ndisplay_data - horizontal");

					width = LARGE_IMAGE_LENGTH;
					height = (width * iGlobalImageHeight) / iGlobalImageWidth;
				}

				memset(str_width, (int) NULL, 10);
				memset(str_height, (int) NULL, 10);

				sprintf(str_width, "%d", width);
				sprintf(str_height, "%d", height);

				if(iTrace)
				{
					printf("\nwidth: %s", str_width);
					printf("\nheight: %s", str_height);
				}

				replace_description_value(0xA002, "ExifImageWidth             ", str_width);
				replace_description_value(0xA003, "ExifImageLength            ", str_height);
			}
		}
	}

	MotorolaOrder = oldMotorolaOrder;

	my_free("temp_ptr", temp_ptr);

	return(iFoundEXIF);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
long get_marker_offset(FILE *rfd, unsigned short us_marker)
{
	int ctr1 = 0;
	int iFoundDB = 0;

	long position = 0l;
	long old_position = 0l;
	int oldMotorolaOrder = 0;

	struct marker
	{
		unsigned char pad;
		unsigned char marker;
		unsigned short length;
	};

	struct marker *temp_ptr;

	oldMotorolaOrder = MotorolaOrder;
	MotorolaOrder = 1;

	old_position = ftell(rfd);

	temp_ptr = (struct marker *) my_malloc("temp_ptr", sizeof(struct marker));
	memset(temp_ptr, (int) NULL, sizeof(struct marker));

	if(!iFile_Type)
		fseek(rfd, 2, 0);
	else
	{
		if(!strncmp(upper_case(model_ptr), "NIKON D100", strlen("NIKON D100")) ||
		   !strncmp(upper_case(model_ptr), "NIKON D2H", strlen("NIKON D2H")))
		{
			fseek(rfd, lThumbnailOffset + calc_maker_note_header_offset + 2, 0);
		}
		else
		{
			fseek(rfd, lThumbnailOffset + 2, 0);
		}
	}

	do
	{
		ctr1++;
		position = ftell(rfd);

		memset(temp_ptr, (int) NULL, sizeof(struct marker));
		fread(temp_ptr, sizeof(struct marker), 1, rfd);

		if(temp_ptr->marker == us_marker)
		{
			iFoundDB = 1;
			break;
		}
		else
			fseek(rfd, Get16u(&temp_ptr->length) - sizeof(struct marker) + 2, 1);

		if(temp_ptr->marker == 0xda)
			break;
	}
	while(temp_ptr->marker != us_marker);

	fseek(rfd, old_position, 0);

	if(iFoundDB)
		return(position);
	else
		return(0);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void remove_exif(char *filename, FILE *rfd)
{
	unsigned long size = 0l;
	FILE *ofd = NULL;
	char *buf_ptr = NULL;
	char *file_ptr = NULL;
	long position = 0l;
	long db_position = 0l;
	long c0_position = 0l;

	db_position = get_marker_offset(rfd, 0x00db);
	c0_position = get_marker_offset(rfd, 0x00c0);

	if((c0_position != 0) && (c0_position < db_position))
		position = c0_position;
	else
		position = db_position;

	if(position)
	{
		fseek(rfd, position, 1);

		buf_ptr = (char *) my_malloc("buf_ptr", (lFileSize - position + 1));
		memset(buf_ptr, (int) NULL, (lFileSize - position + 1));
		fread(buf_ptr, (lFileSize - position), 1, rfd);

		file_ptr = (char *) my_malloc("file_ptr", (strlen(filename) + strlen("NOEXIF_") + 1));
		memset(file_ptr, (int) NULL, (strlen(filename) + strlen("NOEXIF_") + 1));
		sprintf(file_ptr, "%s%s", "NOEXIF_", filename);

		if((ofd = fopen(file_ptr, "wb")) == NULL)
		{
			printf("\nFile not opened: %s\n\n", file_ptr);
			return;
		}

		fputc(0xff, ofd);
		fputc(0xd8, ofd);

		if((size = fwrite(buf_ptr, (lFileSize - position), 1, ofd)) == 0)
		{
			printf("\nFailed to write to file: %s", file_ptr);
			return;
		}

		fclose(ofd);

		printf("\nCreated file: %s", file_ptr);

		my_free("buf_ptr", buf_ptr);
		my_free("file_ptr", file_ptr);
	}
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void output_thumbnail(char *filename, FILE *rfd)
{
	FILE *ofd = NULL;
	long position = 0l;
	unsigned long size = 0l;
	unsigned char *byte_ptr = NULL;
	unsigned char *file_ptr = NULL;
	unsigned char *file_tmp_ptr = NULL;
	int ctr1 = 0;

	char *local_argv[10];

	struct exif_header
	{
		unsigned char ff;
		unsigned char soi;
		unsigned char pad;
		unsigned char marker;
		unsigned short length;
		unsigned char exif[6];
		unsigned char byte_order[2];
		unsigned short flag;
		unsigned int offset;
	};

	struct exif_header *exif_header_ptr = NULL;
	unsigned char *dir_buffer_ptr = NULL;
	unsigned char *temp_dir_buffer_ptr = NULL;
	unsigned char *data_buffer_ptr = NULL;
	unsigned char *temp_data_buffer_ptr = NULL;

	unsigned short dir_count = 0;
	int iStructSize = 0;
	unsigned int data_offset = 0;
	unsigned short short_length = 0;
	unsigned long ulOrientation = ulGlobalOrientation;
	int iSpecialSoftware = 0;
	long lHeaderEndOffset = 0;
	long lLength = 0;
	unsigned long ul_local_sub_ifd_array[4] = { 0,0,0,0 };

	trace("\n+output_thumbnail");
	mtrace("\n+output_thumbnail");

	//output_values();

	exif_header_ptr = (struct exif_header *) my_malloc("exif_header_ptr", sizeof(struct exif_header));
	memset(exif_header_ptr, (int) 0xFF, sizeof(struct exif_header));
	iStructSize = sizeof(struct exif_header);

	if(strlen(software_ptr) >= strlen("Nikon Capture"))
	{
		if(!strncmp(software_ptr, "Nikon Capture", strlen("Nikon Capture")))
		{
			lHeaderEndOffset = tag_thumbnail_ifd_offset;

			// I hate to do this, but I don't know how the Nikon Capture NEF file is structured! - JON JON JON
			// The header length MUST fit in two bytes (unsigned short), in MotorolaOrder!
			if(lHeaderEndOffset > 0xEFFF)
				lHeaderEndOffset = 0xEFFF;
		}
		else
			lHeaderEndOffset = get_header_end_offset(rfd);
	}
	else
		lHeaderEndOffset = get_header_end_offset(rfd);


	lLength = lHeaderEndOffset + 0x0008;

	exif_header_ptr->ff = 0xFF;
	exif_header_ptr->soi = 0xD8;
	exif_header_ptr->pad = 0xFF;
	exif_header_ptr->marker = 0xE1;
	exif_header_ptr->length = (unsigned short) Get16u(&lLength);
	memcpy(exif_header_ptr->exif,"Exif\0\0", 6);

	if(MotorolaOrder)
	{
		memcpy(exif_header_ptr->byte_order,"MM", 2);

		exif_header_ptr->flag = 0x2A00;
		exif_header_ptr->offset = 0x08000000;
	}
	else
	{
		memcpy(exif_header_ptr->byte_order,"II", 2);

		exif_header_ptr->flag = 0x002A;
		exif_header_ptr->offset = 0x00000008;
	}

	if(ALLOW_ROTATE)
	{
		if(ulOrientation == 6 || ulOrientation == 8)
		{
			file_tmp_ptr = (char *) my_malloc("file_tmp_ptr", (strlen(filename) + strlen(".tmp") + strlen(".JPG") + 1));
			memset(file_tmp_ptr, (int) NULL, (strlen(filename) + strlen(".tmp") + strlen(".JPG") + 1));
			strcat(file_tmp_ptr, filename);
			strcat(file_tmp_ptr, ".tmp.JPG");

			if(iTrace)
				printf("\nfile_tmp_ptr: %s\n", file_tmp_ptr);
		}
	}

	file_ptr = (char *) my_malloc("file_ptr", (strlen(filename) + strlen(".JPG") + 1));
	memset(file_ptr, (int) NULL, (strlen(filename) + strlen(".JPG") + 1));
	strcat(file_ptr, filename);
	strcat(file_ptr, ".JPG");

	if(iTrace)
		printf("\nfile_ptr: %s\n", file_ptr);

	data_buffer_ptr = (unsigned char *) my_malloc("data_buffer_ptr", (lHeaderEndOffset + 1));
	memset(data_buffer_ptr, (int) NULL, (lHeaderEndOffset + 1));
	fseek(rfd, 8, 0);
	fread(data_buffer_ptr, lHeaderEndOffset, 1, rfd);

	byte_ptr = (char *) my_malloc("byte_ptr", lThumbnailLength);
	memset(byte_ptr, (int) NULL, lThumbnailLength);

	if(!strncmp(upper_case(model_ptr), "NIKON D100", strlen("NIKON D100")) ||
	   !strncmp(upper_case(model_ptr), "NIKON D2H", strlen("NIKON D2H")))
	{
			fseek(rfd, lThumbnailOffset + calc_maker_note_header_offset + 2, 0);
	}
	else
	{
			fseek(rfd, lThumbnailOffset + 2, 0);
	}

	fread(byte_ptr, lThumbnailLength, 1, rfd);

	if(ulOrientation == 6 || ulOrientation == 8)
	{
		if(ALLOW_ROTATE)
		{
			if((ofd = fopen(file_tmp_ptr, "wb")) == NULL)
			{
				printf("\nFile not created: %s\n\n", file_tmp_ptr);
				exit(1);
			}
		}
		else
		{
			if((ofd = fopen(file_ptr, "wb")) == NULL)
			{
				printf("\nFile not created: %s\n\n", file_ptr);
				exit(1);
			}
		}
	}
	else
	{
		if((ofd = fopen(file_ptr, "wb")) == NULL)
		{
			printf("\nFile not created: %s\n\n", file_ptr);
			exit(1);
		}
	}

	if((size = fwrite(exif_header_ptr, sizeof(struct exif_header), 1, ofd)) == 0)
	{
		if(ulOrientation == 6 || ulOrientation == 8)
		{
			if(ALLOW_ROTATE)
				printf("\nFile not written[1]: %s\n\n", file_tmp_ptr);
			else
				printf("\nFile not written[1]: %s\n\n", file_ptr);
		}
		else
			printf("\nFile not written[1]: %s\n\n", file_ptr);

		exit(1);
	}

	if((size = fwrite(data_buffer_ptr, lHeaderEndOffset - 8, 1, ofd)) == 0)
	{
		if(ulOrientation == 6 || ulOrientation == 8)
		{
			if(ALLOW_ROTATE)
				printf("\nFile not written[2]: %s\n\n", file_tmp_ptr);
			else
				printf("\nFile not written[2]: %s\n\n", file_ptr);
		}
		else
			printf("\nFile not written[2]: %s\n\n", file_ptr);

		exit(1);
	}

	for(ctr1 = 0; ctr1 < 4; ctr1++)
	{
		if(ul_sub_ifd_array[ctr1])
			ul_local_sub_ifd_array[ctr1] = output_sub_ifd(ul_sub_ifd_array[ctr1], rfd, ofd);
	}

	ul_gobal_software_postion = output_software_version(ofd);

	position = ftell(ofd);

	if((size = fwrite(byte_ptr, lThumbnailLength, 1, ofd)) == 0)
	{
		if(ulOrientation == 6 || ulOrientation == 8)
		{
			if(ALLOW_ROTATE)
				printf("\nFile not written[3]: %s\n\n", file_tmp_ptr);
			else
				printf("\nFile not written[3]: %s\n\n", file_ptr);
		}
		else
			printf("\nFile not written[3]: %s\n\n", file_ptr);

		exit(1);
	}

	printf("\nCreated file: %s", file_ptr);

	fclose(ofd);

	if(ulOrientation == 6 || ulOrientation == 8)
	{
		if(ALLOW_ROTATE)
			update_header_length(file_tmp_ptr, position);
		else
			update_header_length(file_ptr, position);
	}
	else
		update_header_length(file_ptr, position);

	for(ctr1 = 0; ctr1 < 4; ctr1++)
	{
		if(ul_local_sub_ifd_array[ctr1])
		{

			if(ulOrientation == 6 || ulOrientation == 8)
				update_sub_ifd_offset(file_tmp_ptr, ul_local_sub_ifd_array[ctr1], ctr1);
			else
				update_sub_ifd_offset(file_ptr, ul_local_sub_ifd_array[ctr1], ctr1);

		}
	}

	if(ALLOW_ROTATE)
	{
		if(ulOrientation == 6 || ulOrientation == 8)
		{
			local_argv[0] = "jpgtran";
			local_argv[1] = "-rot";
	
			if(ulOrientation == 6)
				local_argv[2] = "90";
			else
				local_argv[2] = "270";
	
			local_argv[3] = "-copy";
			local_argv[4] = "all";
			local_argv[5] = file_tmp_ptr;
			local_argv[6] = file_ptr;
			local_argv[7] = NULL;

			jpegtran(7, local_argv);

			if(remove(file_tmp_ptr))
				printf("File not deleted: %s\n", file_tmp_ptr);
		}
	}

	for(ctr1 = 0; ctr1 < 4; ctr1++)
	{
		ul_sub_ifd_array[ctr1] = 0l;
	}

	my_free("byte_ptr", byte_ptr);
	my_free("file_ptr", file_ptr);

	if(file_tmp_ptr != NULL)
		my_free("file_tmp_ptr", file_tmp_ptr);

	if(exif_header_ptr != NULL)
		my_free("exif_header_ptr", exif_header_ptr);

	if(dir_buffer_ptr != NULL)
		my_free("dir_buffer_ptr", dir_buffer_ptr);

	if(data_buffer_ptr != NULL)
		my_free("data_buffer_ptr", data_buffer_ptr);

	trace("\n-output_thumbnail");
	mtrace("\n-output_thumbnail");
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
long get_header_end_offset(FILE *rfd)
{
	long position = 0l;
	long thumbnail_offset = 0l;
	long final_offset = 0l;
	unsigned short ctr1 = 0;
	unsigned short us_dir_num = 0;

	struct image_file_directory_entry ifde;

	position = ftell(rfd);

	fseek(rfd, tag_thumbnail_ifd_offset, 0);

	fread(&us_dir_num, sizeof(unsigned short), 1, rfd);

	do
	{
		if(ctr1 > Get16u(&us_dir_num))
			break;

		fread(&ifde, sizeof(struct image_file_directory_entry), 1, rfd);

		ctr1++;

		if(Get16u(&ifde.tag) == 0x201)
		{
			thumbnail_offset = (long) Get32u(&ifde.value_offset);
			break;
		}
	}
	while(Get16u(&ifde.tag) != 0x201);

	if(thumbnail_offset)
		final_offset = thumbnail_offset + calc_maker_note_header_offset;

	fseek(rfd, position, 0);

	return(final_offset);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
unsigned long output_sub_ifd(unsigned long ul_sub_ifd, FILE *rfd, FILE *ofd)
{
	int ctr1 = 0;
	long ifd_offset = 0l;
	long position = 0l;
	unsigned short us_dir_num = 0;
	unsigned short us_max_dir = 0;
	unsigned long size = 0l;
	char *buf_ptr = NULL;
	struct image_file_directory_entry ifde;
	unsigned long ul_temp = 0l;

/*
struct image_file_directory_entry
{
    unsigned short tag;         	//tag
    unsigned short field_type;      //field type
    unsigned long count;      		//count
    unsigned long value_offset;     //value offset
};
*/
	
	ifd_offset = ftell(ofd);
	position = ftell(rfd);

	fseek(rfd, ul_sub_ifd, 0);

	fread(&us_dir_num, sizeof(unsigned short), 1, rfd);

	if((size = fwrite(&us_dir_num, sizeof(unsigned short), 1, ofd)) == 0)
	{
		printf("\nDirectory Number not Writen\n\n");
		exit(1);
	}

	us_max_dir = Get16u(&us_dir_num);

	for(ctr1 = 0; ctr1 < us_max_dir; ctr1++)
	{
		fread(&ifde, sizeof(struct image_file_directory_entry), 1, rfd);

		if(Get16u(&ifde.tag) == 0x11a)
		{
			if((ulGlobalOrientation == 6) || (ulGlobalOrientation == 8))
				ul_temp = Get32u(&ul_global_XResolution_offset) + 0x0012 + 0x000c;
			else
				ul_temp = Get32u(&ul_global_XResolution_offset) + 0x000c;

			ifde.value_offset = Get32u(&ul_temp);
		}

		if(Get16u(&ifde.tag) == 0x11b)
		{
			if((ulGlobalOrientation == 6) || (ulGlobalOrientation == 8))
				ul_temp = Get32u(&ul_global_YResolution_offset) + 0x0012 + 0x000c;
			else
				ul_temp = Get32u(&ul_global_YResolution_offset) + 0x000c;

			ifde.value_offset = Get32u(&ul_temp);
		}

		if((size = fwrite(&ifde, sizeof(struct image_file_directory_entry), 1, ofd)) == 0)
		{
			printf("\nSub-IFD not Writen\n\n");
			exit(1);
		}
	}

	if((size = fwrite("\0\0", 2, 1, ofd)) == 0)
	{
		printf("\nEnd-Block not Writen\n\n");
		exit(1);
	}

	fseek(rfd, position, 0);
	return(ifd_offset);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void update_sub_ifd_offset(char *file_ptr, unsigned long ul_sub_ifd, int i_sub_ifd_num)
{
	FILE *iofd = NULL;
	int iFoundTag = 0;

	long position = 0l;
	unsigned long ifd_offset = 0l;
	unsigned long tag_offset = 0l;
	unsigned long final_offset = 0l;
	unsigned short ctr1 = 0;
	unsigned short us_dir_num = 0;
	unsigned long ulOrientation = ulGlobalOrientation;

	struct image_file_directory_entry ifde;

	if(ulOrientation == 6 || ulOrientation == 8)
		ul_sub_ifd += 0x12;

	if((iofd = fopen(file_ptr, "r+b")) == NULL)
	{
		printf("\nFile not opened: %s\n\n", file_ptr);
		exit(1);
	}

	fseek(iofd, 0x14, 0);

	fread(&us_dir_num, sizeof(unsigned short), 1, iofd);

	do
	{
		if(ctr1 > Get16u(&us_dir_num))
			break;

		position = ftell(iofd);
		fread(&ifde, sizeof(struct image_file_directory_entry), 1, iofd);

		ctr1++;

		if(Get16u(&ifde.tag) == 0x14a)
		{
			iFoundTag = 1;
			break;
		}
	}
	while(Get16u(&ifde.tag) != 0x14a);

	if(iFoundTag)
	{
		fseek(iofd, position + 8, 0);
		fread(&tag_offset, sizeof(unsigned long), 1, iofd);


		if(i_sub_ifd_num == 0)
		{
			if(ulOrientation == 6 || ulOrientation == 8)
				final_offset = Get32u(&tag_offset) + 0xc + 0x12;
			else
				final_offset = Get32u(&tag_offset) + 0xc;

			tag_offset = Get32u(&final_offset);
			fseek(iofd, position + 8, 0);
			fwrite(&tag_offset, sizeof(unsigned long), 1, iofd);
		}
		else
		{
			final_offset = Get32u(&tag_offset);
		}

		if(ulOrientation == 6 || ulOrientation == 8)
			final_offset = Get32u(&tag_offset) - 0x12;

		switch(i_sub_ifd_num)
		{
			case 0:
				fseek(iofd, final_offset, 0);
				break;
			case 1:
				fseek(iofd, (final_offset + 4), 0);
				break;
			case 2:
				fseek(iofd, (final_offset + 8), 0);
				break;
			case 3:
				fseek(iofd, (final_offset + 12), 0);
				break;
			default:
				break;
		}

		ifd_offset = Get32u(&ul_sub_ifd);
		fwrite(&ifd_offset, sizeof(unsigned long), 1, iofd);
	}

	fclose(iofd);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void update_header_length(char *file_ptr, unsigned long ul_offset)
{
	int iFoundTag = 0;
	FILE *iofd = NULL;
	int oldMotorolaOrder = 0;
	long position = 0l;
	unsigned long ulOrientation = ulGlobalOrientation;
	unsigned long final_offset = 0l;
	unsigned long ul_count = 0l;
	unsigned long tag_offset = 0l;
	unsigned short ctr1 = 0;
	unsigned short ul_local_offset = 0;
	unsigned short ul_temp_offset = 0;
	unsigned short us_dir_num = 0;
	struct image_file_directory_entry ifde;
	char *byte_ptr = NULL;

	oldMotorolaOrder = MotorolaOrder;

	if((iofd = fopen(file_ptr, "r+b")) == NULL)
	{
		printf("\nFile not opened: %s\n\n", file_ptr);
		exit(1);
	}

	fseek(iofd, 0x4, 0);

	ul_offset-=4;

	// The following header length MUST be written in MotorolaOrder! - JON JON JON
	MotorolaOrder = 1;

	ul_temp_offset = (unsigned short) ul_offset;
	ul_local_offset = Get16u(&ul_temp_offset);

	fwrite(&ul_local_offset, sizeof(unsigned short), 1, iofd);

	MotorolaOrder = oldMotorolaOrder;

/*
struct image_file_directory_entry
{
    unsigned short tag;         	//tag
    unsigned short field_type;      //field type
    unsigned long count;      		//count
    unsigned long value_offset;     //value offset
};
*/

	// fix some additional tag offsets:

	fseek(iofd, 0x14, 0);

	fread(&us_dir_num, sizeof(unsigned short), 1, iofd);

	do
	{
		if(ctr1 > Get16u(&us_dir_num))
			break;

		position = ftell(iofd);
		fread(&ifde, sizeof(struct image_file_directory_entry), 1, iofd);

		ctr1++;

		if(Get16u(&ifde.tag) == 0x102)
		{
			fseek(iofd, position + 8, 0);
			fread(&tag_offset, sizeof(unsigned long), 1, iofd);

			if(ulOrientation == 6 || ulOrientation == 8)
				final_offset = Get32u(&tag_offset) + 0xc + 0x12;
			else
				final_offset = Get32u(&tag_offset) + 0xc;

			tag_offset = Get32u(&final_offset);
			fseek(iofd, position + 8, 0);
			fwrite(&tag_offset, sizeof(unsigned long), 1, iofd);
		}
	}
	while(Get16u(&ifde.tag) != 0x102);

	fseek(iofd, 0x16, 0);

	ctr1 = 0;

	do
	{
		if(ctr1 > Get16u(&us_dir_num))
			break;

		position = ftell(iofd);
		fread(&ifde, sizeof(struct image_file_directory_entry), 1, iofd);

		ctr1++;

		if(Get16u(&ifde.tag) == 0x131)
		{
			if(ulOrientation == 6 || ulOrientation == 8)
				final_offset = ul_gobal_software_postion - 0xc;
			else
				final_offset = ul_gobal_software_postion - 0xc;

			ul_count = 0x20;
			ifde.count = Get32u(&ul_count);
			ifde.value_offset = Get32u(&final_offset);

			fseek(iofd, position, 0);
			fwrite(&ifde, sizeof(struct image_file_directory_entry), 1, iofd);
		}
	}
	while(Get16u(&ifde.tag) != 0x131);

	fclose(iofd);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void output_values()
{
	struct jpg_data_header *temp_start_ptr = NULL;

	temp_start_ptr = start_header_ptr;

	printf("RecordType, Tag, FieldType, Count, Offset, Data");

	do
	{
		printf("\n%d,%x,%x,%x,%x, %s", temp_start_ptr->record_type,
			temp_start_ptr->tag,
			temp_start_ptr->field_type,
			temp_start_ptr->count,
			temp_start_ptr->value_offset, temp_start_ptr->data_at_offset);

		temp_start_ptr = temp_start_ptr->next;
	}
	while(temp_start_ptr->next != NULL);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
unsigned long output_software_version(FILE *ofd)
{
	char buffer[64];
	char *buf_ptr = NULL;
	unsigned long position = 0;

	position = ftell(ofd);

	buf_ptr = (char *) my_malloc("buf_ptr", 128);
	memset(buf_ptr, (int) NULL, 128);

	memset(buffer, (int) NULL, 64);
	sprintf(buffer, "D70Reader - v%s\0", D70_VERSION);
	memcpy(buf_ptr, buffer, strlen(buffer));

	fwrite(buf_ptr, 128, 1, ofd);

	my_free("buf_ptr", buf_ptr);

	return(position);
}
