#include "Hook.h"

SimHook::SimHook(BYTE* o_func, BYTE* h_func, BYTE* t_func, UINT length) {
	original_func = o_func;
	hook_func = h_func;
	tramp_func = t_func;
	this->length = length;
}
BYTE jmp_opcode[6] = {0xFF, 0x25,0x00,0x00,0x00,0x00};

void Detour(BYTE* address, BYTE* func, int length)
{
	DWORD oldprotection;
	VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldprotection);
	DWORD64 absoluteAddress = (DWORD64)func;
	memcpy(address, jmp_opcode, 6);
	memcpy(address + 6, &absoluteAddress, 8);

	VirtualProtect(address, length, oldprotection, &oldprotection);
}

BYTE* Trampoline_Hook(BYTE* address, BYTE* func, int length) // MinLength = 14
{
	BYTE* gatewayptr = (BYTE*)VirtualAlloc(0, length, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	DWORD64 absoluteAddress = (DWORD64)address + length;

	memcpy_s(gatewayptr, length, address, length); // Stolen Bytes
	memcpy((gatewayptr + length), jmp_opcode, 6); // Set Jmp
	memcpy((gatewayptr + length + 6), &absoluteAddress, 8); // Set Address

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