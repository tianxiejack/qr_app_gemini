#ifndef THREAD_IDLE_H_
#define THREAD_IDLE_H_
#include "StlGlDefines.h"
class thread_idle
{
public:
	thread_idle();
	   bool isToIdle(int idx);
		void threadIdle(int idx);
		void threadRun(int idx);
private:
			bool Idle[MAX_CC];
};
#endif
