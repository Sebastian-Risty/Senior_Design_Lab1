#pragma once
#include "common.h"

namespace global {
	class globals {
	public:
		HANDLE hSerial = INVALID_HANDLE_VALUE;

		vector<float> tempData;
		int tempIndex = -1;

		bool enableLED = true;
		bool faren = true;
	};

	inline globals g_globals;
}