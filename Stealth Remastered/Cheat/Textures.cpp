#include "main.h"

CTextures* pTextures;

CTextures::CTextures(IDirect3DDevice9* pDevice)
{
	D3DXCreateTextureFromFileInMemoryEx(pDevice, &byteLogo, sizeof(byteLogo), 225, 69, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tLogo);

    for (auto i = 0; i < 144; i++) 
    {
        CStreaming::RequestModel(gRadarTextures[i] + 20000, 10);
        CStreaming::LoadAllRequestedModels(false);
        tMap.push_back(GetFirstTexture(CTxdStore::ms_pTxdPool->m_pObjects[gRadarTextures[i]].m_pRwDictionary));
    }
}

CTextures::~CTextures()
{
	tLogo->Release();
    tMap.clear();
}