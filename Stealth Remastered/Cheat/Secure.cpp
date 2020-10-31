#include "main.h"

CSecure* pSecure = new CSecure();

void CSecure::Add(DWORD dwAddress, BYTE byteSize)
{
	for (auto i = 0; i < byteSize; i++)
	{
		if (isAddressSecured(dwAddress + i) == -1)
		{
			g_Memory.dwAddress = dwAddress + i;
			g_Memory.origByte = *(BYTE*)(dwAddress + i);
			vecMemory.push_back(g_Memory);
		}
	}
}

int CSecure::isAddressSecured(DWORD dwAddress)
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
	Add(*(DWORD*)(ppPointer), 7);
	DetourAttach(ppPointer, pDetour);
}

void CSecure::memcpy_safe(void* _dest, const void* _src, uint32_t len)
{
	Add((DWORD)_dest, len);
	Memory::memcpy_safe(_dest, _src, len);
}

void CSecure::HookInstallCall(DWORD dwInstallAddress, DWORD dwHookFunction)
{
	Add(dwInstallAddress, 5);
	Memory::HookInstallCall(dwInstallAddress, dwHookFunction);
}

void CSecure::CreateJump(BYTE* pAddress, DWORD dwJumpTo, DWORD dwLen)
{
	Add((DWORD)pAddress, (BYTE)dwLen);
	Memory::CreateJump(pAddress, dwJumpTo, dwLen);
}