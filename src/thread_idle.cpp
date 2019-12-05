#include "thread_idle.h"
using namespace std;
thread_idle tIdle;
thread_idle::thread_idle()
{
Idle[0]=false;
Idle[1]=false;
Idle[2]=false;
Idle[3]=false;
Idle[4]=false;
Idle[5]=false;
}
bool thread_idle::isToIdle(int idx){
	return Idle[idx];}
		void thread_idle::threadIdle(int idx){
			Idle[idx]=true;
			};
		void thread_idle::threadRun(int idx){
			Idle[idx]=false;
			};
