/*
 * StlGlDefines.h
 *
 *  Created on: Nov 3, 2016
 *      Author: hoover
 */

#ifndef STLGLDEFINES_H_
#define STLGLDEFINES_H_
#define ADD_FUCNTION_BY_JIMMY   1
#define DELETE_BY_JIMMY   0


#if ADD_FUCNTION_BY_JIMMY
typedef unsigned char uint8;
typedef char int8;
#define  GREEN_STATE 0x00
#define RED_STATE 0x01
#define GRAY_STATE 0x02
#endif

#define CORE_AND_POS_LIMIT  4.5
#define INNER_RECT_AND_PANO_ONE_TIME_CAM_LIMIT 7.7
#define	OUTER_RECT_AND_PANO_TWO_TIMES_CAM_LIMIT 40//16.8
typedef enum{
	CAM_0,
	CAM_1,
	CAM_2,
	CAM_3,
	CAM_4,
	CAM_5,
	CAM_6,
	CAM_7,
	CAM_8,
	CAM_9,
	CAM_10,
	CAM_11,
	CAM_TOTAL_COUNT
} CAM_TEXTURE_IDX;


#if !defined(CAM_COUNT)
#define CAM_COUNT CAM_TOTAL_COUNT
#else
#define PRINT_MACRO_HELPER(x) #x
#define PRINT_MACRO(x) #x"="PRINT_MACRO_HELPER(x)
#pragma message(PRINT_MACRO(CAM_COUNT) )
#endif

#define USE_BIG_OVERLAP 1
//跳过10路拼接的前两个  因为pbosend里面　idx 1, 2 被固定设置为2*3　2*2的纹理
// 所以跳过idx 1,2,设置为３则起始值则为3
#define MAGICAL_NUM 3
#if USE_GAIN
enum{
       RIGHT_ROI,
       LEFT_ROI,
       ROI_COUNT
};
#endif

#define CENTER_X 0
#define CENTER_Y 215
#define CENTER_WIDTH 1920
#define CENTER_HEIGHT 650

#define NUM_OF_W 5
#define NUM_OF_H 2

#define FPGA_SCARE_X  (1920.0f/3200.0f)


#define THREAD_L_M_RENDER  99
#define THREAD_L_S_RENDER	99

#define THREAD_L_4  	50
#define THREAD_L_6		50
#define THREAD_L_M_1	50
#define THREAD_L_S_1		50
#define THREAD_L_MVDECT	50

#define THREAD_L_GST	1

enum
{
	MVDECT_ADD_CN=0,
	 FPGA_FOUR_CN  =1,
	 SUB_CN ,
	MAIN_CN ,
	 MVDECT_CN	,
	 FPGA_SIX_CN ,
	 MAX_CC
};

#define NUM2_OF_W 2
#define NUM2_OF_H 2
#define PARTITIONS1 10
#define PARTITIONS2 4
#define FPGA_SCREEN_WIDTH 1280
#define FPGA_SCREEN_HEIGHT 1080



#define DECTOR_TARGET_MAX 40
#define ALARM_MAX_COUNT 4
#define MAX_SCREEN_WIDTH 1920
#define MAX_SCREEN_HEIGHT 1080
#define PAL_WIDTH 720
#define PAL_HEIGHT 576

#define SRCUYVY 0
#define SRCUYVUYV 1

#if USE_12
#define TELXLIMIT 8.0
enum{
	FORESIGHT_POS_LEFT ,
	FORESIGHT_POS_MID ,
	FORESIGHT_POS_RIGHT ,

	TEL_FORESIGHT_POS_FRONT ,
	TEL_FORESIGHT_POS_RIGHT ,
	TEL_FORESIGHT_POS_BACK ,
	TEL_FORESIGHT_POS_LEFT ,

	TRACK_VGA,
	TRACK_SDI,
	TRACK_PAL,

	FORESIGHT_POS_COUNT
};

