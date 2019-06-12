/*
 * Zodiac_Message1.h
 *
 *  Created on: Sep 8, 2017
 *      Author: wang
 */
#ifndef ZODIAC_MESSAGE_H
#define ZODIAC_MESSAGE_H
#if 0
typedef struct set_mode {
	unsigned char display_mode[2];/*显示模式：01--19*/
	unsigned char save;/*存储：1/0：1保存 0不存*/
	unsigned char turn;/*调转：1/0：1开始 0停止*/
	unsigned char focal_length[6];/*白光焦距：000.00-999.99*/
	unsigned char orientation_angle[4];/*方位密位值：0-6000*/
	unsigned char pitch_angle[5];/*俯仰密位值：1字节符号：+/-：2b/2d 4字节数据：-167-+1000*/
	unsigned char distance[4];/*测距值：0-9999*/
	unsigned char mobile_hor[2];/*水平移动，1字节符号(左移-右移+)：+/-：2b/2d 1字节数据：0-5*/
	unsigned char mobile_ver[2];/*高低移动，1字节符号(左移-右移+)：+/- ：2b/2d 1字节数据：0-5*/
} set_m;

typedef struct coordinates {
	unsigned char coordinates_hor[4];/*水平向坐标低位高位*/
	unsigned char acoordinates_ver[5];/*高低向坐标低位高位*/
} set_coord;

typedef enum {
	IPC_MSG_TYPE_INVALID, IPC_MSG_TYPE_SELF_CHECK = 0x01,/*开机自检包*/
	IPC_MSG_TYPE_40MS_HEARTBEAT,/*模式设置等信息包*/
	/*feedback msg*/
	IPC_MSG_TYPE_START_SELF_CHECK_FEEDBACK,/*图像拼接板开机自检返回包*/
	IPC_MSG_TYPE_COORDINATES_FEEDBACK,/*返回十字分化坐标*/
	IPC_MSG_TYPE_CYCLE_SELF_CHECK_FEEDBACK,/*图象处理板周期自检包*/
	IPC_MSG_TYPE_AUTOTRACKING,/*自动跟踪返回包，返回目标角速度*/
	IPC_MSG_COUNT
} IPC_MSG_TYPE;

typedef struct fault_code_3bit {
	unsigned char fault_code[3];/*开始自检故障码：E00-E99 3字节故障码*/
} fc3;
typedef struct fault_code_4bit {
	unsigned char fault_code[4];/*周期自检故障码：QE00-QE99 3字节故障码*/
} fc4;

typedef struct {
	long msg_type;
	union Zodiac_p {
		struct start_selfcheak_fualtcode {
			unsigned char fault_code_number;/*只记录故障码的个数*/
			unsigned char start_selfcheak[99];/*故障码：3字节一个：E00-E99*/
		} ipc_s_faultcode;
		struct control_borad_data {
			unsigned char display_mode;/*显示模式：01--19*/
			unsigned char save;/*存储：1/0：1保存 0不存*/
			unsigned char turn;/*调转：1/0：1开始调转 0停止调转*/
			float focal_length;/*白光焦距：000.00-999.99*/
			int orientation_angle;/*方位密位值：0-6000*/
			int pitch_angle;/*俯仰密位值：-167-+1000*/
			int distance;/*测距值：0-9999*/
			signed char mobile_hor;/*水平移动，1字节符号(左移-右移+)：+/-：2b/2d 1字节数据：0-5*/
			signed char mobile_ver;/*高低移动，1字节符号(左移-右移+)：+/- ：2b/2d 1字节数据：0-5*/
		} ipc_settings;
		struct process_borad_data {
			int coordinates_orientation;/*方位密位值：0-6000*/
			int acoordinates_ver;/*俯仰密位值:-167-+1000*/
		} ipc_coordinates;/*返回十字分化坐标*/
		struct zodiac_pack_cycle_selfcheck_faultcode {
			unsigned char fault_code_number;/*添加的错误码个数记录*/
			unsigned char circle_faultcode[99];/*错误码*/
		} ipc_c_faultcode;/*周期自检故障码数据包*/
		struct auto_tracking {
			float palstance;/*角速度*/
		} ipc_tracking;
	} payload;
} IPC_msg;

#ifdef __cplusplus
extern "C" {
#endif
void InitIPCModule();
void WriteMessage(IPC_msg* value);
IPC_msg ReadMessage(IPC_MSG_TYPE t);
void IPC_Destroy(void);
#ifdef __cplusplus
}
#endif
#endif
#endif /* ZODIAC_MESSAGE1_H_ */
