/*
 * NetManager.h
 *
 *  Created on: 2019年11月4日
 *      Author: wsh
 */

#ifndef NETMANAGER_H_
#define NETMANAGER_H_

#include <vector>
//#include "PortFactory.hpp"
#include "osa_thr.h"
#include "osa_mutex.h"
#include "CNetProc.hpp"
#include <opencv/cv.hpp>



class NetManager
{
public:
	NetManager();
	~NetManager();

	void createPort();
	void getExtcmd();
	static void* runUpExtcmd(void*);

	static void *thread_Getaccept(void *p);
	void getaccept();

	float getHorizontalAngle();	//获取水平角度
	float getVerticalAngle();	//获取垂直角度

	bool readParams(const char* file);
	bool writeParams(const char* file);

protected:
	void findValidData(unsigned char *tmpRcvBuff, int sizeRcv);
	void parsing();
	unsigned int recvcheck_sum();


private:
	CNetProc* pNetcom;
	int com1fd, connfd;
//	unsigned char m_senddata[12];
	unsigned char m_recvdata[1024];

	bool existRecvThread;
	std::vector<unsigned char>  m_rcvBuf;
	int m_cmdlength;
	float ptz[3];
	float m_K;
	OSA_MutexHndl mutexConn;
//	sendIpcMsgCallback pFunc_SendIpc;


};




#endif /* NETMANAGER_H_ */