enum
{
	MAIN,
	SUB,
	MS_COUNT
};
enum
{
	MAIN_FPGA_FOUR,
	MAIN_ONE_OF_TEN,
	MAIN_1,
	MAIN_2,
	MAIN_3,
	MAIN_4,
	MAIN_5,
	MAIN_6,
	MAIN_7,
	MAIN_8,
	MAIN_9,
	MAIN_10,
	MAIN_FPGA_SIX,

	SUB_FPGA_FOUR,
	SUB_ONE_OF_TEN,
	SUB_FPGA_SIX,
	MAIN_ONE_OF_TEN_TOUCH,
	QUE_CHID_COUNT
};

#define   MOVE_UP  1
#define   MOVE_DOWN  2
#define   MOVE_LEFT  3
#define   MOVE_RIGHT 4
#define   MOVE_COUNT 4

enum{
    CHAN_GRAY =1,
	CHAN_YUV ,
};


#define TRACKFRAMEX  60
#define TRACKFRAMEY  60
enum{
	SEND_TRACK_START_X,  //LEFT UP
	SEND_TRACK_START_Y,
	SEND_TRACK_FRAME_X,
	SEND_TRACK_FRAME_Y,
	SEND_TRACK_NUM,
	SEND_COUNT
};

enum{
	RECV_TRACK_START_X,  //LEFT UP
	RECV_TRACK_START_Y,
	RECV_TRACK_FRAME_X,
	RECV_TRACK_FRAME_Y,
	RECV_TRACK_NUM
};
enum{
	CHOSEN_CAM_MAIN,
	CHOSEN_CAM_SUB,
	CHOSEN_CAM_COUNT
};


enum{
	SDI_CAM_0,
//	SDI_CAM_1,
	SDI_CAM_COUNT
};
enum{
	MAIN_TARGET_0,
	MAIN_TARGET_1,
	SUB_TARGET_0,
	SUB_TARGET_1,
	TARGET_CAM_COUNT
};

enum{
	VGA_CAM_0,
	VGA_CAM_COUNT
};
#endif
#define VGA_WIDTH 1024
#define VGA_HEIGHT 768

#define SDI_WIDTH 1920
#define SDI_HEIGHT 1080
#define FPGA_SINGLE_PIC_W 640
#define FPGA_SINGLE_PIC_H 540
#define VGA_DEV_NUM 5
#define SDI_DEV_NUM 4

#define VGA_DEV_chId 20
#define SDI_DEV_chId  16
#if USE_BMPCAP &&FORCE_1920
#define SCREEN_W 1024
#define SCREEN_H 768
#else
#define SCREEN_W 1920
#define SCREEN_H 1080
#endif

#define CURRENT_SCREEN_WIDTH 1920
#define CURRENT_SCREEN_HEIGHT 1080

#define PBO_ALTERNATE_NUM 2
enum{
	VGA_COUNT=1,
	SDI_COUNT=1,
	HD_CAM_COUNT
};

enum{
	VGA_CHAN_NUM =1,
	SDI_CHAN_NUM,
};

#define DS_CHAN_MAX         (4)

enum{
	MAIN_EXT_0,
	SUB_EXT_1,
	MAIN_AND_SUB_EXT_COUNT
};
enum{
	SUB_EXT_0,
	SUB_EXT_COUNT
};
enum{
	MVDECT_CAM_0,
	MVDECT_CAM_1,
	MVDECT_CAM_2,
	MVDECT_CAM_3,
	MVDECT_CAM_4,
	MVDECT_CAM_5,
	MVDECT_CAM_6,
	MVDECT_CAM_7,
	MVDECT_CAM_8,
	MVDECT_CAM_9,
	MVDECT_CAM_COUNT
};

enum{
	EXT_CAM_0,
	EXT_CAM_1,
	EXT_CAM_COUNT
};
extern int MODLE_CIRCLE;
extern int PER_CIRCLE  ;
extern int BLEND_OFFSET;
extern int LOOP_RIGHT;
extern int BLEND_HEAD;
extern int BLEND_REAR;
extern int ISDYNAMIC;
extern float VALUE_THRESHOLD;
#define FOV 30   // Field of view for perspective mode
/* ViewFlag arguments */
#define ORTHO 1
#define PERSPECTIVE 0
#define GL_YES 1
#define GL_NO 0

