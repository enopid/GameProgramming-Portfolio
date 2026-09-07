#include "InteractObject.h"
#include "GameInstance.h"
#include "BaseModel.h"
#include "Player.h"
#include "MeshRenderer.h"
#include "BlackBoard.h"
#include "AnimController.h"

void CInteractObject::Free()
{
	__super::Free();
	Safe_Release(m_ctxDesc.pBlackboard);
}

HRESULT CInteractObject::Reset()
{
	CHKFAIL(__super::Reset());
	return S_OK;
}

CInteractObject::ESTATE CInteractObject::GetState()
{
	return m_eCurState;
}

EInteractType CInteractObject::GetInteractType()
{
	return m_eInteractType;
}

CInteractObject::CInteractObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteractObject::CInteractObject(const CInteractObject& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CInteractObject::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
	return S_OK;
}

HRESULT CInteractObject::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	m_upDeactiveTimer = m_pGameInstance->CreateTimerWithDuration(m_desc.m_fDeactiveTime, false,
		[=]() { if (m_desc.m_fDeactiveTime > 0.1f && !m_bDeactivateTimer) Deactivate(); },
		false
	);
	for (_uint i = 0; i < 3; i++) for (_uint j = 0; j < 3; j++) 
		m_vecFuncs[i][j]=list<function<void(float)>>();

	return S_OK;
};

HRESULT CInteractObject::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())

	BindSameGroup();

	return S_OK;
}

