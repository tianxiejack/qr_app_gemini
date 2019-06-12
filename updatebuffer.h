/*
 * updatebuffer.h
 *
 *  Created on: Jan 18, 2017
 *      Author: wang
 */

#ifndef UPDATEBUFFER_H_
#define UPDATEBUFFER_H_

#include "Camera.h"

class update_buffer:public BaseVCap{
public:
	update_buffer(int Id);
	virtual ~update_buffer();
	void mainloop();
	void getPtr(char** img){*img=(char*)buffer;};
	inline bool Open() {return true;};
	inline void Close() {};
	inline void Capture(char* ptr) {};
private:
	int devId;
	unsigned char* buffer;
};

//-------------decorator implementations-------------------------
class AsyncShow:public Interface_VCap{
	public:
		AsyncShow(auto_ptr<BaseVCap> coreCap);
		virtual ~AsyncShow();
		virtual bool Open();
		virtual void Close();
		virtual void Capture(char* ptr);
		virtual void Capture(char* ptr0, char* ptr1, char* ptr2, char* ptr3);
		virtual void SetDefaultImg( char *);
		virtual void SavePic(const char* name);
		auto_ptr<BaseVCap> m_core;
	private:
		enum{
			THREAD_READY,
			THREAD_RUNNING,
			THREAD_STOPPING,
			THREAD_IDLE
		} thread_state;
		AsyncShow(){};
		void destroyLock();
		void initLock();
		void lock_read(char *ptr);
		void lock_write(char *ptr);
		static void* capThread(void*);
		void Run();
		void Start();
		pthread_t tid;
		pthread_rwlock_t rwlock;
        pthread_rwlockattr_t rwlockattr;
		unsigned int sleepMs;
};
#endif /* UPDATEBUFFER_H_ */
