#pragma once
#include "globals.h"


int GUI();

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();

// Populate finalTempData using tempData
void setupTempData();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);