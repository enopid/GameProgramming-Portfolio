#pragma once

#include "Rhythm_Manager_API.h"
#include "Base.h"


NS_BEGIN(Engine)

class CRhythm_Manager final : public CBase
{
private:
	CRhythm_Manager();
	virtual ~CRhythm_Manager() = default;
public:
	void		AddBeatNotifiy			(float fSrtBeat, float fEndBeat, wstring sNotyfyTag);
	void		AddImpulseBeatNotifiy	(float fSrtBeat, wstring sNotyfyTag);

	UniqueBeatListener		CreateBeatListener(wstring sNotifyTag, function<void(float)> funcUpdate, function<void(void)> funcEnter, function<void(void)> funcExit, _bool IsOnce);
private:
	void		AddNotify(ANIMNOTIFY_DESC& _desc);
public:
	static CRhythm_Manager* Create();
	virtual void Free() override;
	void Update(_float fDeltaTime);
public:
	void	SetBPM(_float _fBPM);
	_float	GetBPM();
	_float	GetElapsedBeat();
	void	SetBeat(_int _iBeat);
	_int	GetBeat();

	bool	IsOnBeat(initializer_list<_float> fBeatLst = {});
	void	ResetBeat();
private:
	void DestroyListener(wstring sNotifyTag, CBeatListener* _pBeatListener);
	_float	m_fBPM		= 120.f;
	_int	m_iBeat		= 4;
	_float	m_fCurBeat	= 0.f;
	_float	m_fPrevBeat	= 0.f;
	
	vector<ANIMNOTIFY_DESC>					m_vecNotifies;
	_uint									m_iCurNotifyIdx = 0;

	map<wstring, list<CBeatListener*>>	m_mapListeners;
};

NS_END