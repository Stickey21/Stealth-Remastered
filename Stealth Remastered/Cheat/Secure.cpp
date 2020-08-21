#include "main.h"

CSecure* pSecure = new CSecure();

int CSecure::isAddressPresent(DWORD dwAddress)
{
	for (size_t i = 0; i < vecMemory.size(); i++)
	{
		if (vecMemory[i].dwAddress == dwAddress)
			return i;
	}
	return -1;
}

void CSecure::SDetourAttach(PVOID* ppPointer, PVOID pDetour)
{
	for (size_t i = 0; i < 7; i++)
	{
		if (isAddressPresent(*(DWORD*)(ppPointer) + i) == -1)
		{
			g_Memory.dwAddress = *(DWORD*)(ppPointer) + i;
			g_Memory.origByte = *(uint8_t*)(*(DWORD*)(ppPointer) + i);
			vecMemory.push_back(g_Memory);
		}
	}

	DetourAttach(ppPointer, pDetour);
}

void CSecure::memcpy_safe(void* _dest, const void* _src, uint32_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		if (isAddressPresent((DWORD)(_dest) + i) == -1)
		{
			g_Memory.dwAddress = (DWORD)(_dest) + i;
			g_Memory.origByte = *(uint8_t*)((DWORD)(_dest) + i);
			vecMemory.push_back(g_Memory);
		}
	}

	Memory::memcpy_safe(_dest, _src, len);
}

void CSecure::HookInstallCall(DWORD dwInstallAddress, DWORD dwHookFunction)
{
	for (size_t i = 0; i < 5; i++)
	{
		if (isAddressPresent(dwInstallAddress + i) == -1)
		{
			g_Memory.dwAddress = dwInstallAddress + i;
			g_Memory.origByte = *(uint8_t*)(dwInstallAddress + i);
			vecMemory.push_back(g_Memory);
		}
	}

	Memory::HookInstallCall(dwInstallAddress, dwHookFunction);
}

void CSecure::CreateJump(BYTE* pAddress, DWORD dwJumpTo, DWORD dwLen)
{
	for (size_t i = 0; i < dwLen; i++)
	{
		if (isAddressPresent((DWORD)(pAddress) + i) == -1)
		{
			g_Memory.dwAddress = (DWORD)(pAddress) + i;
			g_Memory.origByte = *(uint8_t*)((DWORD)(pAddress) + i);
			vecMemory.push_back(g_Memory);
		}
	}

	Memory::CreateJump(pAddress, dwJumpTo, dwLen);
}