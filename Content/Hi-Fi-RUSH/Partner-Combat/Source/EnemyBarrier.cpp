#include "EnemyBarrier.h"
#include "GameInstance.h"

#include "MeshRenderer.h"
#include "Mesh.h"

#include "Enemy_BAB00.h"

void CEnemyBarrier::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CEnemyBarrier::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fBarrierAmount += fTimeDelta;
	if (m_upDissolveTimer && m_upDissolveTimer->IsActive()) {
		m_fScratchAmount = (Engine::_float)m_upDissolveTimer->GetElapsedRatio();
	}
	else {
		m_fScratchAmount = 0.f;
	}

	if (!m_bIsBroken && m_fCurBarrierGauge <= 0.f) {		
		BarrierBroken();
	}
}
void CEnemyBarrier::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}
void CEnemyBarrier::SetCollidor()
{
	if (auto spCom = Get_Component(L"Com_Collidor_Barrier").lock()) {
		m_wpCollidor[Barrier] = ConvertWPComponent<CCollidor>(spCom);
		m_wpCollidor[Barrier].lock()->Set_Func(EKEYACTIONSTATE::ENTER,
			[=](const CONTACTMF_DESC& mfDesc, CCollidor* pCollidor)
			{
				
			});
		m_wpCollidor[Barrier].lock()->Set_Func(EKEYACTIONSTATE::STAY,
			[=](const CONTACTMF_DESC& mfDesc, CCollidor* pCollidor)
			{

			});
	}

	//if (auto spCom = Get_Component(L"Com_Collidor_Body").lock()) {
	//	m_wpCollidor[Body] = ConvertWPComponent<CCollidor>(spCom);		
	//}
}
void CEnemyBarrier::BarrierRecovery()
{
	m_fCurBarrierGauge = m_fMaxBarrierGauge;
	m_bIsBroken = false;
	SetActive(true);
}
void CEnemyBarrier::BarrierBroken()
{
	m_bIsBroken = true;	
	if (!m_upDissolveTimer->IsActive()) m_upDissolveTimer->Restart();
}
void CEnemyBarrier::Initialize_BarrierGauge(_float fMaxGauge)
{
	m_fMaxBarrierGauge = fMaxGauge;
}
void CEnemyBarrier::OnDamage(_float fDamage)
{
	if (IsActive())
		m_fCurBarrierGauge -= fDamage;
}
CEnemyBarrier::CEnemyBarrier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}
CEnemyBarrier::CEnemyBarrier(const CEnemyBarrier& Prototype)
	:CGameObject(Prototype),
	m_desc(Prototype.m_desc)
{
}
CGameObject* CEnemyBarrier::Clone(void* pArg)
{
	return CloneBase<CEnemyBarrier>(pArg);
}
HRESULT CEnemyBarrier::Save(void* _pDesc, _uint& _iSize) const
{
	static_assert(is_trivially_copyable_v<FBARRIERDESC>);
	_iSize = sizeof FBARRIERDESC;
	memcpy(_pDesc, &m_desc, _iSize);
	return S_OK;
}
HRESULT CEnemyBarrier::Load(void* _pDesc)
{
	static_assert(is_trivially_copyable_v<FBARRIERDESC>);
	_uint _iSize = sizeof FBARRIERDESC;
	memcpy(&m_desc, _pDesc, _iSize);
	return S_OK;
}
void CEnemyBarrier::Free()
{
	__super::Free();
}
HRESULT CEnemyBarrier::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg));
	return S_OK;
}
HRESULT CEnemyBarrier::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc));
	m_upDissolveTimer = m_pGameInstance->CreateTimerWithDuration(m_desc.m_fDissolveTime, false, [=]() {
		if (m_pParentObject) static_cast<CEnemy_BAB00*>(m_pParentObject)->StartBarrierCooldown();
		SetActive(false);
		}, false);
	return S_OK;
}
HRESULT CEnemyBarrier::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize());
	m_wpModel			= ConvertWPComponent<CModel>(Get_Component(L"Com_Model_Main"));
	m_wpShader			= ConvertWPComponent<CShader>(Get_Component(L"Com_MeshShader_Main"));
	m_wpMeshRenderer	= m_wpModel.lock()->GetMeshRenderer(0);
	m_wpMeshRenderer.lock()->GetMesh().lock()->SetShaderIdx(0);
	m_wpMeshRenderer.lock()->DeactiveDefaultVar();
	m_wpMeshRenderer.lock()->UseDepth(true);
	m_wpMeshRenderer.lock()->UseCam(true);

	m_pGameInstance->Change_RenderGroup(RENDERGROUP::BLEND, m_wpMeshRenderer);

	SetCollidor();
	Set_ShaderParam();
	return S_OK;
}
HRESULT CEnemyBarrier::Load_Asset()
{
	CHKFAIL(__super::Load_Asset());

	auto wpComp = Get_Component<CModel>();
	m_wpModel = ConvertWPComponent<CModel>(wpComp);

	SP_Asset _spAsset;
	m_pGameInstance->LoadAsset(_spAsset, L"T_VFX_tk_Barrier_03r");
	m_spBarrierTexture[TT_MASK0] = static_pointer_cast<CTextureAsset>(_spAsset);

	m_pGameInstance->LoadAsset(_spAsset, L"T_VFX_tk_Barrier_03_");
	m_spBarrierTexture[TT_MASK1] = static_pointer_cast<CTextureAsset>(_spAsset);

	m_pGameInstance->LoadAsset(_spAsset, L"T_VFX_tk_noise_01_");
	m_spBarrierTexture[TT_NOISE] = static_pointer_cast<CTextureAsset>(_spAsset);

	m_pGameInstance->LoadAsset(_spAsset, L"T_VFX_tk_scratch_08_");
	m_spBarrierTexture[TT_SCRATCH] = static_pointer_cast<CTextureAsset>(_spAsset);

	return S_OK;
}
HRESULT CEnemyBarrier::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents());
	CModel::MODEL_DESC _modelDesc;
	lstrcpy(_modelDesc.m_sModelAssetName, L"sphere_00_LOD0.001");

	_modelDesc.m_bIsCopy = false;
	Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);
	Get_Component(L"Com_Model_Main").lock()->SetInitialAdd();

	Add_Component(L"Shader_Barrier_Component", L"Com_MeshShader_Main");
	Get_Component(L"Com_MeshShader_Main").lock()->SetInitialAdd();

	return S_OK;
}
void CEnemyBarrier::Render_Properties()
{
	ImGui::ColorPicker4	("BarrierColor0",			reinterpret_cast<_float*>(&m_desc.m_vBarrierColor0));
	ImGui::ColorPicker4	("BarrierColor1",			reinterpret_cast<_float*>(&m_desc.m_vBarrierColor1));
	ImGui::ColorPicker4	("BarrierColor2",			reinterpret_cast<_float*>(&m_desc.m_vBarrierColor2));

	ImGui::ColorPicker4	("BarrierShoreColor0",		reinterpret_cast<_float*>(&m_desc.m_vBarrierShoreColor0));
	ImGui::ColorPicker4	("BarrierShoreColor1",		reinterpret_cast<_float*>(&m_desc.m_vBarrierShoreColor1));
	
	ImGui::DragFloat2	("Mask  UV Scale0",		reinterpret_cast<_float*>(&m_desc.m_vMaskUVScale),		0.01f);
	ImGui::DragFloat2	("Noise UV Scale",			reinterpret_cast<_float*>(&m_desc.m_vNoiseUVScale),		0.01f);
	ImGui::DragFloat2	("Scratch UV Scale",		reinterpret_cast<_float*>(&m_desc.m_vScratchUVScale),		0.01f);
	ImGui::DragFloat2	("Noise Shore UV Scale",	reinterpret_cast<_float*>(&m_desc.m_vShoreNoiseUVScale),	0.01f);
	
	ImGui::DragFloat	("Rim Threshold0",			reinterpret_cast<_float*>(&m_desc.m_fBarrierRimThreshold0),	0.01f);
	ImGui::DragFloat	("Rim Threshold1",			reinterpret_cast<_float*>(&m_desc.m_fBarrierRimThreshold1),	0.01f);
	ImGui::DragFloat	("Shore Depth",			reinterpret_cast<_float*>(&m_desc.m_fBarrierShoreDepth),		0.01f);

	ImGui::DragFloat	("Barrier Noise Speed",	reinterpret_cast<_float*>(&m_desc.m_fShoreNoiseSpeed),		0.01f);
	ImGui::DragFloat	("Shore Noise Speed",		reinterpret_cast<_float*>(&m_desc.m_fNoiseSpeed),		0.01f);
	
	ImGui::DragFloat	("Shore Alpha",			reinterpret_cast<_float*>(&m_desc.m_fShoreAlpha),			0.01f);
	ImGui::DragFloat	("Barrier Alpha",			reinterpret_cast<_float*>(&m_desc.m_fBarrierAlpha),				0.01f);
	ImGui::DragFloat	("Noise Intensity",		 reinterpret_cast<_float*>(&m_desc.m_fNoiseIntensity),			0.01f);
	ImGui::DragFloat	("Shore Intensity",		 reinterpret_cast<_float*>(&m_desc.m_fShoreIntensity),			0.001f);
	ImGui::DragFloat	("Shore Noise Intensity",		 reinterpret_cast<_float*>(&m_desc.m_fShoreNoiseIntensity),			0.01f);
	
	ImGui::DragFloat	("Scratch Amount",			reinterpret_cast<_float*>(&m_fScratchAmount),		0.01f);
	if (ImGui::DragFloat("DissolveTime", reinterpret_cast<_float*>(&m_desc.m_fDissolveTime), 0.01f)) {
		m_upDissolveTimer->SetDuration(m_desc.m_fDissolveTime);
	}
	if (ImGui::Button("Dissolve")) {
		m_upDissolveTimer->Restart();
	}
}

