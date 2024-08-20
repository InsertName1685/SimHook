#pragma once

#include <Windows.h>
#include<iostream>

BYTE* Trampoline_Hook(BYTE* address, BYTE* func, int length);
void Detour(BYTE* address, BYTE* func, int length);

class SimHook
{
public:
	BYTE* original_func = 0;
	BYTE* hook_func = 0;
	BYTE* tramp_func = 0;

	UINT length = 0;

	BYTE stolenBytes[10]{ 0 };


	SimHook(BYTE* o_func, BYTE* h_func, BYTE* t_func, UINT length);
	
	void Enable();
	void Disable();
	

};



