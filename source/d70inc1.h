/************************************************************************/
/*      $Id: d70inc1.h,v 1.2 2005/04/14 17:31:39 joman Exp joman $	*/
/*                                                                      */
/*      Include file name:   d70inc1.h                                  */
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Mon Mar 21 17:16:56 EST 2005:                    */
/*              First coding.                                           */
/************************************************************************/

#define NUM_FORMATS 12

#define FMT_BYTE       1
#define FMT_STRING     2
#define FMT_USHORT     3
#define FMT_ULONG      4
#define FMT_URATIONAL  5
#define FMT_SBYTE      6
#define FMT_UNDEFINED  7
#define FMT_SSHORT     8
#define FMT_SLONG      9
#define FMT_SRATIONAL 10
#define FMT_SINGLE    11
#define FMT_DOUBLE    12

#define TAG_EXIF_OFFSET		0x8769
#define TAG_MAKER_NOTE		0x927C
#define TAG_SUBIFD_S		0x014A
#define TAG_ORIENTATION		0x0112
#define TAG_THUMBNAIL_OFFSET	0x0201
#define TAG_THUMBNAIL_LENGTH	0x0202
#define TAG_THUMBNAIL_IFD	0x0011

#define MAKER_NOTE_PRE_HEADER_LENGTH	10
#define ASCII_LABEL_SKIP_LENGTH	8

struct html_header
{
	char * Desc;
	char * Value;
	unsigned short Tag;
	struct html_header *next;
};

struct data_header
{
	unsigned short Tag;
    unsigned short field_type;     	//field type
    unsigned long count;      		//count     
	char * Desc;
	char * Value;
	struct data_header *next;
};

struct jpg_data_header
{
	unsigned short tag;                     
	unsigned short field_type;              
	unsigned long count;                    
	unsigned long value_offset;             
	unsigned char *data_at_offset;
	int record_type;
	int byte_count;
	struct jpg_data_header *next;
};

struct data_header_ptr
{
	char *data_type;
	struct data_header *header_ptr;
	struct data_header_ptr *prev;
	struct data_header_ptr *next;
};

struct image_file_header
{
    char byte_order[2];                     //byte order indicator 'II' or 'MM'
    unsigned short image_file_type;         //image file type                  
    unsigned long offset_to_first_ifd;      //offset to first IFD              
};

struct jpg_image_file_header
{
	unsigned char start;
	unsigned char soi;
	unsigned char pad;
	unsigned char exif_ind;
	unsigned short length;
	char exif_header_text[6];
	char byte_order[2];                     //byte order indicator 'II' or 'MM'
	unsigned short image_file_type;         //image file type                  
	unsigned long offset_to_first_ifd;      //offset to first IFD              
};

struct image_file_directory_entry
{
    unsigned short tag;         	//tag
    unsigned short field_type;      //field type
    unsigned long count;      		//count
    unsigned long value_offset;     //value offset
};

struct wanted_tags
{
	unsigned short tag;
	struct wanted_tags *next;
};

struct all_wanted_tags
{
	unsigned short tag;
	struct all_wanted_tags *next;
};

struct xmp_packet
{
	unsigned long length;
	char *value;
};

typedef struct {
    unsigned short Tag;
    char * Desc;
}TagTable_t;

int iMemTrace = 0;
int iTrace = 0;		// set this with -t option 
int iVerbos = 0;	// set this with -v option
int iHtml = 0;		// set this with -w option
int iXml = 0;		// set this with -x option
int iThumbnail = 0;	// set this with -j option
int iThumbnailOld = 0;	// set this with -jold option
int iCsv = 0;		// set this with -c option
int iJS = 0;		// set this with -js option
int iWeb = 0;		// set this with -web option
int iRemoveEXIF = 0;	// set this with -r option

long lFileSize = 0;

int MotorolaOrder = 1;	//All NEF files that I have seen are MotorolaOrder files
			//Correction, Nikon Capture saves in Intel order!
int old_byte_order;
int iJPG_Type;
int iFile_Type;
int iCsv_First_Time = 1;
int iJS_First_Time = 1;
int js_ctr1 = 0;
int iTagCount = 0;
int iSystemType = 0;

int iWeb_First_Time = 1;
int iWeb_Image_count = 0;
int iRoot_Web_Image_count = 0;
int iWeb_Gallery_count = 0;
char *web_file_name_src_ptr = NULL;
char *web_file_name_tmp_ptr = NULL;
FILE *gallery_ofd = NULL;
FILE *root_ofd = NULL;

char *GlobalOrientationPtr = NULL;
unsigned long ulGlobalOrientation = 0l;
unsigned long ul_sub_ifd_array[4] = { 0,0,0,0 };
unsigned long ul_global_XResolution_offset = 0l;
unsigned long ul_global_YResolution_offset = 0l;
unsigned long ul_gobal_software_postion = 0l;

