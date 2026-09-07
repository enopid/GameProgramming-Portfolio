#include "UC_CutScene.h"
#include "GameInstance.h"

CUC_CutScene::CUC_CutScene()
	: CUIController()
{

}

HRESULT CUC_CutScene::Initialize(void* pDesc, _wstring sLVLTag, _wstring sUCName)
{
	CHKFAIL(__super::Initialize(pDesc, sLVLTag, sUCName));
	m_pIllustTime = m_pGameInstance->CreateTimerWithDuration(m_fIllustDuration, false, [this] {m_vecUIMainObjects[m_iCurImage]->SetActive(false, false); }, false);
	return S_OK;
}

void CUC_CutScene::Late_Initialize()
{
	//m_vecUIMainObjects.resize(NONE, nullptr);
	//SetUI(MIMOSA_INTRO, L"UI_CutScene_Mimosa_Intro");
	//SetUI(MIMOSA_END, L"UI_CutScene_Mimosa_RhythmParryEnd");
	//SetUI(KALE_INTRO, L"UI_CutScene_Kale_Intro");
	//SetUI(KALE_CHAI, L"UI_CutScene_Kale_Finish_Pepper");
	//SetUI(KALE_PEPP, L"UI_CutScene_Kale_Finish_Macaron");
	//SetUI(KALE_MACA, L"UI_CutScene_Kale_Finish_Korsica");
	//SetUI(KALE_KOR, L"UI_CutScene_Kale_Finish_Chai");
	
}

void CUC_CutScene::Update(float fDeltaTime)
{
	if (!m_pIllustTime->IsActive())
	{
		for (auto iter : m_vecUIMainObjects)
			iter->SetActive(false, false);
	}
}

void CUC_CutScene::IllustImage(_int iImage, _float fDutation)
{
	if (iImage >= 7)
		return;

	m_fIllustDuration = fDutation;
	m_pIllustTime->SetDuration(m_fIllustDuration);
	m_pIllustTime->Restart();
	m_iCurImage = iImage;
	m_vecUIMainObjects[m_iCurImage]->SetActive(true, false);
}

void CUC_CutScene::SpeedLine(_int iType, _float fDuration)
{

}

CUC_CutScene* CUC_CutScene::Create(void* pArg, _wstring sLVLTag, _wstring sUCName)
{
	CUC_CutScene* pInstance = new CUC_CutScene();
	if (FAILED(pInstance->Initialize(pArg, sLVLTag, sUCName))) {
		MSG_BOX("Fail to Create CUC_CutScene");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUC_CutScene::Enter()
{
	for (auto iter : m_vecUIMainObjects)
		iter->SetActive(false, true);
}

void CUC_CutScene::Exit()
{
	for (auto iter : m_vecUIMainObjects)
		iter->SetActive(false, true);
}

void CUC_CutScene::Free()
{
	__super::Free();
}

