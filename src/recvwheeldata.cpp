/*
 * recvwheeldata.cpp
 *
 *  Created on: 2017年4月14日
 *      Author: fsmdn118
 */
#include "recvwheeldata.h"
#include "GLRender.h"
#include <pthread.h>
#include <sys/msg.h>
#include <netinet/in.h>

extern Render render;

typedef struct
{
	float angle;
	int direction;
	int reserve;
}msg_param;

typedef struct
{
	long msgId;
	msg_param msgParam;
}ipc_msg;

typedef struct {
	unsigned short angle_hor;
	unsigned short angle_ver;
	unsigned short focal_grade;
	unsigned short laser_lenth;
	int direction;
	int reserve;
} msg_param_send;

typedef struct {
	long msgId;
	msg_param_send msgParam;
} ipc_msg_send;


void *recv_thread(void* )
{
	int ipc_port = 126;//125
 	key_t ipc_key = -1;
	int ipc_qid = -1;

  	ipc_msg msgs;
  	float angleValue;
	int ret_value;

	int hor,ver;
	float follow_angle[2];
	float angle_hor,angle_ver;
	unsigned short buf[4];
	ipc_msg_send msgs_send;

	ipc_key = ftok("/home/ubuntu/", ipc_port);

	while(1)
	{

	 	ipc_qid = msgget(ipc_key, IPC_EXCL);/*检查消息队列是否存在 */
	 	if(ipc_qid < 0)
		{
	    	//printf("msq not existed! errno=%d!\n",errno);
		//	perror(" ");
	    	usleep(10*1000);
	    	continue;
	 	}
	 	/*接收消息队列*/
/*	 	ret_value = msgrcv(ipc_qid, &msgs, sizeof(msgs), 0, 0);
	 	if(ret_value!=-1)
	 	{
	 		static bool b_set_arcWidth = true;
			if(b_set_arcWidth)
			{
				float set_arcWidth = DEFAULT_ARC_WIDTH_SET;
				if(render.SetWheelArcWidth(set_arcWidth) != 0)
					continue;
				b_set_arcWidth = false;
			}
	 		render.SetWheelAngle(msgs.msgParam.angle);
	 	}*/

	 	if(render.getSendFollowAngleEnable())
	 	{
	 		render.setSendFollowAngleEnable(false);
	 		memcpy(follow_angle,render.getFollowAngle(),sizeof(follow_angle));
	 		angle_hor=follow_angle[0];
	 		angle_ver=follow_angle[1];
			hor = angle_hor * 10;//10倍
			ver = angle_ver * 10;
			msgs_send.msgId = IPC_CLIENT;//类型
			buf[0] = hor >> 8 & 0x00FF;//转化为16进制
			buf[1] = hor & 0x00FF;
			buf[2] = ver >> 8 & 0x00FF;
			buf[3] = ver & 0x00FF;
			memcpy(&(msgs_send.msgParam.angle_hor), &buf[0], sizeof(short));
			memcpy(&(msgs_send.msgParam.angle_ver), &buf[2], sizeof(short));
			msgs_send.msgParam.angle_hor = ntohs(msgs_send.msgParam.angle_hor );
			msgs_send.msgParam.angle_ver = ntohs(msgs_send.msgParam.angle_ver ) ;

			printf("\nsend angle:hor: %f,ver: %f\n",angle_hor,angle_ver);
			/* 发送消息队列 */
			ret_value = msgsnd(ipc_qid, &msgs_send, sizeof(msgs_send.msgParam), IPC_NOWAIT);
			if (ret_value < 0) {
				printf("msgsnd() write msg failed \n");
				perror(" ");
				exit(-1);
			}
			usleep(100000);
	 	}

	 	}
}


void startrecv(void)
{
	pthread_t th;
	int ret;
	int arg = 10;

	ret = pthread_create( &th, NULL,recv_thread, &arg );
}