void CInteractObject::Render_Properties()
{
	__super::Render_Properties();

	ImGui::BeginChild("InteractObject", ImVec2(0, 140), true);
	if (ImGui::InputInt("GroupIdx", &m_desc.m_iGroupIdx)) {
		BindSameGroup();
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("IsMaster", &m_desc.m_bIsMaster)) {
		BindSameGroup();
	}
	if (m_desc.m_bIsMaster && ImGui::CollapsingHeader("Child")) {
		for (auto pInteractObject : m_vecInteractObjects) {
			ImGui::Text(ws2s(pInteractObject->GetName()).c_str());
		}
	}
	if (ImGui::DragInt("InitState", &m_desc.m_iInitState,1.f, 0, 2)) {

	}

	ImGui::PushItemWidth(100);
	if (ImGui::DragFloat("DeactiveTime", &m_desc.m_fDeactiveTime, 0.1f, 0.f, 60.f)) {
		m_upDeactiveTimer->SetDuration(m_desc.m_fDeactiveTime);
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::Checkbox("Deactivate DeactivateTimer", &m_bDeactivateTimer);
	if (m_eCurState == ESTATE::TRIGGER && m_desc.m_fDeactiveTime > 0.1f) {
		string sCurState = "Remain Time : ";
		sCurState += to_string(m_desc.m_fDeactiveTime - m_upDeactiveTimer->GetElapsedTime());
		ImGui::Text(sCurState.c_str());
	}

	if (ImGui::Button("Activate"))		Activate();
	ImGui::SameLine();
	if (ImGui::Button("Deactivate"))	Deactivate();
	ImGui::SameLine();
	if (ImGui::Button("Trigger"))		Trigger();
	ImGui::SameLine();
	string sCurState = "Cur State : ";
	sCurState += ((m_eCurState == ESTATE::DEACTIVE) ? "DEACTIVE" : ((m_eCurState == ESTATE::ACTIVE) ? "ACTIVE" : "TRIGGER"));
	ImGui::Text(sCurState.c_str());
	
	ImGui::EndChild();
}

void CInteractObject::Activate()
{
	if (m_eCurState != ESTATE::DEACTIVE) return;
	m_ePrevState	= m_eCurState;
	m_eCurState		= ESTATE::ACTIVE;

	for (auto& _func : m_vecFuncs[ENUM_TO_UINT(EKEYACTIONSTATE::EXIT)][ENUM_TO_UINT(m_ePrevState)])	_func(0.f);
	for (auto& _func : m_vecFuncs[ENUM_TO_UINT(EKEYACTIONSTATE::ENTER)][ENUM_TO_UINT(m_eCurState)])	_func(0.f);
}

void CInteractObject::Trigger()
{
	if (m_eCurState != ESTATE::ACTIVE) return;
	m_ePrevState	= m_eCurState;
	m_eCurState		= ESTATE::TRIGGER;
	if (m_upDeactiveTimer && m_desc.m_fDeactiveTime >=0.1f) m_upDeactiveTimer->Restart();
	

	for (auto& _func : m_vecFuncs[ENUM_TO_UINT(EKEYACTIONSTATE::EXIT)]	[ENUM_TO_UINT(m_ePrevState)])	_func(0.f);
	for (auto& _func : m_vecFuncs[ENUM_TO_UINT(EKEYACTIONSTATE::ENTER)]	[ENUM_TO_UINT(m_eCurState)])	_func(0.f);

	for (auto pInteractObject : m_vecInteractObjects) pInteractObject->Trigger();
}

void CInteractObject::Deactivate()
{
	if (m_eCurState == ESTATE::DEACTIVE) return;
	m_ePrevState = m_eCurState;
	m_eCurState = ESTATE::DEACTIVE;

	for (auto& _func : m_vecFuncs[ENUM_TO_UINT(EKEYACTIONSTATE::EXIT)][ENUM_TO_UINT(m_ePrevState)])	_func(0.f);
	for (auto& _func : m_vecFuncs[ENUM_TO_UINT(EKEYACTIONSTATE::ENTER)][ENUM_TO_UINT(m_eCurState)])	_func(0.f);

	for (auto pInteractObject : m_vecInteractObjects) pInteractObject->Deactivate();
}

bool CInteractObject::IsMaster()
{
	return m_desc.m_bIsMaster;
}

void CInteractObject::BindSameGroup()
{
	m_vecInteractObjects.clear();
	
	auto lstOBjects = m_pGameInstance->Get_Objects(
		m_pGameInstance->GetCurrentLevelTag(),
		L"Layer_Interact"
	);

	for (auto pObject : lstOBjects)
	{
		if (pObject == this) continue;
		if (auto pInteractObject = dynamic_cast<CInteractObject*>(pObject)) {
			if (pInteractObject->m_desc.m_iGroupIdx == m_desc.m_iGroupIdx) {
				if (m_desc.m_bIsMaster) {
					BindSameGroup(pInteractObject);
				}
				else {
					if (pInteractObject->IsMaster()) {
						pInteractObject->BindSameGroup(this);
					}
				}
			}
			else {
				if (pInteractObject->IsMaster()) {
					pInteractObject->RemoveGroup(this);
				}
			}
		}
	}
}

void CInteractObject::BindSameGroup(CInteractObject* pObject)
{
	if (pObject->IsMaster()) return;
	for (auto pInteractObject : m_vecInteractObjects) {
		if (pInteractObject == pObject) return;
	}
	m_vecInteractObjects.push_back(pObject);
}

void CInteractObject::RemoveGroup(CInteractObject* pObject)
{
	m_vecInteractObjects.erase(
		std::remove_if(m_vecInteractObjects.begin(), m_vecInteractObjects.end(), [=](auto _pObject) {
			return pObject == _pObject;
			}),
		m_vecInteractObjects.end()
	);
}

HRESULT CInteractObject::Save(void* _pDesc, _uint& _iSize) const
{
	static_assert(is_trivially_copyable_v<FINTERACTOBJECTDESC>);
	_iSize = sizeof(FINTERACTOBJECTDESC);
	memcpy(_pDesc, &m_desc, _iSize);
	return S_OK;
}

HRESULT CInteractObject::Load(void* _pDesc)
{
	m_desc = *static_cast<FINTERACTOBJECTDESC*>(_pDesc);
	m_upDeactiveTimer->SetDuration(m_desc.m_fDeactiveTime);
	m_eCurState = m_ePrevState = ESTATE(m_desc.m_iInitState);
	return S_OK;
}


void CInteractObject::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInteractObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_ctxDesc.pBlackboard) {
		m_ctxDesc.pBlackboard->SetBoolElement("ACTIVATE",	m_eCurState == ESTATE::ACTIVE);
		m_ctxDesc.pBlackboard->SetBoolElement("DEACTIVATE",m_eCurState == ESTATE::DEACTIVE);
		m_ctxDesc.pBlackboard->SetBoolElement("TRIGGER",	m_eCurState == ESTATE::TRIGGER);
	}
	if (m_upAnimController) {
		m_upAnimController->UpdateCTX(m_ctxDesc);
		m_upAnimController->Update(fTimeDelta);
	}

	for (auto& _func : m_vecFuncs[ENUM_TO_UINT(EKEYACTIONSTATE::STAY)][ENUM_TO_UINT(m_eCurState)])	_func(fTimeDelta);
}

