#ifndef XIN_IPC_YUAN_RECV_MESSAGE_H
#define XIN_IPC_YUAN_RECV_MESSAGE_H

typedef enum {
	TRANSFER_TO_APP_ETHOR = 0x00,
	TRANSFER_TO_APP_DRIVER = 0x01,
	TRANSFER_TO_COMM = 0x02,
	IPC_NUM,
} IPC_NUM_TYPE;

typedef enum {
	Mode_Type_START = 0x00,

	Mode_Type_SINGLE_POPUP_WINDOWS, Mode_Type_DEBUG,

	Mode_Type_RESERVE,
} Mode_Type; //物理按键模式切换

typedef enum {
	MOVETYPE_START = 0x00,

	MOVE_TYPE_MOVEUP,
	MOVE_TYPE_MOVEDOWN,
	MOVE_TYPE_MOVELEFT,
	MOVE_TYPE_MOVERIGHT,

	MOVE_TYPE_COUNT,
} MOVE_TYPE; //物理按键移动方向

typedef enum {
	POINT_GREEN = 0x00, POINT_RED = 0x01, POINT_GRAY = 0x02,
} PIONT_COLOUR; //圆点颜色用这个表示

typedef struct {
	char cameraFrontState;
	char cameraFrontTest;
	char cameraFront_FAULT_Colour;
	char cameraLeft1State;
	char cameraLeft1Test;
	char cameraLeft1_FAULT_Colour;
	char cameraRight1State;
	char cameraRight1Test;
	char cameraRight1_FAULT_Colour;
	char cameraLeft2State;
	char cameraLeft2Test;
	char cameraLeft2_FAULT_Colour;
	char cameraRight2State;
	char cameraRight2Test;
	char cameraRight2_FAULT_Colour;
	char cameraLeft3State;
	char cameraLeft3Test;
	char cameraLeft3_FAULT_Colour;
	char cameraRight3State;
	char cameraRight3Test;
	char cameraRight3_FAULT_Colour;
	char cameraBackState;
	char cameraBackTest;
	char cameraBack_FAULT_Colour;
	char Cap_BoxState;
	char Cap_BoxTest;
	char Cap_FAULT_Colour;
	char passengerState;
	char passengerTest;
	char passenger_FAULT_Colour;
	char nearBoardState;
	char nearBoardTest;
	char nearBoard_FAULT_Colour;
} Cap_Msg; //预留的采集信息

typedef struct coordinate_point {
	int point_x;
	int point_y;
} coor_p; //点击的坐标

typedef struct {
	char type;
	float hor_angle;
	float ver_angle;
} ANGLE_GROUP;

typedef enum {
	DEBUG_ORDER_ORIGIN,

	DEBUG_ORDER_PERISCOPIC_MODE = 0x01,
	DEBUG_ORDER_CROSS_MOVEUP,
	DEBUG_ORDER_CROSS_MOVEDOWN,
	DEBUG_ORDER_CROSS_MOVELEFT,
	DEBUG_ORDER_CROSS_MOVERIGHT,
	DEBUG_ORDER_TARGETDETECTION_ON,
	DEBUG_ORDER_TARGETDETECTION_OFF,
	DEBUG_ORDER_TRIMMING_ON,
	DEBUG_ORDER_CHOOSECAMERA_LEFT,
	DEBUG_ORDER_CHOOSECAMERA_RIGHT,
	DEBUG_ORDER_CHECKEDCAMERA_MOVEUP,
	DEBUG_ORDER_CHECKEDCAMERA_MOVEDOWN,
	DEBUG_ORDER_CHECKEDCAMERA_MOVELEFT,
	DEBUG_ORDER_CHECKEDCAMERA_MOVERIGHT,
	DEBUG_ORDER_SAVE_TRIMMING_RESULT,
	DEBUG_ORDER_CLEAN_CHECKEDCAMERA_RESULT,
	DEBUG_ORDER_CLEAN_ALLCAMERA_RESULT,
	DEBUG_ORDER_TRIMMING_OFF,
	DEBUG_ORDER_SINGLECAMERA_MODE,
	DEBUG_ORDER_SINGLECAMERA_0,
	DEBUG_ORDER_SINGLECAMERA_1,
	DEBUG_ORDER_SINGLECAMERA_2,
	DEBUG_ORDER_SINGLECAMERA_3,
	DEBUG_ORDER_SINGLECAMERA_4,
	DEBUG_ORDER_SINGLECAMERA_5,
	DEBUG_ORDER_SINGLECAMERA_6,
	DEBUG_ORDER_SINGLECAMERA_7,
	DEBUG_ORDER_SINGLECAMERA_8,
	DEBUG_ORDER_SINGLECAMERA_9,
	DEBUG_ORDER_LEFT_HANDED_IMAGE,
	DEBUG_ORDER_RIGH_THANDED_IMAGE,
	DEBUG_ORDER_LONGITUDINAL_TENSILE_IMAGE,
	DEBUG_ORDER_LONGITUDINAL_COMPRESSION_IMAGE,
	DEBUG_ORDER_TRANSVERSE_TENSILE_IMAGE,
	DEBUG_ORDER_TRANSVERSE_COMPRESSION_IMAGE,


	DEBUG_ORDER_RESERVE,
} DEBUG_ORDER;

typedef struct {
	unsigned int ping_state1;
	unsigned int ping_state2;
} PingState;

void IPC_Init_All(); //初始化所有消息队列
void IPC_Destroy_All(); //删除所有消息队列

coor_p getEphor_CoorPoint(IPC_NUM_TYPE n); //得到车长触摸屏坐标,只能调用一次，之后数据会清除
Mode_Type getKey_SwitchMode(IPC_NUM_TYPE n); //得到车长显示器按键切换的模式,只能调用一次，之后数据会清除
int getOverlayInformation(IPC_NUM_TYPE n); //得到叠加信息的显示状态，数据会保存不清除，返回值0：默认全部显示；1：隐藏检测信息；2：同时隐藏炮塔和周视镜方位信息
int getKey_TargetDetectionState(IPC_NUM_TYPE n); //得到按键切换的目标检测状态，只能调用一次，之后数据会清除
int getKey_ImageEnhancementState(IPC_NUM_TYPE n); //得到按键切换的图像增强状态，只能调用一次，之后数据会清除
MOVE_TYPE getKey_MoveDirection(IPC_NUM_TYPE n); //得到按键切换的移动方向,只能调用一次，之后数据会清除,没有移动返回零
ANGLE_GROUP getAngleFar_PeriscopicLens(IPC_NUM_TYPE n); //得到周视镜方位信息，数据会一直保存当前角度，并根据信息更新
ANGLE_GROUP getAngleFar_GunAngle(IPC_NUM_TYPE n); //得到枪塔方位信息，数据会一直保存当前角度，并根据信息更新
ANGLE_GROUP getAngleFar_CanonAngle(IPC_NUM_TYPE n); //得到炮塔方位信息，数据会一直保存当前角度，并根据信息更新
Cap_Msg getCaptureMessage(); //采集信息，数据会一直保存，并根据新信息更新
int getF9Value();//得到F9按键是否被按下，1为起效，0为无效，数据会一直保存，调用后不会清除。
/*微调*/
DEBUG_ORDER getDebugModeOrder(IPC_NUM_TYPE n); //得到微调发送的命令编号，调用一次后清除
int getDebugStepSize(IPC_NUM_TYPE n);//返回微调步长：1,5,10,50，调用后不清除，默认为1

PingState getPingState();//得到ping的结果，1为通，0为不通，数据默认为1，一直保留，返回值成员state1表示载员显示器状态

#endif
