#pragma once
#include "PCH.h"
#include "Mesh.h"
#include "Shader.h"

class CShader;

enum class ObjectType {
	NONE,
	PLAYER,
	ENEMY,
	WALL,
	FLOOR,
	ITEM,
	BULLET,
	CROSS
};

class CGameObject {
protected:
	std::shared_ptr<CMesh> mesh = nullptr;
	std::shared_ptr<CShader> shader = nullptr;
	XMFLOAT3 Position = { 0.f, 0.f, 0.f };
	XMFLOAT3 Rotation = { 0.f, 0.f, 0.f };
	ObjectType Type = ObjectType::NONE;
	XMFLOAT4X4 WorldMatrix = Matrix4x4::Identity();
public:
	CGameObject() {}
	CGameObject(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
		XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type);
	~CGameObject() {}
	
	void SetWorldMatrix();

	void VindingMatrix(ID3D12GraphicsCommandList* CommandList);

	void Draw(ID3D12GraphicsCommandList* CommandList);

	ObjectType GetType() const { return Type; }
	virtual BoundingOrientedBox GetWorldBoundingBox() {
		BoundingOrientedBox worldBoundingBox;
		if (mesh) {
			BoundingOrientedBox localBoundingBox = mesh->m_LocalBoundingBox;
			XMMATRIX matWorld = XMLoadFloat4x4(&WorldMatrix);
			localBoundingBox.Transform(worldBoundingBox, matWorld);
		}
		return worldBoundingBox;
	}

	virtual void Animate(float time) {}

	virtual void OnCollision(std::shared_ptr<CGameObject> pOther) {}

	void Move(float x, float y, float z) { Position.x += x; Position.y += y; Position.z += z; }
	void Rotate(float x, float y, float z) { Rotation.x += x; Rotation.y += y; Rotation.z += z; }

	bool isdead = false;
};

class CCrossHair : public CGameObject {
public:
	CCrossHair() {}
	CCrossHair(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
		XMFLOAT3 position, XMFLOAT3 rotation);
	~CCrossHair() {}
};