int iGlobalImageWidth = 0;
int iGlobalImageHeight = 0;
int	iSizeTestCompleted = 0;
char *str_root_temp_dir = NULL;
unsigned long ulGlobalPosition = 0l;

int malloc_count = 0;
int free_count = 0;

unsigned long calc_maker_note_header_offset;
unsigned long tag_thumbnail_ifd_offset;

char *make_ptr = NULL;
char *model_ptr = NULL;
char *software_ptr = NULL;

unsigned long lThumbnailOffset;
unsigned long lThumbnailLength;

struct all_wanted_tags *main_all_wanted_tags_ptr;
struct all_wanted_tags *start_all_wanted_tags_ptr;
struct wanted_tags *main_wanted_tags_ptr;
struct wanted_tags *start_wanted_tags_ptr;

struct data_header *header_ptr;
struct html_header *html_header_ptr = NULL;
struct html_header *html_header_start_ptr = NULL;

struct data_header_ptr main_header;
struct data_header_ptr *main_header_ptr = NULL;
struct data_header_ptr *main_header_start_ptr = NULL;

struct jpg_data_header *jpg_header_ptr;
struct jpg_data_header *start_header_ptr;
struct jpg_data_header *current_header_ptr;

static TagTable_t TagTable[] = {
  { 0x001,   "FileSystemVersion          "},
  { 0x002,   "ISOUsed                    "},
  { 0x004,   "Quality                    "},
  { 0x005,   "WhiteBalance               "},
  { 0x006,   "Sharpening                 "},
  { 0x007,   "FocusMode                  "},
  { 0x008,   "FlashSetting               "},
  { 0x009,   "FlashMode                  "},
  { 0x00A,   "Unknown                    "},
  { 0x00B,   "WhiteBalanceFine           "},
  { 0x00C,   "WhiteBalanceRB             "},
  { 0x00D,   "Unknown                    "},
  { 0x00E,   "Unknown                    "},
  { 0x011,   "ThumbnailImageIFD          "},
  { 0x012,   "FlashCompensation          "},
  { 0x013,   "ISOSet                     "},
  { 0x016,   "Unknown                    "},
  { 0x017,   "Unknown                    "},
  { 0x018,   "Unknown                    "},
  { 0x019,   "Unknown                    "},
  { 0x081,   "ToneCompensation           "},
  { 0x083,   "LensType                   "},
  { 0x084,   "Lens                       "},
  { 0x087,   "FlashUsed                  "},
  { 0x088,   "AFPoint                    "},
  { 0x089,   "Unknown                    "},
  { 0x08A,   "Unknown                    "},
  { 0x08B,   "LensInfo                   "},
  { 0x08C,   "Curve                      "},
  { 0x08D,   "ColorMode                  "},
  { 0x08E,   "Unknown                    "},
  { 0x090,   "LightSource                "},
  { 0x091,   "Unknown                    "},
  { 0x092,   "HueAdjustment              "},
  { 0x093,   "Unknown                    "},
  { 0x095,   "NoiseReduction             "},
  { 0x096,   "NEFCurve2                  "},
  { 0x097,   "ColorBalanceD70            "},
  { 0x098,   "Unknown                    "},
  { 0x099,   "NEFThumbnailSize           "},
  { 0x09A,   "Unknown                    "},
  { 0x0A0,   "SerialNumber               "},
  { 0x0A1,   "Unknown                    "},
  { 0x0A2,   "Unknown                    "},
  { 0x0A3,   "Unknown                    "},
  { 0x0A4,   "Unknown                    "},
  { 0x0A5,   "Unknown                    "},
  { 0x0A6,   "Unknown                    "},
  { 0x0A7,   "ShutterCount               "},
  { 0x0A8,   "Unknown                    "},
  { 0x0A9,   "ImageOptimization          "},
  { 0x0AA,   "Saturation                 "},
  { 0x0AB,   "VariProgram                "},
  { 0x0FE,   "NewSubfileType             "},
  { 0x100,   "ImageWidth                 "},
  { 0x101,   "ImageLength                "},
  { 0x102,   "BitsPerSample              "},
  { 0x103,   "Compression                "},
  { 0x106,   "PhotometricInterpretation  "},
  { 0x10A,   "FillOrder                  "},
  { 0x10D,   "DocumentName               "},
  { 0x10E,   "ImageDescription           "},
  { 0x10F,   "Make                       "},
  { 0x110,   "Model                      "},
  { 0x111,   "StripOffsets               "},
  { 0x112,   "Orientation                "},
  { 0x115,   "SamplesPerPixel            "},
  { 0x116,   "RowsPerStrip               "},
  { 0x117,   "StripByteCounts            "},
  { 0x11A,   "XResolution                "},
  { 0x11B,   "YResolution                "},
  { 0x11C,   "PlanarConfiguration        "},
  { 0x128,   "ResolutionUnit             "},
  { 0x12D,   "TransferFunction           "},
  { 0x131,   "Software                   "},
  { 0x132,   "DateTime                   "},
  { 0x13B,   "Artist                     "},
  { 0x13E,   "WhitePoint                 "},
  { 0x13F,   "PrimaryChromaticities      "},
  { 0x14A,   "SubIFDs                    "},
  { 0x156,   "TransferRange              "},
  { 0x200,   "JPEGProc                   "},
  { 0x201,   "ThumbnailOffset            "},
  { 0x202,   "ThumbnailLength            "},
  { 0x211,   "YCbCrCoefficients          "},
  { 0x212,   "YCbCrSubSampling           "},
  { 0x213,   "YCbCrPositioning           "},
  { 0x214,   "ReferenceBlackWhite        "},
  { 0xE01,   "Unknown                    "},
  { 0xE05,   "Unknown                    "},
  { 0xE0E,   "Unknown                    "},
  { 0xE09,   "Unknown                    "},
  { 0xE10,   "Unknown                    "},
  { 0x1001,  "RelatedImageWidth          "},
  { 0x1002,  "RelatedImageLength         "},
  { 0x828D,  "CFARepeatPatternDim        "},
  { 0x828E,  "CFAPattern                 "},
  { 0x828F,  "BatteryLevel               "},
  { 0x8298,  "Copyright                  "},
  { 0x829A,  "ExposureTime               "},
  { 0x829D,  "FNumber                    "},
  { 0x83BB,  "IPTC/NAA                   "},
  { 0x8769,  "ExifOffset                 "},
  { 0x8773,  "InterColorProfile          "},
  { 0x8822,  "ExposureProgram            "},
  { 0x8824,  "SpectralSensitivity        "},
  { 0x8825,  "GPSInfo                    "},
  { 0x8827,  "ISOSpeedRatings            "},
  { 0x8828,  "OECF                       "},
  { 0x9000,  "ExifVersion                "},
  { 0x9003,  "DateTimeOriginal           "},
  { 0x9004,  "DateTimeDigitized          "},
  { 0x9101,  "ComponentsConfiguration    "},
  { 0x9102,  "CompressedBitsPerPixel     "},
  { 0x9201,  "ShutterSpeedValue          "},
  { 0x9202,  "ApertureValue              "},
  { 0x9203,  "BrightnessValue            "},
  { 0x9204,  "ExposureBiasValue          "},
  { 0x9205,  "MaxApertureValue           "},
  { 0x9206,  "SubjectDistance            "},
  { 0x9207,  "MeteringMode               "},
  { 0x9208,  "LightSource                "},
  { 0x9209,  "Flash                      "},
  { 0x920A,  "FocalLength                "},
  { 0x9216,  "TIFF_EPStandardId          "},
  { 0x9217,  "SensingMethod              "},
  { 0x927C,  "MakerNote                  "},
  { 0x9286,  "UserComment                "},
  { 0x9290,  "SubSecTime                 "},
  { 0x9291,  "SubSecTimeOriginal         "},
  { 0x9292,  "SubSecTimeDigitized        "},
  { 0xA000,  "FlashPixVersion            "},
  { 0xA001,  "ColorSpace                 "},
  { 0xA002,  "ExifImageWidth             "},
  { 0xA003,  "ExifImageLength            "},
  { 0xA004,  "RelatedAudioFile           "},
  { 0xA005,  "InteroperabilityOffset     "},
  { 0xA20B,  "FlashEnergy                "},
  { 0xA20C,  "SpatialFrequencyResponse   "},
  { 0xA20E,  "FocalPlaneXResolution      "},
  { 0xA20F,  "FocalPlaneYResolution      "},
  { 0xA210,  "FocalPlaneResolutionUnit   "},
  { 0xA214,  "SubjectLocation            "},
  { 0xA215,  "ExposureIndex              "},
  { 0xA217,  "SensingMethod              "},
  { 0xA300,  "FileSource                 "},
  { 0xA301,  "SceneType                  "},
  { 0xA302,  "CFA Pattern                "},
  { 0xa401,  "CustomRendered             "},
  { 0xa402,  "ExposureMode               "},
  { 0xa403,  "WhiteBalance               "},
  { 0xa404,  "DigitalZoomRatio           "},
  { 0xa405,  "FocalLengthIn35mmFilm      "},
  { 0xa406,  "SceneCaptureType           "},
  { 0xa407,  "GainControl                "},
  { 0xa408,  "Contrast                   "},
  { 0xa409,  "Saturation                 "},
  { 0xa40a,  "Sharpness                  "},
  { 0xa40c,  "SubjectDistanceRange       "},
  { 0xffff,  "Ignored Tag                "}
} ;

const int SizeTagTable = sizeof( TagTable ) / sizeof( TagTable_t );

typedef unsigned char uchar;
char trace_buffer[256];

