#pragma once
#include "Partner.h"

NS_BEGIN(Client)
class CPartner_Character_Macaron final : public CPartner
{
	enum ETARGET {
		NONE,
		Wall,
		PLATFORM,
		SHIELD
	};
	enum MACARONVFX {
		VFX_MACARON_HIT_NORMAL,
		VFX_MACARON_HIT_BOOM,
		VFX_MACARON_ATK_NORMAL0,
		VFX_MACARON_ATK_NORMAL1,
		VFX_MACARON_END
	};
public:
	CPartner_Character_Macaron(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartner_Character_Macaron(const CPartner_Character_Macaron& Prototype);
	virtual ~CPartner_Character_Macaron() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	virtual void    Add_Listener() override;
public:
	CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;
	virtual HRESULT	Reset() override;
public:
	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc) override;
	HRESULT Add_InitComponents() override;

	void	CheckOnTarget();
public:
	HRESULT Load_Config();

	void Update_PB(_float fTimeDelta);
	void Update_PN(_float fTimeDelta);

	void Update_PG_CTX(_float fTimeDelta);
	void Update_PB_CTX(_float fTimeDelta);
	void Update_PN_CTX(_float fTimeDelta);

private:
	virtual void InitUI() override;;
	virtual void SetUIState() override;;

	CBone* m_pCameraBone	= nullptr;

	_float	m_fYaw			= 0.f; 
	_float	m_fPitch		= 0.f; 
	_int	m_iTurnSide		= 0;
	ETARGET	m_eTarget		= ETARGET::NONE;

	// CPartner을(를) 통해 상속됨
	_bool OnDamage(FDamageInfo _fDamageInfo) override;
	HRESULT InitFX() override;
private:
	class CUC_RhythmChallenge_Macaron*	m_pUC_RC_Macaron	= nullptr;
	CBone*								m_pRC_Bone			= nullptr;
	
	CBone*								m_pParticle_Bone	= nullptr;
	CBone*								m_pGravity_Bone		= nullptr;
	CEffectRoot* m_pMacaronEffects[VFX_MACARON_END] = { nullptr, nullptr };

	class CGravitySphere*				m_pSphereInner = nullptr;
	class CGravitySphere*				m_pSphereOuter = nullptr;
};
NS_END
