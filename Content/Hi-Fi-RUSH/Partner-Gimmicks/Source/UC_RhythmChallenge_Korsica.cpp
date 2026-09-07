#include "UC_RhythmChallenge_Korsica.h"
#include "UI_Image.h"
#include "GameInstance.h"

CUC_RhythmChallenge_Korsica::CUC_RhythmChallenge_Korsica()
	: CUIController()
{

}

HRESULT CUC_RhythmChallenge_Korsica::Initialize(void* pDesc, _wstring sLVLTag, _wstring sUCName)
{
	CHKFAIL(__super::Initialize(pDesc, sLVLTag, sUCName));

	m_upFailTimer	= m_pGameInstance->CreateTimerWithDuration(0.8f, false, [=]() {
		SetActive(false);
		}, false);
	m_upHitTimer= m_pGameInstance->CreateTimerWithDuration(0.16f, false, [=]() {
		m_vecUIMainObjects[UT_HIT]->SetActive(false, true);
		}, false);
	m_upSuccessTimer = m_pGameInstance->CreateTimerWithDuration(0.8f, false, [=]() {
		SetActive(false);
		//PrintDebug("success");
	}, false);

	return S_OK;
}

void CUC_RhythmChallenge_Korsica::Late_Initialize()
{
	m_vecUIMainObjects.resize(UT_END, nullptr);

	//DISK
	m_vecUIMainObjects[UT_DISK] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_DISK");
	m_vecUIMainObjects[UT_DISK]->SetActive(false, true);

	m_vecUIMainObjects[UT_DISK_CURSOR] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_CURSOR");
	m_vecUIMainObjects[UT_DISK_CURSOR]->SetActive(false, true);

	m_vecUIMainObjects[UT_DISK_SUCCESS0] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_SUCCESSPANEL0");
	m_vecUIMainObjects[UT_DISK_SUCCESS0]->SetActive(false, true);

	m_vecUIMainObjects[UT_DISK_SUCCESS1] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_SUCCESSPANEL1");
	m_vecUIMainObjects[UT_DISK_SUCCESS1]->SetActive(false, true);

	m_vecUIMainObjects[UT_DISK_SUCCESS2] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_SUCCESSPANEL2");
	m_vecUIMainObjects[UT_DISK_SUCCESS2]->SetActive(false, true);

	m_vecUIMainObjects[UT_DISK_SUCCESS3] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_SUCCESSPANEL3");
	m_vecUIMainObjects[UT_DISK_SUCCESS3]->SetActive(false, true);

	//COUNTDOWN
	m_vecUIMainObjects[UT_COUNTDOWN] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_COUNTDOWN");
	m_vecUIMainObjects[UT_COUNTDOWN]->SetActive(false, true);
	m_vecUIMainObjects[UT_COUNTDOWN0] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_CD0");
	m_vecUIMainObjects[UT_COUNTDOWN0]->SetActive(false, true);
	m_vecUIMainObjects[UT_COUNTDOWN1] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_CD1");
	m_vecUIMainObjects[UT_COUNTDOWN1]->SetActive(false, true);

	//NOTE
	m_vecUIMainObjects[UT_NOTE] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_NOTES");
	m_vecUIMainObjects[UT_NOTE]->SetActive(false, true);

	m_vecUIMainObjects[UT_NOTE0] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_NOTE0");
	m_vecUIMainObjects[UT_NOTE0]->SetActive(false, true);
	m_vecUIMainObjects[UT_NOTE1] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_NOTE1");
	m_vecUIMainObjects[UT_NOTE1]->SetActive(false, true);
	m_vecUIMainObjects[UT_NOTE2] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_NOTE2");
	m_vecUIMainObjects[UT_NOTE2]->SetActive(false, true);
	m_vecUIMainObjects[UT_NOTE3] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_NOTE3");
	m_vecUIMainObjects[UT_NOTE3]->SetActive(false, true);

	//FAIL
	m_vecUIMainObjects[UT_FAIL] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_FAILURE");
	m_vecUIMainObjects[UT_FAIL]->SetActive(false, true);
	m_vecUIMainObjects[UT_FAILMARKER] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_FAILUREMARK");
	m_vecUIMainObjects[UT_FAILMARKER]->SetActive(false, true);

	//HIT
	m_vecUIMainObjects[UT_HIT] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_KORSICA_HIT");
	m_vecUIMainObjects[UT_HIT]->SetActive(false, true);
	m_vecUIMainObjects[UT_YEAH] = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_YEAH");
	m_vecUIMainObjects[UT_YEAH]->SetActive(false, true);



}

