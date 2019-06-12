/*
 * Parayml.cpp
 *
 *  Created on: Dec 26, 2016
 *      Author: wang
 */

#include "Parayml.h"
#include <iostream>
using namespace std;

int MODLE_CIRCLE=79;
int PER_CIRCLE  =320;//triangles
int BLEND_OFFSET=1;//polygon nums
int LOOP_RIGHT=1;
int ISDYNAMIC=1;
float VALUE_THRESHOLD=5.0000;
int BLEND_HEAD=10;
int BLEND_REAR=10;
Parayml::Parayml()//:mode(1.5)
{
	bzero(&param,sizeof(param));
}

Parayml::~Parayml()
{
	if(readfs.isOpened())
		readfs.release();

	if(writefs.isOpened())
		writefs.release();
}

void Parayml::getParams()
{
	param.modleCircle = (int)readfs["modleCircle"];
	param.perCircle   = (int)readfs["perCircle"];
	param.blend_offset= (int)readfs["blend_offset"];
	param.loop_right  = (int)readfs["loop_right"];
	param.isdynamic   = (int)readfs["isdynamic"];
	param.value_threshold= (float)readfs["value_threshold"];
	param.blend_head  = (int)readfs["blend_head"];
	param.blend_rear  = (int)readfs["blend_rear"];
	inputs();
}

void Parayml::inputs()
{
	MODLE_CIRCLE = (param.modleCircle>=0) ? (param.modleCircle) : (MODLE_CIRCLE);
	PER_CIRCLE   = (param.perCircle >=0) ? (param.perCircle)   : (PER_CIRCLE);
	BLEND_OFFSET = (param.blend_offset>=0) ? (param.blend_offset) : (BLEND_OFFSET);
	LOOP_RIGHT   = (param.loop_right>=0) ? (param.loop_right)  : (LOOP_RIGHT);
	ISDYNAMIC    = (param.isdynamic ==0) ? (param.isdynamic)   : (ISDYNAMIC);
	VALUE_THRESHOLD= (param.value_threshold>0)?(param.value_threshold):(VALUE_THRESHOLD);
	BLEND_HEAD   = (param.blend_head>0 ) ? (param.blend_head)  : (BLEND_HEAD);
	BLEND_REAR   = (param.blend_rear>0 ) ? (param.blend_rear)  : (BLEND_REAR);
}

bool Parayml::readParams(const char* file)
{
	readfs.open(file,FileStorage::READ);

	if(readfs.isOpened())
	{
		getParams();
		return true;
	}
	return false;
}

void Parayml::outputs()
{
	writefs << "perCircle"      << param.perCircle;
	writefs << "blend_offset"   << param.blend_offset;
	writefs << "modleCircle"    << param.modleCircle;
	writefs << "loop_right"     << param.loop_right;
	writefs << "value_threshold"<< param.isdynamic;
	writefs << "value_threshold"<< param.value_threshold;
	writefs << "blend_head"     << param.blend_head;
	writefs << "blend_rear"     << param.blend_rear;
	writefs.release();
}

void Parayml::setParams()
{
	outputs();
}

bool Parayml::writeParams(const char* file)
{
	writefs.open(file,FileStorage::WRITE);

	if(writefs.isOpened())
	{
		setParams();
		return true;
	}
	return false;
}
