/*
 * EnhBhv.h
 *
 *  Created on: 2018年12月17日
 *      Author: fsmdn121
 */

#ifndef ENHBHV_H_
#define ENHBHV_H_
class GLEnv;
class GLShaderManager;
class IFEnhBhv
{
public:
	virtual void SendData(int i,bool needSendData)=0;
//	virtual void UseYUVShaderOnPano(GLT_STOCK_SHADER nShaderID,)=0;
//	virtual void UseYUVShaderOnOverLap(GLT_STOCK_SHADER nShaderID,)=0;
//	virtual void UseRGBShaderOnPano(GLT_STOCK_SHADER nShaderID,)=0;
//	virtual void UseRGBShaderOnOverLap(GLT_STOCK_SHADER nShaderID,)=0;
};






#endif /* ENHBHV_H_ */
