/*
 * EnhStateMachine.h
 *
 *  Created on: 2018年12月17日
 *      Author: fsmdn121
 */

#ifndef ENHSTATEMACHINE_H_
#define ENHSTATEMACHINE_H_

#include "EnhBhv.h"
class GLEnv;

typedef enum ENHSTATE
{
	ENH_ON_STATE,
	ENH_ON2OFF_STATE,
	ENH_OFF_STATE,
	ENH_OFF2ON_STATE,
	ENH_STATE_COUNT
}ENHSTATE;


class IFEnhStateMachineGroup
{
public:
	virtual void SendData(int i,bool needSendData) = 0;
	virtual ENHSTATE GetState()=0;
	virtual	 void updateState(bool onoff) = 0;
};

class IFEnhStateMachine
{
public:
	virtual  void SendData(int i,bool needSendData)=0;
	 virtual ENHSTATE GetState()=0;
	virtual ENHSTATE nextState(bool onoff) = 0;
	virtual void clear()=0;
};
#if 0
class IFEnhStateGLRenderBridge
{
public:
	void UseShaderOnPano();
	void UseShaderOnOverLap();
};

class YubBridge:public EnhStateGLRenderBridge
{
public:
	YubBridge(IFEnhBhv *host):mphost(host){};
	void UseShaderOnPano();
	void UseShaderOnOverLap();
private:
	 IFEnhBhv * mphost;
};
class RgbBridge:public EnhStateGLRenderBridge
{
public:
	RgbBridge(IFEnhBhv *host):mphost(host){};
	void UseShaderOnPano();
	void UseShaderOnOverLap();
private:
	 IFEnhBhv * mphost;
};
#endif
class BaseTransitionState:public IFEnhStateMachine
{
public:
	virtual void SendData(int i,bool needSendData);
	static const int TRANS_COUNT=0;
};

class EnhOnState:public IFEnhStateMachine
{
public:
	EnhOnState(IFEnhBhv * phost);
	 void SendData(int i,bool needSendData);
	 void clear(){};
	 ENHSTATE nextState(bool ison){	return ison?  ENH_ON_STATE:ENH_ON2OFF_STATE;};
	 ENHSTATE GetState(){return ENH_ON_STATE;};
private:
	 IFEnhBhv * mphost;
};

class EnhOn2OffState:public BaseTransitionState
{
public:
	EnhOn2OffState(IFEnhBhv * phost);
	 void SendData(int i,bool needSendData){	BaseTransitionState::SendData(i,false);
		mphost->SendData(i,false);
	 };
	 void clear(){counter=0;};
	 ENHSTATE nextState(bool ison)
	 {
		 return (counter++>=TRANS_COUNT)?  ENH_OFF_STATE:ENH_ON2OFF_STATE;
	 };
	 ENHSTATE GetState(){return ENH_ON2OFF_STATE;};
private:
	 IFEnhBhv * mphost;
	 int counter;
};

class EnhOffState:public IFEnhStateMachine
{
public:
	EnhOffState(IFEnhBhv * phost);
	 void SendData(int i,bool needSendData);
	 void clear(){};
	 ENHSTATE nextState(bool ison)
	 {	return ison?  ENH_OFF2ON_STATE:ENH_OFF_STATE;};
	 ENHSTATE GetState(){return ENH_OFF_STATE;};
private:
	 IFEnhBhv * mphost;
};

class EnhOff2OnState:public BaseTransitionState
{
public:
	EnhOff2OnState(IFEnhBhv * phost);
	 void SendData(int i,bool needSendData);
	 void clear(){counter=0;};
	 ENHSTATE nextState(bool ison)
	 {
		 return (counter++>=TRANS_COUNT)?  ENH_ON_STATE:ENH_OFF2ON_STATE;
	 };
	 ENHSTATE GetState(){return ENH_OFF2ON_STATE;};
private:
	 IFEnhBhv * mphost;
	 int counter;
};
class EnhStateMachine:public IFEnhStateMachineGroup
{
public:
	EnhStateMachine(IFEnhBhv * phost);
	~EnhStateMachine();
	 ENHSTATE GetState(){return pActiveState->GetState();};
	 void SendData(int i,bool needSendData);
	 void updateState(bool onoff) {
		 ENHSTATE next = pActiveState->nextState(onoff);
		 SetActiveState(next);
	 };
private:
	 void SetActiveState(ENHSTATE s){
		 static ENHSTATE lastState=ENH_OFF_STATE;
		 pActiveState =pMyStates[s];
		 if(s!=lastState)
		 {
			 lastState=s;
			 pActiveState->clear();
		 }};
	 IFEnhBhv * mphost;
	 IFEnhStateMachine * pActiveState;
	 ENHSTATE mEnhState;
	 IFEnhStateMachine* pMyStates[ENH_STATE_COUNT];
};



#endif /* ENHSTATEMACHINE_H_ */
