#pragma once

#include "Base.h"

/* 예를 들어 대기모션에 해당하는 애니메이션 객체 */
/* 대기모션을 위해 어떤 뼈(Channel)들을 0부터 끝날까지 어떤 상태(KeyFrame)로 제어를 해나가야하는지? */

NS_BEGIN(Engine)
class CBone;


class ENGINE_DLL CAnimation : public CBase
{
protected:
	const static _uint ANIMATIONHEADER = 0x900DFACE;
	const static _uint ANIMATIONFOOTER = 0xBAADFACE;
	static inline const char* ROOTMOTIONTYPES[3] = {"NONE", "INPLACE", "DELTA" };
	static inline const char* LOOPDELTATYPES[3] = {"DELTAIGNORE", "DELTAKEEP", "DELTASNAP" };
public:
	enum EROOTMOTIONTYPE {
		NONE,
		INPLACE,
		DELTA
	};
	enum ELOOPDELTATYPE {
		DELTAIGNORE,
		DELTAKEEP,
		DELTASNAP
	};
	typedef struct tagAnimationDesc {
		_tchar		m_sAnimationName[60]	= L"";
		_float		m_fDuration				= {};
		_float		m_fTickPerSecond		= {};
		_bool		m_bLoop					= false;
		_int		m_iRootBoneIdx			= -1;
		_int		m_iRootMotionType		= EROOTMOTIONTYPE::DELTA;		//None, Inplace, delta
		_int		m_iLoopDeltaType		= ELOOPDELTATYPE::DELTAIGNORE;	//Ignore, keep, snap
	} ANIMATION_DESC;
protected:
	CAnimation();
	CAnimation(const CAnimation& copy);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(fstream& _fs);
	HRESULT Save(fstream& _fs);
	list<ANIMNOTIFY_DESC>	Update_AnimationsNotifies();
	list<ANIMNOTIFY_DESC>	Reset_Notifies();
	_matrix		Update_TransformationMatrices			(_float fTimeDelta, const vector<CBone*>& Bones);
	_matrix		Update_Additive_TransformationMatrices	(_float fTimeDelta, const vector<CBone*>& Bones, const vector<CBone*>& refBones, _float fWeight);
	list<ANIMNOTIFY_DESC>	Reset();
	_bool IsEnd() { return m_bIsEnd; };
	const ANIMATION_DESC& GetDesc() { return m_desc; }

	void SetLoop(bool bLoop) { m_desc.m_bLoop = bLoop; }
	void SetRootMotionType(EROOTMOTIONTYPE	eRootMotionType)	{ m_desc.m_iRootMotionType	= eRootMotionType;	}
	void SetLoopDeltaType(ELOOPDELTATYPE	eLoopDeltaType)		{ m_desc.m_iLoopDeltaType	= eLoopDeltaType;	}
	virtual HRESULT Render_Animation_Property();
	void AddNotify(ANIMNOTIFY_DESC& _desc);
	void SetRootBone(_uint i);
	_float GetRatio();
	void SetSpeed(_float _fSpeed) {
		m_fTimeCoeff = _fSpeed;
	}
	
protected:
	ANIMATION_DESC	m_desc = {};
	_float			m_fCurrentTrackPosition = {};
	_float			m_fTimeCoeff			= {1.f};
	_uint			m_iNumChannels = {};
	vector<class CChannel*>	m_vecChannels;
	//Notify
	vector<ANIMNOTIFY_DESC>	m_vecNotifies;
	_uint					m_iCurNotifyIdx = 0;

	vector<wstring>			m_vecBoneNames;
	_bool			m_bIsEnd = false;

public:
	CAnimation* Clone();
	static CAnimation* Create(fstream& _fs);
	virtual void Free() override;
};

NS_END

