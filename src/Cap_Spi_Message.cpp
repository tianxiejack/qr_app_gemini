/*
 * Cyclops_Message.c
 *
 *  Created on: Dec 8, 2017
 *      Author: xz
 */

/*Cyclops_Message.c*/
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
#include <sys/time.h>
#include <time.h>
#include"Cap_Spi_Message.h"
#include "spiH.h"

void SpiSet() {
	int err, ret;
	if (ret = open_device("/dev/spidev3.0")) {   //open spi
		printf("spi0.0 open failed\n");
	}
	transfer_init(3, 1, 115200);
}

void WriteMessage(MSG_TYPE n, char buf) {
	unsigned char data[1024];
	memset(data, 0, sizeof(data));
	switch (n) {
	case MSG_TYPE_YUANJING_DATA1:
		data[0] = 0xaa;
		data[1] = 0x55;
		data[3] = 0x5a;
		data[4] = 0xa5;
		if (buf <= 10 && buf >= 1) {
			data[2] = buf & 0xf;
			sendDataToSpi(3, data, 5);
		} else {
			printf("远景相机无此编号！\n");
			break;
		}
		break;
	case MSG_TYPE_YUANJING_DATA2:
		data[0] = 0xaa;
		data[1] = 0x55;
		data[3] = 0x5a;
		data[4] = 0xa5;
		if (buf <= 10 && buf >= 1) {
			data[2] = (buf & 0xf) << 4;
			sendDataToSpi(3, data, 5);
		} else {
			printf("远景相机无此编号！\n");
			break;
		}
		break;
	case MSG_TYPE_JINJING_DATA1:
		data[0] = 0xaa;
		data[1] = 0x55;
		data[4] = 0x5a;
		data[5] = 0xa5;
		if (buf <= 10 && buf >= 1) {
			data[2] = buf & 0xf;
			data[3] = 0x0;
			sendDataToSpi(3, data, 6);
		} else {
			printf("近景相机无此编号！\n");
			break;
		}
		break;
	case MSG_TYPE_JINJING_DATA2:
		data[0] = 0xaa;
		data[1] = 0x55;
		data[4] = 0x5a;
		data[5] = 0xa5;
		if (buf <= 10 && buf >= 1) {
			data[2] = (buf & 0xf) << 4;
			data[3] = 0x0;
			sendDataToSpi(3, data, 6);
		} else {
			printf("近景相机无此编号！\n");
			break;
		}
		break;
	case MSG_TYPE_JINJING_DATA3:
		data[0] = 0xaa;
		data[1] = 0x55;
		data[4] = 0x5a;
		data[5] = 0xa5;
		if (buf <= 4 && buf >= 1) {
			data[2] = 0x0;
			data[3] = buf & 0xf;
			sendDataToSpi(3, data, 6);
		} else {
			printf("近景相机无此编号！\n");
			break;
		}
		break;
	default:
		printf("不存在的选项！\n");
	}

}
