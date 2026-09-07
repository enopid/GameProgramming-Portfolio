#pragma once


#include "Client_Defines.h"
#include "Block.h"

NS_BEGIN(Client)

class CBlock_HoleWall final  : public CBlock
{
private:
	CBlock_HoleWall(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBlock_HoleWall(const CBlock_HoleWall& Prototype);
	virtual ~CBlock_HoleWall() = default;
public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	void Priority_Update(_float fTimeDelta)	override;
	void Update(_float fTimeDelta)			override;
	void Late_Update(_float fTimeDelta)		override;
	HRESULT Render()						override;
private:
	HRESULT	Ready_Components();
public:
	static CBlock_HoleWall* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	void Free() override;

	// CBlock을(를) 통해 상속됨
	virtual Engine::CGameObject* Clone(void* pArg);
};

NS_END