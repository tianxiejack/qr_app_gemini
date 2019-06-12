/*
 * IPC_Sender.c
 *
 *  Created on: Apr 15, 2017
 *      Author: hoover
 */

/*IPC_sender.c*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h> 	/* add this: exit返回,不会报提示信息 */
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>  /* ture false 有效*/
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "Zodiac_Message.h"
#include "Serial_port.h"

#define FTOK_PATH "/home/ubuntu"
#if 0
int ret_value;
int ipc_port = 125;
key_t ipc_key;
int ipc_qid;

int fd;
IPC_msg g_MSG[2] = { 0 };

typedef struct zodiac_pack_result_ok {
	unsigned char frame_header[3];/*帧头：0x244f4b:"$OK"*/
	unsigned char separator;/*分隔符：0x2c:','*/
	unsigned char checksum;/*和校验：1-4字节的和:0xea*/
} Zodiac_ok;/*接收自检信息成功，返回OK包*/

typedef struct zodiac_pack_result_msg {
	unsigned char frame_header[3];/*帧头：0x245a42:"$ZB"*/
	unsigned char separator_head;/*分隔符：0x2c:','*/
	unsigned char coordinates_orientation[4];/*方位密位值：0-6000*/
	unsigned char acoordinates_ver[5];/*俯仰密位值:1字节符号：+/-：2b/2d 4字节数据：-167-+1000*/
	unsigned char separator_tail;/*分隔符：0x2c:','*/
	unsigned char checksum;/*和校验：1-14字节的和*/
} Zodiac_coordinates;/*发送坐标信息*/

typedef struct zodiac_pack_cycle_selfcheck {
	unsigned char frame_header[3];/*帧头：0x24514a:"$QJ"*/
	unsigned char separator_head;/*分隔符：0x2c:','*/
	unsigned char fault_code_number[2];/*添加的错误码个数记录*/
	fc4 cycle_faultcode[0];/*错误码*/
//	unsigned char separator_tail;/*分隔符：0x2c:','*/
//	unsigned char checksum;/*和校验：1~(7+n*4)字节的和*/
} Zodiac_cycle_slefcheck;/*周期自检故障码数据包*/

typedef struct zodiac_auto_tracking {
	unsigned char frame_header;/*帧头：0xEE*/
	unsigned char palstance_byte[3];/*角速度*/
	unsigned char checksum;/*校验和*/
} pack_tracking;