void CInteractObject::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_ePrevState = m_eCurState;
}

HRESULT CInteractObject::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents());

	Add_Component(L"Shader_Mesh_Component", L"Com_MeshShader_Main");
	Add_Component(L"Shader_AnimMesh_Component", L"Com_AnimMeshShader_Main");
	Get_Component(L"Com_MeshShader_Main").lock()->SetInitialAdd();
	Get_Component(L"Com_AnimMeshShader_Main").lock()->SetInitialAdd();

	return S_OK;
}

void CInteractObject::Set_Animator(wstring sAnimatorPath)
{
	Json::Value tmp;
	m_pGameInstance->LoadJson(sAnimatorPath, tmp);
	m_upAnimController = CAnimController::Create(tmp, m_wpModel.lock());
	m_upAnimController->SetActive(true);

	SetBBFromAnimJson(tmp, m_ctxDesc);
	if (m_ctxDesc.pBlackboard) {
		m_ctxDesc.pBlackboard->AddBoolElement("ACTIVATE");
		m_ctxDesc.pBlackboard->AddBoolElement("DEACTIVATE");
		m_ctxDesc.pBlackboard->AddBoolElement("TRIGGER");
	}
}

HRESULT CInteractObject::AddStateFunc(ESTATE eState, EKEYACTIONSTATE eKeyAction, function<void(float)> pFunc)
{
	if (eKeyAction == EKEYACTIONSTATE::NONE)	return E_FAIL;
	m_vecFuncs[ENUM_TO_UINT(eKeyAction)][ENUM_TO_UINT(eState)].push_back(pFunc);

	return S_OK;
}

void CInteractObject::SetBBFromAnimJson(const Json::Value& root, ANIMCTX_DESC& m_ctxDesc)
{

	if (!m_ctxDesc.pBlackboard)
		m_ctxDesc.pBlackboard = CBlackBoard::Create();

	if (!root.isObject())
		return;

	if (!root.isMember("AnimTransitions") || !root["AnimTransitions"].isArray())
		return;

	std::unordered_set<std::string> setCtx;

	const Json::Value& transitions = root["AnimTransitions"];
	for (Json::ArrayIndex i = 0; i < transitions.size(); ++i)
	{
		const Json::Value& trans = transitions[i];
		if (!trans.isObject())
			continue;

		if (!trans.isMember("Conditions") || !trans["Conditions"].isArray())
			continue;

		const Json::Value& conditions = trans["Conditions"];
		for (Json::ArrayIndex j = 0; j < conditions.size(); ++j)
		{
			const Json::Value& cond = conditions[j];
			if (!cond.isObject())
				continue;

			if (!cond.isMember("CTX"))
				continue;

			std::string ctx = cond["CTX"].asString();
			if (ctx.empty())
				continue;

			// 중복 제거
			if (!setCtx.insert(ctx).second)
				continue;

			// Value 타입 기준으로 기본값 세팅
			if (!cond.isMember("Value"))
			{
				bool bDefault = false;
				m_ctxDesc.pBlackboard->AddElement(ctx.c_str(), bDefault);
				continue;
			}

			const Json::Value& value = cond["Value"];

			if (value.isBool())
			{
				bool bValue = value.asBool();
				m_ctxDesc.pBlackboard->AddElement(ctx.c_str(), bValue);
			}
			else if (value.isInt())
			{
				int iValue = value.asInt();
				m_ctxDesc.pBlackboard->AddElement(ctx.c_str(), iValue);
			}
			else if (value.isDouble())
			{
				float fValue = value.asFloat();
				m_ctxDesc.pBlackboard->AddElement(ctx.c_str(), fValue);
			}
			else
			{
				bool bDefault = false;
				m_ctxDesc.pBlackboard->AddElement(ctx.c_str(), bDefault);
			}
		}
	}
}