/* Other Constants */
#define PI 3.14159265358979323846

#define  H_EXPAND_TIMES  1.0/1.0
#define  W_EXPAND_TIMES  1.0/1.0

#if WHOLE_PIC
#define	DEFAULT_IMAGE_WIDTH  1280
#define	DEFAULT_IMAGE_HEIGHT 720
#define	PANO_TEXTURE_WIDTH 1280
#define	PANO_TEXTURE_HEIGHT 720
#define NUM_OF_W_ORI 1
#define  KX ((PANO_TEXTURE_WIDTH/NUM_OF_W)/	(MAX_SCREEN_WIDTH/NUM_OF_W_ORI))


#else
#define	DEFAULT_IMAGE_WIDTH  1920
#define	DEFAULT_IMAGE_HEIGHT 1080
#define	PANO_TEXTURE_WIDTH 1920
#define	PANO_TEXTURE_HEIGHT 1080
#endif


#define DEFAULT_IMAGE_DEPTH	 3

#define IMAGEWIDTH DEFAULT_IMAGE_WIDTH
#define IMAGEHEIGHT DEFAULT_IMAGE_HEIGHT
#define IMAGEBYTES (IMAGEWIDTH* IMAGEHEIGHT * DEFAULT_IMAGE_DEPTH)

#define DEFAULT_VEHICLE_SCALE             0.95f //1.3f //0.48f //1.6f //
#define DEFAULT_VEHICLE_SCALE_HEIGHT      0.85f //1.3f //0.48f //1.6f //
#define DEFAULT_VEHICLE_TRANSLATION_0     0.0f
#define DEFAULT_VEHICLE_TRANSLATION_1    0.2f
#define DEFAULT_VEHICLE_TRANSLATION_2     0.0f
#define DEFAULT_TRACK2_VEHICLE_WIDTH_RATE 0.95f
#define DEFAULT_TRACK_LENGTH_METER		  (1.0/24*26)
#define DEFAULT_FRONT_TRACK_LENGTH_METER  (3.0/24*26)

#define DEFAULT_SHADOW_TO_VEHICLE_RATE_WIDTH  1.1f //1.1f //2.1f //0.6f //
#define DEFAULT_SHADOW_TO_VEHICLE_RATE_LENGTH 0.9f //0.75f //0.5f //0.95f //
#define DEFAULT_SHADOW_TRANSLATE_LENGTH_METER -0.05f

static const float vBlack[] 	= { 0.0f, 0.0f, 0.0f, 1.0f };
static const float vWhite[] 	= { 1.0f, 1.0f, 1.0f, 1.0f};
static const float vLtBlue[]    = { 0.00f, 0.00f, 1.00f, 0.90f };
static const float vBlue[]    = { 0.00f, 0.00f, 1.00f, 0.90f };
static const float vLtPink[]    = { 0.40f, 0.00f, 0.20f, 0.50f };
static const float vLtYellow[]  = { 0.98f, 0.96f, 0.14f, 0.30f };
static const float vYellow[]    = { 0.98f, 0.96f, 0.00f, 1.00f };
static const float vLtMagenta[] = { 0.83f, 0.04f, 0.83f, 0.70f };
static const float vLtGreen[]   = {18/255.0,77/255.0,36/255.0,1.0f};//{ 0.05f, 1.0f, 0.14f, 0.90f };
static const float vGreen[]     = {0.0f, 255/255.0, 0.0f, 1.0f};
static const float vLtGrey[]    = { 0.85f, 0.85f, 0.85f, 0.9f };
static const float vGrey[]      = { 0.5f, 0.5f, 0.5f, 1.0f };
static const float vRed[]       = {109/255.0,10/255.0,12/255.0,1.0f};//{ 0.7f, 0.0f, 0.0f, 1.0f };
static const float vBRed[]       = {1.0,0.0,0.0,1.0f};//{ 0.7f, 0.0f, 0.0f, 1.0f };
static const float *DEFAULT_TEXT_COLOR = vWhite;
#if ADD_FUCNTION_BY_JIMMY
static const float vReal_Red[]       = {1.0,0.0,0.0,1.0f };   //{ 0.7f, 0.0f, 0.0f, 1.0f };
#endif

