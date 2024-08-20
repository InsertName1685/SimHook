#include "Hook.h"

SimHook::SimHook(BYTE* o_func, BYTE* h_func, BYTE* t_func, UINT length) {
	original_func = o_func;
	hook_func = h_func;
	tramp_func = t_func;
	this->length = length;
}

void Detour(BYTE* address, BYTE* func, int length)
{
	DWORD oldprotection;
	VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldprotection);
	DWORD relativeAddress = (func - address) - 5;
	*address = 0xE9;
	*(DWORD*)(address + 1) = relativeAddress;
	VirtualProtect(address, length, oldprotection, &oldprotection);
}

BYTE* Trampoline_Hook(BYTE* address, BYTE* func, int length)
{
	BYTE* gatewayptr = (BYTE*)VirtualAlloc(0, length, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy_s(gatewayptr, length, address, length);

	DWORD relativeAddress = (address - gatewayptr - 5);
	*(gatewayptr + length) = 0xE9;
	*(DWORD*)(gatewayptr + length + 1) = relativeAddress;
	Detour(address, func, length);
	return gatewayptr;
}

void SimHook::Enable()
{
	memcpy(stolenBytes,original_func,length);
	*(uintptr_t*)tramp_func = (uintptr_t)Trampoline_Hook(original_func, hook_func,length);

}
void SimHook::Disable()
{
	DWORD oldprotect;
	VirtualProtect(original_func, length, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(original_func, stolenBytes, length);
	VirtualProtect(original_func, length, oldprotect, &oldprotect);
}