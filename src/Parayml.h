/*
 * Parayml.h
 *
 *  Created on: Dec 26, 2016
 *      Author: wang
 */

#ifndef PARAYML_H_
#define PARAYML_H_

#include <opencv/cv.hpp>
using namespace cv;

extern "C"

typedef struct _Param{
	int modleCircle ;
	int perCircle   ;
	int blend_offset;
	int loop_right;
    int isdynamic;
	float value_threshold;
	int blend_head;
	int blend_rear;
}Params;

class Parayml {
public:
	Parayml();
	virtual ~Parayml();
	bool readParams(const char* file);
	bool writeParams(const char* file);
private:
	void inputs();
	void getParams();
	void outputs();
	void setParams();

	Params param;
	FileStorage readfs;
	FileStorage writefs;
};

#endif /* PARAYML_H_ */
