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
#include <fcntl.h>
#include "Zodiac_GPIO_Message.h"

#define Z2G_FTOK_PATH "/home/ubuntu"

#define GPIO_PIN_MAX		512
static int gDirection[GPIO_PIN_MAX] = {-1 };

int  z2g_ret_value;
int  z2g_ipc_port = 145;
key_t z2g_ipc_key;
int  z2g_ipc_qid;
int gpio_init()
{
	gpio_open( GPIO_FAR_0_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_FAR_1_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_FAR_2_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_FAR_3_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_FAR_4_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_FAR_5_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_FAR_6_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_FAR_7_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_FAR_8_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_FAR_9_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_NEAR_0_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_NEAR_1_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_NEAR_2_NUM, DX_GPIO_DIRECTION_IN );
	gpio_open( GPIO_NEAR_3_NUM, DX_GPIO_DIRECTION_IN );
return 0;
}
int gpio_open(unsigned int nPin, DX_GPIO_DIRECTION direction)
{
	int fd,nRet;
	char strSysfs[128];
	// Export control of the selected GPIO to userspace
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0)
	{
		perror("gpio/export");
		return -1;
	}
	if(nPin >= GPIO_PIN_MAX)
	{
		printf("gpio nPin out of range\r\n");
		return -1;
	}
	nRet = snprintf(strSysfs, sizeof(strSysfs), "%d", nPin);
	write(fd, strSysfs, nRet);
	close(fd);
	// Set the GPIO direction
	nRet = snprintf(strSysfs, sizeof(strSysfs), "/sys/class/gpio/gpio%d/direction", nPin);
	fd = open(strSysfs, O_WRONLY);
	if (fd < 0)
	{
		perror("gpio/direction");
		return -1;
	}
	if(DX_GPIO_DIRECTION_IN == direction)
		nRet = snprintf(strSysfs, sizeof(strSysfs), "in");
	else
		nRet = snprintf(strSysfs, sizeof(strSysfs), "out");
	write(fd, strSysfs, nRet);
	close(fd);
	gDirection[nPin] = (int)direction;
	return 0;
}

int get_gpioNum_Value(unsigned int gpio_num)
{
	int fd, nRet;
	char strSysfs[128];
	 snprintf(strSysfs, sizeof(strSysfs), "/sys/class/gpio/gpio%d/value", gpio_num);
	if(DX_GPIO_DIRECTION_IN == gDirection[gpio_num]){
		fd = open(strSysfs, O_RDONLY);
		if (fd < 0)
		{
			perror("gpio/value");
			return -1;
		}
		read(fd, strSysfs, 128);
		close(fd);
		strSysfs[1] = '\0';
		nRet = atoi(strSysfs);
		//if(gpio_num == GPIO_FAR_7_NUM ){
		//	printf("Read GPIO_FAR_7_NUM == 184 : <%d> \r\n",gpio_num);
		//}
		return nRet;
	}
	return -1;
}

void set_gpioNum_Value(int gpio_num,int gpio_value)
{
	SET_Gpio set_gpio;
	memset(&set_gpio,-1,sizeof(SET_Gpio));
	set_gpio.msgtype=NUM_VALUE;
	set_gpio.Data.gpioNum=gpio_num;
	set_gpio.Data.value=gpio_value;
	SendMessage(&set_gpio);
}


void delete_GPIO_IPCMessage()
{
	if(msgctl(z2g_ipc_qid,IPC_RMID,0)==-1)
	{
		printf("msgctl (IPC_RMID) failed\n");
		exit(-1);
	}
}


/*
void set_gpioDirection(int gpio_direction)
{
	SET_GPIO set_gpio;
	memset(&set_gpio,-1,sizeof(SET_GPIO));
	set_gpio.msgtype=DIRECTION;
	set_gpio.direction=gpio_direction;
	SendMessage(&set_gpio);
}
void set_gpioValue(int gpio_value)
{
	SET_GPIO set_gpio;
	memset(&set_gpio,-1,sizeof(SET_GPIO));
	set_gpio.msgtype=DIRECTION;
	set_gpio.value=VALUE;
	SendMessage(&set_gpio);
}
*/
void init_GPIO_IPCMessage()
{
	 z2g_ipc_key = ftok(Z2G_FTOK_PATH,  z2g_ipc_port);
		if ( z2g_ipc_key == -1) {
			printf(" port %d ftok key failed erro:  \n", z2g_ipc_port);
			perror(Z2G_FTOK_PATH);
			exit(-1);
		}
		 z2g_ipc_qid = msgget( z2g_ipc_key, IPC_EXCL); /*检查消息队列是否存在*/
		if ( z2g_ipc_qid < 0) {
			 z2g_ipc_qid = msgget( z2g_ipc_key, IPC_CREAT | 0666);/*创建消息队列*/
			if ( z2g_ipc_qid < 0) {
				printf("failed to create msq | errno=%d \n", errno);
				perror(" ");
				exit(-1);
			}
		} else {
			printf("port %d already exist! \n",  z2g_ipc_port);
		}
}
void SendMessage(Set_Gpio* value)
{
	if(msgsnd(z2g_ipc_qid,value,sizeof(value->Data),0)==-1)
	{
		perror("msgsnd error\n");
		exit(1);
	}
}
