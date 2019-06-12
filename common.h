/*
 * common.h
 *
 *  Created on: Dec 15, 2016
 *      Author: wang
 */

#ifndef COMMON_H_
#define COMMON_H_
#include "StlGlDefines.h"
#include <string.h>

class Common{
public:
	Common();
	~Common();
	inline void setWindowName(char* name)	{ strcpy(window_name,name);	};
	inline void setViewFlag(int flag)		{ ViewFlag   = flag;		};
	inline void setIdleDraw(int flag)		{ idle_draw  = flag;		};
	inline void setVerbose(int flag)		{ verbose    = flag;		};
	inline void setFrameRate(float rate)	{ FrameRate  = rate;		};
	inline void setFrameCount(int count)	{ FrameCount = count;		};
	inline void setUpdate(int flag)			{ update     = flag;		};
	inline bool isUpdate()					{return update;				};
	inline bool isCountUpdate()				{return FrameCount==0;		};
	inline float getFrameRate()				{return FrameRate;			};
	inline int  plusAndGetFrameCount()		{return ++FrameCount;		};
	inline int  getFrameCount()				{return FrameCount;			};
	inline bool isPerspective()				{return (ViewFlag==PERSPECTIVE);};
	inline bool isOrtho()					{return (ViewFlag==ORTHO);	};
	inline bool isVerbose()					{return (verbose==GL_YES);	};
	inline bool isIdleDraw()				{return (idle_draw==GL_YES);};
	inline char* getWindowName()			{return window_name;		};
	inline bool isStateStitch()				{return stateStitch;		};
	inline bool isStateRecover()			{return !stateStitch;		};
	inline void setStitchState(bool value)	{ stateStitch=value;		};
	inline void setStateChannel(int index)	{ stateChId[index]=true;	};
	inline bool getStateChannel(int index)	{return stateChId[index];	};
	inline bool Scaned()                    {return stateScanned;       };
	inline void setScanned(bool state)      {stateScanned = state;      };
private:
	char window_name[128];
	int verbose;
	int idle_draw;
	int ViewFlag;
	float FrameRate;
	int FrameCount;
	int update;
	bool stateScanned;
	bool stateStitch;
	bool stateChId[CAM_COUNT];
};


#endif /* COMMON_H_ */
