/************************************************************************/
/*      $Id: d70inc2.h,v 1.2 2005/04/14 17:32:32 joman Exp joman $	*/
/*                                                                      */
/*      Include file name:   d70inc2.h                                  */
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Mon Mar 21 17:16:56 EST 2005:                    */
/*              First coding.                                           */
/************************************************************************/

extern const int SizeTagTable;
extern int iMemTrace;
extern int iTrace;			// set this with -t option 
extern int iVerbos;			// set this with -v option
extern int iHtml;			// set this with -w option
extern int iXml;			// set this with -x option
extern int iThumbnail;		// set this with -j option
extern int iThumbnailOld;	// set this with -jold option
extern int iCsv;			// set this with -c option
extern int iJS;				// set this with -js option
extern int iWeb;			// set this with -web option

extern int MotorolaOrder;	//All NEF files that I have seen are MotorolaOrder files
							//Correction, Nikon Capture saves in Intel order!
extern int old_byte_order;
extern int iJPG_Type;
extern int iFile_Type;
extern int iCsv_First_Time;
extern int iJS_First_Time;
extern int js_ctr1;
extern int iTagCount;
extern int iSystemType;

extern int iWeb_First_Time;
extern int iWeb_Image_count;
extern int iRoot_Web_Image_count;
extern int iWeb_Gallery_count;
extern char *web_file_name_src_ptr;
extern char *web_file_name_tmp_ptr;
extern FILE *gallery_ofd;
extern FILE *root_ofd;

extern char *GlobalOrientationPtr;
extern unsigned long ulGlobalOrientation;
extern unsigned long ul_sub_ifd_array[4];
extern unsigned long ul_global_XResolution_offset;
extern unsigned long ul_global_YResolution_offset;
unsigned long ul_gobal_software_postion;

extern int iGlobalImageWidth;
extern int iGlobalImageHeight;
extern int	iSizeTestCompleted;
extern char *str_root_temp_dir;
extern unsigned long ulGlobalPosition;

extern int malloc_count;
extern int free_count;

extern unsigned long calc_maker_note_header_offset;
extern unsigned long tag_thumbnail_ifd_offset;

extern char *make_ptr;
extern char *model_ptr;

extern unsigned long lThumbnailOffset;
extern unsigned long lThumbnailLength;

extern struct all_wanted_tags *main_all_wanted_tags_ptr;
extern struct all_wanted_tags *start_all_wanted_tags_ptr;
extern struct wanted_tags *main_wanted_tags_ptr;
extern struct wanted_tags *start_wanted_tags_ptr;

extern struct data_header *header_ptr;
extern struct html_header *html_header_ptr;
extern struct html_header *html_header_start_ptr;

extern struct data_header_ptr main_header;
extern struct data_header_ptr *main_header_ptr;
extern struct data_header_ptr *main_header_start_ptr;

extern struct jpg_data_header *jpg_header_ptr;
extern struct jpg_data_header *start_header_ptr;
extern struct jpg_data_header *current_header_ptr;
extern struct image_file_directory_entry *ifde_ptr;

#define LARGE_IMAGE_HEIGHT 600
#define LARGE_IMAGE_WIDTH 800
#define LARGE_IMAGE_LENGTH 800

