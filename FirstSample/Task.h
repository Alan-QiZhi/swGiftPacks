#pragma once
#include "GobalVariables.h"

namespace rc17
{
	void Correct();
	// 扫描键盘输入的函数 （新加的线程）
	void scanfKey();
	void keyCmd();
	void sendToChild();
}
