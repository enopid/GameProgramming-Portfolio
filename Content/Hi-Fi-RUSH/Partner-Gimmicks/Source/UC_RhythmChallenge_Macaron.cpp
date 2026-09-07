#include "UC_RhythmChallenge_Macaron.h"
#include "UI_Image.h"
#include "GameInstance.h"

CUC_RhythmChallenge_Macaron::CUC_RhythmChallenge_Macaron()
	: CUIController()
{

}

HRESULT CUC_RhythmChallenge_Macaron::Initialize(void* pDesc, _wstring sLVLTag, _wstring sUCName)
{
	CHKFAIL(__super::Initialize(pDesc, sLVLTag, sUCName));

	m_upSuccessTimer = m_pGameInstance->CreateTimerWithDuration(0.1f, false, 
	[=]() {
		if (m_vecUIMainObjects[1]) {
			m_vecUIMainObjects[1]->SetActive(false, true);
			m_vecUIMainObjects[1]->Exit();
		}
	}, false);

	return S_OK;
}

void CUC_RhythmChallenge_Macaron::Late_Initialize()
{
	//SetLoopUI
	auto pUIMainObjects = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag, 
		L"UI_RC_MACARON_LOOP");
	if (auto pAim = dynamic_cast<CUI_Image*>(pUIMainObjects))
	{
		m_vecUIMainObjects.push_back(pUIMainObjects);
		pUIMainObjects->SetActive(false, true);
	}
	else
	{
		MSG_BOX("m_pUI pRC_Macaron is nullptr");
	}

	//SetOuterRing
	m_pRingImg = dynamic_cast<CUI_Image*>(m_vecUIMainObjects[0]->Get_ChildObject(L"UI_RC_MACARON_OUTER"));

	//SetSuccessUI
	pUIMainObjects = Get_UIObject(
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		L"UI_RC_MACARON_SUCCESS");
	if (auto pAim = dynamic_cast<CUI_Image*>(pUIMainObjects))
	{
		m_vecUIMainObjects.push_back(pUIMainObjects);
		pUIMainObjects->SetActive(false, true);
	}
	else
	{
		MSG_BOX("m_pUI pRC_Macaron is nullptr");
	}
}

void CUC_RhythmChallenge_Macaron::Update(float fDeltaTime)
{
	__super::Update(fDeltaTime);

	if (m_pRingImg) {
		auto fElapsedBeat = m_pGameInstance->GetElapsedBeat();
		auto fRatio = abs(fElapsedBeat - 2.f);
		fRatio	= abs(fRatio - 1.f);
		fRatio	= 1.f - fRatio;
		//sfRatio	= easeInOutSine(fRatio);

		auto fScale = lerp(0.12f, 0.32f, fRatio);
		m_pRingImg->Set_ObjectScale(_float2(fScale, fScale));
	}
}

void CUC_RhythmChallenge_Macaron::Enter()
{
	m_vecUIMainObjects[0]->SetActive(true, true);
	m_vecUIMainObjects[1]->SetActive(false, true);
}

void CUC_RhythmChallenge_Macaron::Exit()
{
	m_vecUIMainObjects[0]->SetActive(false, true);
	m_vecUIMainObjects[1]->SetActive(false, true);
}

CUC_RhythmChallenge_Macaron* CUC_RhythmChallenge_Macaron::Create(void* pArg, _wstring sLVLTag, _wstring sUCName)
{
	CUC_RhythmChallenge_Macaron* pInstance = new CUC_RhythmChallenge_Macaron();
	if (FAILED(pInstance->Initialize(pArg, sLVLTag, sUCName))) {
		MSG_BOX("Fail to Create CUC_RhythmChallenge_Macaron");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUC_RhythmChallenge_Macaron::Free()
{
	__super::Free();
}

void CUC_RhythmChallenge_Macaron::SetPos(_float2 _vPos)
{
	//Continuously set pos when loop active
	if (m_vecUIMainObjects.empty()) return;
	if (!m_vecUIMainObjects[0]->IsActive()) return;

	m_vecUIMainObjects[0]->Set_ObjectPosition(_vPos);
	m_vecUIMainObjects[1]->Set_ObjectPosition(_vPos);
}

void CUC_RhythmChallenge_Macaron::OnChallenge()
{
	//Active	Loop
	SetActive(true);
}

void CUC_RhythmChallenge_Macaron::Success()
{
	//Deactive	Loop
	m_vecUIMainObjects[0]->SetActive(false, true);
	//Active	Success
	m_vecUIMainObjects[1]->SetActive(true, true);
	m_vecUIMainObjects[1]->Enter();
	//Active	Success Anim
	m_upSuccessTimer->Restart();
	
	//DeActive	Success after all animation

}

void CUC_RhythmChallenge_Macaron::Fail()
{
	//Deactive	All
	SetActive(false);
}
