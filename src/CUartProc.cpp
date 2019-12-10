#include "CUartProc.hpp"
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/opencv.hpp>
#include "StlGlDefines.h"
#include "Thread_Priority.h"


#define K			(0.02)
#define ENABLE_LOG	(1)
#define LOG(msg)	std::cout << msg
#define LOGLN(msg)	std::cout << msg <<std::endl
extern float mx;
//extern float mz;
static CUartProc* pThis;

Int32 t;

CUartProc::CUartProc(const string dev_name,const int baud_rate, const int flow, const int data_bits, const char parity, const int stop_bits)
	:comfd(-1), m_cmdlength(12)
{
	devname = dev_name;
	baudrate = baud_rate;
	c_flow = flow;
	databits = data_bits;
	paritybits = parity;
	stopbits = stop_bits;

	m_rcvBuf.clear();
	pThis = this;
	memset(m_recvdata, 0, sizeof(m_recvdata));
	OSA_mutexCreate(&m_hndl);
}

CUartProc::~CUartProc()
{
	OSA_mutexDelete(&m_hndl);
}

int CUartProc::copen()
{
	if((comfd = open(devname.c_str(), O_RDWR| O_NOCTTY)) <= 0)
	{
		printf("ERR: Can not open the Uart port --(%s)\r\n",devname.c_str());
		return -1;
	}
	else
		printf("open uart port %s success.fd=%d\n",devname.c_str(), comfd);

	setPort(comfd, baudrate, c_flow, databits, paritybits, stopbits);

	return comfd;
}

int CUartProc::cclose()
{
	close(comfd);
}

int CUartProc::crecv(int fd, void *buf,int len)
{
	int fs_sel;
	fd_set fd_uartRead;
	struct timeval timeout;
	FD_ZERO(&fd_uartRead);
	FD_SET(fd,&fd_uartRead);

	timeout.tv_sec = 0;
	timeout.tv_usec = 60*1000;//40000;

	fs_sel = select(fd+1,&fd_uartRead,NULL,NULL,&timeout);
	if(-1 == fs_sel)
	{
		printf("ERR: Uart Recv  select  Error!!\r\n");
		return -1;
	}
	else if(fs_sel)
	{
		int recvValue = 0;
		read(fd,buf,len);
		return  recvValue;//read(fd,buf,len);
	}
	else if(0 == fs_sel)
	{
		printf("Warning: Uart Recv  time  out!!\r\n");
		return 0;
	}
}

int CUartProc::csend(int fd, void *buf,int len)
{
	return write(fd, buf, len);
}

int CUartProc::setPort(int fd, const int baud_rate, const int c_flow, const int data_bits, char parity, const int stop_bits)
{
    struct termios newtio;
    memset(&newtio,  0, sizeof(newtio));

    if(tcgetattr(fd, &newtio))
    {
        printf( "tcgetattr error : %s\n", strerror(errno));
        return -1;
    }
    //set baud rate
    switch (baud_rate)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 19200:
            cfsetispeed(&newtio, B19200);
            cfsetospeed(&newtio, B19200);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 38400:
            cfsetispeed(&newtio, B38400);
            cfsetospeed(&newtio, B38400);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 57600:
            cfsetispeed(&newtio, B57600);
            cfsetospeed(&newtio, B57600);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            printf("[setPort]baudrate:%d\n", baud_rate);
    }


    newtio.c_cflag |= CLOCAL;
    newtio.c_cflag |= CREAD;//3
    

    switch(c_flow)
    {
        default:
        case 0:// none
            newtio.c_cflag &= ~CRTSCTS;
	   printf("[setPort]c_flow:none\n");
            break;
        case 1:// hardware
            newtio.c_cflag |= CRTSCTS;
	   printf("[setPort]c_flow:hardware\n");
            break;
        case 2:// software
            newtio.c_cflag |= IXON|IXOFF|IXANY;
	   printf("[setPort]c_flow:software\n");
            break;
    }


    //set data bits
    newtio.c_cflag &= ~CSIZE;
    switch(data_bits)
    {
        case 5:
	    newtio.c_cflag |= CS5;
	    printf("[setPort]data_bits:5\n");
	    break;
        case 6:
	    newtio.c_cflag |= CS6;
	    printf("[setPort]data_bits:6\n");
	    break;
        case 7:
            newtio.c_cflag |= CS7;
	   printf("[setPort]data_bits:7\n");
            break;
        case 8:
            newtio .c_cflag |= CS8;
	   printf("[setPort]data_bits:8\n");
            break;
        default:
            newtio.c_cflag |= CS8;
            printf("[setPort]data_bits:8\n");
            break;
    }
	
    //set parity
    switch (parity)
    {
        default:
        case 'n':
        case 'N':
            newtio.c_cflag &= ~PARENB;
            newtio.c_iflag &= ~INPCK;
	   printf("[setPort]parity:N\n");
            break;
        case 'o':
        case 'O':
            newtio.c_cflag |= (PARODD|PARENB);
            newtio.c_iflag |= INPCK;
	   newtio.c_iflag |= ISTRIP;
	   printf("[setPort]parity:O\n");
            break;
        case 'e':
        case 'E':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            newtio.c_cflag |= INPCK;
            newtio.c_iflag |= ISTRIP;
	   printf("[setPort]parity:E\n");
            break;
        case 's':
        case 'S':
            newtio.c_cflag &= ~PARENB;
            newtio.c_cflag &= ~CSTOPB;
            //newtio.c_cflag |= INPCK;
            printf("[setPort]parity:S\n");
            break;
    }

    //set stop bits
    switch (stop_bits)
    {
        default:
        case 1:
            newtio.c_cflag &= ~CSTOPB;
	    printf("[setPort]stop_bits:1\n");
            break;
        case 2:
            newtio.c_cflag |= CSTOPB;
	   printf("[setPort]stop_bits:2\n");
            break;
    }
	
    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_oflag &= ~OPOST;                 // 杈撳嚭鏁版嵁妯″紡锛屽師濮嬫暟鎹?    Opt.c_oflag &= ~(ONLCR | OCRNL);          //娣诲姞鐨?
    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
   
    newtio.c_iflag &= ~(ICRNL | INLCR);
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);   // 涓嶄娇鐢ㄨ蒋浠舵祦鎺э紱
    newtio.c_cflag &= ~CRTSCTS;   //  涓嶄娇鐢ㄦ祦鎺у埗

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN]  = 1;
    tcflush(fd, TCIOFLUSH);
    if(tcsetattr(fd, TCSANOW, &newtio)){
        return -1;
    }
	
    //printf( "tcsetattr done, baud_rate:%d, c_flow:%d, data_bits:%d, parity:%c, stop_bit:%d\n", baud_rate, c_flow, data_bits, parity, stop_bits);
    return 0;
}

