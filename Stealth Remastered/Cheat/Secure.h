#pragma once

class CSecure
{
public:
	void Add(DWORD dwAddress, BYTE byteSize);
	int isAddressSecured(DWORD dwAddress);
	void SDetourAttach(PVOID* ppPointer, PVOID pDetour);
	void memcpy_safe(void* _dest, const void* _src, uint32_t len);
	void HookInstallCall(DWORD dwInstallAddress, DWORD dwHookFunction);
	void CreateJump(BYTE* pAddress, DWORD dwJumpTo, DWORD dwLen);

	template<class T>
	void Write(DWORD dwAddress, T Value)
	{
		Add(dwAddress, sizeof(Value));
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