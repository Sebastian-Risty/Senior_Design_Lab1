#pragma once
#include "common.h"

namespace global {
	class globals {
	public:
		HANDLE hSerial = INVALID_HANDLE_VALUE;

		vector<float> tempData;
		float finalTempData[300] = { 0 };
		int tempIndex = 50;

		bool enableLED = true;
		bool faren = false;
	};

	inline globals g_globals;
}