#pragma once
#include "GobalVariables.h"
namespace rc17t 
{
	enum whereiscar
	{
		carAtRight,
		carAtLeft
	};
	
	void timeorder();
	whereiscar carAt();
	
	int checkPillarOrder(int order[], bool pillarState[7]);
}