void CEnemyBarrier::Set_ShaderParam()
{
	CRenderer::SHADERPARAM_DESC _desc;
	_desc.eParamType = CRenderer::EPARAMTYPE::PT_RAWDATA;
	_desc.iSize = sizeof(_float4);

	_desc.sConstantName = "g_vBarrierColor0";
	_desc.pData			= &m_desc.m_vBarrierColor0;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_vBarrierColor1";
	_desc.pData			= &m_desc.m_vBarrierColor1;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_vBarrierColor2";
	_desc.pData			= &m_desc.m_vBarrierColor2;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.sConstantName = "g_vBarrierShoreColor0";
	_desc.pData = &m_desc.m_vBarrierShoreColor0;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_vBarrierShoreColor1";
	_desc.pData = &m_desc.m_vBarrierShoreColor1;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.iSize = sizeof(_float2);
	_desc.sConstantName = "g_vMaskUVScale";
	_desc.pData = &m_desc.m_vMaskUVScale;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_vNoiseUVScale";
	_desc.pData = &m_desc.m_vNoiseUVScale;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_vScratchUVScale";
	_desc.pData = &m_desc.m_vScratchUVScale;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_vShoreNoiseUVScale";
	_desc.pData = &m_desc.m_vShoreNoiseUVScale;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.iSize = sizeof(_float);
	_desc.sConstantName = "g_fBarrierRimThreshold0";
	_desc.pData = &m_desc.m_fBarrierRimThreshold0;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fBarrierRimThreshold1";
	_desc.pData = &m_desc.m_fBarrierRimThreshold1;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fBarrierShoreDepth";
	_desc.pData = &m_desc.m_fBarrierShoreDepth;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	
	_desc.sConstantName = "g_fShoreNoiseSpeed";
	_desc.pData = &m_desc.m_fShoreNoiseSpeed;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fNoiseSpeed";
	_desc.pData = &m_desc.m_fNoiseSpeed;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fBarrierAlpha";
	_desc.pData = &m_desc.m_fBarrierAlpha;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fShoreAlpha";
	_desc.pData = &m_desc.m_fShoreAlpha;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.sConstantName = "g_fBarrierAmount";
	_desc.pData = &m_fBarrierAmount;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fScratchAmount";
	_desc.pData = &m_fScratchAmount;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);


	_desc.sConstantName = "g_fNoiseIntensity";
	_desc.pData = &m_desc.m_fNoiseIntensity;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fShoreIntensity";
	_desc.pData = &m_desc.m_fShoreIntensity;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fShoreNoiseIntensity";
	_desc.pData = &m_desc.m_fShoreNoiseIntensity;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.eParamType	= CRenderer::EPARAMTYPE::PT_SRV;
	_desc.sConstantName = "g_MaskTexture0";
	_desc.pData			= (*m_spBarrierTexture[TT_MASK0])[0];
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_MaskTexture1";
	_desc.pData			= (*m_spBarrierTexture[TT_MASK1])[0];
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_NoiseTexture";
	_desc.pData			= (*m_spBarrierTexture[TT_NOISE])[0];
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_ScartchTexture";
	_desc.pData			= (*m_spBarrierTexture[TT_SCRATCH])[0];
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
}
