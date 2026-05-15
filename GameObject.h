#pragma once
#include "PCH.h"
#include "Mesh.h"

enum class ObjectType {
	PLAYER,
	ENEMY,
	WALL,
	FLOOR,
	ITEM,
	BULLET,
};

class CGameObject {
public:
	CGameObject() {}
	virtual ~CGameObject() {}
	
	void SetMesh(std::shared_ptr<CMesh> pMesh) { mesh = pMesh; }
	virtual void SetPosition(float x, float y, float z) { Position = XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { Rotation = XMFLOAT3(x, y, z); }
	void SetColor(COLORREF color) { MeshColor = color; }
	void SetWorldMatrix();

	void SetType(ObjectType type) { Type = type; }
	ObjectType GetType() const { return Type; }

	std::shared_ptr<CMesh> GetMesh() { return mesh; }
	XMFLOAT3 GetPosition() { return Position; }
	XMFLOAT3 GetRotation() { return Rotation; }
	COLORREF GetColor() { return MeshColor; }
	XMFLOAT4X4 GetWorldMatrix() { return WorldMatrix; }

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
private:
	std::shared_ptr<CMesh> mesh;
	XMFLOAT3 Position = { 0.f, 0.f, 0.f };
	XMFLOAT3 Rotation = { 0.f, 0.f, 0.f };
	COLORREF MeshColor = RGB(255, 0, 0);
	XMFLOAT4X4 WorldMatrix = Matrix4x4::Identity();
	ObjectType Type;
};

class CWall : public CGameObject {
public:
	CWall();
	virtual ~CWall() {}
};

class CFloor : public CGameObject {
public:
	CFloor();
	virtual ~CFloor() {}
};

class CItem : public CGameObject {
public:
	CItem();
	virtual ~CItem() {}

	virtual void Animate(float time);
	virtual void OnCollision(std::shared_ptr<CGameObject> pOther) override;
	int GetRandomValue() const { return randomValue; }
private:
	int randomValue;
};

class CBullet : public CGameObject {
public:
    CBullet() { SetType(ObjectType::BULLET); }
	virtual ~CBullet() {}

	virtual void SetPosition(float x, float y, float z) override { 
		CGameObject::SetPosition(x, y, z);
		StartPosition = XMFLOAT3(x, y, z);
	}
    void SetDirection(XMFLOAT3 dir) { Direction = dir; }
    void SetSpeed(float s) { Speed = s; }
	
 	virtual void Animate(float time);
	virtual void OnCollision(std::shared_ptr<CGameObject> pOther) override;
private:
	XMFLOAT3 Direction = {0.f, 0.f, 1.f};
	float Speed = 1.f;
	XMFLOAT3 StartPosition = { 0.f, 0.f, 0.f };
};