/*接收线程*/
void *ipc_recv(void* arg) {
	int i;
	IPC_msg msg;
	memset(&msg, 0, sizeof(IPC_msg));
	while (1) {
		printf("\n%x\n", ipc_qid);
		ret_value = msgrcv(ipc_qid, &msg, sizeof(msg), 0, 0);
		if (ret_value < 0) {
			printf("msgsnd() receive msg failed,errno=%d \n", errno);
		}
		switch (msg.msg_type) {
		case IPC_MSG_TYPE_SELF_CHECK:
			g_MSG[0].msg_type = IPC_MSG_TYPE_SELF_CHECK;
			g_MSG[0].payload.ipc_s_faultcode.fault_code_number =
					msg.payload.ipc_s_faultcode.fault_code_number;
			for (i = 0; i < g_MSG[0].payload.ipc_s_faultcode.fault_code_number;
					i++) {
				g_MSG[0].payload.ipc_s_faultcode.start_selfcheak[i] =
						msg.payload.ipc_s_faultcode.start_selfcheak[i];
			}
			printf("g_MSG[0].msg_type:%.02lx\n", g_MSG[0].msg_type);
			printf("g_MSG[0].payload.ipc_s_faultcode.fault_code_number:%.02x\n",
					g_MSG[0].payload.ipc_s_faultcode.fault_code_number);
			for (i = 0; i < g_MSG[0].payload.ipc_s_faultcode.fault_code_number;
					i++) {
				printf(
						"g_MSG[0].payload.ipc_s_faultcode.start_selfcheak[%d]:%.02x\n",
						i, g_MSG[0].payload.ipc_s_faultcode.start_selfcheak[i]);
			}
			break;
		case IPC_MSG_TYPE_40MS_HEARTBEAT:
			g_MSG[1].msg_type = IPC_MSG_TYPE_40MS_HEARTBEAT;
			g_MSG[1].payload.ipc_settings.display_mode =
					msg.payload.ipc_settings.display_mode;
			g_MSG[1].payload.ipc_settings.save = msg.payload.ipc_settings.save;
			g_MSG[1].payload.ipc_settings.turn = msg.payload.ipc_settings.turn;
			g_MSG[1].payload.ipc_settings.focal_length =
					msg.payload.ipc_settings.focal_length;
			g_MSG[1].payload.ipc_settings.orientation_angle =
					msg.payload.ipc_settings.orientation_angle;
			g_MSG[1].payload.ipc_settings.pitch_angle =
					msg.payload.ipc_settings.pitch_angle;
			g_MSG[1].payload.ipc_settings.distance =
					msg.payload.ipc_settings.distance;
			g_MSG[1].payload.ipc_settings.mobile_hor =
					msg.payload.ipc_settings.mobile_hor;
			g_MSG[1].payload.ipc_settings.mobile_ver =
					msg.payload.ipc_settings.mobile_ver;

			printf("g_MSG[1].msg_type:%.02lx\n", g_MSG[1].msg_type);

			printf("g_MSG[1].payload.ipc_settings.display_mode:%.02x\n",
					g_MSG[1].payload.ipc_settings.display_mode);
			printf("g_MSG[1].payload.ipc_settings.save:%.02x\n",
					g_MSG[1].payload.ipc_settings.save);
			printf("g_MSG[1].payload.ipc_settings.turn:%.02x\n",
					g_MSG[1].payload.ipc_settings.turn);
			printf("g_MSG[1].payload.ipc_settings.focal_length:%.02f\n",
					g_MSG[1].payload.ipc_settings.focal_length);
			printf("g_MSG[1].payload.ipc_settings.orientation_angle:%.02d\n",
					g_MSG[1].payload.ipc_settings.orientation_angle);
			printf("g_MSG[1].payload.ipc_settings.pitch_angle:%d\n",
					g_MSG[1].payload.ipc_settings.pitch_angle);
			printf("g_MSG[1].payload.ipc_settings.distance:%.02d\n",
					g_MSG[1].payload.ipc_settings.distance);
			printf("g_MSG[1].payload.ipc_settings.mobile_hor:%.02d\n",
					g_MSG[1].payload.ipc_settings.mobile_hor);
			printf("g_MSG[1].payload.ipc_settings.mobile_ver:%.02d\n",
					g_MSG[1].payload.ipc_settings.mobile_ver);
			if (g_MSG[1].payload.ipc_settings.mobile_ver < 0) {
				printf("g_MSG[1].payload.ipc_settings.mobile_ver小于零！\n");
			}
			break;
		default:
			printf("Recv:msg.msg_type out of range!\n");
		}
	}
}
/*消息队列模块：包括初始化和开接收线程*/
void Zodiac_InitIPCModule() {
	pthread_t thread;
	int ret, err;
	char port[] = "/dev/ttyTHS2";
	ipc_key = ftok(FTOK_PATH, ipc_port);
	if (ipc_key == -1) {
		printf(" port %d ftok key failed erro:  \n", ipc_port);
		perror(FTOK_PATH);
		exit(-1);
	}
	ipc_qid = msgget(ipc_key, IPC_EXCL); /*检查消息队列是否存在*/
	if (ipc_qid < 0) {
		ipc_qid = msgget(ipc_key, IPC_CREAT | 0666);/*创建消息队列*/
		if (ipc_qid < 0) {
			printf("failed to create msq | errno=%d \n", errno);
			perror(" ");
			exit(-1);
		}
	} else {
		printf("port %d already exist! \n", ipc_port);
	}
	ret = pthread_create(&thread, NULL, ipc_recv, NULL);
	if (ret != 0) {
		printf("Creat thread failed!\n");
		exit(ret);
	}
	fd = UART0_Open(fd, port); //打开串口，返回文件描述符
	do {
		err = UART0_Init(fd, 115200, 0, 8, 1, 'N');
		//printf("Set Port Exactly!\n");
		//printf("%d  %d\n", fd, err);
	} while (-1 == err || -1 == fd);
}

