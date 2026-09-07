#pragma once

#include "Boss.h"
#include "Time_Manager_API.h"

NS_BEGIN(Client)

class COzraeen_Part;

class COzraeen final : public CBoss
{
	const  static	_uint	WAVEFIREBALLCNT		 = 100;
	const  static	_uint	MAXGUIDEDFIREBALLCNT = 2;
	const  static	_uint	MAXNUMFIREPELLETS	 = 3;

	const inline static _float	Pattern1Offset	  = 3.f;
	const inline static _float	ZOffset			  = -1.f;
	const inline static _float	ProjectileZOffset = -0.0f;
	

	const inline static _float3 Pattern1MovePos0 = _float3{ g_iMapSizeX - Pattern1Offset,	g_iMapSizeY - Pattern1Offset, ZOffset }; // 오른쪽 끝
	const inline static _float3 Pattern1MovePos1 = _float3{ g_iMapSizeX * 0.5f,				g_iMapSizeY - Pattern1Offset, ZOffset }; // 중앙
	const inline static _float3 Pattern1MovePos2 = _float3{ Pattern1Offset,					g_iMapSizeY - Pattern1Offset, ZOffset }; // 왼쪽 끝
	const inline static _float3 Pattern1MovePos3 = _float3{ g_iMapSizeX * .5f,				g_iMapSizeY * .5f,		      ZOffset }; // 화면 중앙

	const inline static _float3	vInitPos = Pattern1MovePos0;

	const inline static _float3 PatternMovePos[12]	= {
		Pattern1MovePos1,
		Pattern1MovePos0, //F
		Pattern1MovePos1, //F
		Pattern1MovePos2, //F
		Pattern1MovePos1, //F

		Pattern1MovePos3, //BW

		Pattern1MovePos1,
		Pattern1MovePos2, //F
		Pattern1MovePos1, //F
		Pattern1MovePos0, //F
		Pattern1MovePos1, //F

		Pattern1MovePos3, //BW
	};

	const inline static _float	MoveThreshold		= 0.5f;
	const inline static _float	MoveSpeed			= 3.0f;

public:
	enum EBOSSSTATE {
		//상태
		BS_IDLE,
		BS_SCREAM,
		BS_HIT,
		BS_DIE,
		//이동
		BS_MOVE,
		BS_CHASE,
		//실패턴
		BS_BIGFIREBALL,
		BS_FIREPELLET,
		BS_BLACKFIREBALL,
		BS_BLACKWAVE,

		BS_END
	};
	enum EBOSSPART {
		BP_RHEAD,
		BP_LHEAD,

		BP_NOSE,
		
		BP_MOUTH0,
		BP_MOUTH1,
		BP_MOUTH2,
		BP_MOUTH3,
		BP_MOUTH4,

		BP_BODY0,
		BP_BODY1,
		BP_BODY2,

		BP_END
	};
private:
	inline static _uint NUMPATTERNGROUP = 3;
	inline static _uint arrPattern1Group[] = { BS_MOVE,		BS_BIGFIREBALL,		BS_SCREAM,		BS_BLACKWAVE };
	inline static _uint arrPattern2Group[] = { BS_CHASE,	BS_BLACKFIREBALL,	BS_FIREPELLET };
	inline static _uint arrPattern3Group[] = { BS_HIT,		BS_DIE,				BS_IDLE };

protected:
	COzraeen(LPDIRECT3DDEVICE9 pGraphic_Device);
	COzraeen(const COzraeen& Prototype);
	virtual ~COzraeen() = default;
public:
	virtual HRESULT Initialize_Prototype()				override; /* 원형을 위한 초기화(서버, 파일) */
	virtual HRESULT Initialize(void* pArg)				override; /* 사본객체를 위한 초기화 */

	virtual void	Priority_Update(_float fTimeDelta)	override;
	virtual void	Update(_float fTimeDelta)			override;
	virtual void	Late_Update(_float fTimeDelta)		override;
private:
	virtual HRESULT	Ready_Components()					override;

	//Set State Func
	HRESULT SetMoveState();
	HRESULT SetChaseState();

	HRESULT SetFireBallState();
	HRESULT SetBlackWaveState();
	HRESULT SetGuidedFireBallState();
	void AdjustBodyScale(const Engine::_float& _fRatio);
	HRESULT SetFirePelletState();

	HRESULT SetHitState();
	HRESULT SetDieState();
	HRESULT SetIdleState();


public:
	static COzraeen*		Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject*	Clone(void* pArg)	override;
	virtual void			Free()				override;

	HRESULT AddBodyPart(COzraeen_Part* _pBodyPart, EBOSSPART _eBossPart);

	HRESULT	CreateResource(_uint iLayerLevelIndex, const _wstring& strLayerTag);

	void OnCollision(FCollisionInfo  _fCollisionInfo) override;
public:
	HRESULT		CreateBody();

	void		HitBlink_Reset();
	virtual HRESULT Reset() override;
private:
	//Move Pattern
	_bool	IsArrived();
	void	Move(_float fTimeDelta);
	void	Chase(_float fTimeDelta);
	void	UpdateBodyChain(_float fTimeDelta);
private:
	CTransform*				m_pivotTransformComs[BP_END];
	list<COzraeen_Part*>	m_ListBodyParts	 	[BP_END];

	_uint					m_iPattern1Idx = 0;
	_float3					m_vTargetPos = {};
	
	class CFireBall*		m_pFireBall								= { nullptr };
	class CFireBall*		m_pWaveFireBalls[WAVEFIREBALLCNT]		= { nullptr };
	class CFireBall*		m_pGuidedFireBalls[MAXGUIDEDFIREBALLCNT]	= { nullptr };
	class CFirePellet*		m_pFirePellets[MAXNUMFIREPELLETS]		= { nullptr };

	UniqueTimer				m_pChaseTimer			= { nullptr };

	UniqueTimer				m_pFireballTimer		= { nullptr };
	UniqueTimer				m_pBlackFireballTimer	= { nullptr };
	UniqueTimer				m_pFIrePelletTimer		= { nullptr };
	UniqueTimer				m_pWaveTimer			= { nullptr };

	UniqueTimer				m_pBounceTimer			= { nullptr };
	UniqueTimer				m_pHitTimer				= { nullptr };

	_uint					m_iCurFirePelletShotCnt = {};
	_uint					m_iCurGuidedFireShotCnt = {};
	_uint					m_iInitFirePelletShotCnt= {3};
	_uint					m_iInitGuidedFireShotCnt= {2};

	_bool					m_bHit = { false };
private:
	class CPSystem_Ozraeen* m_pSystemOzraeen = { nullptr };
};

NS_END