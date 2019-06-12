/*
 * buffer.h
 *
 *  Created on: Jan 17, 2017
 *      Author: wang
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "osa_buf.h"
#include "queue_display.h"

void destroy_buffer();
void init_buffer();
void get_buffer(unsigned char* ptr, int currentchanl);
void get_bufferyuv(unsigned char* ptr, int chId);
extern Alg_Obj * queue_dis;

#endif /* BUFFER_H_ */
