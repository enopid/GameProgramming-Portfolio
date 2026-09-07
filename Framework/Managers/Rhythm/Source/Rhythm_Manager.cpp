#include "Rhythm_Manager_Impl.h"
#include "Listener.h"
#include "GameInstance.h"

CRhythm_Manager::CRhythm_Manager()
{
}

void CRhythm_Manager::AddNotify(ANIMNOTIFY_DESC& _desc)
{
	m_vecNotifies.push_back(_desc);
	sort(m_vecNotifies.begin(), m_vecNotifies.end(), [](const ANIMNOTIFY_DESC& _desc0, const ANIMNOTIFY_DESC& _desc1) {
		return _desc0.m_fPosition < _desc1.m_fPosition;
		}
	);
}

void CRhythm_Manager::AddBeatNotifiy(float fSrtBeat, float fEndBeat, wstring sNotyfyTag)
{
	ANIMNOTIFY_DESC _desc;
	lstrcpy(_desc.m_sName, sNotyfyTag.c_str());
	_desc.m_fDuration = 0.00f;

	_desc.m_bIsStart = true;
	_desc.m_fPosition = fSrtBeat;
	AddNotify(_desc);
	_desc.m_bIsStart = false;
	_desc.m_fPosition = fEndBeat;
	AddNotify(_desc);
}

void CRhythm_Manager::AddImpulseBeatNotifiy(float fSrtBeat, wstring sNotyfyTag)
{
	ANIMNOTIFY_DESC _desc;
	lstrcpy(_desc.m_sName, sNotyfyTag.c_str());
	_desc.m_fDuration = 0.01f;
	_desc.m_bIsStart = true;
	_desc.m_fPosition = fSrtBeat;
	AddNotify(_desc);
}

UniqueBeatListener CRhythm_Manager::CreateBeatListener(wstring sNotifyTag, function<void(float)> funcUpdate, function<void(void)> funcEnter, function<void(void)> funcExit, _bool IsOnce)
{

	if (!m_mapListeners.count(sNotifyTag)) m_mapListeners.emplace(sNotifyTag, list<class CBeatListener*>());
	auto it = m_mapListeners.find(sNotifyTag);

	CBeatListener* _pBeatListener = new CBeatListener(CListener::Create(funcUpdate, funcEnter, funcExit, IsOnce));
	(it->second).push_back(_pBeatListener);

	return UniqueBeatListener(_pBeatListener, [=](CBeatListener* _pBeatListener) { DestroyListener(sNotifyTag, _pBeatListener); });
}

void CRhythm_Manager::DestroyListener(wstring sNotifyTag, CBeatListener* _pBeatListener)
{
	auto it = m_mapListeners.find(sNotifyTag);
	if (it != m_mapListeners.end()) {
		it->second.remove(_pBeatListener);
		if (_pBeatListener) delete _pBeatListener;
	}
}

CRhythm_Manager* CRhythm_Manager::Create()
{
	return new CRhythm_Manager();
}

void CRhythm_Manager::Free()
{
	__super::Free();
}

void CRhythm_Manager::Update(_float fDeltaTime)
{
	list<ANIMNOTIFY_DESC> _list;
	m_fPrevBeat = m_fCurBeat;
	m_fCurBeat += fDeltaTime * m_fBPM / 60.f;

	while (m_iCurNotifyIdx < m_vecNotifies.size()) {
		if (m_vecNotifies[m_iCurNotifyIdx].m_fPosition <= m_fCurBeat) {
			_list.push_back(m_vecNotifies[m_iCurNotifyIdx]);
			m_iCurNotifyIdx++;
		}
		else {
			break;
		}
	}
	for (auto& pNotify : _list)
	{
		auto it = m_mapListeners.find(wstring(pNotify.m_sName));
		if (it != m_mapListeners.end()) {
			for (auto& upListenr : it->second) {
				if (pNotify.m_bIsStart)
					upListenr->Activate(pNotify.m_fDuration);
				else
					upListenr->Deactivate();
			}
		}
	}

	if (m_fCurBeat > m_iBeat) {
		m_fCurBeat		-= m_iBeat;
		m_iCurNotifyIdx = 0;
	}

	for (auto& [_, lstListenrs] : m_mapListeners) {
		auto it = lstListenrs.begin();
		while (it != lstListenrs.end()) {
			if ((*it)->Update(fDeltaTime)) {
				it = lstListenrs.erase(it);
			}
			else {
				it++;
			}
		}
	}
}

void CRhythm_Manager::SetBPM(_float _fBPM)
{
	m_fBPM = _fBPM;
}

_float CRhythm_Manager::GetBPM() 
{
	return m_fBPM;
}

_float CRhythm_Manager::GetElapsedBeat()
{
	return m_fCurBeat;
}

void CRhythm_Manager::SetBeat(_int _iBeat)
{
	m_iBeat = _iBeat;
}

_int CRhythm_Manager::GetBeat()
{
	return m_iBeat;
}

bool CRhythm_Manager::IsOnBeat(initializer_list<_float> fBeatLst)
{
	if (fBeatLst.size() == 0) {
		for (int i = 0; i < m_iBeat; i++)
		{
			if (m_fPrevBeat < m_fCurBeat) {
				if (m_fPrevBeat < i && i <= m_fCurBeat) return true;
			}
			else {
				if (m_fPrevBeat < i || i <= m_fCurBeat) return true;
			}
		}
	}
	for (_float fBeat : fBeatLst)
	{
		if (m_fPrevBeat < m_fCurBeat) {
			if (m_fPrevBeat < fBeat && fBeat <= m_fCurBeat) return true;
		}
		else {
			if (m_fPrevBeat < fBeat || fBeat <= m_fCurBeat) return true;
		}
	}
	return false;
}

void CRhythm_Manager::ResetBeat()
{
	m_fPrevBeat = m_fCurBeat = 0.f;
	for (auto& _lstNotifies : m_mapListeners) 
		for (auto _pNotify : _lstNotifies.second) 
			_pNotify->Reset();
}