void* CUartProc::thrRecv(void* org)
{
	pThis->RecvData();
	return NULL;
}

void CUartProc::RecvData()
{
	int sizerecv;
	//setCurrentThreadHighPriority(99);
		struct timeval tmp;
	while(1)
	{

//				tmp.tv_sec = 0;
//				tmp.tv_usec = 15000;
//				select(0, NULL, NULL, NULL, &tmp);
//				cout<<"-------------------time"<<endl;
//
//		LOGLN("\tgetDataTime :"<<( OSA_getCurTimeInMsec() - t )<< "\tms");
		static int a = 0;
		OSA_mutexLock(&m_hndl);
		memset(m_recvdata, 0, sizeof(m_recvdata));
//		t = OSA_getCurTimeInMsec();

		sizerecv = crecv(comfd, m_recvdata,sizeof(m_recvdata));
		LOGLN("\tgetDataTime :"<<( OSA_getCurTimeInMsec() - t )<< "\tms");

		t = OSA_getCurTimeInMsec();
////		findValidData(m_recvdata, sizerecv);
//
		OSA_mutexUnlock(&m_hndl);
		a++;
	}
}

void CUartProc::findValidData(unsigned char *tmpRcvBuff, int sizeRcv)
{
	unsigned int uartdata_pos = 0;
	unsigned char frame_head[]={0xAA, 0x55};

	static struct data_buf
	{
		unsigned int len;
		unsigned int pos;
		unsigned char reading;
		unsigned char buf[1024];
	}swap_data = {0, 0, 0,{0}};

	if(sizeRcv>0)
	{
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
					Parsing();
					memset(&swap_data, 0, sizeof(struct data_buf));
				}
			}
		}
	}
	else
	{
		printf("sizeRcv:%d", sizeRcv);
	}
	return;
}

void CUartProc::Parsing()
{

	if(m_rcvBuf.size() < m_cmdlength)
		return ;
	static int tmpCnt = 1, count = 1;
	unsigned char checkSum = 0;
	for(int i=2;i<10;i++)
		checkSum += m_rcvBuf[i];



	if( ((checkSum>>8)&0xff) == m_rcvBuf[10] && ((checkSum&0xff) == m_rcvBuf[11]))
	{
		unsigned char temp[4] = {0}, temp2[4] = {0};
		float out1 = 0.0f, out2 = 0.f;
		int cnt = 0;
		temp[0] =m_rcvBuf[2];
		temp[1] =m_rcvBuf[3];
		temp[2] =m_rcvBuf[4];
		temp[3] =m_rcvBuf[5];
		temp2[0] = m_rcvBuf[6];
		temp2[1] = m_rcvBuf[7];
		temp2[2] = m_rcvBuf[8];
		temp2[3] = m_rcvBuf[9];
//		memcpy(&mx, temp, sizeof(temp));


		memcpy(&out1, temp, sizeof(temp));
		memcpy(&out2, temp2, sizeof(temp2));

		if(out1 >= -1.5 && out1 <=1.5)
		{
			mx = K * (180/PI) * out1;
	//		mz = K * (180/PI) * out2;
		}


		cnt = (int)out1;
		if(tmpCnt != cnt)
		{
//			LOGLN("mx:"<<mx<<"\tgetDataTime :"<<( (cv::getTickCount() - t) / cv::getTickFrequency() )<< " \trecvdata:"<<out1);
			LOGLN("mx:"<<mx<<"\tgetDataTime :"<<( OSA_getCurTimeInMsec() - t )<< " \trecvdata:"<<out1);
			//cout<<"mx:"<<mx<<"-------------/dev/ttyTHS2*********recvdata:"<<out1<<"**************count:  "<<count<<endl;
			count ++;
		}
		t = OSA_getCurTimeInMsec();
		tmpCnt = (cnt+1)%100;
	}
	m_rcvBuf.erase(m_rcvBuf.begin(),m_rcvBuf.begin()+m_cmdlength);
	return ;
}
