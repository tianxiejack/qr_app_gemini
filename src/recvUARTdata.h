/*
 * recvUARTdata.h
 *
 *  Created on: 2017年9月4日
 *      Author: fsmdn118
 */

#ifndef RECVUARTDATA_H_
#define RECVUARTDATA_H_

typedef enum{
	IPC_CMD_SET_MODE = 0x01,
	IPC_CMD_SET_ALARM_LINE,
	IPC_CMD_SET_ALARM_AREA,
	IPC_CMD_RESERVED,
	IPC_CMD_ALERT_ALARM,
	IPC_CMD_TRACKING_RESULT,
	IPC_CMD_STOP_TRACKING,
	IPC_CMD_INVALID
} IPC_COMMAN_ID;

#define WAIT40MS 40000
//note: all short has been converted to host byte-order
typedef struct{
	long cmdId;
	union command_payload {
		//request from UART_Daemon to GLRender
		struct request_mode {
			unsigned char start_area_alarm;//0xff:alarmOn;  0x00:alarmOff
			unsigned char set_alarm_mode;//0xf1:trespass; 0xf2:reverse; 0xf3:invade; 0xf4:lost
			unsigned char set_image_mode;//0xf1:704x360; 0xf2:704x576
			unsigned char start_camera_track;//0xff:trackingOn; 0x00:trackingOff
			unsigned short target[2];//targetX, targetY. valid when tracking is On.
		}req_m;
		struct request_alarm_line {
			unsigned short alarm_line[4];//x_start., y_start, x_end,y_end
		}req_al;
		struct request_alarm_area {
			unsigned short alarm_area[8];//x0,y0,x1,y1,x2,y2,x3,y3
		}req_aa;
		//feedback from GLRender to UART_Deamon
		struct feedback_alarm_msg {
			unsigned char alarm_msg[4];//trespass;reverse;invasion;lost
		}fb_am;
		struct feedback_tracking_result {
			unsigned short tracking_result[4];// targetX,Y,Width,Height. host byte-order
		}fb_tr;
		struct feedback_stop_tracking {
			unsigned char stop_tracking;//always 0xff
		}fb_st;
	} payload;
} IPC_msg;

void startUARTrecv(void);




#endif /* RECVUARTDATA_H_ */