extern void extract_and_format_data(struct image_file_directory_entry *ifde_ptr, struct data_header *local_ptr, int iHex);
extern void *my_simple_malloc(size_t size);
extern void my_simple_free(void *memblock);
extern void *my_malloc(char *str_ptr, size_t size);
extern void my_free(char * str_ptr, void *memblock);
extern void replace_description_value(unsigned short Tag, char *text_ptr, char *val_ptr);
extern void extract_thumbnail(char *filename, FILE *rfd);
extern void create_root_index(char * filename);
extern void create_css(char * filename);
extern void create_gallery_index(char * filename, int iGalleryNumber);
extern void create_gallery_photos(char * filename, int iGalleryNumber);
extern void create_gallery(char *str_ptr);
extern int my_mkdir(char *str_ptr);
extern int my_rmdir(char *str_ptr);
extern void output_web();
extern void resize(char *file_src, char *file_dst, long width, long height, int jpeg_quality);
extern void free_xmp_data_header_list(struct xmp_data *header_ptr);
extern struct xmp_data *xmp_display(const char * fileName, unsigned long length, char *data_ptr, int iVerbos);
extern struct xmp_packet *chk_get_xmp_data(FILE *rfd);
extern int xmp_main(char *argv_filename);
extern int precision(char *str_value);
extern char * get_description_label(unsigned short Tag, int iStrip);
extern char * get_description_value(unsigned short Tag);
extern char *get_description(char *str_input);
extern int is_tag_printed(unsigned short tag);
extern int tag_is_yes(char *local_ptr);
extern struct all_wanted_tags *get_new_main_all_wanted_tags_ptr();
extern struct wanted_tags *get_new_main_wanted_tags_ptr();
extern void store_tags(char *local_ptr);
extern void store_all_tags(char *local_ptr);
extern void check_for_config();
extern int jpegtran(int argc, char **argv);
extern int check_for_exif_data(FILE *rfd);
extern void output_thumbnail(char *filename, FILE *rfd);
//extern void extract_and_format_data(struct image_file_directory_entry *ifde_ptr, struct data_header *local_ptr, int iHex);
extern char *strip(char *str_input);
extern void free_html_header_list(struct html_header *header_ptr);
extern void output_html();
extern void output_xml();
extern void output_csv();
extern void output_js(FILE *ofd, int iFirstTime, int iIndex);
extern struct html_header * get_new_html_header_ptr();
extern void debug_printf(char *str_format, unsigned short us_tag, char *str_desc, char *str_value);
extern void my_printf(char *str_format, unsigned short us_tag, char *str_desc, char *str_value);
extern char *truncate(char *str_token, int dec_len);
extern char * convert_lens(char *str_lens);
extern void free_data_header_list(struct data_header *header_ptr);
extern void free_memory(struct data_header_ptr *free_header_start_ptr, struct html_header *free_html_header_start_ptr);
extern void ProcessFile(int argc, char *filename);
extern void ProcessFileJPG(int argc, char *filename);
extern char *upper_case(char *local_ptr);
extern char *lower_case(char *local_ptr);
extern char *get_extension(char *string);
extern void get_xmp_data(FILE *rfd, struct data_header *local_ptr);
extern int jpg_type();
extern int jpg_type_offset();
extern unsigned long get_jpg_type_and_offset(FILE *rfd, unsigned long offset);
extern void my_exit(char *text_ptr);
extern void my_jpg_exit(char *text_ptr);
extern void display_data(struct data_header *local_ptr, char *data_type);
extern struct data_header * get_new_data_header_ptr();
extern struct data_header_ptr * get_new_main_header_ptr();
extern unsigned long rotate_right(unsigned long Long);
extern void  instruct(void);
extern int Get16u(void * Short);
extern unsigned Get32u(void * Long);
//extern static int Get32s(void * Long);
extern int Get32s(void * Long);
extern void my_error(FILE *rfd);
extern void print_8bit_bytes(unsigned char *byte_ptr, int ByteCount, struct data_header *local_ptr);
extern void print_8bit_hex(unsigned char *byte_ptr, int ByteCount, struct data_header *local_ptr);
extern void print_16bit_bytes(unsigned char *byte_ptr, int components, int ByteCount , struct data_header *local_ptr);
extern void print_16bit_hex(unsigned char *byte_ptr, int components, int ByteCount, struct data_header *local_ptr);
extern void my_fseek(FILE *rfd, long pos, int offset);
extern int dump_image_file_directory(FILE *rfd, unsigned long offset_ifd, unsigned long offset_base, struct data_header *local_ptr, char *type_ptr);
extern void dump_jpg_image_file_directory(FILE *rfd, unsigned long offset_ifd, unsigned long offset_base, char *type_ptr);
extern void mtrace(char *str);
extern void trace(char *str);
extern void trace_formatted(char *str);
extern unsigned char * pad_field(unsigned char *temp_ptr, int length, char pad_char, int iPrePend);
extern unsigned long inspect_image_file_header(FILE *rfd, int iType);
extern void dump_it();
extern int get_sof0_data(FILE *rfd);
extern long get_marker_offset(FILE *rfdm, unsigned short us_marker);
extern void remove_exif(char *filename, FILE *rfd);
extern long get_header_end_offset(FILE *rfd);
extern unsigned long output_sub_ifd(unsigned long ul_sub_ifd, FILE *rfd, FILE *ofd);
extern void update_sub_ifd_offset(char *file_ptr, unsigned long ul_sub_ifd, int ctr1);
extern void update_header_length(char *file_ptr, unsigned long ul_offset);
extern void output_values();
extern unsigned long output_software_version(FILE *ofd);
extern void output_thumbnail_old(char *filename, FILE *rfd);
