#pragma once
#include "Partner.h"

NS_BEGIN(Client)
class CPartner_Character_Korsica final : public CPartner
{
	enum ETARGET {
		NONE,
		FIRE,
		GENERATOR
	};
	enum KORSICAVFX {
		VFX_KORSICA_TURN,
		VFX_KORSICA_GIMMIC_LOOP0,
		VFX_KORSICA_GIMMIC_LOOP1,
		VFX_KORSICA_GIMMIC_SUCCESS,
		VFX_KORSICA_ATK_NORMAL,
		VFX_KORSICA_ATK_HOLD,
		VFX_KORSICA_END
	};
public:
	CPartner_Character_Korsica(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartner_Character_Korsica(const CPartner_Character_Korsica& Prototype);
	virtual ~CPartner_Character_Korsica() = default;
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

	_int	GetRhythmResult() override { return m_iRhythmResult; };
private:
	void Set_Weapon();
private:
	virtual void InitUI() override;;
	virtual void SetUIState() override;;

	CBone*	m_pCameraBone	= nullptr;
	ETARGET	m_eTarget		= ETARGET::NONE;
	int		m_iRhythmResult = 0;

	class CWeapon* m_pWeaponL = nullptr;
	class CWeapon* m_pWeaponR = nullptr;

	// CPartner을(를) 통해 상속됨
	_bool OnDamage(FDamageInfo _fDamageInfo) override;
private:
	class CUC_RhythmChallenge_Korsica* m_pUC_RC_Korsica = nullptr;

	HRESULT InitFX() override;
	CEffectRoot* m_pKorsicaEffects[VFX_KORSICA_END];

	class CBone*			m_pTornadoBone[2]	= { nullptr, nullptr };
	class CBone*			m_pSpinBone			= nullptr;
	class CKorsica_Wind*	m_pHoldWind			= nullptr;
};
NS_END
