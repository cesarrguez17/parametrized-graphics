#include "StdAfx.h"
#include "Texture.h"
#include "GraphicsLayer.h"



CTexture::CTexture(const TCHAR *FileName,DWORD stage)
{
	m_pShaderResourceView = NULL;
	//ID3D11Texture2D	*pTemp=0;
	m_pTexture=0;
	m_name=std::wstring(FileName);
	m_stage=stage;
	ReadDDSTexture(&m_pTexture);

}
//----------------------------------------------------
CTexture::~CTexture(void)
{


}

//------------------------------------------------
void CTexture::ReadDDSTexture(ID3D11Texture2D **pTexture)
{
	HRESULT r=0;
	r = D3DX11CreateTextureFromFile(Graphics()->GetDevice(),m_name.c_str(),NULL,NULL,(ID3D11Resource**)pTexture,NULL);

}
//--------------------------------------------------
ID3D11Texture2D	*CTexture::GetTexture()
{
	return m_pTexture;
}
//--------------------------------------------
ID3D11ShaderResourceView *CTexture::GetShaderView()
{
	if(!m_pShaderResourceView)
	{
		D3D11_TEXTURE2D_DESC DescTexture;
		m_pTexture->GetDesc(&DescTexture);

		D3D11_SHADER_RESOURCE_VIEW_DESC DescShaderView;
		memset(&DescShaderView,0,sizeof(DescShaderView));
		DescShaderView.Format=DescTexture.Format;
		DescShaderView.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
		DescShaderView.Texture2D.MipLevels=DescTexture.MipLevels;
		Graphics()->GetDevice()->CreateShaderResourceView(m_pTexture,&DescShaderView,&m_pShaderResourceView);
	}

	return m_pShaderResourceView;
}