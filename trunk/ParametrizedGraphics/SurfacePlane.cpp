#include "StdAfx.h"
#include "SurfacePlane.h"
#include "GraphicsLayer.h"
#include <iostream>


CSurfacePlane::CSurfacePlane(void){
	m_pVertexBuffer=NULL;
	m_pPatchIndexBuffer=NULL;
	//m_texture = 0;
	m_iNumVertex=0;
	m_iNumPatches=0;
	m_Ang=1.1;
	
	SetVertex();
	//SetTexCoords();
	SetPatches();
	//LoadTextures();
	CreateBuffers();

}

CSurfacePlane::~CSurfacePlane(void){

}

int CSurfacePlane::GetNumVerts(void)
{
	return m_iNumVertex;
}

//--------------------------------------
int CSurfacePlane::GetNumPatches()
{
	return  m_iNumPatches;
}
HRESULT CSurfacePlane::CreateBuffers()
{

	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC DescBuffer;
	memset(&DescBuffer,0,sizeof(DescBuffer));
	D3D11_SUBRESOURCE_DATA resData;
	memset(&resData,0,sizeof(resData));

	//Settings for vertex buffer
	DescBuffer.Usage=D3D11_USAGE_DYNAMIC;
	DescBuffer.ByteWidth=sizeof(CSurfacePlaneVertex)*GetNumVerts();
	DescBuffer.BindFlags= D3D11_BIND_VERTEX_BUFFER;
	DescBuffer.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	DescBuffer.MiscFlags=0;
	resData.pSysMem=&m_BoxVertex[0];
	hr=Graphics()->GetDevice()->CreateBuffer(&DescBuffer,&resData,&m_pVertexBuffer);
	if(hr!=S_OK)return hr;

	
	//Settings for Patch Index Buffer
	DescBuffer.Usage=D3D11_USAGE_DEFAULT;
	DescBuffer.ByteWidth=sizeof(WORD)*GetNumPatches()*16;
	DescBuffer.BindFlags=D3D11_BIND_INDEX_BUFFER;
	DescBuffer.CPUAccessFlags = 0;
	DescBuffer.MiscFlags = 0;
	resData.pSysMem=&m_PatchIndx[0];
	hr=Graphics()->GetDevice()->CreateBuffer(&DescBuffer,&resData,&m_pPatchIndexBuffer);
	

	return S_OK;
}

void CSurfacePlane::Draw(void)
{
	//actualize the transformations
	Graphics()->SetWorldMtx(m_mToWorldXFrom);
	//Graphics()->SetCullNone();
	Graphics()->SetCullFront();
	Graphics()->SetSmooth();
	//get the buffers
	UINT uiStride=sizeof(CSurfacePlaneVertex);
	UINT uiOffset=0;
	Graphics()->GetDeviceContext()->IASetVertexBuffers(0,1,&m_pVertexBuffer,&uiStride,&uiOffset);
	Graphics()->GetDeviceContext()->IASetIndexBuffer(m_pPatchIndexBuffer,DXGI_FORMAT_R16_UINT,0);
	Graphics()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);
	
	Graphics()->UpdateMatrices();
	Graphics()->SetMatrices();
	
	//draw shell
	
	
	//
	Graphics()->GetDeviceContext()->DrawIndexed(GetNumPatches()*16,0,0);
}

void CSurfacePlane::SetVertex()
{
	
	float x,y,z;

	// Load the vertices
	FILE *file = fopen(".\\SurfaceVertex.txt", "rt");
	if (!file) 
	{
		return;
	}


	int i=0;
	while (!feof(file)) {
		fscanf(file, "%f %f %f,\n", &x,&y,&z);
		m_BoxVertex[i].m_vPoint.x=x;
		m_BoxVertex[i].m_vPoint.y=y;
		m_BoxVertex[i].m_vPoint.z=z;
		i++;
	}
	fclose(file);
	m_iNumVertex = i;
	


}

void CSurfacePlane::SetPatches()
{
	
	
	m_PatchIndx[0].v[0]=0;
	m_PatchIndx[0].v[1]=1;
	m_PatchIndx[0].v[2]=2;
	m_PatchIndx[0].v[3]=3;
	m_PatchIndx[0].v[4]=4;
	m_PatchIndx[0].v[5]=5;
	m_PatchIndx[0].v[6]=6;
	m_PatchIndx[0].v[7]=7;
	m_PatchIndx[0].v[8]=8;
	m_PatchIndx[0].v[9]=9;
	m_PatchIndx[0].v[10]=10;
	m_PatchIndx[0].v[11]=11;
	m_PatchIndx[0].v[12]=12;
	m_PatchIndx[0].v[13]=13;
	m_PatchIndx[0].v[14]=14;
	m_PatchIndx[0].v[15]=15;
	m_iNumPatches = 1;
}

void CSurfacePlane::Update(float DeltaTime)
{
	m_Ang+=DeltaTime;
	D3DXMatrixRotationYawPitchRoll(&m_mToWorldXFrom,m_Ang,0.1*m_Ang,0.1*m_Ang);
	
}