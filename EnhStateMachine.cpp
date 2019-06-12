/*
 * EnhStateMachine.cpp
 *
 *  Created on: 2018年12月17日
 *      Author: fsmdn121
 */
#include "EnhStateMachine.h"
#include "GLEnv.h"
extern GLEnv env1;
extern bool capturePanoCam(GLubyte *ptr, int index, GLEnv &env);
	EnhOnState::EnhOnState(IFEnhBhv * phost):
	mphost(phost)
	{

	}

void EnhOnState::SendData(int i,bool needSendData)
{
	mphost->SendData(i,needSendData);
//	m_env.Getp_PBOMgr()->sendData()
//	mphost->Getp_PBOMgr()->s
}

EnhOn2OffState::EnhOn2OffState(IFEnhBhv * phost):
		mphost(phost)
{

}
void BaseTransitionState::SendData(int i,bool needSendData)
{
	static unsigned char *ptr=NULL;
	if(NULL==ptr)
	{
		ptr=( unsigned char *)malloc(1920*1080*4);
	}
	capturePanoCam(ptr, i,env1);
}

EnhOffState::EnhOffState(IFEnhBhv * phost):
			mphost(phost)
{

}
void EnhOffState::SendData(int i,bool needSendData)
{
	mphost->SendData(i,needSendData);
}

EnhOff2OnState::EnhOff2OnState(IFEnhBhv * phost):
			mphost(phost)
{

}
void EnhOff2OnState::SendData(int i,bool needSendData)
{
	BaseTransitionState::SendData(i,false);
	mphost->SendData(i,false);
}

	EnhStateMachine::EnhStateMachine(IFEnhBhv * phost):
			mphost(phost)
	{
		 pMyStates[ENH_ON_STATE]=new EnhOnState(mphost);
		 pMyStates[ENH_ON2OFF_STATE]=new EnhOn2OffState(mphost);
		 pMyStates[ENH_OFF_STATE]=new EnhOffState(mphost);
		 pMyStates[ENH_OFF2ON_STATE]=new EnhOff2OnState(mphost);
		 pActiveState=pMyStates[ENH_OFF_STATE];
	}

	EnhStateMachine::~EnhStateMachine()
	{
		for(int i=0;i<ENH_STATE_COUNT;i++)
		{
			delete pMyStates[i];
		}
	}

	void EnhStateMachine::SendData(int i,bool needSendData)
	{
		pActiveState->SendData(i,needSendData);
	}