#define VERSION_STRING		       "v0.18"
#define DEFAULT_FRONT_TGA         "lable_00.tga"
#define DEFAULT_REAR_TGA           "lable_03.tga"
#define DEFAULT_FRONT_LEFT_TGA     "lable_05.tga"
#define DEFAULT_REAR_LEFT_TGA	   "lable_04.tga"
#define DEFAULT_FRONT_RIGHT_TGA    "lable_01.tga"
#define DEFAULT_REAR_RIGHT_TGA	   "lable_02.tga"

#define DEFAULT_COMPASS "lable_04.tga"//"compass.tga"

#define DEFAULT_EXT_WG712_TGA	"wg712.tga"


#if USE_12
#define ONEX_REALTIME_TGA     "1X.tga"
#define TWOX_REALTIME_TGA     "2X.tga"
#define FOURX_REALTIME_TGA     "4X.tga"
#define ANGLE_TGA					"angle.tga" //潜望镜下方位抚养

#define LOCATION_TGA				"location.tga" //方位指示 模式
#define RADAR_FRONT_TGA   	"radar_front.tga"
#define RADAR_LEFT_TGA   	"radar_left.tga"
#define RADAR_RIGHT_TGA   	"radar_right.tga"
#define RADAR_BACK_TGA   	"radar_back.tga"

#define SDI1_WHITE_BIG_TGA  "sdi1_white_big.tga"
#define SDI1_WHITE_SMALL_TGA  "sdi1_white_small.tga"
#define SDI2_HOT_BIG_TGA  "sdi2_hot_big.tga"
#define SDI2_HOT_SMALL_TGA  "sdi2_hot_small.tga"

#define PAL1_WHITE_BIG_TGA  "pal1_white_big.tga"
#define PAL1_WHITE_SMALL_TGA  "pal1_white_small.tga"
#define PAL2_HOT_BIG_TGA  "pal2_hot_big.tga"
#define PAL2_HOT_SMALL_TGA  "pal2_hot_small.tga"

#define VGA_WHITE_BIG_TGA  "vga_white_big.tga"
#define VGA_WHITE_SMALL_TGA  "vga_white_small.tga"
#define VGA_HOT_BIG_TGA  "vga_hot_big.tga"
#define VGA_HOT_SMALL_TGA  "vga_hot_small.tga"

#define VGA_FUSE_WOOD_TGA  "vga_fuse_wood.tga"
#define VGA_FUSE_GRASS_TGA  "vga_fuse_grass.tga"
#define VGA_FUSE_SNOW_TGA  "vga_fuse_snow.tga"
#define VGA_FUSE_DESERT_TGA  "vga_fuse_desert.tga"
#define VGA_FUSE_CITY_TGA  "vga_fuse_city.tga"

#define	FINE_TGA						"fine.tga"
#define WRONG_TGA				"wrong.tga"
#define IDLE_TGA						"idle.tga"

#define DEBUG_TGA "debug.tga"
#define TURRET_TGA			"paota.tga"
#define PANORAMIC_MIRROR_TGA "zhoushijing.tga"
#define MENU_TGA				"menu.tga"


#define FAR_START_UP_WINDOW "start_up_window.tga"
#define FAR_STOP_UP_WINDOW "stop_up_window.tga"
#define FAR_START_MOVE_DETECT "start_move_detect.tga"
#define FAR_STOP_MOVE_DETECT "stop_move_detect.tga"
#define FAR_START_ENHANCE "start_enhance.tga"
#define FAR_STOP_ENHANCE "stop_enhance.tga"
#define FAR_UP "up.tga"
#define FAR_DOWN "down.tga"
#define FAR_LEFT "left.tga"
#define FAR_RIGHT "right.tga"
#define F9_CAPTAIN "F9_captain.tga"
#define F9_DRIVER  "F9_driver.tga"
#define F10_RETURN "F10_return.tga"

