#pragma once

#include<Windows.h>
#include <iostream>



namespace Hook_Imgui
{

	void BindHook(void (*callback)());
	void DetachHook();
}