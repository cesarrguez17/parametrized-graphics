#pragma once
#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER


#include <string>

class CTexture
{
protected:
	ID3D11Texture2D			 *m_pTexture;
	ID3D11ShaderResourceView *m_pShaderResourceView;
	std::wstring					  m_name;
	DWORD					  m_stage;


	void ReadDDSTexture(ID3D11Texture2D **pTexture);
public:
	CTexture(const TCHAR *FileName,DWORD stage);
	~CTexture(void);
	ID3D11Texture2D			 *GetTexture();
	ID3D11ShaderResourceView *GetShaderView();
};


#endif