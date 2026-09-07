#include "Interact_RhythmChallenge.h"
#include "BaseModel.h"
#include "AnimController.h"
#include "GameInstance.h"
#include "Character.h"
#include "RhythmChallenge.h"

HRESULT CInteract_RhythmChallenge::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents());

	Add_Component<CTransform>(L"Com_Transform_Partner");
	auto spPartnerTransform = dynamic_pointer_cast<CTransform>(Get_Component(L"Com_Transform_Partner").lock());
	spPartnerTransform->SetInitialAdd();
	m_wpPartnerTransform = spPartnerTransform;

	Add_Component<CTransform>(L"Com_Transform_Player");
	auto spPlayerTransform = dynamic_pointer_cast<CTransform>(Get_Component(L"Com_Transform_Player").lock());
	spPlayerTransform->SetInitialAdd();
	m_wpPlayerTransform = spPlayerTransform;

	return S_OK;
}

CInteract_RhythmChallenge::CInteract_RhythmChallenge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractObject(pDevice, pContext)
{
}

CInteract_RhythmChallenge::CInteract_RhythmChallenge(const CInteract_RhythmChallenge& Prototype)
	: CInteractObject(Prototype)
{
}

HRESULT CInteract_RhythmChallenge::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
		return S_OK;
}

HRESULT CInteract_RhythmChallenge::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))
	AddStateFunc(ESTATE::ACTIVE, EKEYACTIONSTATE::ENTER, [=](auto) {
		m_eInteractResult = EInteractResult::IR_END;
	});
	return S_OK;
}

HRESULT CInteract_RhythmChallenge::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())

	if (!m_wpPlayerTransform.lock()) {
		Add_Component<CTransform>(L"Com_Transform_Player");
	}
	if (!m_wpPartnerTransform.lock()) {
		Add_Component<CTransform>(L"Com_Transform_Partner");
	}

	m_wpPartnerTransform	= dynamic_pointer_cast<CTransform>(Get_Component(L"Com_Transform_Partner").lock());
	m_wpPlayerTransform		= dynamic_pointer_cast<CTransform>(Get_Component(L"Com_Transform_Player").lock());
	if (m_pParentObject) {
		if (auto spComp = m_wpPartnerTransform.lock())	spComp->Set_Parent(m_pParentObject, true);
		if (auto spComp = m_wpPlayerTransform.lock())	spComp->Set_Parent(m_pParentObject, true);
	}

	return S_OK;
}

void CInteract_RhythmChallenge::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInteract_RhythmChallenge::Update(_float fTimeDelta)
{
	if (m_eCurState == ESTATE::ACTIVE) {
		if (m_pRhythmChallenge) {
			EInteractResult eChallengeState = m_pRhythmChallenge->GetState();
		}

		if (m_eInteractResult == EInteractResult::IR_FAIL) Deactivate();
		if (m_eInteractResult == EInteractResult::IR_SUCCESS) Trigger();
	}

	if (m_ctxDesc.pBlackboard)
	{
		m_ctxDesc.pBlackboard->SetIntElement ("INTERACTIVE_RESULT", static_cast<_int>(m_eInteractResult));
	}

	__super::Update(fTimeDelta);
}

void CInteract_RhythmChallenge::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CInteract_RhythmChallenge::Render_Properties()
{
	__super::Render_Properties();
	ImGui::Separator();
	if (ImGui::Button("Challenge"))	Challenge();
	ImGui::SameLine();
	if (ImGui::Button("Success"))		Success();
	ImGui::SameLine();
	if (ImGui::Button("Fail"))			Fail();
	ImGui::SameLine();
	string sCurState = "";
	switch (m_eInteractResult)
	{
	case EInteractResult::IR_IN_PROGRESS:
		sCurState = "InProgress";
		break;
	case EInteractResult::IR_SUCCESS:
		sCurState = "Success";
		break;
	case EInteractResult::IR_FAIL:
		sCurState = "Fail";
		break;
	case EInteractResult::IR_END:
		sCurState = "End";
		break;
	default:
		break;
	}
	ImGui::Text(sCurState.c_str());
	ImGui::Separator();
}

void CInteract_RhythmChallenge::Set_Animator(wstring sAnimatorPath)
{
	__super::Set_Animator(sAnimatorPath);
	if (m_ctxDesc.pBlackboard) {
		m_ctxDesc.pBlackboard->AddIntElement("INTERACTIVE_RESULT");
	}
}

void CInteract_RhythmChallenge::Set_RhythmChallenge(CRhythmChallenge* pRhythmChallenge)
{
	m_pRhythmChallenge = pRhythmChallenge;
}

EInteractResult CInteract_RhythmChallenge::GetRhythmChallengeState()
{
	return m_eInteractResult;
}

void CInteract_RhythmChallenge::Challenge()
{

	if (m_eCurState == ESTATE::ACTIVE && m_eInteractResult == EInteractResult::IR_END)
	{
		m_eInteractResult = EInteractResult::IR_IN_PROGRESS;
		//if (m_pRhythmChallenge) m_pRhythmChallenge->Play();
	}
}

void CInteract_RhythmChallenge::Success()
{
	if (m_eCurState == ESTATE::ACTIVE && m_eInteractResult == EInteractResult::IR_IN_PROGRESS) {
		m_eInteractResult = EInteractResult::IR_SUCCESS;
	}
}

void CInteract_RhythmChallenge::Fail()
{
	if (m_eCurState == ESTATE::ACTIVE && m_eInteractResult == EInteractResult::IR_IN_PROGRESS) {
		m_eInteractResult = EInteractResult::IR_FAIL;
	}
}

void CInteract_RhythmChallenge::Move(CCharacter* pPlayer, CCharacter* pPartner)
{
	if (auto spTransform = m_wpPlayerTransform.lock())
		pPlayer	->Move_TargetPoint(spTransform->GetWorldMatrix());
	if (auto spTransform = m_wpPartnerTransform.lock())
		pPartner->Move_TargetPoint(spTransform->GetWorldMatrix());
}

void CInteract_RhythmChallenge::Free()
{
	__super::Free();
}
