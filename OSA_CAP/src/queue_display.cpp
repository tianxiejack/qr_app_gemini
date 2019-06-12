#include "queue_display.h"
//#include "display.h"

#define NUMCHANAL (4)

AlgLink_Obj_Multi alglink_obj_mul[LINK_CHANL_MAX];
void * alg_buf_init(void)
{
	int i=0, j=0;
	Alg_Obj *alg_handle=(Alg_Obj *)malloc(sizeof(Alg_Obj));
	if(alg_handle==NULL)
	{
		return NULL;
	}

	alg_handle->channalnum = LINK_CHANL_MAX;
	/*   video resource */
	for(i=0; i<alg_handle->channalnum; i++)
	{
		alg_handle->bufCreate[i].numBuf=NUMCHANAL;
		for(j=0;j<alg_handle->bufCreate[i].numBuf;j++)
		{
			alg_handle->bufCreate[i].bufVirtAddr[j]=OSA_memAlloc(OSA_BUFFER_WIDTH*OSA_BUFFER_HEIGHT*OSA_BUFFER_CC);
			OSA_assert(alg_handle->bufCreate[i].bufVirtAddr[j]!=NULL);
		}
		OSA_bufCreate(&alg_handle->bufHndl[i],&alg_handle->bufCreate[i]);
	}
	return alg_handle;
}
void alg_buf_destroy(void *queue_dis)
{
	Alg_Obj *alg_handle = (Alg_Obj *)queue_dis;
	void* p=NULL;
	int i,j;
	/*   video resource */
	for(i=0; i<alg_handle->channalnum; i++)
	{
		alg_handle->bufCreate[i].numBuf=NUMCHANAL;
		for(j=0;j<alg_handle->bufCreate[i].numBuf;j++)
		{
			p = alg_handle->bufCreate[i].bufVirtAddr[j];
			OSA_memFree(p);

					OSA_memFree(p);
		}
		OSA_bufDelete(&alg_handle->bufHndl[i]);
	}
	OSA_memFree(alg_handle);
}
int alg_obj_init(void *handle)
{
	int i=0;
	Alg_Obj *pObj=(Alg_Obj *)handle;
	if(pObj==NULL)
	{
		return -1;
	}
	for(i=0;i<pObj->channalnum;i++)
	{
		alglink_obj_mul[i].algLink_handle=pObj;
		alglink_obj_mul[i].numchannle=i;
	}
	return 0;
}