void WriteMessage(IPC_msg* value) {
	Zodiac_ok uart_ok;
	Zodiac_coordinates uart_pack;
	Zodiac_cycle_slefcheck *uart_csc_pack;
	pack_tracking uart_tracking;
	unsigned char data[512];
	int i, j, len;
	memset(&uart_ok, 0, sizeof(Zodiac_ok));
	memset(&uart_pack, 0, sizeof(Zodiac_coordinates));
	switch (value->msg_type) {
	case IPC_MSG_TYPE_START_SELF_CHECK_FEEDBACK:
		memset(&uart_ok, 0, sizeof(Zodiac_ok));
		uart_ok.frame_header[0] = '$';
		uart_ok.frame_header[1] = 'O';
		uart_ok.frame_header[2] = 'K';
		uart_ok.separator = ',';
		uart_ok.checksum = 0xea;
		len = UART0_Send(fd, (char *) &uart_ok, sizeof(Zodiac_ok));
		memset(g_MSG, 0, sizeof(IPC_msg));
		break;
	case IPC_MSG_TYPE_COORDINATES_FEEDBACK:
		uart_pack.frame_header[0] = '$';
		uart_pack.frame_header[1] = 'Z';
		uart_pack.frame_header[2] = 'B';
		uart_pack.separator_head = ',';
		uart_pack.separator_tail = ',';
		for (i = 0; i < sizeof(uart_pack.coordinates_orientation); i++) {
			uart_pack.coordinates_orientation[i] = '0'
					+ (value->payload.ipc_coordinates.coordinates_orientation
							/ (int) pow(10, 3 - i)) % 10;
		}
		if (value->payload.ipc_coordinates.acoordinates_ver >= 0) {
			uart_pack.acoordinates_ver[0] = '+';
		} else {
			uart_pack.acoordinates_ver[0] = '-';
		}
		for (i = 1; i < sizeof(uart_pack.acoordinates_ver); i++) {
			uart_pack.acoordinates_ver[i] = '0'
					+ (value->payload.ipc_coordinates.acoordinates_ver
							/ (int) pow(10, 4 - i)) % 10;
		}
		memcpy(data, &uart_pack, sizeof(Zodiac_coordinates) - 1);
		for (i = 0; i < sizeof(Zodiac_coordinates) - 1; i++) {
			uart_pack.checksum += data[i];
		}
		len = UART0_Send(fd, (char *) &uart_pack, sizeof(Zodiac_coordinates));
		break;
	case IPC_MSG_TYPE_CYCLE_SELF_CHECK_FEEDBACK:
		len = value->payload.ipc_c_faultcode.fault_code_number * 4 + 8;
		char cycle_self_check[512];
		memset(cycle_self_check, 0, 512);
		cycle_self_check[0] = '$';
		cycle_self_check[1] = 'Q';
		cycle_self_check[2] = 'J';
		cycle_self_check[3] = ',';
		cycle_self_check[4] = '0'
				+ (value->payload.ipc_c_faultcode.fault_code_number
						/ (int) pow(10, 1) % 10);
		cycle_self_check[5] = '0'
				+ (value->payload.ipc_c_faultcode.fault_code_number % 10);
		for (i = 0; i < value->payload.ipc_c_faultcode.fault_code_number; i++) {
			cycle_self_check[6 + 4 * i] = 'Q';
			cycle_self_check[7 + 4 * i] = 'E';
			cycle_self_check[8 + 4 * i] = '0'
					+ (value->payload.ipc_c_faultcode.circle_faultcode[i] / 10)
							% 10;
			cycle_self_check[9 + 4 * i] = '0'
					+ (value->payload.ipc_c_faultcode.circle_faultcode[i] % 10);
		}
		cycle_self_check[6 + 4 * i] = 0x2c;
		for (j = 0; j < 6 + 4 * i; j++) {
			cycle_self_check[511] += cycle_self_check[j];
		}
		cycle_self_check[7 + 4 * i] = cycle_self_check[511];
		len = UART0_Send(fd, (char *) cycle_self_check, len);
		break;
	case IPC_MSG_TYPE_AUTOTRACKING:
		memset(&uart_tracking, 0, sizeof(pack_tracking));
		uart_tracking.frame_header = 0xEE;
		uart_tracking.palstance_byte[0] =
				((int) (value->payload.ipc_tracking.palstance * 1000) & 0xFF0000)
						>> 8;
		uart_tracking.palstance_byte[1] =
				((int) (value->payload.ipc_tracking.palstance * 1000) & 0xFF00)
						>> 4;
		uart_tracking.palstance_byte[2] =
				((int) (value->payload.ipc_tracking.palstance * 1000) & 0xFF);
		for (i = 0; i < 3; i++) {
			uart_tracking.checksum += uart_tracking.palstance_byte[i];
		}
		len = UART0_Send(fd, (char *) &uart_tracking, sizeof(pack_tracking));
		break;
	default:
		printf("IPC_msg.msg_type is out of range or no assignment!\n");
	}
}

IPC_msg ReadMessage(IPC_MSG_TYPE t) {
	int x = 0;
	if (t == IPC_MSG_TYPE_SELF_CHECK) {
		x = 0;
	} else if (t == IPC_MSG_TYPE_40MS_HEARTBEAT) {
		x = 1;
	}
	return g_MSG[x];
}

void IPC_Destroy(void) {
	msgctl(ipc_qid, IPC_RMID, 0); //删除消息队列
	UART0_Close(fd);
}
#endif
