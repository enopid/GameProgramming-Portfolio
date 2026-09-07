#pragma once
#include "Monster.h"

NS_BEGIN(Engine)
class CCollidor;
NS_END

NS_BEGIN(Client)

class CMonster_Stick1 final : public CMonster
{
	enum EMONSTERANIMATION {
		MA_ATK_1,
		MA_ATK_2,

		MA_ATK_COMBO,
		MA_ATK_JUMP,
		MA_ATK_ROCK,

		MA_CHARGE_INTO,
		MA_CHARGE_BASE,
		MA_CHARGE_ATTACK,

		MA_LOCO_WALK,
		MA_LOCO_JUMP_BACK,

		MA_HIT_SMALL,
		MA_HIT_MIDDLE,
		MA_HIT_BIG,

		MA_REACTION_DIE,
		MA_REACTION_REVIVE,
		MA_REACTION_PARRY,
		MA_REACTION_BOMB,
		MA_REACTION_BIND,

		MA_REACTION_TAUNT,

		MA_END,
	};
public:
	struct FMONSTER_STICK_DESC : FMONSTER_DESC {
	};
public:
	CMonster_Stick1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Stick1(const CMonster_Stick1& Prototype);
	virtual ~CMonster_Stick1() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual void SetBT();
	virtual void SetBB();
	virtual HRESULT Late_Initialize() override;

	void OnHit(const CONTACTMF_DESC& _desc, CCollidor* wpCollidor);

	virtual void		Priority_Update(_float fTimeDelta);
	virtual void		Update(_float fTimeDelta);
	virtual void		Late_Update(_float fTimeDelta);
private:
	void				InitializeNotifies();
	virtual void		Revive() override;
public:
	CGameObject*		Clone(void* pArg) override;
	//Node
	CBT_Node*			CreateNode_Hit();

	virtual CBT_Node*	CreateNode_Combat();
	virtual CBT_Node*	CreateNode_MeleeCombat();
	virtual CBT_Node*	CreateNode_MidCombat();
	virtual CBT_Node*	CreateNode_LongCombat();
public:
	virtual void		Free() override;
	virtual HRESULT		Reset() override;
	virtual bool		OnDamage(FDamageInfo _fDamageInfo) override;
public:
	HRESULT				Add_InitComponents() override;
	virtual HRESULT		Load_Config();
private:
	WPCollidor		m_wpAttackCollidor;
private:
	UniqueTimer		m_upHitTimer;
	_bool			m_bHitResetFlag = false;

	_float			m_fMidRange		= 0.f;
	_float			m_fLongRange	= 0.f;
	_float			m_fAggroRange	= 0.f;
	_float			m_fDeAggroRange	= 0.f;

	_float			m_fWalkSpeed	= 0.f;
	_float			m_fChargeSpeed	= 0.f;


	_int			m_iDamageType = 0;
	_float			m_fDamages[4];
};

NS_END