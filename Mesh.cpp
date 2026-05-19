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

CCubeMesh::CCubeMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, XMFLOAT3 size, XMFLOAT4 color) {
	float fHalfWidth = size.x * 0.5f;
	float fHalfHeight = size.y * 0.5f;
	float fHalfDepth = size.z * 0.5f;

	VerticesArray = {
		CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth, color), // 0
		CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth, color), // 1
		CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth, color), // 2
		CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth, color), // 3
		CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth, color), // 4
		CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth, color), // 5
		CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth, color), // 6
		CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth, color)  // 7
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

CObjMesh::CObjMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList,const std::string& filename, XMFLOAT4 color) {
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
			VerticesArray.push_back(CVertex(x, y, z, color));
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