#define FAR_START_TEST "start_test.tga"
#define FAR_EXIT_TEST "stop_test.tga"
#define FAR_NEXT "next.tga"
#define STATE_LABEL "state_label.tga"
#define STATE_LABEL2 "state_label2.tga"
#define NOTHING_TGA "NoThing.tga"

#define POINT_RED_F "point_red.tga"
#define POINT_GREEN_F "point_green.tga"
#define POINT_GREY_F "point_grey.tga"
#define CANON_HOR "canon_hor.tga"
#define CANON_VER "canon_ver.tga"
#define GUN_HOR "gun_hor.tga"
#define GUN_VER "gun_ver.tga"
#define CALC_HOR "calc_hor.tga"
#define CALC_VER "calc_ver.tga"
#define GUN_CANON_COMPASS "m_compass.tga"
#define INFO_SHOW "m_info_show.tga"
#define AROUND_MIRROR "m_around_mirror.tga"
#define CANON_DATA "m_canon_data.tga"
#define ORI00 "m_ori00.tga"
#define ORI01 "m_ori01.tga"
#define ORI02 "m_ori02.tga"
#define ORI03 "m_ori03.tga"
#define ORI04 "m_ori04.tga"
#define ORI05 "m_ori05.tga"
#define ORI06 "m_ori06.tga"
#define ORI07 "m_ori07.tga"
#define ORI08 "m_ori08.tga"
#define ORI09 "m_ori09.tga"
#define NOSIG "noSig.tga"

#define OSD_FAR_CAM_1_TGA	"osd_far_cam_1.tga"
#define OSD_FAR_CAM_2_TGA	"osd_far_cam_2.tga"
#define OSD_FAR_CAM_3_TGA	"osd_far_cam_3.tga"
#define OSD_FAR_CAM_4_TGA	"osd_far_cam_4.tga"
#define OSD_FAR_CAM_5_TGA	"osd_far_cam_5.tga"
#define OSD_FAR_CAM_6_TGA	"osd_far_cam_6.tga"
#define OSD_FAR_CAM_7_TGA	"osd_far_cam_7.tga"
#define OSD_FAR_CAM_8_TGA	"osd_far_cam_8.tga"
#define OSD_GOOD_TGA	"osd_good.tga"
#define OSD_ERROR_TGA	"osd_error.tga"

enum CCT_COUNT {
		ONEX_REALTIME_T ,
		TWOX_REALTIME_T,
		FOURX_REALTIME_T,
		ANGLE_T,
		LOCATION_T,
		RADAR_FRONT_T,
		RADAR_LEFT_T,
		RADAR_RIGHT_T,
		RADAR_BACK_T,

		SDI1_WHITE_BIG_T,
		SDI1_WHITE_SMALL_T,
		SDI2_HOT_BIG_T,
		SDI2_HOT_SMALL_T,

		PAL1_WHITE_BIG_T,
		PAL1_WHITE_SMALL_T,
		PAL2_HOT_BIG_T,
		PAL2_HOT_SMALL_T,

		VGA_WHITE_BIG_T,
		VGA_WHITE_SMALL_T,
		VGA_HOT_BIG_T,
		VGA_HOT_SMALL_T,

		VGA_FUSE_WOOD_T,
		VGA_FUSE_GRASS_T,
		VGA_FUSE_SNOW_T,
		VGA_FUSE_DESERT_T,
		VGA_FUSE_CITY_T,

		FINE_T,
		WRONG_T,
		IDLE_T,

		TURRET_T,
		PANORAMIC_MIRROR_T,

		DEBUG_T,
		MENU_T,

		F1_ON_T,
		F1_OFF_T,
		F2_ON_T,
		F2_OFF_T,
		F3_ON_T,
		F3_OFF_T,
		F4_T,
		F5_T,
		F6_T,
		F7_T,
		F9_Captain,
		F9_Driver,
		F10_Return,
		
