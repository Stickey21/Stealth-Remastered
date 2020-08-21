#pragma once

namespace Memory
{
	int memcpy_safe(void* dest, const void* src, uint32_t len, int check = NULL, const void* checkdata = NULL);
	int memset_safe(void* _dest, int c, uint32_t len);
	int memcmp_safe(const void* _s1, const void* _s2, uint32_t len);
	void* memdup(const void* src, int len);

	void HookInstallCall(DWORD dwInstallAddress, DWORD dwHookFunction);
	void CreateJump(BYTE* pAddress, DWORD dwJumpTo, DWORD dwLen);

	bool isBadPtr_handlerAny(void* pointer, ULONG size, DWORD dwFlags);
	bool isBadPtr_readAny(void* pointer, ULONG size);
	bool isBadPtr_writeAny(void* pointer, ULONG size);

	int __page_size_get(void);
	int __page_write(void* _dest, const void* _src, uint32_t len);
	int __page_read(void* _dest, const void* _src, uint32_t len);
};