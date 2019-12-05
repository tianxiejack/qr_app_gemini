/*
 * scanner.h
 *
 *  Created on: Dec 28, 2016
 *      Author: wang
 */

#ifndef SCANNER_H_
#define SCANNER_H_

enum{
       NO_STITCH=0,
       STITCH_ONLY,
       SAVEPIC_STITCH
};

void* thread_scanner(void*);
void* thread_overlap(void*);
bool iniGenerate(bool needSavePic);
void* thread_stitch(void *);
int GetThreadStitchState();
void SetThreadStitchState(int state);


#endif /* SCANNER_H_ */
