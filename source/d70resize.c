/************************************************************************/
/*      $Id: d70resize.c,v 1.1 2005/03/21 23:06:48 joman Exp joman $    */
/*                                                                      */
/*      Source file name:   d70resize.c                                 */
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Mon Mar 21 17:16:56 EST 2005:                    */
/*              First coding.                                           */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <setjmp.h>

#include "d70types.h"
#include "d70image.h"
#include "d70reader.h"
#include "d70inc2.h"

#ifdef MPATROL_DEBUG
#include "mpatrol.h"
#endif

jmp_buf setjmp_buffer;

void resize(char *file_src, char *file_dst, long width, long height, int jpeg_quality);
image *image_new_from_jpeg (char *file_name);
image *image_new(int32 width, int32 height);
void image_free(image *pimage);
void image_resize_bilinear(image *pdest, image *psrc, int32 width, int32 height);
int image_save_to_jpeg(image *pimage, char *file,int jpeg_quality);
static void g_JPEGFatalErrorHandler(j_common_ptr cinfo);
pix get_pix(image *pimage, int32 x, int32 y);
void put_pix_alpha_replace(image *pimage, int32 x, int32 y, pix col);
void reduce_image_size(image *pdest, image *psrc, int32 width, int32 height);

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void resize(char *file_src, char *file_dst, long width, long height, int jpeg_quality)
{
	image *vimage = NULL;
	image *vimage_resize = NULL;

	vimage = image_new_from_jpeg(file_src);

	// If we only know the width or the height, build 
	// the other, using the image ratio
	if(height <= 0)
		height = (width * vimage->height) / vimage->width;

	if(width <= 0)
		width = (height * vimage->width) / vimage->height;

	// create the destination image 
	vimage_resize = image_new(width,height);

	reduce_image_size(vimage_resize, vimage, width, height);
	image_save_to_jpeg(vimage_resize, file_dst, jpeg_quality);

	image_free(vimage);
	image_free(vimage_resize);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
image *image_new_from_jpeg (char *file_name)
{ 
	image *vimage;
	FILE  *file;
	unsigned char      *line[16], *ptr;
	int                 x, y, i, w, h, ofs;
	int maxbuf;
	struct jpeg_error_mgr pub;    
	struct jpeg_decompress_struct cinfo;

	if((file = fopen(file_name,"rb")) == NULL) 
		return NULL;

	cinfo.err = jpeg_std_error(&pub);

	i = sizeof(struct jpeg_decompress_struct);

	jpeg_create_decompress(&cinfo);
	pub.error_exit =g_JPEGFatalErrorHandler;

	// error handler to longjmp to, we want to preserve signals
	if (setjmp(setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(file);
		return NULL;
	}


	jpeg_stdio_src(&cinfo, file);
	jpeg_read_header(&cinfo, TRUE);
	cinfo.do_fancy_upsampling = FALSE;
	cinfo.do_block_smoothing = FALSE;
	jpeg_start_decompress(&cinfo);
	w = cinfo.output_width;
	h = cinfo.output_height;
	vimage = image_new(w,h);

	if(!vimage)
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(file);
		return NULL;
	}

	if(cinfo.rec_outbuf_height > 16)
	{
		fprintf(stderr, "ERROR image_from_jpeg : (image_from_jpeg.c) JPEG uses line buffers > 16. Cannot load.\n");
		image_free(vimage);
		fclose(file);
		return NULL;
	}

	maxbuf = vimage->width * vimage->height;
	
	if (cinfo.output_components == 3)
	{ 
		ofs = 0;

		if((ptr = (char *)my_malloc("ptr", w*3*cinfo.rec_outbuf_height)) == NULL)
		{ 
			perror("malloc failed "); exit(1); 
		}

		for (y = 0; y < h; y += cinfo.rec_outbuf_height)
		{
			for (i = 0; i < cinfo.rec_outbuf_height; i++)
			{
				line[i] = ptr + (w*3*i);
			}

			jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
			
			for(x=0;x<w*cinfo.rec_outbuf_height;x++)
			{
				if( ofs < maxbuf ) 
				{
					vimage->buf[ofs] = COL(ptr[x+x+x], ptr[x+x+x+1], ptr[x+x+x+2]);
					ofs++;
				}
			}
		}

		my_free("ptr", ptr);
	}
	else if(cinfo.output_components == 1)
	{ 
		ofs = 0;

		for (i = 0; i < cinfo.rec_outbuf_height; i++)
		{
			if((line[i] = (unsigned char *)my_malloc("line[i]", w)) == NULL)
			{
				int t = 0;

				for(t = 0; t < i; t++) my_free("line[t]", line[t]);

				jpeg_destroy_decompress(&cinfo);
				image_free(vimage);
				fclose(file);
				return NULL;
			}
		}

		for(y = 0; y < h; y += cinfo.rec_outbuf_height)
		{
			jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
			
			for(i = 0; i < cinfo.rec_outbuf_height; i++)
			{
				for(x = 0; x < w; x++)
				{
					vimage->buf[ofs++] = COL(line[i][x], line[i][x], line[i][x]);
				}
			}
		}

		for(i = 0; i < cinfo.rec_outbuf_height; i++) 
			my_free("line[i]", line[i]);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(file);
	return vimage;
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
image* image_new(int32 width, int32 height)
{
	image *vimage;

	if((vimage = (image *) my_malloc("vimage", sizeof(image))) == NULL)
	{
		perror("malloc failed ");
		exit(1);
	}

	vimage->width = width; 
	vimage->height = height;

	if((vimage->buf=(pix *)my_malloc("vimage-buf", width*height*sizeof(pix))) == NULL)
	{
		perror("malloc failed ");
		exit(1);
	}

	return(vimage);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void image_free(image *pimage)
{
	my_free("pimage->buf", pimage->buf);
	my_free("pimage", pimage);
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void image_resize_bilinear(image *pdest, image *psrc, int32 width, int32 height)
{
	int32 vx,vy;
	pix   vcol,vcol1,vcol2,vcol3,vcol4;
	float rx,ry;
	float width_scale, height_scale;
	float x_dist, y_dist;

	width_scale  = (float)psrc->width  / (float)width;
	height_scale = (float)psrc->height / (float)height;

	for(vy = 0;vy < height; vy++)  
	{
		for(vx = 0;vx < width; vx++)
		{
			rx = vx * width_scale;
			ry = vy * height_scale;
			vcol1 = get_pix(psrc, (int32)rx, (int32)ry);
			vcol2 = get_pix(psrc, ((int32)rx)+1, (int32)ry);

			vcol3 = get_pix(psrc, (int32)rx, ((int32)ry)+1);
			vcol4 = get_pix(psrc, ((int32)rx)+1, ((int32)ry)+1);

			x_dist = rx - ((float)((int32)rx));
			y_dist = ry - ((float)((int32)ry));
			
			vcol = COL_FULL(
			(uint8)((COL_RED(vcol1)*(1.0-x_dist)
			+ COL_RED(vcol2)*(x_dist))*(1.0-y_dist)
			+ (COL_RED(vcol3)*(1.0-x_dist)
			+ COL_RED(vcol4)*(x_dist))*(y_dist)),
			(uint8)((COL_GREEN(vcol1)*(1.0-x_dist)
			+ COL_GREEN(vcol2)*(x_dist))*(1.0-y_dist)
			+ (COL_GREEN(vcol3)*(1.0-x_dist)
			+ COL_GREEN(vcol4)*(x_dist))*(y_dist)),
			(uint8)((COL_BLUE(vcol1)*(1.0-x_dist)
			+ COL_BLUE(vcol2)*(x_dist))*(1.0-y_dist)
			+ (COL_BLUE(vcol3)*(1.0-x_dist)
			+ COL_BLUE(vcol4)*(x_dist))*(y_dist)),
			(uint8)((COL_ALPHA(vcol1)*(1.0-x_dist)
			+ COL_ALPHA(vcol2)*(x_dist))*(1.0-y_dist)
			+ (COL_ALPHA(vcol3)*(1.0-x_dist)
			+ COL_ALPHA(vcol4)*(x_dist))*(y_dist))
			);

			put_pix_alpha_replace(pdest, vx, vy, vcol);
		}
	}
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
int image_save_to_jpeg(image *pimage, char *file, int jpeg_quality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride;
	FILE *f;
	char *data;
	int i,x;
		 
	f = fopen(file, "wb");

	if(f)
	{
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&cinfo);
		jerr.error_exit =g_JPEGFatalErrorHandler;  
     
		// error handler to longjmp to, we want to preserve signals 
		if (setjmp(setjmp_buffer))
		{
			jpeg_destroy_compress(&cinfo);
			fclose(f);
			return -1;
		}
         
		jpeg_stdio_dest(&cinfo, f);
		cinfo.image_width = pimage->width;
		cinfo.image_height = pimage->height;
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;
		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, (jpeg_quality * JPEG_QUALITY) >> 8, TRUE);
		jpeg_start_compress(&cinfo, TRUE);
		row_stride = cinfo.image_width * 3;

		if((data = (char *)my_malloc("data", row_stride)) == NULL)
		{ 
			perror("malloc failed "); exit(1); 
		}

		i = 0;

		while(cinfo.next_scanline < cinfo.image_height)
		{
			for(x = 0; x < pimage->width; x++) 
			{
				data[x+x+x]   = COL_RED(pimage->buf[i]);
				data[x+x+x+1] = COL_GREEN(pimage->buf[i]);
				data[x+x+x+2] = COL_BLUE(pimage->buf[i]);
				i++;
			}

			row_pointer[0] = data;

			jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}

		jpeg_finish_compress(&cinfo);
		my_free("data", data);
		fclose(f);
		return 0;
	}

	return -1;
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
static void g_JPEGFatalErrorHandler(j_common_ptr cinfo)
{
	cinfo->err->output_message(cinfo);
	longjmp(setjmp_buffer,1);
	return;
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
pix get_pix(image *pimage, int32 x, int32 y)
{
	if((x >= 0) && (y >= 0) && (x < pimage->width) && (y < pimage->height))
		return(pimage->buf[(y * pimage->width) + x]);
	else 
	{
		pix vpix = BLACK;
		return(vpix);
	}
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void put_pix_alpha_replace(image *pimage, int32 x, int32 y, pix col)
{
	if((x >= 0) && (y >= 0) && (x < pimage->width) && (y < pimage->height))
		pimage->buf[(y * pimage->width) + x] = col;
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void reduce_image_size(image *pdest, image *psrc, int32 width, int32 height)
{
	int32 vx,vy;
	pix   vcol;
	float rx,ry,rx_next,ry_next;
	float width_scale, height_scale;
	float red, green, blue, alpha;
	float factor;
	int32   i,j;

	if((pdest == NULL) || (psrc == NULL))
		return;

	if ((psrc->width < width) || (psrc->height < height)) 
	{
		image_resize_bilinear(pdest, psrc, width, height);
		return;
	}

	width_scale  = (float)psrc->width  / (float)width;
	height_scale = (float)psrc->height / (float)height;

	for(vy = 0;vy < height; vy++) 
	{
		for(vx = 0;vx < width; vx++) 
		{
			rx = vx * width_scale;
			ry = vy * height_scale;

			red = green = blue = alpha = 0.0;

			rx_next = rx + width_scale;
			ry_next = ry + width_scale;
			factor = 0;

			for( j = (int32)rx; (float)j < rx_next; j++) 
			{
				for( i = (int32)ry; (float)i < ry_next; i++) 
				{
					factor += 1;
					vcol = get_pix(psrc, j, i);

					red   += (float)COL_RED  (vcol);
					green += (float)COL_GREEN(vcol);
					blue  += (float)COL_BLUE (vcol);
					alpha += (float)COL_ALPHA(vcol);
				}
			}

			red   /= factor;
			green /= factor;
			blue  /= factor;
			alpha /= factor;

			red   = (float) (red   > (float) 255.0)? (float) 255.0 : ((red   < (float) 0.0)? (float) 0.0:red  );
			green = (float) (green > (float) 255.0)? (float) 255.0 : ((green < (float) 0.0)? (float) 0.0:green);
			blue  = (float) (blue  > (float) 255.0)? (float) 255.0 : ((blue  < (float) 0.0)? (float) 0.0:blue );
			alpha = (float) (alpha > (float) 255.0)? (float) 255.0 : ((alpha < (float) 0.0)? (float) 0.0:alpha);

			put_pix_alpha_replace(pdest, vx, vy,
			COL_FULL((uint8)red,(uint8)green,(uint8)blue,(uint8)alpha));
		}
	}
}

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
/*
int main(int argc, char *argv[])
{
//	resize("../DSC_0021.JPG", "../DSC_0021.sml.JPG", 0, 600, 75);
//	resize("F:\\NikonD70Images0003\\02142005_0001\\DSC_0176.NEF.JPG", "F:\\NikonD70Images0003\\02142005_0001\\DSC_0176.NEF.sml.JPG", 600, 0, 75);
	resize("F:\\share\\d70\\DSC_0021.JPG", "F:\\share\\d70\\DSC_0021.sml.JPG", 0, 600, 75);
	exit(0);
}
*/
