#pragma once

class CSecure
{
public:
	int isAddressPresent(DWORD dwAddress);
	void SDetourAttach(PVOID* ppPointer, PVOID pDetour);
	void memcpy_safe(void* _dest, const void* _src, uint32_t len);
	void HookInstallCall(DWORD dwInstallAddress, DWORD dwHookFunction);
	void CreateJump(BYTE* pAddress, DWORD dwJumpTo, DWORD dwLen);

	template<class T>
	void Write(DWORD dwAddress, T Value)
	{
		for (size_t i = 0; i < sizeof(Value); i++)
		{
			if (isAddressPresent(dwAddress + i) == -1)
			{
				g_Memory.dwAddress = dwAddress + i;
				g_Memory.origByte = *(uint8_t*)(dwAddress + i);
				vecMemory.push_back(g_Memory);
			}
		}

		*(T*)dwAddress = (T)Value;
	}

	struct stMemory
	{
		DWORD dwAddress;
		BYTE origByte;
	} g_Memory;

	std::vector<stMemory> vecMemory;
};

extern CSecure* pSecure;