#include "CutScene.h"
#include "CutScene_Value.h"
#include "BaseModel.h"
#include "GameInstance.h"
#include "Actor.h"
#include "AnimController.h"
#include "CameraHandler.h"
#include "MeshRenderer.h"
#include "UC_Message.h"
#include "UC_Talkbox.h"
#include "CameraObject.h"
#include "RhythmLift.h"
#include "UC_CutRhythmGame.h"
#include "Bone.h"
#include "Mesh.h"
#include "EffectRoot.h"

CCutScene::CCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	
}

CCutScene::CCutScene(const CCutScene& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CCutScene::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
	return S_OK;
}

HRESULT CCutScene::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	wstring sBackdancerTag = L"Backdancer";
	wstring sAntenaTag = L"Antena";

	for (_uint i = BACKDANCER1; i <= BACKDANCER6; i++)
	{
		AddActor(sBackdancerTag + to_wstring(i), L"Prototype_Actor_Backdancer", EACTOR(i));
	}
	for (_uint i = ANTENA1; i <= ANTENA8; i++)
	{
		AddActor(sAntenaTag + to_wstring(i), L"Prototype_Actor_Antena", EACTOR(i));
	}
	AddActor(L"Mimosa", L"Prototype_Actor_Mimosa",                                  MIMOSA);												                                    
	AddActor(L"Player", L"Prototype_Actor_Player",                                  PLAYER);
	AddActor(L"Prototype_Actor_Guitar", L"Prototype_Actor_Guitar",                  GUITAR);
	AddActor(L"Prototype_Actor_Guitar_Origin", L"Prototype_Actor_Guitar_Origin",    GUITARORIGIN);
	AddActor(L"Prototype_Actor_Guitar_Bar", L"Prototype_Actor_Guitar_Bar",          GUITAR_BAR);
	AddActor(L"Prototype_Actor_Guitar_Trash", L"Prototype_Actor_Guitar_Trash",      TRASHGUITAR);
																				    
	AddActor(L"Mimosa2",				L"Prototype_Actor_Mimosa2",				    MIMOSA2);
	AddActor(L"MimosaMike",				L"Prototype_Actor_MimosaMike",			    MIMOSA_MIKE);
	AddActor(L"MimosaDrum",				L"Prototype_Actor_Musician_Drum",		    MUSICIAN1);
	AddActor(L"MimosaTrombone0",		L"Prototype_Actor_Musician_Trombone",	    MUSICIAN2);
	AddActor(L"MimosaTrombone1",		L"Prototype_Actor_Musician_Trombone",	    MUSICIAN3);
	AddActor(L"MimosaTrombone2",		L"Prototype_Actor_Musician_Trombone",	    MUSICIAN4);
	AddActor(L"MimosaTrombone3",		L"Prototype_Actor_Musician_Trombone",	    MUSICIAN5);
	AddActor(L"MimosaTrumpet0",			L"Prototype_Actor_Musician_Trumpet",	    MUSICIAN6);
	AddActor(L"MimosaTrumpet1",			L"Prototype_Actor_Musician_Trumpet",		MUSICIAN7);
	AddActor(L"MimosaTrumpet2",			L"Prototype_Actor_Musician_Trumpet",		MUSICIAN8);
	AddActor(L"MimosaTrumpet3",			L"Prototype_Actor_Musician_Trumpet",		MUSICIAN9);

	AddActor(L"Cat808",			        L"Prototype_Actor_808_CAT",		            CAT_808);
	AddActor(L"Ball808",			    L"Prototype_Actor_808_Ball",		        BALL_808);
	AddActor(L"Battle808",			    L"Prototype_Actor_808_Battle",		        BATTLE_808);


	AddActor(L"Platform",			    L"Prototype_Actor_Platform",		        PLATFORM);
	AddActor(L"CHAI_101",			    L"Prototype_Actor_Chai_101",		        CHAI_101);
	AddActor(L"Peppermint",			    L"Prototype_Actor_Peppermint",		        PEPPERMINT);
	AddActor(L"Macaron",			    L"Prototype_Actor_Macaron",		            MACARON);
	AddActor(L"Korsica",			    L"Prototype_Actor_Korsica",		            KORSICA);


	AddActor(L"SUIT_KALE",              L"Prototype_Actor_SuitKale",                SUIT_KALE);
	AddActor(L"KALE_TRANIT",            L"Prototype_Actor_KaleTransit",             KALE_TRANIT);
	AddActor(L"PHASE1_KALE",            L"Prototype_Actor_Kale1Phase",              PHASE1_KALE);
	AddActor(L"KALE_WEAPON",            L"Prototype_Actor_Kale_Weapon",             KALE_WEAPON);
	AddActor(L"KALE_SPECTRA",           L"Prototype_Actor_Kale_Spectra",            KALE_SPECTRA);
	AddActor(L"KALE_BLINDCLOTH",        L"Prototype_Actor_Kale_Blind",              KALE_BLINDCLOTH);
	AddActor(L"KALE_CLOAK",             L"Prototype_Actor_Kale_Cloak",              KALE_CLOAK);
	AddActor(L"KALE_BARRIER_ARM",       L"Prototype_Actor_Kale_BarrierArm",         KALE_BARRIER_ARM);
	AddActor(L"KALE_FIRE_ARM",          L"Prototype_Actor_Kale_FireArm",            KALE_FIRE_ARM);
	AddActor(L"KALE_SHELD_ARM",         L"Prototype_Actor_Kale_ShieldArm",          KALE_SHELD_ARM1);
	AddActor(L"KALE_SHELD_ARM2",        L"Prototype_Actor_Kale_ShieldArm",          KALE_SHELD_ARM2);
	AddActor(L"KALE_BEFORE_ELECTRIC",   L"Prototype_Actor_Kale_BeforeElectric",     KALE_BEFORE_ELECTRIC);
	AddActor(L"KALE_AFTER_ELECTRIC",    L"Prototype_Actor_Kale_AfterElectric",      KALE_AFTER_ELECTRIC);
	AddActor(L"KALE_LAST",              L"Prototype_Actor_Kale_Last",               KALE_LAST);
	AddActor(L"PEPP_WEAPON_L",          L"Prototype_Actor_PeppWeapon_L",				PEPP_WEAPON_L);
	AddActor(L"PEPP_WEAPON_R",          L"Prototype_Actor_PeppWeapon_R",				PEPP_WEAPON_R);
	AddActor(L"PEPP_WEAPON_SHOTGUN",    L"Prototype_Actor_PeppWeapon_Shotgun", PEPP_WEAPON_SHOTGUN);
	AddActor(L"KOR_WEAPON_L",           L"Prototype_Actor_Korsica_Weapon", KOR_WEAPON_L);
	AddActor(L"KOR_WEAPON_R",           L"Prototype_Actor_Korsica_Weapon", KOR_WEAPON_R);
	          

    //AddActor(L"CAT808" ,                L"Prototype_Actor")
	//timer
	m_upSeqTimer = m_pGameInstance->CreateTimerWithDuration(0.2f, false, [=]() {
		if (m_iActIdx < m_desc.m_iNumSequence - 1 && !m_bSpecificSeq) {
			m_iActIdx++;
			SetAct();
		}
		else {
			m_iActIdx = -1;
			for (auto _pActor : m_vecActors) {
				_pActor->SetAct(-1);
			}

			

			auto _lstCameras = m_pGameInstance->Get_Objects(
				m_pGameInstance->GetLevelDesc().nextLevelTag,
				L"Layer_Camera"
			);

			SetExplosion(false);
			for (auto pCam : _lstCameras)
			{
				if (auto pCamera = dynamic_cast<CCamera*>(pCam)) {
					pCamera->SetFOV(XM_PI/3.f);
					break;
				}
			}

			m_pCameraHandler->SetHandle(false);
			m_pGameInstance->SetBPM((Engine::_float)m_iGamePlayBPM);
			CRhythmLift::SetConstantHeiht(false);
			IsCutScene = false;
			 
			if (m_desc.m_iSeqIdx == 40)
			{
				ChangeLevel_Kale();
			}


		}
	}, false);

	//Editor
	CGameObject* pUI = nullptr;
	if (m_pGameInstance->GetCurrentLevelTag() == L"Level_Editor") {
		CGameObject::LoadPrototype(
			L"UI_UIText_Subtitle_SpeakerName",
			L"Level_Editor",
			&pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(
			L"UI_Illust_CutScene",
			L"Level_Editor",
			&pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(
			L"UI_ScreenTransition",
			L"Level_Editor",
			&pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(
			L"UI_ScreenTransition",
			L"Level_Editor",
			&pUI);
		if (pUI) pUI->SetLevelObject();


		CGameObject::LoadPrototype(L"UI_UITalkbox_Talkbox", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(L"UI_UITalkbox_Talk_Korsica", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(L"UI_UITalkbox_Talk_Kale", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(L"UI_UITalkbox_Talk_Macaron", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(L"UI_UITalkbox_Talk_Mimosa", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(L"UI_UITalkbox_Talk_Peppermint", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(L"UI_UITalkbox_Talk_Chai", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();

		CGameObject::LoadPrototype(L"UI_CutRhythmGame_Note_phase1", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(L"UI_CutRhythmGame_Note_phase2", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(L"UI_CutRhythmGame_QTE", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();
		CGameObject::LoadPrototype(L"UI_RC_KORSICA_CS", L"Level_Editor", &pUI);
		if (pUI) pUI->SetLevelObject();

		m_pUCSubtitle = CUC_Message::Create(nullptr, L"Level_Editor", L"UC_Message");
		m_pUCSubtitle->Late_Initialize();		
		m_pUCTalkbox = CUC_Talkbox::Create(nullptr, L"Level_Editor", L"UC_Message");
		m_pUCTalkbox->Late_Initialize();
		//m_pUCTalkbox = CUC_Talkbox::Create(nullptr, L"Level_Editor", L"UC_Message");
		//m_pUCTalkbox->Late_Initialize();
		m_pUCRhythmGame = CUC_CutRhythmGame::Create(nullptr, L"Level_Editor", L"UC_CutRhythmGame");
		m_pUCRhythmGame->Late_Initialize();
	}

	CGameObject::LoadPrototype(
		L"VFX_CH1000_SpeedLine",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pSpeedLines[0])
	);
	CGameObject::LoadPrototype(
		L"VFX_CH2000_SpeedLine",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pSpeedLines[1])
	);
	CGameObject::LoadPrototype(
		L"VFX_CH4000_SpeedLine",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pSpeedLines[2])
	);
	CGameObject::LoadPrototype(
		L"VFX_Player_SpeedLine",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pSpeedLines[3])
	);
	for (size_t i = 0; i < 4; i++)
	{
		m_pSpeedLines[i]->Stop();
	}
	
	return S_OK;
}

HRESULT CCutScene::Late_Initialize()
{
	__super::Late_Initialize();

	for (auto pChild : GetChildrens()) {
		if (auto pObject = dynamic_cast<CCameraHandler*>(pChild))
			m_pCameraHandler = pObject;
	}
	if (m_pCameraHandler) 
		m_pCameraHandler->SetTarget(this, 0);

	for (auto _pActor : m_vecActors) {
		if (!_pActor) continue;
		_pActor->Get_MainTransform().lock()->Set_Parent(this, true);
		_pActor->SetLevelObject();
	}
		
	if (auto pController = m_pGameInstance->GetUC(L"UC_Message")) {
		m_pUCSubtitle	= static_cast<CUC_Message*>(pController);
	}
	if (auto pController = m_pGameInstance->GetUC(L"UC_Talkbox")) {
		m_pUCTalkbox = static_cast<CUC_Talkbox*>(pController);
	}
	if (auto pController = m_pGameInstance->GetUC(L"UC_CutRhythmGame")) {
		m_pUCRhythmGame = static_cast<CUC_CutRhythmGame*>(pController);
	}

	LoadSubtitle	(L"../Cutscene/" + GetName() + L".json");
	LoadIllust		(L"../Cutscene/" + GetName() + L".json");
	LoadTransition	(L"../Cutscene/" + GetName() + L".json");
	LoadSpolights	(L"../Cutscene/" + GetName() + L".json");
	LoadVFXs		(L"../Cutscene/" + GetName() + L".json");
	LoadVOXs		(L"../Cutscene/" + GetName() + L".json");
	LoadTalks		(L"../Cutscene/" + GetName() + L".json");
	LoadRG			(L"../Cutscene/" + GetName() + L".json");
	LoadAttach		(L"../Cutscene/" + GetName() + L".json");
	LoadBoneScale   (L"../Cutscene/" + GetName() + L".json");
	LoadSpeedLines	(L"../Cutscene/" + GetName() + L".json");
	
	m_iGamePlayBPM = (Engine::_int)m_pGameInstance->GetBPM();


	m_vecActors[BATTLE_808]->SetPos(XMVectorSet(0.f, -0.3f, 0.1f, 1.f));

	m_upTrashTimer = { m_pGameInstance->CreateTimerWithDuration(1.f, false, [&]() {m_bTrash = { false }; m_fTrash = { 0.f }; }, false) };

	FCSFUNCDESC _desc;
	if (m_desc.m_iSeqIdx == 1225) {
		_desc.iActIdx	= 10;
		_desc.fTime		= 1.4f;
		_desc.pFunc		= [=]() { ChangeOrigin(); };
		AddCSFunc(_desc);
	}

	if (m_desc.m_iSeqIdx == 40) {
		_desc.iActIdx	= 7;
		_desc.fTime		= 2.2f;
		_desc.pFunc		= [=]() { ChangeTrash(); };
		AddCSFunc(_desc);
	}

	return S_OK;
}

void CCutScene::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (m_vOriginBlue.z > 0.f)
		m_vOriginBlue.z -= fTimeDelta * 4.f;
	if (m_vOriginBlue.z < 0.f)
		m_vOriginBlue.z = 0.f;
	if (m_vOriginBlue.y > 0.f)
		m_vOriginBlue.y -= fTimeDelta * 4.f;
	if (m_vOriginBlue.y < 0.f)
		m_vOriginBlue.y = 0.f;

	if (m_bTrash)
	{
		Engine::_float fTime = { (Engine::_float)m_upTrashTimer->GetElapsedRatio() * 5.f };
		m_fTrash = { (1.f - fTime + floor(fTime)) * 0.3f };
		m_vGuitarSky = { 0.255f * m_fTrash * 5.f, m_fTrash * 5.f, m_fTrash * 5.f };
	}
}

void CCutScene::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (auto spModel = ConvertWPComponent<CModel>(m_vecActors[PLAYER]->Get_Component<CModel>()).lock()) {
		spModel->GetMeshRenderer(19)->SetActive(false);
		spModel->GetMeshRenderer(20)->SetActive(false);
		spModel->GetMeshRenderer(21)->SetActive(false);
	}
	if (auto spModel = ConvertWPComponent<CModel>(m_vecActors[CHAI_101]->Get_Component<CModel>()).lock()) {
		spModel->GetMeshRenderer(17)->SetActive(false);
		spModel->GetMeshRenderer(27)->SetActive(false);
		spModel->GetMeshRenderer(28)->SetActive(false);
	}

	if (auto spModel = ConvertWPComponent<CModel>(m_vecActors[TRASHGUITAR]->Get_Component<CModel>()).lock()) {
		spModel->GetMeshRenderer(3)->SetActive(false);
	}

	if (auto pActor = m_vecActors[KOR_WEAPON_R]) {
		if (auto pAnimCOntroller = pActor->GetAnimController()) {
			pAnimCOntroller->SetLayerIdx("ACTORIDX", 1);
		}
	}
	for (_uint i = BACKDANCER1; i <= BACKDANCER6; i++)
	{
		if (auto pActor = m_vecActors[i]) {
			if (auto pAnimCOntroller = pActor->GetAnimController()) {
				pAnimCOntroller->SetLayerIdx("DANCERIDX", i);
			}
		}
	}
	for (_uint i = ANTENA1; i <= ANTENA8; i++)
	{
		if (auto pActor = m_vecActors[i]) {
			if (auto pAnimCOntroller = pActor->GetAnimController()) {
				pAnimCOntroller->SetLayerIdx("ACTORIDX", i - ANTENA1);
			}
		}
	}
	for (_uint i = KALE_SHELD_ARM1; i <= KALE_SHELD_ARM2; i++)
	{
		if (auto pActor = m_vecActors[i]) {
			if (auto pAnimCOntroller = pActor->GetAnimController()) {
				pAnimCOntroller->SetLayerIdx("ACTORIDX", i - KALE_SHELD_ARM1);
			}
		}
	}


	for (_uint i = 0; i <= EACTOREND; i++)
	{
		if (auto pActor = m_vecActors[i]) {
			if (in(i, { MIMOSA }))
				pActor->SetSpotlight(CHASING_TYPE::BOSS);
			if (in(i, { BACKDANCER1, BACKDANCER2, BACKDANCER3, BACKDANCER4, BACKDANCER5, BACKDANCER6 }))
				pActor->SetSpotlight(CHASING_TYPE::ENEMY);
			if (in(i, { PLAYER }))
				pActor->SetSpotlight(CHASING_TYPE::PLAYER);
		}
	}

	for (_uint i = MUSICIAN1; i <= MUSICIAN9; i++)
	{
		if (auto pActor = m_vecActors[i]) {
			if (auto pAnimCOntroller = pActor->GetAnimController()) {
				pAnimCOntroller->SetLayerIdx("MUSICIANIDX", i - MUSICIAN1);
			}
		}
	}

	if (m_iActIdx == -1) {
		for (size_t i = 0; i < EACTOREND; i++)
		{
			if (m_vecActors[i] && m_vecActors[i]->GetSeq() == m_desc.m_iSeqIdx)
				m_vecActors[i]->SetRender(false);
		}
	}
	else {
		if (m_iCurSubtitleIdx < m_vecSubtitles.size()) {
			if (m_vecSubtitles[m_iCurSubtitleIdx].iActIdx <= m_iActIdx) {
				if (m_vecSubtitles[m_iCurSubtitleIdx].fTime <= m_upSeqTimer->GetElapsedTime()) {
					if (m_pUCSubtitle) {
						m_pUCSubtitle->Subtitle(
							m_vecSubtitles[m_iCurSubtitleIdx].strSpeaker,
							m_vecSubtitles[m_iCurSubtitleIdx].strDialogue,
							m_vecSubtitles[m_iCurSubtitleIdx].fDuration
						);
					}
					m_iCurSubtitleIdx++;
				}
			}
		}
		if (m_iCurIllustIdx < m_vecIllusts.size()) {
			if (m_vecIllusts[m_iCurIllustIdx].iActIdx <= m_iActIdx) {
				if (m_vecIllusts[m_iCurIllustIdx].fTime <= m_upSeqTimer->GetElapsedTime()) {
					if (m_pUCSubtitle) {
						m_pUCSubtitle->IllustImage(
							m_vecIllusts[m_iCurIllustIdx].iIllustIdx,
							m_vecIllusts[m_iCurIllustIdx].fDuration
						);
					}
					if (m_vecIllusts[m_iCurIllustIdx].iActIdx == 9
						&& m_vecIllusts[m_iCurIllustIdx].iIllustIdx == 3)
					{
						ChangeKale();
					}
					m_iCurIllustIdx++;
				}
			}
		}
		if (m_iCurTransitionIdx < m_vecTransitions.size()) {
			while (m_iCurTransitionIdx < m_vecTransitions.size() && m_vecTransitions[m_iCurTransitionIdx].iActIdx <= m_iActIdx && m_vecTransitions[m_iCurTransitionIdx].fTime <= m_upSeqTimer->GetElapsedTime()) {
				if (m_pUCSubtitle) {
					m_pUCSubtitle->ScreenTransition(
						m_vecTransitions[m_iCurTransitionIdx].iIllustIdx,
						m_vecTransitions[m_iCurTransitionIdx].fDuration
					);
				}
				m_iCurTransitionIdx++;
			}
		}

		if (m_iCurSpotlightIdx < m_vecSpotlights.size()) {
			while (m_iCurSpotlightIdx < m_vecSpotlights.size() && m_vecSpotlights[m_iCurSpotlightIdx].iActIdx <= m_iActIdx && m_vecSpotlights[m_iCurSpotlightIdx].fTime <= m_upSeqTimer->GetElapsedTime()) {
				if (m_vecSpotlights[m_iCurSpotlightIdx].bStart) {
					if (auto pActor = m_vecActors[m_vecSpotlights[m_iCurSpotlightIdx].iActorIdx])
					{
						pActor->ActiveSpotlight(true);
						m_pGameInstance->Play_SFX(L"enm_em5000_se_atk_dark_light_on_01_Play", 1.f);
					}
						
				}
				else {
					if (auto pActor = m_vecActors[m_vecSpotlights[m_iCurSpotlightIdx].iActorIdx])
						pActor->ActiveSpotlight(false);
				}
				m_iCurSpotlightIdx++;
			}
		}

		if (m_iCurVFXIdx < m_vecVFXs.size()) {
			while (
				m_iCurVFXIdx < m_vecVFXs.size()
				&& m_vecVFXs[m_iCurVFXIdx].iActIdx <= m_iActIdx
				&& m_vecVFXs[m_iCurVFXIdx].fTime <= m_upSeqTimer->GetElapsedTime())
			{
				PlayVFX(m_vecVFXs[m_iCurVFXIdx]);
				m_iCurVFXIdx++;
			}
		}

		if (m_iCurVOXIdx < m_vecVOXs.size()) {
			while (
				m_iCurVOXIdx < m_vecVOXs.size()
				&& m_vecVOXs[m_iCurVOXIdx].iActIdx <= m_iActIdx
				&& m_vecVOXs[m_iCurVOXIdx].fTime <= m_upSeqTimer->GetElapsedTime())
			{
				m_pGameInstance->Play_VOX(s2ws(m_vecVOXs[m_iCurVOXIdx].sVOXTag), m_vecVOXs[m_iCurVOXIdx].fVolume);
				m_iCurVOXIdx++;
			}
		}

		if (m_iCurTalkIdx < m_vecTalks.size()) {
			while (
				m_iCurTalkIdx < m_vecTalks.size()
				&& m_vecTalks[m_iCurTalkIdx].iActIdx <= m_iActIdx
				&& m_vecTalks[m_iCurTalkIdx].fTime <= m_upSeqTimer->GetElapsedTime())
			{
				m_pUCTalkbox->Talkbox(
					CUC_Talkbox::SPEAKER(m_vecTalks[m_iCurTalkIdx].iSpeakerIdx),
					CUC_Talkbox::EMOTION(m_vecTalks[m_iCurTalkIdx].iEmotion),
					m_vecTalks[m_iCurTalkIdx].strDialogue,
					m_vecTalks[m_iCurTalkIdx].fDuration);
				m_iCurTalkIdx++;
			}
		}

		if (m_iCurRGIdx < m_vecRGs.size()) {
			while (
				m_iCurRGIdx < m_vecRGs.size()
				&& m_vecRGs[m_iCurRGIdx].iActIdx <= m_iActIdx
				&& m_vecRGs[m_iCurRGIdx].fTime <= m_upSeqTimer->GetElapsedTime())
			{
				m_pUCRhythmGame->RhythmGameStart(m_vecRGs[m_iCurRGIdx].iRGIdx, m_vecRGs[m_iCurRGIdx].fDuration);
				
				m_iCurRGIdx++;
			}
		}

		if (m_iCurBoneScaleIdx < m_vecBoneScale.size()) {
			while (
				m_iCurBoneScaleIdx < m_vecBoneScale.size()
				&& m_vecBoneScale[m_iCurBoneScaleIdx].iActIdx <= m_iActIdx
				&& m_vecBoneScale[m_iCurBoneScaleIdx].fTime <= m_upSeqTimer->GetElapsedTime())
			{
				auto pBoneDesc = m_vecBoneScale[m_iCurBoneScaleIdx];

				m_vecActors[pBoneDesc.iTargetActorIdx]->Get_Model().lock()
					->FindBoneWithName(pBoneDesc.strTargetActorBoneName)
					->SetRemove(pBoneDesc.isRemove);

				m_iCurBoneScaleIdx++;
			}
		}

		if (m_iCSFuncIdx < m_vecCSFuncs.size()) {
			while (
				m_iCSFuncIdx < m_vecCSFuncs.size()
				&& m_vecCSFuncs[m_iCSFuncIdx].iActIdx <= m_iActIdx
				&& m_vecCSFuncs[m_iCSFuncIdx].fTime <= m_upSeqTimer->GetElapsedTime())
			{
				auto pBoneDesc = m_vecCSFuncs[m_iCSFuncIdx];
				if (pBoneDesc.pFunc) pBoneDesc.pFunc();
				m_iCSFuncIdx++;
			}
		}

		if (m_iSLDescIdx < m_vecSLDescs.size()) {
			while (
				m_iSLDescIdx < m_vecSLDescs.size()
				&& m_vecSLDescs[m_iSLDescIdx].iActIdx <= m_iActIdx
				&& m_vecSLDescs[m_iSLDescIdx].fTime <= m_upSeqTimer->GetElapsedTime())
			{
				if (m_vecSLDescs[m_iSLDescIdx].bIsStart) {
					m_pSpeedLines[m_vecSLDescs[m_iSLDescIdx].iSLIdx]->Restart();
				}
				else {
					m_pSpeedLines[m_vecSLDescs[m_iSLDescIdx].iSLIdx]->Stop();
				}
				m_iSLDescIdx++;
			}
		}

		if (m_desc.m_iSeqIdx == 1225 && m_iActIdx == 16 && 2.9 <= m_upSeqTimer->GetElapsedTime() && !m_bIsExploded) {
			SetExplosion(true);
			m_bIsExploded = true;
		}
	}

	if (m_pGameInstance->GetCurrentLevelTag() == L"Level_Editor")
	{
		m_pUCSubtitle->Update(fTimeDelta);
		m_pUCRhythmGame->Update(fTimeDelta);
	}

	UpdateAttach();
	auto _vPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
	auto _vDir = XMLoadFloat4(m_pGameInstance->Get_CamLook());

	for (size_t i = 0; i < 4; i++)
	{
		m_pSpeedLines[i]->Set_Pos(_vPos);
		m_pSpeedLines[i]->Set_Dir(_vDir);
	}
}

void CCutScene::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (!m_bReadyInit) {
		m_bReadyInit = true;
		ReadyEmissive();
	}
}


HRESULT CCutScene::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents())

	return S_OK;
}

void CCutScene::AddActor(wstring sActorTag, wstring sPrototypeTag, EACTOR eActorType)
{
	auto pObject = m_pGameInstance->Get_Object(
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		L"Layer_CutScene_Actor",
		sActorTag
	);
	if (pObject) {
		m_vecActors[eActorType] = static_cast<Actor*>(pObject);
	    //pObject->SetLevelObject();
	}
	else {
		CGameObject::LoadPrototype(
			sPrototypeTag,
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_vecActors[eActorType])
		);
		if (m_vecActors[eActorType]) m_vecActors[eActorType]->ChangeName(sActorTag);
		//m_vecActors[eActorType]->SetLevelObject();
	}
}

void CCutScene::Render_Properties()
{
	__super::Render_Properties();

	static  _bool Pause;

	if (m_pGameInstance->IsKeyState(DIK_SPACE, EKEYACTIONSTATE::ENTER)) 
	{
		Pause = !Pause;
		if (Pause)
			Stop();
		else
			Restart();
	}
	

	ImGui::PushID("CS");
	if (ImGui::Button("RefreshJson")) {
		LoadSubtitle	(L"../Cutscene/" + GetName() + L".json");
		LoadIllust		(L"../Cutscene/" + GetName() + L".json");
		LoadTransition	(L"../Cutscene/" + GetName() + L".json");
		LoadSpolights	(L"../Cutscene/" + GetName() + L".json");
		LoadVFXs		(L"../Cutscene/" + GetName() + L".json");
		LoadVOXs		(L"../Cutscene/" + GetName() + L".json");
		LoadTalks		(L"../Cutscene/" + GetName() + L".json");
		LoadRG			(L"../Cutscene/" + GetName() + L".json");
		LoadAttach      (L"../Cutscene/" + GetName() + L".json");
		LoadBoneScale   (L"../Cutscene/" + GetName() + L".json");
		LoadSpeedLines	(L"../Cutscene/" + GetName() + L".json");
	}

	ImGui::DragInt("Seq", &m_desc.m_iSeqIdx);
	ImGui::DragInt("Act", &m_iActIdx);
	_float fElapsedTime = 0.f;
	if (m_upSeqTimer->IsActive()) {
		fElapsedTime = (Engine::_float)m_upSeqTimer->GetElapsedTime();
	}
	ImGui::Text(("ElapsedTime : " + to_string(fElapsedTime)).c_str());


	ImGui::Separator();

	if (ImGui::Button("Add")) {
		if (m_desc.m_iNumSequence <= MAXCUTSCENE - 1) {
			m_desc.m_seqInstances[m_desc.m_iNumSequence++].fDuration = 0.f;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Play"))
		Play();

	if (ImGui::Button("Stop"))
		Stop();
	ImGui::SameLine();
	if (ImGui::Button("Restart"))
		Restart();
	ImGui::SameLine();
	if (ImGui::Button("Add KeyFrame"))
		m_pCameraHandler->AddCSFrame(m_iActIdx);

	if (ImGui::DragFloat("FOV", &m_pCameraHandler->m_fCurFOV, 1.f, 20, 120)) {
		m_pCameraHandler->m_pCameraObject->SetFOV(XMConvertToRadians(m_pCameraHandler->m_fCurFOV));
	}

	int iDeleteSeqIdx = -1;
	for (size_t i = 0; i < m_desc.m_iNumSequence; i++)
	{
		ImGui::PushID((Engine::_int)i);
		string sTag = "Act " + to_string(i) + " : ";
		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat(sTag.c_str(), &m_desc.m_seqInstances[i].fDuration, 0.f, 0.f, 1000.f);
		ImGui::SameLine();
		if (ImGui::Button("Play Act")) {
			Play((Engine::_int)i);
		}
		ImGui::PopItemWidth();

		for (unsigned long long j = 0; j < EACTOREND; j++)
		{
			ImGui::PushID((Engine::_int)j);
			bool bIsCheck = m_desc.m_seqInstances[i].iActorMask & (1ull << j);
			if (ImGui::Checkbox("", &bIsCheck)) {
				m_desc.m_seqInstances[i].iActorMask ^= (1ull << j);
			}
			if (j < EACTOREND - 1 && !in(j, { MIMOSA_MIKE, MUSICIAN9, BATTLE_808, PLATFORM ,KORSICA ,KALE_LAST }))
				ImGui::SameLine();
			ImGui::PopID();
		}
		if (ImGui::Button("Delete")) {
			iDeleteSeqIdx = (Engine::_int)i;
		}
		ImGui::PopID();
	}
	if (iDeleteSeqIdx != -1) {
		m_desc.m_iNumSequence--;
		for (_int i = iDeleteSeqIdx; i < MAXCUTSCENE-1; i++)
			m_desc.m_seqInstances[i] = m_desc.m_seqInstances[i + 1];
	}

	ImGui::PopID();

	static ImGuiTextFilter filter;
	auto _vecSoundNames = m_pGameInstance->Get_SoundNames(ESOUNDTYPE::VOX);

	SP_Asset _spAsset;
	filter.Draw("Search");
	if (ImGui::BeginChild("##Sound List", ImVec2(0, 100), true))
	{
		if (ImGui::Selectable("None")) {
			filter.Clear();
			strcpy_s(filter.InputBuf, "None");
			filter.Build();
			lstrcpy(m_desc.sVOXName, L"None");
		}
		for (auto& name : _vecSoundNames)
		{
			auto _sName = ws2s(name);
			if (!filter.PassFilter(_sName.c_str()))
				continue;

			bool bSelected = (lstrcmp(m_desc.sVOXName, name.c_str()) == 0);

			if (ImGui::Selectable(_sName.c_str()))
			{
				filter.Clear();
				strcpy_s(filter.InputBuf, _sName.c_str());
				filter.Build();
				lstrcpy(m_desc.sVOXName, s2ws(_sName).c_str());
			}
		}
	}
	ImGui::EndChild();
}

void CCutScene::Play()
{
	if (m_desc.m_iNumSequence == 0) return;

	m_iSeqActorMask = 0ull;
	for (size_t i = 0; i < m_desc.m_iNumSequence; i++)
		m_iSeqActorMask |= m_desc.m_seqInstances[i].iActorMask;

	for (unsigned long long i = 0; i < EACTOREND; i++) {
		if (m_vecActors[i]) {
			m_vecActors[i]->SetActive(m_iSeqActorMask & (1ull << i));
		}
	}

	IsCutScene = true;
	m_pUCSubtitle->SubtitleEnd();
	m_bSpecificSeq = false;
	m_iActIdx = 0;
	if (wstring(m_desc.sVOXName) != L"None")
		m_pGameInstance->Play_VOX(m_desc.sVOXName);
	SetAct();
	m_iCurSubtitleIdx	= 1000;
	m_iCurIllustIdx		= 1000;
	m_iCurTransitionIdx = 0;
	m_iCurSpotlightIdx	= 0;
	m_iCurVFXIdx		= 0;
	m_iCurVOXIdx		= 0;
	m_iCurTalkIdx		= 1000;
	m_iCurRGIdx			= 1000;
	m_iCSFuncIdx		= 0;
	m_iSLDescIdx		= 1000;
	m_iCurAttachIdx		= 0;
	m_pCameraHandler->SetHandle(true);
	m_pGameInstance->SetBPM(120);
	for (auto _pActor : m_vecActors) {
		if (!_pActor) continue;
		_pActor->Get_MainTransform().lock()->Set_Parent(this, true);
	}
	CRhythmLift::SetConstantHeiht(true);
}

void CCutScene::Stop()
{
	m_pGameInstance->SetTimeScale(0.f);
	m_pCameraHandler->SetHandle(false);
	m_pCameraHandler->m_upSeqTimer->SetActive(false);
}
void CCutScene::Restart()
{
	m_pGameInstance->SetTimeScale(1.f);
	m_pCameraHandler->SetHandle(true);
	m_pCameraHandler->m_upSeqTimer->SetActive(true);
}

void CCutScene::Play(int _iActIdx)
{
	if (m_desc.m_iNumSequence < _iActIdx) return;
	IsCutScene = true;
	m_bSpecificSeq = true;
	m_iActIdx = _iActIdx;
	SetAct();
	m_iCurSubtitleIdx	= 1000;
	m_iCurIllustIdx		= 1000;
	m_iCurTransitionIdx = 0;
	m_iCurSpotlightIdx	= 0;
	m_iCurVFXIdx		= 0;
	m_iCurVOXIdx		= 0;
	m_iCurTalkIdx		= 1000;
	m_iCurRGIdx			= 1000;
	m_iCSFuncIdx		= 0;
	m_iSLDescIdx		= 1000;
	m_iCurAttachIdx		= 0;
	m_pCameraHandler->SetHandle(true);
	m_pGameInstance->SetBPM(120);


	if (m_iCurTransitionIdx < m_vecTransitions.size()) {
		while (
			m_iCurTransitionIdx < m_vecTransitions.size() && 
			m_vecTransitions[m_iCurTransitionIdx].iActIdx < _iActIdx) {
			m_iCurTransitionIdx++;
		}
	}
	if (m_iCurIllustIdx < m_vecIllusts.size()) {
		while (
			m_iCurIllustIdx < m_vecIllusts.size() &&
			m_vecIllusts[m_iCurIllustIdx].iActIdx < _iActIdx) {
			m_iCurIllustIdx++;
		}
	}
	if (m_iCurSubtitleIdx < m_vecSubtitles.size()) {
		while (
			m_iCurSubtitleIdx < m_vecSubtitles.size() &&
			m_vecSubtitles[m_iCurSubtitleIdx].iActIdx < _iActIdx) {
			m_iCurSubtitleIdx++;
		}
	}
	if (m_iCurSpotlightIdx < m_vecSpotlights.size()) {
		while (
			m_iCurSpotlightIdx < m_vecSpotlights.size() &&
			m_vecSpotlights[m_iCurSpotlightIdx].iActIdx < _iActIdx) {
			m_iCurSpotlightIdx++;
		}
	}
	if (m_iCurVFXIdx < m_vecVFXs.size()) {
		while (
			m_iCurVFXIdx < m_vecVFXs.size() &&
			m_vecVFXs[m_iCurVFXIdx].iActIdx < _iActIdx) {
			m_iCurVFXIdx++;
		}
	}
	if (m_iCurVOXIdx < m_vecVOXs.size()) {
		while (
			m_iCurVOXIdx < m_vecVOXs.size() &&
			m_vecVOXs[m_iCurVOXIdx].iActIdx < _iActIdx) {
			m_iCurVOXIdx++;
		}
	}
	if (m_iCurTalkIdx < m_vecTalks.size()) {
		while (
			m_iCurTalkIdx < m_vecTalks.size() &&
			m_vecTalks[m_iCurTalkIdx].iActIdx < _iActIdx) {
			m_iCurTalkIdx++;
		}
	}
	if (m_iCurAttachIdx < m_vecAttach.size()) {
		while (
			m_iCurAttachIdx < m_vecAttach.size() &&
			m_vecAttach[m_iCurAttachIdx].iActIdx < _iActIdx) {
			m_iCurAttachIdx++;
		}
	}
	if (m_iCurRGIdx < m_vecRGs.size()) {
		while (
			m_iCurRGIdx < m_vecTalks.size() &&
			m_vecRGs[m_iCurRGIdx].iActIdx < _iActIdx) {
			m_iCurRGIdx++;
		}
	}
}

bool CCutScene::IsEnd()
{
	return m_iActIdx == -1;
}

void CCutScene::SetAct()
{
	m_upSeqTimer->SetDuration(m_desc.m_seqInstances[m_iActIdx].fDuration);
	m_upSeqTimer->Restart();
	for (auto _pActor : m_vecActors) {
		_pActor->SetSeq(m_desc.m_iSeqIdx);
		_pActor->SetAct(m_iActIdx);
	}
	if (m_pCameraHandler) m_pCameraHandler->PlayCameraSequence(m_iActIdx, false);
	for (unsigned long long i = 0; i < EACTOREND; i++)
	{
		if (m_vecActors[i]) {
			m_vecActors[i]->SetRender(m_desc.m_seqInstances[m_iActIdx].iActorMask & (1ull << i));
		}
	}
}

void CCutScene::ChangeLevel_Kale()
{
	if (m_pGameInstance->GetCurrentLevelTag() == L"Level_Editor")
		return;

	wstring _sNxtLVLTag = L"Level_GamePlay";
	wstring _sNxtLvLStagetTag = L"Level_Gameplay_Kale";
	LVLTRANSIT_DESC _desc;
	_desc.nextLevelTag = L"Level_Loading";
	_desc.userData0 = &_sNxtLVLTag;
	_desc.userData1 = &_sNxtLvLStagetTag;


	m_pGameInstance->Load_Level(_desc);
	if (FAILED(m_pGameInstance->Change_Level()))
		return;

}

void CCutScene::SetWeapon(const FATTACHDESC& desc)
{
	if (desc.iTargetActorIdx < 0 || desc.iTargetActorIdx >= EACTOREND)
		return;

	if (desc.iWeaponActorIdx < 0 || desc.iWeaponActorIdx >= EACTOREND)
		return;

	Actor* pTargetActor = m_vecActors[desc.iTargetActorIdx];
	Actor* pWeaponActor = m_vecActors[desc.iWeaponActorIdx];

	if (!pTargetActor || !pWeaponActor)
		return;

	auto spTargetModel = ConvertWPComponent<CModel>(
		pTargetActor->Get_Component<CModel>().lock()
	).lock();

	auto spWeaponModel = ConvertWPComponent<CModel>(
		pWeaponActor->Get_Component<CModel>().lock()
	).lock();

	if (!spTargetModel || !spWeaponModel)
		return;

	auto pTargetBone = spTargetModel->FindBoneWithName(desc.strTargetActorBoneName);
	auto pWeaponBone = spWeaponModel->FindBoneWithName(desc.strWeaponActorBoneName);

	if (!pTargetBone || !pWeaponBone)
		return;

	auto targetBoneDesc = pTargetBone->GetDesc();
	auto weaponBoneDesc = pWeaponBone->GetDesc();

	_vector vTargetBonePos = XMLoadFloat4(
		reinterpret_cast<_float4*>(targetBoneDesc.m_CombinedTransformationMatrix.m[3])
	);

	_vector vWeaponBonePos = XMLoadFloat4(
		reinterpret_cast<_float4*>(weaponBoneDesc.m_CombinedTransformationMatrix.m[3])
	);

	if (auto spWeaponTransform = pWeaponActor->Get_MainTransform().lock())
	{
		spWeaponTransform->Translate(vTargetBonePos - vWeaponBonePos, false);
	}
}

void CCutScene::Free()
{
	__super::Free();
	if (m_pGameInstance->GetCurrentLevelTag() == L"Level_Editor") {
		Safe_Release(m_pUCSubtitle);
	}
}

CGameObject* CCutScene::Clone(void* pArg)
{
	return CloneBase<CCutScene>(pArg);
}

HRESULT CCutScene::Save(void* _pDesc, _uint& _iSize) const
{
	static_assert(is_trivially_copyable_v<FCUTSCENEDESC>);
	_iSize = sizeof(FCUTSCENEDESC);
	memcpy(_pDesc, &m_desc, _iSize);

	return S_OK;
}

HRESULT CCutScene::Load(void* _pDesc)
{
	static_assert(is_trivially_copyable_v<FCUTSCENEDESC>);
	memcpy(&m_desc, _pDesc, sizeof(FCUTSCENEDESC));

	return S_OK;
}

void CCutScene::LoadSubtitle(wstring sSubtitleJsonTag)
{
	m_vecSubtitles.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& subtitles = _root["Subtitles"];

	if (!subtitles.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)subtitles.size(); ++i)
	{
		const Json::Value& data = subtitles[i];
		FSUBTITLEDESC desc{};

		desc.iActIdx		= data["ACT"].asInt();
		desc.fTime			= data["TIME"].asFloat();
		desc.fDuration		= data["DURATION"].asFloat();
		string strSpeaker	= data["SPEAKERTAG"].asString();
		string strDialogue	= data["DIALOGUE"].asString();

		desc.strSpeaker		= s2ws(strSpeaker);
		desc.strDialogue	= s2ws(strDialogue);

		m_vecSubtitles.push_back(desc);
	}


	sort(
		m_vecSubtitles.begin(),
		m_vecSubtitles.end(),
		[=](FSUBTITLEDESC _desc0, FSUBTITLEDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadTalks(wstring sSubtitleJsonTag)
{
	m_vecTalks.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& subtitles = _root["Talks"];

	if (!subtitles.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)subtitles.size(); ++i)
	{
		const Json::Value& data = subtitles[i];
		FTALKBOXDESC desc{};

		desc.iActIdx = data["ACT"].asInt();
		desc.fTime = data["TIME"].asFloat();
		desc.fDuration = data["DURATION"].asFloat();
		desc.iSpeakerIdx = data["SPEAKERIDX"].asInt();
		desc.iEmotion = data["EMOTION"].asInt();
		string strDialogue = data["DIALOGUE"].asString();

		desc.strDialogue = s2ws(strDialogue);

		m_vecTalks.push_back(desc);
	}


	sort(
		m_vecTalks.begin(),
		m_vecTalks.end(),
		[=](FTALKBOXDESC _desc0, FTALKBOXDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadIllust(wstring sSubtitleJsonTag)
{
	m_vecIllusts.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& illustrations = _root["Illustrations"];

	if (!illustrations.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)illustrations.size(); ++i)
	{
		const Json::Value& data = illustrations[i];
		FILLUSTEDESC desc{};

		desc.iActIdx		= data["ACT"].asInt();
		desc.fTime			= data["TIME"].asFloat();
		desc.fDuration		= data["DURATION"].asFloat();
		desc.iIllustIdx		= data["ILLUSTIDX"].asInt();

		m_vecIllusts.push_back(desc);
	}


	sort(
		m_vecIllusts.begin(),
		m_vecIllusts.end(),
		[=](FILLUSTEDESC _desc0, FILLUSTEDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadTransition(wstring sSubtitleJsonTag)
{
	m_vecTransitions.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& illustrations = _root["SceneTransition"];

	if (!illustrations.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)illustrations.size(); ++i)
	{
		const Json::Value& data = illustrations[i];
		FILLUSTEDESC desc{};

		desc.iActIdx = data["ACT"].asInt();
		desc.fTime = data["TIME"].asFloat();
		desc.fDuration = data["DURATION"].asFloat();
		desc.iIllustIdx = data["ILLUSTIDX"].asInt();

		m_vecTransitions.push_back(desc);
	}


	sort(
		m_vecTransitions.begin(),
		m_vecTransitions.end(),
		[=](FILLUSTEDESC _desc0, FILLUSTEDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadSpolights(wstring sSubtitleJsonTag)
{
	m_vecSpotlights.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& illustrations = _root["SPOTLIGHTS"];

	if (!illustrations.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)illustrations.size(); ++i)
	{
		const Json::Value& data = illustrations[i];
		FSPOTLIGHTDESC desc{};

		desc.iActIdx	= data["ACT"].asInt();
		desc.fTime		= data["TIME"].asFloat();
		desc.iActorIdx	= data["ACTOR"].asInt();
		desc.bStart		= data["ISSTART"].asBool();

		m_vecSpotlights.push_back(desc);
	}


	sort(
		m_vecSpotlights.begin(),
		m_vecSpotlights.end(),
		[=](FSPOTLIGHTDESC _desc0, FSPOTLIGHTDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadVFXs(wstring sSubtitleJsonTag)
{
	m_vecVFXs.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& illustrations = _root["VFXs"];

	if (!illustrations.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)illustrations.size(); ++i)
	{
		const Json::Value& data = illustrations[i];
		FVFXDESC desc{};

		desc.iActIdx	= data["ACT"].asInt();
		desc.fTime		= data["TIME"].asFloat();
		desc.iActorIdx	= data["ACTOR"].asInt();
		desc.sVFXTag	= data["VFX"].asString();
		desc.sBoneTag	= data["BONE"].asString();
		if (data.isMember("DIRLOCK"))
			desc.bDirLock	= data["DIRLOCK"].asBool();
		if (data.isMember("STOP"))
			desc.bStop		= data["STOP"].asBool();
		if (data.isMember("NonBone"))
			desc.bNonBone	= data["NonBone"].asBool();


		if (EACTOREND <= desc.iActorIdx)
			continue;
		/*if (m_vecActors[desc.iActorIdx])
			m_vecActors[desc.iActorIdx]->SetVFX(&desc);*/

		m_vecVFXs.push_back(desc);
	}


	sort(
		m_vecVFXs.begin(),
		m_vecVFXs.end(),
		[=](FVFXDESC _desc0, FVFXDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadVOXs(wstring sSubtitleJsonTag)
{
	m_vecVOXs.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& illustrations = _root["VOXs"];

	if (!illustrations.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)illustrations.size(); ++i)
	{
		const Json::Value& data = illustrations[i];
		FVOXDESC desc{};

		desc.iActIdx	= data["ACT"].asInt();
		desc.fTime		= data["TIME"].asFloat();
		desc.sVOXTag	= data["VOXTAG"].asString();
		if (data.isMember("Volume"))
			desc.fVolume = data["Volume"].asFloat();

		m_vecVOXs.push_back(desc);
	}


	sort(
		m_vecVOXs.begin(),
		m_vecVOXs.end(),
		[=](FVOXDESC _desc0, FVOXDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadAttach(wstring sSubtitleJsonTag)
{
	m_vecAttach.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& attachs = _root["Attachs"];

	if (!attachs.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)attachs.size(); ++i)
	{
		const Json::Value& data = attachs[i];
		FATTACHDESC desc{};

		desc.iActIdx                = data["ACT"].asInt();
		desc.iWeaponActorIdx        = data["WEAPON_ACT"].asInt();
		desc.fTime                  = data["TIME"].asFloat();
		desc.fDuration              = data["DURATION"].asFloat();
		desc.iTargetActorIdx        = data["TARGET_IDX"].asInt();

		desc.strTargetActorBoneName = s2ws(data["TARGET_BONENAME"].asString());
		desc.strWeaponActorBoneName = s2ws(data["WEAPON_BONENAME"].asString());

		m_vecAttach.push_back(desc);
	}


	sort(
		m_vecAttach.begin(),
		m_vecAttach.end(),
		[=](FATTACHDESC _desc0, FATTACHDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadBoneScale(wstring sSubtitleJsonTag)
{
	m_vecBoneScale.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& attachs = _root["BoneScales"];

	if (!attachs.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)attachs.size(); ++i)
	{
		const Json::Value& data = attachs[i];
		FBONESCALEDESC desc{};

		desc.iActIdx                = data["ACT"].asInt();
		desc.fTime                  = data["TIME"].asFloat();
		desc.iTargetActorIdx        = data["ACTOR"].asInt();
		desc.isRemove               = data["IsRemove"].asBool();
		desc.strTargetActorBoneName = s2ws(data["TARGET_BONENAME"].asString());

		m_vecBoneScale.push_back(desc);
	}


	sort(
		m_vecBoneScale.begin(),
		m_vecBoneScale.end(),
		[=](FBONESCALEDESC _desc0, FBONESCALEDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadRG(wstring sSubtitleJsonTag)
{
	m_vecRGs.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& attachs = _root["RG"];

	if (!attachs.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)attachs.size(); ++i)
	{
		const Json::Value& data = attachs[i];
		FRGDESC desc{};

		desc.iActIdx	= data["ACT"].asInt();
		desc.fTime		= data["TIME"].asFloat();
		desc.fDuration	= data["DURATION"].asFloat();
		desc.iRGIdx		= data["TYPE"].asInt();

		m_vecRGs.push_back(desc);
	}


	sort(
		m_vecRGs.begin(),
		m_vecRGs.end(),
		[=](FRGDESC _desc0, FRGDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::LoadSpeedLines(wstring sSubtitleJsonTag)
{
	m_vecSLDescs.clear();

	Json::Value _root;
	m_pGameInstance->LoadJson(sSubtitleJsonTag, _root);

	const Json::Value& illustrations = _root["SL"];

	if (!illustrations.isArray())
		return;

	for (_int i = 0; i < (Engine::_int)illustrations.size(); ++i)
	{
		const Json::Value& data = illustrations[i];
		FSPEEDLINEDESC desc{};

		desc.iActIdx	= data["ACT"].asInt();
		desc.fTime		= data["TIME"].asFloat();
		desc.iSLIdx		= data["TYPE"].asInt();
		desc.bIsStart	= data["ISSTART"].asBool();

		m_vecSLDescs.push_back(desc);
	}


	sort(
		m_vecSLDescs.begin(),
		m_vecSLDescs.end(),
		[=](FSPEEDLINEDESC _desc0, FSPEEDLINEDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::AddCSFunc(FCSFUNCDESC _desc)
{
	m_vecCSFuncs.push_back(_desc);
	sort(
		m_vecCSFuncs.begin(),
		m_vecCSFuncs.end(),
		[=](FCSFUNCDESC _desc0, FCSFUNCDESC _desc1) {
			if (_desc0.iActIdx == _desc1.iActIdx) {
				return _desc0.fTime < _desc1.fTime;
			}
			else {
				return _desc0.iActIdx < _desc1.iActIdx;
			}
		}
	);
}

void CCutScene::UpdateAttach()
{
	if (m_iActIdx == -1)
		return;

	if (!m_upSeqTimer)
		return;

	const _float fElapsed = (Engine::_float)m_upSeqTimer->GetElapsedTime();

	for (const auto& desc : m_vecAttach)
	{
		if (desc.iActIdx != m_iActIdx)
			continue;

		const _float fStart = desc.fTime;
		const _float fEnd = desc.fTime + desc.fDuration;

		if (fElapsed < fStart)
			continue;

		if (fElapsed > fEnd) 
		{
			if (desc.iActIdx == 10 && desc.iWeaponActorIdx == 8)
				m_vecActors[GUITAR]->SetActive(false);
				continue;
		}
			

		SetWeapon(desc);
	}
}

void CCutScene::PlayVFX(FVFXDESC _desc)
{
	if (EACTOREND <= _desc.iActorIdx) 
		return;
	if (m_vecActors[_desc.iActorIdx]) 
		m_vecActors[_desc.iActorIdx]->PlayVFX(&_desc);
}

void CCutScene::ReadyEmissive()
{
	SP_Asset spAsset;
	Engine::CRenderer::SHADERPARAM_DESC desc = {};

	//PLATFORM
	if (auto spModel = m_vecActors[PLATFORM]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(9);
		m_pGameInstance->LoadAsset(spAsset, L"T_SpectraLogoB_01_BC");
		m_spPlatform = { dynamic_pointer_cast<Engine::CTextureAsset>(spAsset) };
		desc = {};
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_SRV };
		desc.sConstantName = { "g_CustomTexture" };
		desc.pData = { (*m_spPlatform)[0] };
		spMeshRenderer->Add_Parameters(desc);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vPlatformMarkColor };
		spMeshRenderer->Add_Parameters(desc);
		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomTexture" };
		desc.pData = { &m_uiOne };
		spMeshRenderer->Add_Parameters(desc);
	}

	//KALETRANSIT
	
	if (auto spModel = m_vecActors[KALE_TRANIT]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(10);
		m_pGameInstance->LoadAsset(spAsset, L"T_em7500_core_E");
		m_spKaleCore = { dynamic_pointer_cast<Engine::CTextureAsset>(spAsset) };
		desc = {};
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_SRV };
		desc.sConstantName = { "g_CustomTexture" };
		desc.pData = { (*m_spKaleCore)[0] };
		spMeshRenderer->Add_Parameters(desc);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vWhite };
		spMeshRenderer->Add_Parameters(desc);
		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomTexture" };
		desc.pData = { &m_uiOne };
		spMeshRenderer->Add_Parameters(desc);
	}

	//PHASE1_KALE
	if (auto spModel = m_vecActors[PHASE1_KALE]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(10);
		desc = {};
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_SRV };
		desc.sConstantName = { "g_CustomTexture" };
		desc.pData = { (*m_spKaleCore)[0] };
		spMeshRenderer->Add_Parameters(desc);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vWhite };
		spMeshRenderer->Add_Parameters(desc);
		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomTexture" };
		desc.pData = { &m_uiOne };
		spMeshRenderer->Add_Parameters(desc);
	}

	//KALE_BARRIER_ARM
	if (auto spModel = m_vecActors[KALE_BARRIER_ARM]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(5);
		m_pGameInstance->LoadAsset(spAsset, L"T_em7510_core");
		m_spArmCore = { dynamic_pointer_cast<Engine::CTextureAsset>(spAsset) };
		desc = {};
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_SRV };
		desc.sConstantName = { "g_CustomTexture" };
		desc.pData = { (*m_spArmCore)[0] };
		spMeshRenderer->Add_Parameters(desc);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vGray };
		spMeshRenderer->Add_Parameters(desc);
		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomTexture" };
		desc.pData = { &m_uiTwo };
		spMeshRenderer->Add_Parameters(desc);
	}

	//KALE_FIRE_ARM
	if (auto spModel = m_vecActors[KALE_FIRE_ARM]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(2);
		m_pGameInstance->LoadAsset(spAsset, L"T_em7520_circle_E");
		m_spArmCircle = { dynamic_pointer_cast<Engine::CTextureAsset>(spAsset) };
		desc = {};
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_SRV };
		desc.sConstantName = { "g_CustomTexture" };
		desc.pData = { (*m_spArmCircle)[0] };
		spMeshRenderer->Add_Parameters(desc);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vDouble };
		spMeshRenderer->Add_Parameters(desc);
		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomTexture" };
		desc.pData = { &m_uiTwo };
		spMeshRenderer->Add_Parameters(desc);
	}

	//KALE_BEFORE_ELECTRIC
	if (auto spModel = m_vecActors[KALE_BEFORE_ELECTRIC]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(13);
		desc = {};
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_SRV };
		desc.sConstantName = { "g_CustomTexture" };
		desc.pData = { (*m_spKaleCore)[0] };
		spMeshRenderer->Add_Parameters(desc);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vWhite };
		spMeshRenderer->Add_Parameters(desc);
		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomTexture" };
		desc.pData = { &m_uiOne };
		spMeshRenderer->Add_Parameters(desc);
	}

	//KALE_AFTER_ELECTRIC
	if (auto spModel = m_vecActors[KALE_AFTER_ELECTRIC]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(13);
		m_pGameInstance->LoadAsset(spAsset, L"T_em7500_core_E_Red");
		m_spKaleCoreRed = { dynamic_pointer_cast<Engine::CTextureAsset>(spAsset) };
		desc = {};
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_SRV };
		desc.sConstantName = { "g_CustomTexture" };
		desc.pData = { (*m_spKaleCoreRed)[0] };
		spMeshRenderer->Add_Parameters(desc);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vKaleCoreRed };
		spMeshRenderer->Add_Parameters(desc);
		desc.sConstantName = { "g_vDiffusePlus" };
		desc.pData = { &m_vKaleCorePlus };
		spMeshRenderer->Add_Parameters(desc);
		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomTexture" };
		desc.pData = { &m_uiOne };
		spMeshRenderer->Add_Parameters(desc);
	}

	//KALE_LAST
	if (auto spModel = m_vecActors[KALE_LAST]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(13);
		desc = {};
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_SRV };
		desc.sConstantName = { "g_CustomTexture" };
		desc.pData = { (*m_spKaleCoreRed)[0] };
		spMeshRenderer->Add_Parameters(desc);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vKaleCoreRed };
		spMeshRenderer->Add_Parameters(desc);
		desc.sConstantName = { "g_vDiffusePlus" };
		desc.pData = { &m_vKaleCorePlus };
		spMeshRenderer->Add_Parameters(desc);
		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomTexture" };
		desc.pData = { &m_uiOne };
		spMeshRenderer->Add_Parameters(desc);
	}

	//KALE_WEAPON
	if (auto spModel = m_vecActors[KALE_WEAPON]->Get_Model().lock())
	{
		auto spMeshRenderer1 = spModel->GetMeshRenderer(0);
		auto spMeshRenderer2 = spModel->GetMeshRenderer(1);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vDiffuseColor" };
		desc.pData = { &m_vKaleWeaponBlue };
		spMeshRenderer2->Add_Parameters(desc);
		desc.pData = { &m_vKaleWeaponSky };
		spMeshRenderer1->Add_Parameters(desc);

		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomDiffuse" };
		desc.pData = { &m_uiOne };
		spMeshRenderer2->Add_Parameters(desc);
		spMeshRenderer1->Add_Parameters(desc);
	}

	//GUITAR
	if (auto spModel = m_vecActors[GUITAR]->Get_Model().lock())
	{
		auto spMeshRenderer1 = spModel->GetMeshRenderer(2);
		spMeshRenderer1->GetMesh().lock()->SetShaderIdx(11);
		m_pGameInstance->Change_RenderGroup(Engine::RENDERGROUP::LATE_EFT, spMeshRenderer1);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float) };
		desc.sConstantName = { "g_fTrash" };
		desc.pData = { &m_fTrash };
		spMeshRenderer1->Add_Parameters(desc);
		auto spMeshRenderer2 = spModel->GetMeshRenderer(0);
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_f3EmissiveColor" };
		desc.pData = { &m_vGuitarSky };
		spMeshRenderer2->Add_Parameters(desc);
	}

	//GUITARORIGIN
	if (auto spModel = m_vecActors[GUITARORIGIN]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(1);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vOriginBlue };
		spMeshRenderer->Add_Parameters(desc);
	}
}

void CCutScene::ChangeKale()
{
	Engine::CRenderer::SHADERPARAM_DESC desc = {};
	//KALE_BEFORE_ELECTRIC
	if (auto spModel = m_vecActors[KALE_BEFORE_ELECTRIC]->Get_Model().lock())
	{
		auto spMeshRenderer = spModel->GetMeshRenderer(13);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_SRV };
		desc.sConstantName = { "g_CustomTexture" };
		desc.pData = { (*m_spKaleCoreRed)[0] };
		spMeshRenderer->Add_Parameters(desc);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vEmissiveColor" };
		desc.pData = { &m_vKaleCoreRed };
		spMeshRenderer->Add_Parameters(desc);
		desc.sConstantName = { "g_vDiffusePlus" };
		desc.pData = { &m_vKaleCorePlus };
		spMeshRenderer->Add_Parameters(desc);
		desc.iSize = { sizeof(Engine::_uint) };
		desc.sConstantName = { "g_uiUseCustomTexture" };
		desc.pData = { &m_uiOne };
		spMeshRenderer->Add_Parameters(desc);
	}
	if (auto spModel = m_vecActors[KALE_WEAPON]->Get_Model().lock())
	{
		auto spMeshRenderer1 = spModel->GetMeshRenderer(0);
		auto spMeshRenderer2 = spModel->GetMeshRenderer(1);
		desc.eParamType = { Engine::CRenderer::EPARAMTYPE::PT_RAWDATA };
		desc.iSize = { sizeof(Engine::_float3) };
		desc.sConstantName = { "g_vDiffuseColor" };
		desc.pData = { &m_vKaleWeaponRed };
		spMeshRenderer2->Add_Parameters(desc);
		desc.pData = { &m_vKaleWeaponOrange };
		spMeshRenderer1->Add_Parameters(desc);
	}
}

void CCutScene::SetExplosion(bool bExplosion)
{
	m_pGameInstance->SetExplosion(bExplosion);
	for (auto iActorIdx : { CHAI_101, PEPPERMINT, MACARON, KORSICA })
	{
		m_vecActors[iActorIdx]->SetExplosion(bExplosion);
	}
	if (bExplosion)
		m_bIsExploded = false;
}

void CCutScene::ChangeOrigin()
{
	m_vOriginBlue = { 0.f,3.f,4.f };
	//PrintDebug(L"ChangeOrigin");
}

void CCutScene::ChangeTrash()
{
	m_bTrash = { true };
	m_upTrashTimer->Restart();
	//PrintDebug(L"ChangeTrash");
}
