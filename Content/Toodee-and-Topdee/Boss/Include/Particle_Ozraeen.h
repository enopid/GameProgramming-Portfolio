#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Time_Manager_API.h"
#include "Particle.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CDimensionSwitcher;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CParticle_Ozraeen :
	public CParticle
{
private:
	CParticle_Ozraeen(LPDIRECT3DDEVICE9 pGraphic_Device);
	CParticle_Ozraeen(const CParticle_Ozraeen& Prototype);
	virtual ~CParticle_Ozraeen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Ready_Components() override;

public:
	static CParticle_Ozraeen* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

	_bool IsAlive() { return m_ParticleAttr._isAlive; }
	void Set_Alive(_bool _Alive) { m_ParticleAttr._isAlive = _Alive; }
	void Respawn(_float3 vPos, _float3 vDir, _float fSpeed, _float fSize);
	void SetBlink(_bool _bBlink) { m_bBlink = _bBlink; }
private:
	class CTexture* m_pOLTextureCom = { nullptr };
	_bool		m_bBlink = false;

};

NS_END