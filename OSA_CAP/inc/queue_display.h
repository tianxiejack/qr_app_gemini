#ifndef _QUEUE_DISPLAY_H_
#define _QUEUE_DISPLAY_H_

#include <osa.h>
#include <osa_que.h>
#include <osa_tsk.h>
#include <osa_debug.h>
#include <osa_buf.h>
#include <osa_sem.h>

#if !defined LINK_CHANL_MAX
#define LINK_CHANL_MAX 17  //12
#endif
typedef struct Alg_Obj_st
{
    OSA_TskHndl     tsk;
    
    OSA_BufCreate bufCreate[LINK_CHANL_MAX];
    OSA_BufHndl   bufHndl[LINK_CHANL_MAX];
    OSA_MutexHndl sendmuxLock[LINK_CHANL_MAX];
    OSA_SemHndl sendtskOsdSem[LINK_CHANL_MAX];
	
    OSA_BufCreate recvbufCreate[LINK_CHANL_MAX];
    OSA_BufHndl   recvbufHndl[LINK_CHANL_MAX];
    OSA_MutexHndl recvmuxLock[LINK_CHANL_MAX];
    OSA_SemHndl recvtskOsdSem[LINK_CHANL_MAX];
    OSA_ThrHndl recvtskOsdHndl[LINK_CHANL_MAX];
    

	OSA_ThrHndl sendtskOsdHndl[LINK_CHANL_MAX];

    OSA_BufCreate Algfrom;
    OSA_BufHndl Algfromhandle;

    OSA_BufCreate Algto;
    OSA_BufHndl Algtohandle;

    OSA_ThrHndl tskOsdHndl[LINK_CHANL_MAX];

    void *Alg_commulication_share;

    Int32  channalnum;
}
Alg_Obj;

typedef struct 
{
  Alg_Obj *algLink_handle;
  int numchannle;	
}AlgLink_Obj_Multi;

extern AlgLink_Obj_Multi alglink_obj_mul[LINK_CHANL_MAX];
void * alg_buf_init(void);
int alg_obj_init(void *handle);
void alg_buf_destroy(void *queue_dis);

#endif
