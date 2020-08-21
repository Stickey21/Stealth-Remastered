#include "main.h"

CTextures* pTextures;

CTextures::CTextures(IDirect3DDevice9* pDevice)
{
	D3DXCreateTextureFromFileInMemoryEx(pDevice, &byteLogo, sizeof(byteLogo), 225, 69, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tLogo);
}

CTextures::~CTextures()
{
	tLogo->Release();
}