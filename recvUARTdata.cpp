#include "recvUARTdata.h"
#include "GLRender.h"
#include <pthread.h>
#include <sys/msg.h>
#include <netinet/in.h>
#include"GLEnv.h"
extern GLEnv env1,env2;
extern Render render;

int changeLowtoHigh(unsigned char data)
{
	int result=0;
	result=((data&0xff00)>>8)+((data&0x00ff)<<8);
	return result;
}

void *recv_UART_thread(void* )
{
	GLEnv &env=env1;
	int ipc_port = 126;//125
 	key_t ipc_key = -1;
	int ipc_qid = -1;

  	IPC_msg msgs;
  	float angleValue;
	int ret_value;

	int hor,ver;
	float follow_angle[2];
	float angle_hor,angle_ver;
	unsigned short buf[4];
	IPC_msg msgs_send;//NEED SEND STRUCT

	ipc_key = ftok("/home/ubuntu/", ipc_port);

	int get_track_control_params[4];
	int i=0;

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
	 		 	ret_value = msgrcv(ipc_qid, &msgs, sizeof(msgs), 0, 0);
	 		 	if(ret_value!=-1)
	 		 	{
	 		 		switch(msgs.cmdId)
	 		 		{
	 		 		case IPC_CMD_SET_MODE:
	 		 			//msgs.payload.req_m.start_area_alarm;
	 		 			//msgs.payload.req_m.set_alarm_mode;
	 		 			if(msgs.payload.req_m.set_image_mode==0xf2)
	 		 			{
	 		 				render.ProcessOitKeys(env,'9', 0, 0);
	 		 			}
	 		 			else
	 		 			{
	 		 				render.ProcessOitKeys(env,'0', 0, 0);
	 		 			}

	 		 			if((msgs.payload.req_m.start_camera_track&0xff)==0xff)
	 		 			{
	 		 				get_track_control_params[0]=msgs.payload.req_m.target[0]-30;
	 		 				get_track_control_params[1]=msgs.payload.req_m.target[1]-30;
	 		 				get_track_control_params[2]=60;
	 		 				get_track_control_params[3]=60;

		 		 			render.settrackcontrolparams(get_track_control_params);
		 		 			render.ProcessOitKeys(env,'{', 0, 0);
	 		 			}
	 		 			else
	 		 			{
	 		 				render.ProcessOitKeys(env,'}', 0, 0);
	 		 			}
	 		 			//msgs.payload.req_m.target;

	 		 			break;
	 		 		case IPC_CMD_SET_ALARM_LINE:
	 		 			//msgs.payload.req_al.alarm_line;
	 		 			break;
	 		 		case IPC_CMD_SET_ALARM_AREA:
	 		 			//msgs.payload.req_aa.alarm_area;
	 		 			break;
	 		 		case IPC_CMD_RESERVED:
	 		 			break;
	 		 		case IPC_CMD_ALERT_ALARM:
	 		 			//msgs.payload.fb_am.alarm_msg;
	 		 			break;
	 		 			/*
	 		 		case IPC_CMD_TRACKING_RESULT:
	 		 			//msgs.payload.fb_tr.tracking_result;
	 		 			//for(i=0;i<4;i++)
	 		 			//{
	 		 			//	get_track_control_params[i]=changeLowtoHigh(msgs.payload.fb_tr.tracking_result[i]);
	 		 			//}

	 		 			break;
	 		 		case IPC_CMD_STOP_TRACKING:
	 		 			if(msgs.payload.fb_st.stop_tracking==0xff)
	 		 			{
	 		 				;
	 		 			}
	 		 			break;
	 		 			*/
	 		 		case IPC_CMD_INVALID:
	 		 			break;
	 		 		default:
	 		 			break;
	 		 		}
	 		 	}
/*
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

			ret_value = msgsnd(ipc_qid, &msgs_send, sizeof(msgs_send.msgParam), IPC_NOWAIT);
			if (ret_value < 0) {
				printf("msgsnd() write msg failed \n");
				perror(" ");
				exit(-1);
			}
			usleep(100000);
	 	}*/

	 	}
}


void startUARTrecv(void)
{
	pthread_t th;
	int ret;
	int arg = 10;

	ret = pthread_create( &th, NULL,recv_UART_thread, &arg );
}

