#pragma once
#include "PCH.h"
#include "Mesh.h"
#include "Shader.h"

// 순환 참조 문제 해결을 위한 전방 선언
class CShader;

//게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다.
struct CB_GAMEOBJECT_INFO {
	XMFLOAT4X4 m_xmf4x4World;
};

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
	void SetShader(std::shared_ptr<CShader> pShader) { shader = pShader; }
	virtual void SetPosition(float x, float y, float z) { Position = XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { Rotation = XMFLOAT3(x, y, z); }
	void SetType(ObjectType type) { Type = type; }

	void SetWorldMatrix();
	void VindingMatrix(ID3D12GraphicsCommandList* CommandList);

	void Draw(ID3D12GraphicsCommandList* CommandList);

	std::shared_ptr<CMesh> GetMesh() { return mesh; }
	XMFLOAT3 GetPosition() { return Position; }
	XMFLOAT3 GetRotation() { return Rotation; }
	ObjectType GetType() const { return Type; }
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
	std::shared_ptr<CShader> shader;
	XMFLOAT3 Position = { 0.f, 0.f, 0.f };
	XMFLOAT3 Rotation = { 0.f, 0.f, 0.f };
	ObjectType Type;
	XMFLOAT4X4 WorldMatrix = Matrix4x4::Identity();
};