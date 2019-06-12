#include "common.h"

Common::Common():idle_draw(1),verbose(false),FrameRate(30.0),
ViewFlag(PERSPECTIVE),FrameCount(0),update(0),stateStitch(false),stateScanned(false)
{
	memset(window_name,0,sizeof(window_name));
	bzero(stateChId,sizeof(stateChId));
}

Common::~Common()
{
}