void CUC_RhythmChallenge_Korsica::Update(float fDeltaTime)
{
	__super::Update(fDeltaTime);

	Countdown();
	if (m_iCnt >= 4) {
		TurnCursor();

		if(m_pGameInstance->IsOnBeat())
			m_pGameInstance->Play_SFX(L"ui_gmk_korsica_beat_guide_01_Play", 1.f);
	}
}

void CUC_RhythmChallenge_Korsica::Enter()
{
	m_iPhase	= 0;
	for (size_t i = 0; i < UT_END; i++)
		m_vecUIMainObjects[i]->SetActive(false, true);
	m_vecUIMainObjects[UT_DISK]->SetActive(true, true);
	m_vecUIMainObjects[UT_DISK_SUCCESS0]->SetActive(false, true);
	m_vecUIMainObjects[UT_DISK_SUCCESS1]->SetActive(false, true);
	m_vecUIMainObjects[UT_DISK_SUCCESS2]->SetActive(false, true);
	m_vecUIMainObjects[UT_DISK_SUCCESS3]->SetActive(false, true);

	m_vecUIMainObjects[UT_DISK_CURSOR]->Set_Rotation(0.f);

	m_pGameInstance->Play_SFX(L"obj_gmk_PowerPlant_RhythmChallenge_BGM_Play", 1.f);
}

void CUC_RhythmChallenge_Korsica::Exit()
{
	m_iCnt = -1;
	for (size_t i = 0; i < UT_END; i++)
		m_vecUIMainObjects[i]->SetActive(false, true);

	m_pGameInstance->Stop_Sound(ESOUNDTYPE::SFX, L"obj_gmk_PowerPlant_RhythmChallenge_BGM_Play");
}