		F8_ON_T,
		F8_OFF_T,
		F9_T,
		STATE_LABEL_T,
		STATE_LABEL2_T,
		TGA_NoThing,
		POINT_RED_T,
		POINT_GREEN_T,
		POINT_GREY_T,
		CANON_HOR_T,
		CANON_VER_T,
		GUN_HOR_T,
		GUN_VER_T,
		CALC_HOR_T,
		CALC_VER_T,

		GUN_CANON_COMPASS_T,

		INFO_SHOW_T,
		CANON_DATA_T,
		AROUND_MIRROR_T,

		ORI00_T,
		ORI01_T,
		ORI02_T,
		ORI03_T,
		ORI04_T,
		ORI05_T,
		ORI06_T,
		ORI07_T,
		ORI08_T,
		ORI09_T,
		NOSIG_T,

		OSD_FAR_CAM_1_T,
		OSD_FAR_CAM_2_T,
		OSD_FAR_CAM_3_T,
		OSD_FAR_CAM_4_T,
		OSD_FAR_CAM_5_T,
		OSD_FAR_CAM_6_T,
		OSD_FAR_CAM_7_T,
		OSD_FAR_CAM_8_T,
		OSD_GOOD_T,
		OSD_ERROR_T,

		CCT_COUNT
		};
#define CBB_X 500
#define CBB_Y 155
#define CBB_WIDTH 150
#define CBB_HEIGHT	(155)
typedef enum TelBreak
{
	TEL_LEFT_BREAK,
	TEL_RIGHT_BREAK,
	TEL_NOT_BREAK
}telBreak;

enum Direction
{
	FRONT,
	RIGHT,
	BACK,
	LEFT
};
typedef enum Fore_Sight_Constitute
{
	pano_cross,
	WHOLE_IN_FRAME,
	WHOLE_OUT_FRAME,
	TEL_CROSS,
	TEL_IN_FRAME,
	TEL_OUT_FRAME,
	FS_CONSTITUTE_COUNT
}fsConstitute;


typedef enum Recv_Show_Mode
{
	RECV_VGA_WHITE_MODE = 0x01,
	RECV_VGA_WHITE_SMALL_USELESS_MODE,
	RECV_VGA_HOT_BIG_MODE ,
	RECV_VGA_HOT_SMALL_MODE,
	RECV_VGA_FUSE_WOOD_MODE ,
	RECV_VGA_FUSE_GRASS_MODE ,
	RECV_VGA_FUSE_SNOW_MODE ,
	RECV_VGA_FUSE_DESERT_MODE,
	RECV_VGA_FUSE_CITY_MODE,
	RECV_WHOLE_MODE,
	RECV_TELESCOPE_MODE,
	RECV_SDI1_WHITE_BIG_MODE,
	RECV_SDI1_WHITE_SMALL_MODE,
	RECV_SDI2_HOT_BIG_MODE,
	RECV_SDI2_HOT_SMALL_MODE,
	RECV_PAL1_WHITE_BIG_MODE,
	RECV_PAL1_WHITE_SMALL_MODE,
	RECV_PAL2_HOT_BIG_MODE,
	RECV_PAL2_HOT_SMALL_MODE,
	RECV_ENABLE_TRACK,
	RECV_SHOWMODE_COUNT
}RECV_MODE;



enum TRACK_VIDEO_CHID{
	SDI_TRACK=0,
	VGA_TRACK,
	PAL_TRACK,
	PAL2_TRACK};
/*
enum Recv_Fuse_Choice
{
	RECV_FUSE_WOODLAND =0x00,
	RECV_FUSE_GRASSLAND,
	RECV_FUSE_SNOWFIELD,
	RECV_FUSE_DESERT,
	RECV_FUSE_CITY,
	RECV_FUSECHOICE_COUND
};



enum Recv_Work_State
{
	RECV_WHITE_BIG=0x00,
	RECV_WHITE_SMALL,
	RECV_RED_BIG,
	RECV_RED_SMALL,
	RECV_WORKSTATE_COUNT
};
*/

