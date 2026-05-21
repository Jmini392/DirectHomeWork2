#include "Mesh.h"

void CMesh::IAVinding(ID3D12GraphicsCommandList* CommandList) {
	CommandList->IASetPrimitiveTopology(PrimitiveTopology);

	CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	
	CommandList->IASetIndexBuffer(&IndexBufferView);
	CommandList->DrawIndexedInstanced(IndicesArray.size(), 1, 0, 0, 0);
}

void CMesh::CalculateLocalBoundingBox() {
	if (VerticesArray.empty()) return;

	BoundingOrientedBox::CreateFromPoints(
		m_LocalBoundingBox,
		VerticesArray.size(),
		&(VerticesArray[0].vertex),
		sizeof(CVertex)
	);
}

CCubeMesh::CCubeMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, XMFLOAT3 size) {
	float fHalfWidth = size.x * 0.5f;
	float fHalfHeight = size.y * 0.5f;
	float fHalfDepth = size.z * 0.5f;

	VerticesArray = {
		CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth), // 0
		CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth), // 1
		CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth), // 2
		CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth), // 3
		CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth), // 4
		CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth), // 5
		CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth), // 6
		CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth)  // 7
	};
	VertexBuffer = ::CreateBufferResource(Device, CommandList, VerticesArray.data(), sizeof(CVertex) * VerticesArray.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &VertexUploadBuffer);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = sizeof(CVertex);
	VertexBufferView.SizeInBytes = sizeof(CVertex)* VerticesArray.size();

	IndicesArray = {
		0, 1, 2, 0, 2, 3, // Front face
		4, 5, 6, 4, 6, 7, // Back face
		0, 5, 4, 0, 4, 1, // Top face
		2, 6, 3, 2, 7, 6, // Bottom face
		3, 5, 0, 3, 6, 5, // Left face
		1, 7, 2, 1, 4, 7  // Right face
	};

	IndexBuffer = ::CreateBufferResource(Device, CommandList, IndicesArray.data(), sizeof(UINT) * IndicesArray.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &IndexUploadBuffer);

	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = sizeof(UINT) * IndicesArray.size();

	CalculateLocalBoundingBox();
}

CStairMesh::CStairMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList) {
	VerticesArray = {
		CVertex(-5.f, 5.f, 5.f), // 0
		CVertex(5.f, 5.f, 5.f), // 1
		CVertex(-5.f, -5.f, 5.f), // 2
		CVertex(5.f, -5.f, 5.f), // 3
		CVertex(-5.f, -5.f, -5.f), // 4
		CVertex(5.f, -5.f, -5.f), // 5
	};
	VertexBuffer = ::CreateBufferResource(Device, CommandList, VerticesArray.data(), sizeof(CVertex) * VerticesArray.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &VertexUploadBuffer);
	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = sizeof(CVertex);
	VertexBufferView.SizeInBytes = sizeof(CVertex) * VerticesArray.size();
	IndicesArray = { 
		0, 1, 2, 1, 3, 2,
		0, 1, 4, 1, 5, 4,
		0, 4, 2, 1, 3, 5
	};
	IndexBuffer = ::CreateBufferResource(Device, CommandList, IndicesArray.data(), sizeof(UINT) * IndicesArray.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &IndexUploadBuffer);
	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = sizeof(UINT) * IndicesArray.size();
	CalculateLocalBoundingBox();
}


CCrosshairMesh::CCrosshairMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList) {
	VerticesArray = {
		CVertex(-0.075f, 0.02f, 0.0f), // 0
		CVertex(-0.02f, 0.02f, 0.0f), // 1
		CVertex(-0.02f, 0.075f, 0.0f), // 2
		CVertex(0.02f, 0.075f, 0.0f), // 3
		CVertex(0.02f, 0.02f, 0.0f), // 4
		CVertex(0.075f, 0.02f, 0.0f), // 5
		CVertex(-0.075f, -0.02f, 0.0f), // 6
		CVertex(-0.02f, -0.02f, 0.0f), // 7
		CVertex(-0.02f, -0.075f, 0.0f), // 8
		CVertex(0.02f, -0.075f, 0.0f), // 9
		CVertex(0.02f, -0.02f, 0.0f), // 10
		CVertex(0.075f, -0.02f, 0.0f) // 11
	};
	VertexBuffer = ::CreateBufferResource(Device, CommandList, VerticesArray.data(), sizeof(CVertex) * VerticesArray.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &VertexUploadBuffer);
	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = sizeof(CVertex);
	VertexBufferView.SizeInBytes = sizeof(CVertex) * VerticesArray.size();
	IndicesArray = { 
		0, 1, 6, 6, 1, 7,
		1, 2, 4, 2, 3 , 4,
		4, 5, 11, 4, 11, 10,
		1, 4, 7, 4, 10, 7,
		7, 9, 8, 7, 10, 9
	};
	IndexBuffer = ::CreateBufferResource(Device, CommandList, IndicesArray.data(), sizeof(UINT) * IndicesArray.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &IndexUploadBuffer);
	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = sizeof(UINT) * IndicesArray.size();
	CalculateLocalBoundingBox();
}

CObjMesh::CObjMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		return;
	}
	std::string line;
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string prefix;
		ss >> prefix;

		if (prefix == "v") {
			float x, y, z;
			ss >> x >> y >> z;
			VerticesArray.push_back(CVertex(x, y, z));
		}
		else if (prefix == "f") {
			for (int i = 0; i < 3; ++i) {
				std::string faceData;
				ss >> faceData;

				size_t slashPos = faceData.find('/');
				std::string vIdxStr = (slashPos != std::string::npos) ? faceData.substr(0, slashPos) : faceData;

				int vIdx = std::stoi(vIdxStr) - 1;
				IndicesArray.push_back(vIdx);
			}
		}
	}

	VertexBuffer = ::CreateBufferResource(Device, CommandList, VerticesArray.data(), sizeof(CVertex) * VerticesArray.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &VertexUploadBuffer);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = sizeof(CVertex);
	VertexBufferView.SizeInBytes = sizeof(CVertex) * VerticesArray.size();

	IndexBuffer = ::CreateBufferResource(Device, CommandList, IndicesArray.data(), sizeof(UINT) * IndicesArray.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &IndexUploadBuffer);

	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = sizeof(UINT) * IndicesArray.size();

	CalculateLocalBoundingBox();
}