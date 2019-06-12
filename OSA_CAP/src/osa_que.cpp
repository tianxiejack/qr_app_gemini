

#include <osa_que.h>
#include"unistd.h"
#include"sys/time.h"
int OSA_queCreate(OSA_QueHndl *hndl, Uint32 maxLen)
{
  pthread_mutexattr_t mutex_attr;
  pthread_condattr_t cond_attr;
  int status=OSA_SOK;

  hndl->curRd = hndl->curWr = 0;
  hndl->count = 0;
  hndl->len   = maxLen;
  hndl->queue = (Int32 *)OSA_memAlloc(sizeof(Int32)*hndl->len);
  
  if(hndl->queue==NULL) {
    OSA_ERROR("OSA_queCreate() = %d \r\n", status);
    return OSA_EFAIL;
  }
 
  status |= pthread_mutexattr_init(&mutex_attr);
  status |= pthread_condattr_init(&cond_attr);  
  
  status |= pthread_mutex_init(&hndl->lock, &mutex_attr);
  status |= pthread_cond_init(&hndl->condRd, &cond_attr);    
  status |= pthread_cond_init(&hndl->condWr, &cond_attr);  

  if(status!=OSA_SOK)
    OSA_ERROR("OSA_queCreate() = %d \r\n", status);
    
  pthread_condattr_destroy(&cond_attr);
  pthread_mutexattr_destroy(&mutex_attr);
    
  return status;
}

int OSA_queDelete(OSA_QueHndl *hndl)
{
  if(hndl->queue!=NULL)
    OSA_memFree(hndl->queue);
    
  pthread_cond_destroy(&hndl->condRd);
  pthread_cond_destroy(&hndl->condWr);
  pthread_mutex_destroy(&hndl->lock);  
  
  return OSA_SOK;
}



int OSA_quePut(OSA_QueHndl *hndl, Int32 value, Uint32 timeout,OSA_QueHndl *hndl2)
{
  int status = OSA_EFAIL;

  pthread_mutex_lock(&hndl->lock);

  while(1) {
    if( hndl->count < hndl->len ) {
      hndl->queue[hndl->curWr] = value;
      hndl->curWr = (hndl->curWr+1)%hndl->len;
      hndl->count++;
  //    if(hndl==hndl2)
//	  printf("putfull count=%d,  %d   p_hndl=%x\n", hndl->count,pthread_self(),hndl);
      status = OSA_SOK;
      pthread_cond_signal(&hndl->condRd);
      break;
    } else {
      if(timeout == OSA_TIMEOUT_NONE)
        break;

      status = pthread_cond_wait(&hndl->condWr, &hndl->lock);
    }
  }

  pthread_mutex_unlock(&hndl->lock);

  return status;
}

int OSA_queGet(OSA_QueHndl *hndl, Int32 *value, Uint32 timeout,OSA_QueHndl *hndl2)
{
  int status = OSA_EFAIL;
  int counter = timeout/5;
  timeval start, end;
  if(timeout > 0){
           gettimeofday(&start, 0);
  }
  pthread_mutex_lock(&hndl->lock);
  
  while(1) {
    if(hndl->count > 0 ) {

      if(value!=NULL) {
        *value = hndl->queue[hndl->curRd];
      }
      
      hndl->curRd = (hndl->curRd+1)%hndl->len;
      hndl->count--;
  //    if(hndl==hndl2)
  //  	  printf("get full count=%d, %d      p_hndl=%x\n    ", hndl->count,pthread_self(),hndl);
         status = OSA_SOK;
      pthread_cond_signal(&hndl->condWr);
      break;
    } else {
      if(timeout == OSA_TIMEOUT_NONE)
        break;
      else if(timeout== OSA_TIMEOUT_FOREVER)
      {
    	  status = pthread_cond_wait(&hndl->condRd, &hndl->lock);
      }
      else
      {
    	  	  if(counter -- > 0){
    	  		  gettimeofday(&end ,0);
    	  		  int delta=(end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    	  		  if(delta < timeout*1000){
    	  			 pthread_mutex_unlock(&hndl->lock);
    	  			  	  usleep(5000);
    	  			  	 pthread_mutex_lock(&hndl->lock);
    	  			  	  continue;
    	  		  }
    	  		  else{//passed timeout ms
    	  		//	  printf("delta=%d      counter=%d\n",delta,counter);
    	  			status = OSA_ERROR_TIMEOUT;
    	  			  break;
    	  		  }
    	  	  }
    	  	  else{// passed timeout count
    	 // 		  printf("counter=%d\n",counter);
    	  		  status = OSA_ERROR_TIMEOUT;
    	  		  break;
    	  	  }
      }
    }
  }

  pthread_mutex_unlock(&hndl->lock);

  return status;
}


Uint32 OSA_queGetQueuedCount(OSA_QueHndl *hndl)
{
  Uint32 queuedCount = 0;

  pthread_mutex_lock(&hndl->lock);
  queuedCount = hndl->count;
  pthread_mutex_unlock(&hndl->lock);
  return queuedCount;
}

int OSA_quePeek(OSA_QueHndl *hndl, Int32 *value)
{
  int status = OSA_EFAIL;
  pthread_mutex_lock(&hndl->lock);
  if(hndl->count > 0 ) {
      if(value!=NULL) {
        *value = hndl->queue[hndl->curRd];
        status = OSA_SOK;
      }
  }
  pthread_mutex_unlock(&hndl->lock);

  return status;
}

Bool OSA_queIsEmpty(OSA_QueHndl *hndl)
{
  Bool isEmpty;

  pthread_mutex_lock(&hndl->lock);
  if (hndl->count == 0)
  {
      isEmpty = TRUE;
  }
  else
  {
      isEmpty = FALSE;
  }
  pthread_mutex_unlock(&hndl->lock);

  return isEmpty;
}



