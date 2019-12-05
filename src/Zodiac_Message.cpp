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

