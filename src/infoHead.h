#ifndef		_INFO_HEAD_H
#define		_INFO_HEAD_H
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define		TRK_TG_NUM		10
#define  		SAMPLE_NUMBER 	256
#define		DETECTOR_NUM		10

#define ASSERT			CV_Assert
#define TRACE			printf

#define		LPBYTE	unsigned char*
#define		BYTE		unsigned char

typedef	enum{
	CV_TYPE_UYVY		=		0x01,
	CV_TYPE_RGB		=		0x02,
	CV_TYPE_YUYV		=		0x03
}IMG_TYPE;

typedef	enum{
	TRK_STATE_IDLE				=		0x00,
	TRK_STATE_ACQ				=		0x01,
	TRK_STATE_TRACK				=		0x02
}TRK_STATE;

typedef	enum{
	WARN_STATE_IDLE			=		0x00,
	WARN_STATE_NORMAL	=		0x01,
	WARN_STATE_INVADE		=		0x02,
	WARN_STATE_LOST			=		0x04,
	WARN_STATE_ACTIVE		=		0x08,
	WARN_STATE_EDGE			=		0x10,
}WARN_ROI_STATE;

typedef	enum{
	TARGET_NORAM					=	0x00,
	TARGET_IN_POLYGON			=	0x01,
	TARGET_IN_EDGE					=	0x02,
	TARGET_OUT_POLYGON		=	0x03
}TARGET_TYPE;



typedef struct _pattern_t
{
	cv::Point  	lefttop;
	cv::Point  	rightbottom;//
	bool		   	bValid;
	bool		   	bEdge;

}Pattern;

typedef	struct	 _trk_rect_t{
	cv::Rect					targetRect;
	TRK_STATE				trkState;
	TARGET_TYPE				targetType;
	double					distance;
	int						disp_frames;
	int						trk_frames;
	WARN_ROI_STATE			warnType;
	int						index;
}TRK_RECT_INFO;


typedef	struct _trk_thred_t{
	float		searchThed;	//鎼滅礌閲嶅彔绯绘暟0.2
	float		trkThred;		//璺熻釜閲嶅彔绯绘暟0.2
	int			dispFrames;	//璀︽垝鍚庢樉绀哄抚鏁?5
	int			totalFrames;	//璀︽垝鎬绘樉绀哄抚鏁?00
	float		distRoi;			//绂昏鎴掔嚎鏈€澶ц窛绂?.0
	int			targetSize;		//鏈€灏忚鎴掔洰鏍囬潰绉?00锛屼笉鑳藉お灏忥紝鍚﹀垯涓嶆槗璺熻釜
}TRK_THRED;


#endif
