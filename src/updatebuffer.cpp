/*
 * updatebuffer.cpp
 *
 *  Created on: Jan 18, 2017
 *      Author: wang
 */

#include "updatebuffer.h"
#include "buffer.h"
#include "thread.h"
#include <iostream>
#include <unistd.h>
using namespace std;
#define MEMCPY memcpy
update_buffer::update_buffer(int Id):devId(Id),buffer(NULL)
{
	buffer = (unsigned char*)malloc(DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*DEFAULT_IMAGE_DEPTH);
}

update_buffer::~update_buffer()
{
	free(buffer);
}

void update_buffer::mainloop()
{
	get_buffer(buffer,devId);
}


//--------------the decorator cap class------------
AsyncShow::AsyncShow(auto_ptr<BaseVCap> coreCap):
	m_core(coreCap),thread_state(THREAD_READY),sleepMs(DEFAULT_THREAD_SLEEP_MS)
{
	initLock();
	start_thread(capThread,this);
	cout<<" start a capture thread"<<endl;
}
AsyncShow:: ~AsyncShow()
{
	Close();
}
bool AsyncShow::Open()
{
	if(thread_state != THREAD_READY)
		return false;//do not support reopen.
	bool isOpened = m_core->Open();
	if(isOpened){
		Start();
	}
	return isOpened;
}

void AsyncShow::Close()
{
	thread_state = THREAD_STOPPING;
	while( THREAD_IDLE != thread_state){
		usleep(100*1000);
	}
	m_core->Close();
}
void AsyncShow::Capture(char* ptr)
{
	lock_read(ptr);
}
void AsyncShow::Capture(char* ptr0, char* ptr1, char* ptr2, char* ptr3)
{
	//todo read 4 images into the 4 ptrs;
}

void AsyncShow::SetDefaultImg(char *p)
{
	m_core->SetDefaultImg(p);
}

 void AsyncShow::SavePic(const char* name)
 {
	m_core->SavePic(name);
 }

void AsyncShow::Run()
{
	char *p = new char[m_core->GetTotalBytes()*4];
	do{
		usleep(100*1000);
	}while(THREAD_READY == thread_state);
	//cap in background thread
	while(thread_state == THREAD_RUNNING)
	{
		update_buffer * pcore = dynamic_cast<update_buffer*>(m_core.get());
		if(pcore){
			pcore->mainloop();
		}
//		usleep(40*1000);
	}
	thread_state = THREAD_IDLE;
	destroyLock();
	delete []p;
}

void* AsyncShow::capThread(void*p)
{
	AsyncShow *thread = (AsyncShow*)p;
	thread->Run();
	return NULL;
}

void AsyncShow::lock_read(char *ptr)
{
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_rdlock(&rwlock);
	MEMCPY(ptr, pImg,m_core->GetTotalBytes());
	pthread_rwlock_unlock(&rwlock);
}

void AsyncShow::lock_write(char *ptr)
{
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_wrlock(&rwlock);
	MEMCPY(pImg,ptr,m_core->GetTotalBytes());
	pthread_rwlock_unlock(&rwlock);
}
void  AsyncShow::Start()
{
	thread_state = THREAD_RUNNING;
}
void AsyncShow::destroyLock()
{
	pthread_rwlock_destroy(&rwlock);
	pthread_rwlockattr_destroy(&rwlockattr);
}

void AsyncShow::initLock()
{
	pthread_rwlockattr_init(&rwlockattr);
	pthread_rwlockattr_setpshared(&rwlockattr,2);
	pthread_rwlock_init(&rwlock,&rwlockattr);
}
