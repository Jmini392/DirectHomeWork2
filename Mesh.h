#pragma once
#include "PCH.h"

class CVertex {
public:
	CVertex() {}
	CVertex(float x, float y, float z) { vertex.x = x; vertex.y = y; vertex.z = z; }
	~CVertex() {}

	XMFLOAT3 vertex = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

class CFace {
public:
	CFace() {}
	CFace(CVertex v1, CVertex v2, CVertex v3) { Vertex[0] = v1; Vertex[1] = v2; Vertex[2] = v3; }
	~CFace() {}

	// 정점, 법선벡터
	CVertex Vertex[3];
	XMFLOAT3 Normal = { 0.0f, 0.0f, 0.0f };
};

class CMesh {
public:
	CMesh() {}
	virtual ~CMesh() {}

	void IAVinding(ID3D12GraphicsCommandList* CommandList);
	void SetBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	
	// DirectX 바운딩 박스 객체
	BoundingOrientedBox m_LocalBoundingBox;
	// 메쉬크기 만큼 로컬 바운딩 박스 계산
	void CalculateLocalBoundingBox();
protected:
	std::vector<CVertex> VerticesArray = {}; // 정점 배열
	ID3D12Resource* VertexBuffer = nullptr;
	ID3D12Resource* VertexUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = {};

	std::vector<UINT> IndicesArray = {}; // 인덱스 배열
	ID3D12Resource* IndexBuffer = nullptr;
	ID3D12Resource* IndexUploadBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView = {};

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

class CCubeMesh : public CMesh {
public:
	CCubeMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, XMFLOAT3 size = XMFLOAT3(4.f, 4.f, 4.f));
	virtual ~CCubeMesh() {}
};

class CCrosshairMesh : public CMesh {
public:
	CCrosshairMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
	virtual ~CCrosshairMesh() {}
};

class CObjMesh : public CMesh {
public:
	CObjMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, const std::string& filename);
	virtual ~CObjMesh() {}
};