typedef enum SelfCheck_State
{
	SELFCHECK_IDLE,
	SELFCHECK_PASS,
	SELFCHECK_FAIL,
	SELFCHECK_STATE_COUNT
}self_check_state;

typedef enum SelfCheck_Item
{
	CHECK_ITEM_CAPTURE,
	CHECK_ITEM_COMMUNICATION,
	CHECK_ITEM_BOARD,
	SELFCHECK_ITEM_COUNT
}self_check_item;


#define insideDH 2.45
#define insideUH 3.54/6
#define insideLen  200

#define outsideLen 100
#define outsideUH 2/3
#define outsideDH 3

#define inside4DH 2.225
#define inside4UH 1.82
#define inside4LW
#define inside4RW

#endif


#define DEFAULT_FIXED_BBD_HEIGHT 1.0f
#define DEFAULT_FIXED_BBD_WIDTH  1.0f
#define DDS_FILE_1M "1m.dds"
#define DDS_FILE_2M "2m.dds"
#define DDS_FILE_5M "5m.dds"
#define DDS_FILE_8M "8m.dds"

#define BIRD_VIEW_WIDTH_RATE (0.45/*1.0*3/7*/)
#define BIRD_VIEW_HEIGHT_RATE (1)
#define OTHER_VIEW_WIDTH_RATE (1-BIRD_VIEW_WIDTH_RATE)
#define OTHRE_VIEW_HEIGHT_RATE (1.0*1/4)


//dynamic track params in meters
#define DEFAULT_DYNAM_TRACK_COUNT 4
#define DEFAULT_WHEEL_DIST	3.2f
#define DEFAULT_AXLE_DIST	6.5f
#define DEFAULT_ARC_WIDTH	0.0f
#define DEFAULT_ARC_WIDTH_INI	0.0f
#define DEFAULT_ARC_WIDTH_SET	0.2f
#define DEFAULT_DELTA_ANGLE	50.0f
#define DEFAULT_REAR_TIRE_OFFSET (DEFAULT_WHEEL_DIST/2 + 1.5f)*(DEFAULT_VEHICLE_SCALE_HEIGHT)
#define DEGREE2RAD(angle) ((angle)*PI/180.0f)
#define RAD2DEGREE(rad)		((rad)*180.0f/PI)

//follow cross
#define CROSS_WIDTH 0.3
#define CROSS_HEIGHT 0.3
#define CROSS_MARGIN_WIDTH 0.05
#define CROSS_MARGIN_HEIGHT 0.05
#define CROSS_HOR_STEP 0.05
#define CROSS_VER_STEP 0.05
#define CROSS_VER_UP_SCALE 0.85
#define CROSS_VER_DOWN_SCALE 0.15


#define INVALID_DIRECTION -1

//sndmsg
#define IPC_SERVER 0xF001
#define IPC_CLIENT 0xF002
#define IPC_FOCAL   0xF003
#define IPC_LASER    0xF004



//corners definitions
typedef enum{
	CORNER_FRONT_LEFT,
	CORNER_FRONT_RIGHT,
	CORNER_REAR_LEFT,
	CORNER_REAR_RIGHT,
    CORNER_COUNT
} CORNER_POSITION;

typedef enum{
	RULER_45=0,
	RULER_90,
	RULER_180,
	RULER_COUNT
}RULER_ANGLE;
enum{
	ICON_45DEGREESCALE,
	ICON_90DEGREESCALE,
	ICON_180DEGREESCALE,
	CHOSEN_PIC_MAIN,
	CHOSEN_PIC_SUB,
	ICON_COUNT
};
#define MAX_PANO_CAMERA_COUNT 16
#define PANO_SCALE_ARRAY_FILE "pano_scale_array_data.yml"
#define PANO_HOR_VER_SCALE_FILE "pano_hor_ver_scale_data.yml"

#define USE_COMPASS_ICON 0
#endif /* STLGLDEFINES_H_ */
