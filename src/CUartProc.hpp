#ifndef __CUARTPROC_H__
#define __CUARTPROC_H__

//#include "CPortInterface.hpp"
#include <iostream>
#include <vector>
#include <osa_mutex.h>
#include <osa_thr.h>

#define LEN 512

using namespace std;

class CUartProc //: public CPortInterface
{
public:
	CUartProc(const string dev_name,const int baud_rate, const int flow, const int data_bits, const char parity, const int stop_bits);
	~CUartProc();
	int copen();
	int cclose();
	int crecv(int fd, void *buf,int len);
	int csend(int fd, void *buf,int len);

	void findValidData(unsigned char *tmpRcvBuff, int sizeRcv);

	void RecvData();
	static void *thrRecv(void* org);
	void Parsing();

	OSA_ThrHndl recv_thrID;
private:
	int setPort(int fd, const int baud_rate, const int c_flow, const int data_bits, char parity, const int stop_bits);
	
private:
	string devname;
	int baudrate;
	int c_flow;
	int databits;
	char paritybits;
	int stopbits;

	vector<unsigned char>  m_rcvBuf;
	unsigned char m_recvdata[LEN];
	int comfd;
	int m_cmdlength;
	OSA_MutexHndl m_hndl;


};

#endif