CUC_RhythmChallenge_Korsica* CUC_RhythmChallenge_Korsica::Create(void* pArg, _wstring sLVLTag, _wstring sUCName)
{
	CUC_RhythmChallenge_Korsica* pInstance = new CUC_RhythmChallenge_Korsica();
	if (FAILED(pInstance->Initialize(pArg, sLVLTag, sUCName))) {
		MSG_BOX("Fail to Create CUC_RhythmChallenge_Korsica");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUC_RhythmChallenge_Korsica::Free()
{
	__super::Free();
}


void CUC_RhythmChallenge_Korsica::OnChallenge(_uint iBeatMask)
{
	m_iBeatMask		= iBeatMask;
	m_iCnt			= 0;
	m_iHitNoteNum	= 0;

	SetNote();
	SetActive(true);
}

_bool CUC_RhythmChallenge_Korsica::CheckHit()
{
	_float _fTotalBeat =  m_iPhase*4.f + m_pGameInstance->GetElapsedBeat();

	_bool m_bOnHit = false;
	for (_uint i = 0; i < 16; i++)
	{
		if (m_iBeatMask & (1 << i)) {
			auto _fMinV = i + 1 - m_fBeatOffset;
			auto _fMaxV = i + 1 + m_fBeatOffset;
			if (_fMinV <= _fTotalBeat && _fTotalBeat <= _fMaxV) {
				m_bOnHit = true;
				m_iBeatMask ^= (1 << i);

				m_vecUIMainObjects[UT_DISK_SUCCESS0 + m_iHitNoteNum		]->SetActive(true);
				m_vecUIMainObjects[UT_DISK_SUCCESS0 + m_iHitNoteNum		]->Enter();
				m_vecUIMainObjects[UT_NOTE0			+ m_iHitNoteNum++	]->SetActive(false);
				break;
			}
		}
	}

	return m_bOnHit;
}

_bool CUC_RhythmChallenge_Korsica::CheckMiss()
{
	if (m_iCnt < 4) return false;
	_float _fTotalBeat = m_iPhase * 4.f + m_pGameInstance->GetElapsedBeat();

	_bool m_bOnMiss = false;
	for (_uint i = 0; i < 16; i++)
	{
		if (m_iBeatMask & (1 << i)) {
			auto _fMaxV = i + 1 + m_fBeatOffset;
			if (_fMaxV <= _fTotalBeat) {
				m_bOnMiss = true;
				break;
			}
		}
	}

	return m_bOnMiss;
}

_bool CUC_RhythmChallenge_Korsica::CheckSuccess()
{
	return m_iPhase == 4;
}


void CUC_RhythmChallenge_Korsica::Success()
{
	for (size_t i = 0; i < UT_END; i++)
		m_vecUIMainObjects[i]->SetActive(false, true);
	m_vecUIMainObjects[UT_YEAH]->SetActive(true, true);
	m_vecUIMainObjects[UT_YEAH]->Enter();

	m_pGameInstance->Play_SFX(L"ui_gmk_korsica_finish_01_Play", 1.f);

	m_upSuccessTimer->Restart();
}

void CUC_RhythmChallenge_Korsica::Fail()
{
	m_vecUIMainObjects[UT_FAIL]->SetActive(true, true);
	m_vecUIMainObjects[UT_FAIL]->Enter();

	m_pGameInstance->Play_SFX(L"ui_gmk_korsica_fail_01_Play", 1.f);

	m_upFailTimer->Restart();

	_float _fTotalBeat = m_iNoteIndices[min(m_iHitNoteNum, 3)] + 1.f;

	_fTotalBeat = _fTotalBeat / 8.f;
	auto _fAngle = _fTotalBeat * 360.f;
	_float fRadius = 95.f;
	_float2 vPos;
	vPos.x = fRadius * sin(XMConvertToRadians(_fAngle));
	vPos.y = -fRadius * cos(XMConvertToRadians(_fAngle));
	m_vecUIMainObjects[UT_FAILMARKER]->Set_ObjectPosition(vPos);
	//PrintDebug(m_iHitNoteNum);

	m_iHitNoteNum = -1;
}

void CUC_RhythmChallenge_Korsica::Hit()
{
	m_vecUIMainObjects[UT_HIT]->SetActive(true, true);
	m_vecUIMainObjects[UT_HIT]->Enter();
	m_upHitTimer->Restart();
}

void CUC_RhythmChallenge_Korsica::NoteAppear()
{
	for (int idx = 0; idx < 4; idx++)
	{
		auto _fTotalBeat = m_iNoteIndices[idx] + 1.f;
		if ((_fTotalBeat <= 8.f && m_iPhase <= 1) ||
			(_fTotalBeat > 8.f && m_iPhase > 1)
			) {
			if (auto spComp = m_vecUIMainObjects[UT_NOTE0 + idx]->Get_Component<CUIImageRenderer>().lock()) {
				ConvertWPComponent<CUIImageRenderer>(spComp).lock()->ChangeImageFrame(1);
			}
			m_vecUIMainObjects[UT_NOTE0 + idx]->SetActive(true);
			m_vecUIMainObjects[UT_NOTE0 + idx]->Enter();
		}
	}
}

void CUC_RhythmChallenge_Korsica::SetNote()
{
	int idx = 0;
	for (_uint i = 0; i < 16; i++)
	{
		if (m_iBeatMask & (1 << i)) {
			_float _fTotalBeat = i+1.f;
			if (_fTotalBeat > 8.f) {
				_fTotalBeat -= 8.f;
			} 
			else {

			}
			_fTotalBeat = clamp(_fTotalBeat / 8.f, 0.f, 1.f);
			auto _fAngle = _fTotalBeat * 360.f;
			m_vecUIMainObjects[UT_NOTE0 + idx]->Set_Rotation(-_fAngle);
			_float fRadius = 95.f;
			_float2 vPos;
			vPos.x = fRadius * sin(XMConvertToRadians(_fAngle));
			vPos.y = -fRadius	* cos(XMConvertToRadians(_fAngle));
			m_iNoteIndices[idx] = i;
			m_vecUIMainObjects[UT_NOTE0 + idx++]->Set_ObjectPosition(vPos);
		}
	}
}

void CUC_RhythmChallenge_Korsica::UpdateNote()
{
}

void CUC_RhythmChallenge_Korsica::Countdown()
{
	if		(m_iCnt == 0) {
		if (m_pGameInstance->IsOnBeat({ 1 })) {
			if (auto spComp = m_vecUIMainObjects[UT_COUNTDOWN0]->Get_Component<CUIImageRenderer>().lock()) {
				ConvertWPComponent<CUIImageRenderer>(spComp).lock()->ChangeImageFrame(2);
			}
			m_vecUIMainObjects[UT_COUNTDOWN0]->Enter();
			m_vecUIMainObjects[UT_COUNTDOWN0]->SetActive(true);
			m_vecUIMainObjects[UT_COUNTDOWN1]->SetActive(false);
			m_iCnt = 1;

			m_pGameInstance->Play_SFX(L"ui_gmk_korsica_count_3_01_Play", 1.f);
		}
	}
	else if (m_iCnt == 1) {
		if (m_pGameInstance->IsOnBeat({ 2 })) {
			if (auto spComp = m_vecUIMainObjects[UT_COUNTDOWN0]->Get_Component<CUIImageRenderer>().lock()) {
				ConvertWPComponent<CUIImageRenderer>(spComp).lock()->ChangeImageFrame(1);
			}
			m_vecUIMainObjects[UT_COUNTDOWN0]->Enter();
			m_vecUIMainObjects[UT_COUNTDOWN0]->SetActive(true);
			m_vecUIMainObjects[UT_COUNTDOWN1]->SetActive(false);
			m_iCnt = 2;

			m_pGameInstance->Play_SFX(L"ui_gmk_korsica_count_2_01_Play", 1.f);
		}
	}
	else if (m_iCnt == 2) {
		if (m_pGameInstance->IsOnBeat({ 3 })) {
			if (auto spComp = m_vecUIMainObjects[UT_COUNTDOWN0]->Get_Component<CUIImageRenderer>().lock()) {
				ConvertWPComponent<CUIImageRenderer>(spComp).lock()->ChangeImageFrame(0);
			}
			m_vecUIMainObjects[UT_COUNTDOWN0]->Enter();
			m_vecUIMainObjects[UT_COUNTDOWN0]->SetActive(true);
			m_vecUIMainObjects[UT_COUNTDOWN1]->SetActive(false);
			m_iCnt = 3;

			m_pGameInstance->Play_SFX(L"ui_gmk_korsica_count_1_01_Play", 1.f);
		}
	}
	else if (m_iCnt == 3) {//0
		if (m_pGameInstance->IsOnBeat({ 0 })) {
			m_vecUIMainObjects[UT_COUNTDOWN1]->Enter();

			m_iPhase = 0;
			NoteAppear();

			m_vecUIMainObjects[UT_COUNTDOWN0]->SetActive(false);
			m_vecUIMainObjects[UT_COUNTDOWN1]->SetActive(true);

			m_iCnt = 4;

			m_pGameInstance->Play_SFX(L"ui_gmk_korsica_count_go_01_Play", 1.f);
		}
	}
	else if (m_iCnt == 4) {//1
		if (m_pGameInstance->IsOnBeat({ 1 })) {
			m_vecUIMainObjects[UT_COUNTDOWN0]->SetActive(false);
			m_vecUIMainObjects[UT_COUNTDOWN1]->SetActive(false);
		}
		if (m_pGameInstance->IsOnBeat({ 0 })) {
			m_iPhase = 1;
			m_iCnt = 5;
		}
	}
	else if (m_iCnt == 5) {//2
		if (m_pGameInstance->IsOnBeat({ 0 })) {
			m_iPhase = 2;
			NoteAppear();
			m_iCnt = 6;
		}
	}
	else if (m_iCnt == 6) {//3
		if (m_pGameInstance->IsOnBeat({ 0 })) {
			m_iPhase = 3;
			m_iCnt = 7;
		}
	}
	else if (m_iCnt == 7) {//4
		if (m_pGameInstance->IsOnBeat({ 0 })) {
			m_iPhase = 4;
			m_iCnt = 8;
		}
	}
}

void CUC_RhythmChallenge_Korsica::TurnCursor()
{
	if (m_iPhase >= 4) return;
	_float _fTotalBeat = m_iPhase * 4.f + m_pGameInstance->GetElapsedBeat();
	if (_fTotalBeat > 8.f) _fTotalBeat -= 8.f;
	_fTotalBeat  = clamp(_fTotalBeat / 8.f, 0.f, 1.f);

	m_vecUIMainObjects[UT_DISK_CURSOR]->Set_Rotation(-_fTotalBeat * 360.f);
}
