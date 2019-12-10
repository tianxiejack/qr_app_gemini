/*
 * NetManager.cpp
 *
 *  Created on: 2019年11月4日
 *      Author: wsh
 */

//	CPortInterface *pCom = PortFactory::createProduct(2);
//	pCom->copen();
/*
 * 803uart.cpp
 *
 *  Created on: 2019年7月31日
 *      Author: alex
 */

#include "NetManager.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <net/if.h>
#include <arpa/inet.h>

using namespace std;
using namespace cv;

const double PI = 3.14159265358979323846;
//#define Kx			(0.03)
extern float mx;
extern float mz;

//typedef struct{
//	int connfd;
//	bool bConnecting;
//	OSA_ThrHndl getDataThrID;
//	OSA_ThrHndl sendDataThrID;
//}CConnectVECTOR;

static NetManager* gThis;

NetManager::NetManager()
	:pNetcom(NULL),
	 existRecvThread(false),
	 m_cmdlength(16),
	 m_K(0.f)
{
	ptz[0]=0.0f;
	ptz[1]=0.0f;
	ptz[2]=0.0f;
	memset(m_recvdata,0,sizeof(m_recvdata));
	m_rcvBuf.clear();
	gThis = this;
	memset(&mutexConn,0,sizeof(mutexConn));
	OSA_mutexCreate(&mutexConn);
	createPort();
	bool flag = readParams("Coefficient.yml") ;
	if(! flag)
		printf("\t\tread param error\n");
//	writeParams("Coefficient.yml");
//	if(!  )
//		printf("\t\tread param error\n");

}

NetManager::~NetManager()
{
	if(pNetcom != NULL)
	{
		pNetcom->cclose();
		delete pNetcom;
	}
	OSA_mutexDelete(&mutexConn);
}


void NetManager::createPort()
{
	pNetcom= new CNetProc(6666);

	if(pNetcom != NULL)
	{
		com1fd = pNetcom->copen();
		printf("com1fd:%d\n", com1fd);
	}
	return;
}

void* NetManager::runUpExtcmd(void *)
{
	gThis->getExtcmd();
	return NULL;
}

void NetManager::getExtcmd()
{
	int sizercv;
	while(existRecvThread == false)
	{
		OSA_mutexLock(&mutexConn);
		sizercv = pNetcom->crecv(connfd, m_recvdata,sizeof(m_recvdata));
		OSA_mutexUnlock(&mutexConn);

		findValidData(m_recvdata,sizercv);
	}
}

void NetManager::findValidData(unsigned char *tmpRcvBuff, int sizeRcv)
{
	unsigned int uartdata_pos = 0;
	unsigned char frame_head[2]={0xAA, 0x55};

	static struct data_buf
	{
		unsigned int len;
		unsigned int pos;
		unsigned char reading;
		unsigned char buf[1024];
	}swap_data = {0, 0, 0,{0}};

	if(sizeRcv>0)
	{
//		for(int j=0;j<sizeRcv;j++)
//		{
//			printf("%02x ",tmpRcvBuff[j]);
//		}
//		printf("\n");

		while (uartdata_pos < sizeRcv)
		{
			if((0 == swap_data.reading) || (2 == swap_data.reading))
			{
				if(frame_head[swap_data.len] == tmpRcvBuff[uartdata_pos])
				{
					swap_data.buf[swap_data.pos++] =  tmpRcvBuff[uartdata_pos++];
					swap_data.len++;
					swap_data.reading = 2;
					if(swap_data.len == sizeof(frame_head)/sizeof(char))
						swap_data.reading = 1;
				}
				else
				{
					uartdata_pos++;
					if(2 == swap_data.reading)
						memset(&swap_data, 0, sizeof(struct data_buf));
				}
			}
			else if(1 == swap_data.reading)
			{
				swap_data.buf[swap_data.pos++] = tmpRcvBuff[uartdata_pos++];
				swap_data.len++;
				if(swap_data.len == m_cmdlength)
				{
					for(int i=0;i<swap_data.len;i++)
					{
						m_rcvBuf.push_back(swap_data.buf[i]);
					}
					parsing();
					memset(&swap_data, 0, sizeof(struct data_buf));
				}
			}
		}
	}
	return;
}

