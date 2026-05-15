#include "Mesh.h"

CCubeMesh::CCubeMesh(float w, float h, float d) {
	float fHalfWidth = w * 0.5f;
	float fHalfHeight = h * 0.5f;
	float fHalfDepth = d * 0.5f;

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

	IndicesArray = {
		0, 1, 2, 0, 2, 3, // Front face
		4, 5, 6, 4, 6, 7, // Back face
		0, 5, 4, 0, 4, 1, // Top face
		2, 6, 3, 2, 7, 6, // Bottom face
		3, 5, 0, 3, 6, 5, // Left face
		1, 7, 2, 1, 4, 7  // Right face
	};

	CalculateLocalBoundingBox();
}

CPlaneMesh::CPlaneMesh(float w, float h) {
	float fHalfWidth = w * 0.5f;
	float fHalfHeight = h * 0.5f;
	VerticesArray = {
		CVertex(-fHalfWidth, 0, +fHalfHeight), // 0
		CVertex(+fHalfWidth, 0, +fHalfHeight), // 1
		CVertex(+fHalfWidth, 0, -fHalfHeight), // 2
		CVertex(-fHalfWidth, 0, -fHalfHeight)  // 3
	};
	IndicesArray = {
		0, 1, 2,
		0, 2, 3
	};
	CalculateLocalBoundingBox();
}

CObjMesh::CObjMesh(const std::string& filename) {
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
	CalculateLocalBoundingBox();
}