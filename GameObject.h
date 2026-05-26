#pragma once
#include "PCH.h"
#include "Mesh.h"
#include "Shader.h"

class CShader;

struct CB_GAMEOBJECT {
	XMFLOAT4X4 WorldMatrix;
	XMFLOAT4 Color;
};

enum class ObjectType {
	NONE,
	PLAYER,
	ENEMY,
	WALL,
	STAIR,
	FLOOR,
	ITEM,
	CUBE,
	BULLET,
	CROSS,
	START,
	EXIT
};

class CGameObject {
protected:
	std::shared_ptr<CMesh> mesh = nullptr;
	std::shared_ptr<CShader> shader = nullptr;
	XMFLOAT3 Position = { 0.f, 0.f, 0.f };
	XMFLOAT3 Rotation = { 0.f, 0.f, 0.f };
	ObjectType Type = ObjectType::NONE;
	XMFLOAT4 Color = { 1.f, 0.f, 0.f, 1.f };
	XMFLOAT4X4 WorldMatrix = Matrix4x4::Identity();
	XMFLOAT4X4 ParentWorldMatrix = Matrix4x4::Identity();
public:
	CGameObject() {}
	CGameObject(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
		XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type);
	~CGameObject() {}
	
	void SetColor(const XMFLOAT4& color) { Color = color; }
	void SetWorldMatrix();
	void SetParentWorldMatrix(const XMFLOAT4X4& parentWorldMatrix) { ParentWorldMatrix = parentWorldMatrix; }

	void VindingMatrix(ID3D12GraphicsCommandList* CommandList);

	virtual void Draw(ID3D12GraphicsCommandList* CommandList);

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

	virtual void Animate(float time);

	virtual void OnCollision(std::shared_ptr<CGameObject> pOther) {}

	void Move(float x, float y, float z) { Position.x += x; Position.y += y; Position.z += z; }
	void Rotate(float x, float y, float z) { Rotation.x += x; Rotation.y += y; Rotation.z += z; }

	bool isdead = false;
};

class CFloatingObject : public CGameObject {
public:
	CFloatingObject() {}
	CFloatingObject(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
		XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type);
	~CFloatingObject() {}
	void Animate(float time) override;
private:
	float Amplitude = 0.25f;  // ÁøÆø
	float Frequency = 1.5f;  // ºóµµ¼ö
	float TotalTime = 0.0f; // ÃÑ ½Ã°£
	float DeltaY = 0.0f; // YÃà ÀÌµ¿·®
};

class CBullet : public CGameObject {
public:
	CBullet() {}
	CBullet(std::shared_ptr<CMesh> pMesh, std::shared_ptr<CShader> pShader,
		XMFLOAT3 position, XMFLOAT3 rotation, ObjectType type, XMFLOAT3 direction);
	~CBullet() {}
	void Animate(float time) override;
	void OnCollision(std::shared_ptr<CGameObject> pOther) override;
private:
	XMFLOAT3 Direction;
	float Speed;
	float LifeTime;
};