unsigned int NetManager::recvcheck_sum()
{
	unsigned int sum = 0;
	for(int i=2;i<14;i++)
	{
		sum += m_rcvBuf[i];
	}
	return sum;
}


void NetManager::parsing()
{
	int ret =  -1;

	if(m_rcvBuf.size()<m_cmdlength)
	{
		printf("1212345678098765467865675\n");
		return;
	}


	unsigned char temp[12] = {0};
	unsigned char checkSum = recvcheck_sum();

	if( ((checkSum>>8)&0xff) == m_rcvBuf[14] && ((checkSum&0xff) == m_rcvBuf[15]))
	{
		int j = 0;

		for(int i = 0;i < m_rcvBuf.size()-4;i++)
		{
			temp[i] = m_rcvBuf[i+2];
		}
		memcpy(&ptz, temp, sizeof(ptz));
		mx = m_K * (180/PI) * ptz[0];
		printf("p:%f, t:%f, mx:%f\n",ptz[0],ptz[1], mx);
//		IPCSendMSG(ptz);
	}
	else
		printf("%s,%d, checksum error:,cal is %02x,recv is %02x\n",__FILE__,__LINE__,checkSum,((m_rcvBuf[14]<<8)|m_rcvBuf[15]));

	m_rcvBuf.erase(m_rcvBuf.begin(),m_rcvBuf.begin()+m_cmdlength);
	return ;
}

void *NetManager::thread_Getaccept(void *p)
{
	gThis->getaccept();
	return NULL;

}

void NetManager::getaccept()
{
	struct sockaddr_in connectAddr;
	int len = sizeof(connectAddr);
	OSA_ThrHndl getDataThrID;
	while(existRecvThread == false)
	{
		memset(&connectAddr, 0, sizeof(connectAddr));
		connfd = pNetcom->caccept((struct sockaddr*)&connectAddr, (socklen_t*)&len);
		if(connfd < 0)
		{
			printf("ERR: Can not  accept  Client connect\r\n");
			continue;
		}else
		{
			printf("Success\n");

//			CConnectVECTOR *pConnect = (CConnectVECTOR *)malloc(sizeof(CConnectVECTOR));
//			pConnect->connfd = ll;
//			OSA_mutexLock(&pThis->mutexConn);
//			pThis->connetVector.push_back(pConnect);
//			OSA_mutexUnlock(&pThis->mutexConn);

//			pConnect->bConnecting=true;
			OSA_thrCreate(&getDataThrID,
					runUpExtcmd,
			        0,
			        0,
			        NULL);

		}

	}
}


float NetManager::getHorizontalAngle()	//获取水平角度
{
	float hor =  (ptz[1] * 180) / PI;
//	if(hor < 0.000001)
//		hor = fmodf( ( hor + 360.0f), 360.0f);
	return hor;
}
float NetManager::getVerticalAngle()	//获取垂直角度
{
	float ver = (ptz[2] * 180) / PI ;
//	if(ver < 0.000001)
//		ver = fmodf( (ver + 360.0f), 360.0f);
	return ver;
}

bool NetManager::readParams(const char* file)
{
	cv::FileStorage m_readfs(file,cv::FileStorage::READ);

	if(m_readfs.isOpened())
	{
		m_K = (float)m_readfs["coefficient"];
		printf("\tfrom param m_K:%f\n", m_K);
//		getParams();
		return true;
	}
	return false;
}

bool NetManager::writeParams(const char* file)
{
	cv::FileStorage m_writefs(file,FileStorage::WRITE);
	float aaa = 0.03f;
	if(m_writefs.isOpened())
	{
		m_writefs << "coefficient"      << aaa;
		printf("\t\twrite data to yml\n");
//		setParams();
		return true;
	}
	return false;
}

