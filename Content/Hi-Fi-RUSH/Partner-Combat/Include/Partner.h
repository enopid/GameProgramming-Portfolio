#pragma once
//1.BATTLE
//2.GIMMIC
//3.NORMAL

#include "Character.h"
#include "Time_Manager_API.h"
#include "Client_Defines.h" 

enum class EPartnerState {
	NONE,
	BATTLE,
	GIMMIC,
	NORMAL
};

enum class EPartnerGimmicState {
	NONE,
	ENTER,
	IDLE,
	LOCKON,
	SHOT,
	SUCCESS,
	FAIL,
	EXIT,
	END
};

enum class EPartnerBattleState {
	NONE,
	NORMAL,
	PARRY,
	SPECIAL,
	AIRJAM,
	GROUNDJAM,
	EVENT,
	END
};

NS_BEGIN(Engine)
class CBone;
class CModel;
class CEffectRoot;
NS_END

NS_BEGIN(Client)

class CPartner abstract : public CCharacter
{
	friend class CPlayer;
	inline const static wstring PARTNERSTATENAME[]	= {L"NONE", L"BATTLE", L"GIMMIC", L"NORMAL"};
	inline const static wstring PARTNERGIMMICNAME[] = {L"NONE", L"ENTER", L"IDLE", L"LOCKON", L"SHOT", L"SUCCESS", L"FAIL", L"EXIT" };
	inline const static wstring PARTNERBATTLENAME[] = {L"NONE", L"NORMAL", L"PARRY", L"SPECIAL", L"AIRJAM" , L"GROUNDJAM", L"Event"};
public:
	const static int	PARTNERCOUNT		= 3;
	inline static float	COOLDOWNDURATION	= 1.2f;
	inline static float	TURNBACKDURATION	= 0.4f;

	inline static bool TESTVAL = false;
	enum VFX
	{
		VFX_APEEARANCE,
		VFX_BATTLEAPEEARANCE,
		VFX_EXIT,
		VFX_END
	};
protected:
	const static int CALLKEY = DIK_LCONTROL;
public:
	CPartner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartner(const CPartner& Prototype);
	virtual ~CPartner() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	void SetPartnerPos(_float3 vOffset);
	virtual HRESULT Late_Initialize() override;


	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	virtual void    Add_Listener() override;
	void SetSelected(bool bSelected);
public:
	virtual void	Free() override;
	virtual _int	GetRhythmResult() { return 0; };
public:
	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc)				override;
	HRESULT Add_InitComponents()			override;
public:
	_uint GetPSState() { return m_iPSState; };
	_uint GetPGState() { return m_iPGState; };
	_uint GetPBState() { return m_iPBState; };

	void  SetTarget		(class CEnemy* pEnemy);
	void  SetCooldown	(_bool bIsCooldown);

	void  SetGroundJam();
	void  SetAirJam();
	void  SetParryAtk();
	void  SetSpecialAtk();
	void  Play_AppearSFX();
	void  Play_CallSFX();
protected:
	void Update_PS(_float fTimeDelta);

	virtual void Update_PG(_float fTimeDelta);
	virtual void Update_PB(_float fTimeDelta)		PURE;
	virtual void Update_PN(_float fTimeDelta)		PURE;

	virtual void Update_PG_CTX(_float fTimeDelta)	PURE;
	virtual void Update_PB_CTX(_float fTimeDelta)	PURE;
	virtual void Update_PN_CTX(_float fTimeDelta)	PURE;

	void RestartVFX	(VFX eID, _vector pos);
	void VFX_SetPos	(VFX eID, _vector pos);
	void VFX_SetBone(VFX eID, CBone* pBone);

	virtual HRESULT InitFX() PURE;

	virtual void InitUI();
	virtual void SetUIState();
	virtual void SetTarget();
	virtual void SetEventTarget();
	virtual void SetArmTarget();
private:
	void Turnback(float fDeltaTime);
public:
	void SetOnBattle	(_bool bIsOnBattle)		{ m_bIsOnBattle		= bIsOnBattle; }
	float GetCooldownRatio();
protected:
	unique_ptr<CAnimController>			m_upAnimControllers[ENUM_TO_UINT(EPartnerState::NORMAL)];
	ANIMCTX_DESC						m_ctxDescs[ENUM_TO_UINT(EPartnerState::NORMAL)];
protected:   
	unique_ptr<class CPartnerState_FSM> m_upPSFSM			= nullptr;
	unique_ptr<class CPartnerGimmic_FSM>m_upPGFSM			= nullptr;
	unique_ptr<class CPartnerBattle_FSM>m_upPBFSM			= nullptr;

	_uint								m_iPSState			= 0;
	_uint								m_iPGState			= 0;
	_uint								m_iPBState			= 0;
	_uint								m_iPNState			= 0;

	EPartnerType						m_ePartnerType		= EPartnerType::PT_NONE;
protected:
	_bool								m_bSelected			= false;
	_bool								m_bIsCooldown		= false;

	class	CPlayer*					m_pPlayer			= nullptr;
	class	CPlayer_Cam*				m_pPlayerCam		= nullptr;

	_float4								m_vLookDir			= {0.f, 0.f, 1.f, 0.f};
	_float								m_fGroundHeight		= 0.f;

	//common
	_bool								m_bIsOnAvailable	= true;
	_bool								m_bIsOnLock			= false;
	_bool								m_bIsOnBattle		= false;

	_bool								m_bIsKeyPressed		= false;
	_bool								m_bIsIsDone			= false;
	_bool								m_bIsFar			= false;

	//gimmic
	class	CInteractObject*			m_pPartnerTarget	= nullptr;
	_bool								m_bIsOnTarget		= false;
	_bool								m_bIsEnterEnd		= false;
	_bool								m_bIsExitEnd		= false;
	_bool								m_bIsMoveEnd		= false;
	_bool								m_bIsTaskEnd		= false;
	_bool								m_bIsOnFail			= false;
	_bool								m_bIsOnSuccess		= false;
	_bool								m_bGimmicStart		= false;

	//battle
	class	CEnemy*						m_pEnemyTarget		= nullptr;
	class	CInteractObject*			m_pEventTarget		= nullptr;
	class	CKale_Arm*					m_pArmTarget		= nullptr;
	_bool								m_bIsReadyGroundJam	= false;
	_bool								m_bIsReadyAirJam	= false;
	_bool								m_bIsReadyParryAtk	= false;
	_bool								m_bIsReadySpecialAtk= false;
	_bool								m_bIsReadyEvent		= false;
	_bool								m_bIsKaleArm		= false;


	//UI
	class CUC_Aiming*					m_pUC_Aiming		= nullptr;
	class CUC_Interact*					m_pUC_Interact		= nullptr;
	
	//Particle
	CEffectRoot*						m_pEffects[VFX_END] = {};

	//Timer
	UniqueTimer							m_upCooldownTimer    = nullptr;
	UniqueTimer							m_upTurnbackTimer    = nullptr;
	UniqueTimer							m_upPartnerCallTimer = nullptr;


};
NS_END