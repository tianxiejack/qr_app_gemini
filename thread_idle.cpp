#include "thread_idle.h"
using namespace std;
thread_idle tIdle;
thread_idle::thread_idle()
{
Idle[FPGA_FOUR_CN]=false;
Idle[SUB_CN]=true;
Idle[MAIN_CN]=false;
Idle[MVDECT_CN]=false;
Idle[FPGA_SIX_CN]=false;
Idle[MVDECT_ADD_CN]=false;
}
bool thread_idle::isToIdle(int idx){
	return Idle[idx];}
		void thread_idle::threadIdle(int idx){
			Idle[idx]=true;
			};
		void thread_idle::threadRun(int idx){
			Idle[idx]=false;
			};
