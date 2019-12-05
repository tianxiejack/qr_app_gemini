
#include <sched.h>
#include<string.h>
#include<pthread.h>
#include<stdio.h>
#include<assert.h>
#include"Thread_Priority.h"

void setCurrentThreadHighPriority(int level) {
if(level<1||level>99)
{
	printf("setCurrentThreadHighPriority is out of range!\n");
	assert(false);
}
  // Start out with a standard, low-priority setup for the sched params.
  struct sched_param sp;
  bzero((void*)&sp, sizeof(sp));
  int policy = SCHED_OTHER;

  // If desired, set up high-priority sched params structure.
  if (1) {
    // FIFO scheduler, ranked above default SCHED_OTHER queue
    policy = SCHED_FIFO;
    // The priority only compares us to other SCHED_FIFO threads, so we
    // just pick a random priority halfway between min & max.

    sp.sched_priority = level;
  }

  // Actually set the sched params for the current thread.
  if (0 == pthread_setschedparam(pthread_self(), policy, &sp)) {
    printf("IO Thread #%d using high-priority scheduler!", pthread_self());
  }
  else
  {
	  perror("fsb");
  }
}
