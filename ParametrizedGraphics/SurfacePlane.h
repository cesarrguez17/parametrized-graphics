#pragma once

class CSurfacePlaneVertex{
public:
	D3DXVECTOR3 m_vPoint;
};

class Patch{
public:
	WORD v[16];
};

class CSurfacePlane{
public:
	CSurfacePlane(void);
	~CSurfacePlane(void);
	CSurfacePlaneVertex	m_BoxVertex[4];
	Patch				m_PatchIndx[1];

	ID3D11Buffer		*m_pVertexBuffer;
	ID3D11Buffer		*m_pPatchIndexBuffer;

	
	D3DXMATRIX			m_mToWorldXFrom;				//transformation related to the world coordinates
	D3DXMATRIXA16		m_mWorldView;					//transformation to the camera's frame
	D3DXMATRIXA16		m_mWorldViewProjection;		//3D->2D projection
	

	int					m_iNumVertex;
	int					m_iNumPatches;
	float				m_Ang;


	ID3D11ShaderResourceView* m_texture;

	void	 SetVertex();
	//void	 SetTexCoords();
	void	 SetPatches();

	int		 GetNumVerts();
	int		 GetNumPatches();
	void	 Draw();
	void	 Update(float DeltaTime);
	boolean  LoadTextures();
	HRESULT  CreateBuffers();
};