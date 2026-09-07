#include "GravitySphere.h"
#include "GameInstance.h"

#include "MeshRenderer.h"
#include "Mesh.h"

#include "Enemy_BAB00.h"
#include "Enemy.h"
#include "Character.h"
#include "EffectRoot.h"

void CGravitySphere::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CGravitySphere::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fScrollAmount += fTimeDelta * m_desc.m_fScrollSpeed;

	if (m_upDisappearTimer && m_upDisappearTimer->IsActive()) {
		m_fScratchAmount = (Engine::_float)m_upDisappearTimer->GetElapsedRatio();
	}
	else {
		m_fScratchAmount = 0.f;
	}

	_float _fScale = 1.f;
	if (m_upAppearTimer && m_upAppearTimer->IsActive()) {
		auto _fRatio = (Engine::_float)m_upAppearTimer->GetElapsedRatio();
		_fScale = lerp(m_desc.m_fInitScale, m_desc.m_fTargetScale, _fRatio);
	}
	else {
		_fScale = m_desc.m_fTargetScale;
	}
	Get_MainTransform().lock()->Set_Scale(_fScale, _fScale, _fScale);
}
void CGravitySphere::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}
void CGravitySphere::SetCollidor()
{
	if (auto spCom = Get_Component(L"Com_Collidor_Main").lock()) {
		m_wpCollidor = ConvertWPComponent<CCollidor>(spCom);
		m_wpCollidor.lock()->Set_Func(EKEYACTIONSTATE::ENTER,
			[=](const CONTACTMF_DESC& mfDesc, CCollidor* pCollidor)
			{
				PrintDebug("Hit");
				if (auto pEnemy = dynamic_cast<CEnemy*>(pCollidor->GetGameObject()))
				{
					CCharacter::FDamageInfo _damgeDesc;
					_damgeDesc.fDamage = 0.f;
					_damgeDesc.eAttackType = CCharacter::EAttackType::MACARON_HOLD;
					_damgeDesc.pSourceObj = this;
					if (pEnemy && !pEnemy->GetIsInvincible())
					{
						auto iter = std::find(
							m_vecEnemies.begin(),
							m_vecEnemies.end(),
							pEnemy
						);

						if (iter == m_vecEnemies.end())
						{
							m_vecEnemies.push_back(pEnemy);
							PrintDebug("Macaron Hold Enter!");
							pEnemy->OnDamage(_damgeDesc);
						}
					}
				}
			}
		);
	}
}

void CGravitySphere::Appear(_vector vPos, _vector _vDir, bool bUseFX)
{
	SetActive(true);
	Get_MainTransform().lock()->Set_LocalState(STATE::POSITION, vPos);
	if (!m_upAppearTimer->IsActive()) {
		m_upAppearTimer->Restart();
		Get_MainTransform().lock()->Set_Scale(m_desc.m_fInitScale, m_desc.m_fInitScale, m_desc.m_fInitScale);
	}
	m_vecEnemies.clear();

	if (bUseFX) {
		m_pHand->Set_Pos(
			Get_MainTransform().lock()->Get_WorldState(STATE::POSITION)
			+ XMVector3Normalize(_vDir) * (m_desc.m_fTargetScale + 0.3f)
		);
		m_pHand->Set_Dir(_vDir);
		m_pHand->Restart();

		m_pWave->Set_Pos(Get_MainTransform().lock()->Get_WorldState(STATE::POSITION));
		m_pWave->Restart();
	}
}
void CGravitySphere::Disappear()
{
	if (!m_upDisappearDelayTimer->IsActive()) {
		m_upDisappearDelayTimer->Restart();
	}

	m_pHand->DissolveStop(0.2f);
}

CGravitySphere::CGravitySphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}
CGravitySphere::CGravitySphere(const CGravitySphere& Prototype)
	:CGameObject(Prototype),
	m_desc(Prototype.m_desc)
{
}
CGameObject* CGravitySphere::Clone(void* pArg)
{
	return CloneBase<CGravitySphere>(pArg);
}
HRESULT CGravitySphere::Save(void* _pDesc, _uint& _iSize) const
{
	static_assert(is_trivially_copyable_v<FGRAVITYSPHEREDESC>);
	_iSize = sizeof FGRAVITYSPHEREDESC;
	memcpy(_pDesc, &m_desc, _iSize);
	return S_OK;
}
HRESULT CGravitySphere::Load(void* _pDesc)
{
	static_assert(is_trivially_copyable_v<FGRAVITYSPHEREDESC>);
	_uint _iSize = sizeof FGRAVITYSPHEREDESC;
	memcpy(&m_desc, _pDesc, _iSize);
	return S_OK;
}
void CGravitySphere::Free()
{
	__super::Free();
}
HRESULT CGravitySphere::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg));
	return S_OK;
}
HRESULT CGravitySphere::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc));
	m_upDisappearDelayTimer = m_pGameInstance->CreateTimerWithDuration(2.f, false, [=]() {
			m_upDisappearTimer->Restart();
			m_pWave->DissolveStop(0.2f);
			for (auto pEnemy : m_vecEnemies) {
				pEnemy->MacaronHoldEnd();
				PrintDebug("Macaron Hold Exit!");
			}
			m_vecEnemies.clear();
		}, 
	false);
	m_upDisappearTimer = m_pGameInstance->CreateTimerWithDuration(m_desc.m_fDisAppearTime, false, [=]() {
		SetActive(false);

		}, 
	false);
	m_upAppearTimer = m_pGameInstance->CreateTimerWithDuration(m_desc.m_fApearTime, false, [=]() {

		}, 
	false);
	return S_OK;
}
HRESULT CGravitySphere::Late_Initialize()
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


	CGameObject::LoadPrototype(
		L"VFX_CH2000_Attack_Hold_Ground",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pWave)
	);
	CGameObject::LoadPrototype(
		L"VFX_CH2000_Attack_Hold_Hand",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pHand)
	);
	m_pWave->Stop();
	//m_pWave->SetLevelObject();
	m_pHand->Stop();
	//m_pHand->SetLevelObject();

	SetActive(false);

	return S_OK;
}
HRESULT CGravitySphere::Load_Asset()
{
	CHKFAIL(__super::Load_Asset());

	auto wpComp = Get_Component<CModel>();
	m_wpModel = ConvertWPComponent<CModel>(wpComp);

	SP_Asset _spAsset;
	m_pGameInstance->LoadAsset(_spAsset, L"T_VFX_tk_Barrier_05b_ds");
	m_spGravitySphereTexture[TT_MASK] = static_pointer_cast<CTextureAsset>(_spAsset);

	m_pGameInstance->LoadAsset(_spAsset, L"T_VFX_tk_scratch_06_ds");
	m_spGravitySphereTexture[TT_SCRATCH] = static_pointer_cast<CTextureAsset>(_spAsset);

	return S_OK;
}
HRESULT CGravitySphere::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents());
	CModel::MODEL_DESC _modelDesc;
	lstrcpy(_modelDesc.m_sModelAssetName, L"sphere_00_LOD0.001");

	_modelDesc.m_bIsCopy = false;
	Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);
	Get_Component(L"Com_Model_Main").lock()->SetInitialAdd();

	Add_Component(L"Shader_GravitySphere_Component", L"Com_MeshShader_Main");
	Get_Component(L"Com_MeshShader_Main").lock()->SetInitialAdd();

	return S_OK;
}
void CGravitySphere::Render_Properties()
{
	ImGui::ColorPicker4	("GravitySphereColor0",	reinterpret_cast<_float*>(&m_desc.m_vGravitySphereColor0));
	ImGui::ColorPicker4	("GravitySphereColor1",	reinterpret_cast<_float*>(&m_desc.m_vGravitySphereColor1));

	ImGui::DragFloat2	("Mask UV Scale0",			reinterpret_cast<_float*>(&m_desc.m_vMaskUVScale0),		0.01f);
	ImGui::DragFloat2	("MAsk UV Scale1",			reinterpret_cast<_float*>(&m_desc.m_vMaskUVScale1),		0.01f);
	ImGui::DragFloat2	("Scratch UV Scale",		reinterpret_cast<_float*>(&m_desc.m_vScratchUVScale),	0.01f);
	
	ImGui::DragFloat	("Rim Threshold",			reinterpret_cast<_float*>(&m_desc.m_fGravitySphereRimThreshold),0.001f);
	ImGui::DragFloat	("Shore Depth",			reinterpret_cast<_float*>(&m_desc.m_fGravitySphereShoreDepth),	0.001f);
	ImGui::DragFloat	("Scroll Speed",		reinterpret_cast<_float*>(&m_desc.m_fScrollSpeed),		0.01f);
	
	ImGui::DragFloat	("Shore Alpha",		reinterpret_cast<_float*>(&m_desc.m_fShoreAlpha),			0.01f);
	ImGui::DragFloat	("GravitySphere Alpha",	reinterpret_cast<_float*>(&m_desc.m_fGravitySphereAlpha),		0.01f);

	ImGui::DragFloat	("Rim Intensity0",		 reinterpret_cast<_float*>(&m_desc.m_fRimIntensity0),			0.001f);
	ImGui::DragFloat	("Rim Intensity1",		 reinterpret_cast<_float*>(&m_desc.m_fRimIntensity1),			0.001f);
	ImGui::DragFloat	("Layer Intensity",		 reinterpret_cast<_float*>(&m_desc.m_fLayerIntensity),			0.001f);
	
	ImGui::DragFloat	("Init Scale",			reinterpret_cast<_float*>(&m_desc.m_fInitScale),			0.01f);
	ImGui::DragFloat	("Target Scale",		 reinterpret_cast<_float*>(&m_desc.m_fTargetScale),			0.01f);
	
	if (ImGui::DragFloat("DisapeearTime", reinterpret_cast<_float*>(&m_desc.m_fDisAppearTime), 0.01f)) {
		m_upDisappearTimer->SetDuration(m_desc.m_fDisAppearTime);
	}
	if (ImGui::DragFloat("ApeearTime", reinterpret_cast<_float*>(&m_desc.m_fApearTime), 0.01f)) {
		m_upAppearTimer->SetDuration(m_desc.m_fApearTime);
	}

	if (ImGui::Button("Appear")) {
		Appear(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), true);
	}
	if (ImGui::Button("Disappear")) {
		Disappear();
	}
}

void CGravitySphere::Set_ShaderParam()
{
	CRenderer::SHADERPARAM_DESC _desc;
	_desc.eParamType = CRenderer::EPARAMTYPE::PT_RAWDATA;
	_desc.iSize = sizeof(_float4);

	_desc.sConstantName = "g_vGravitySphereColor0";
	_desc.pData			= &m_desc.m_vGravitySphereColor0;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_vGravitySphereColor1";
	_desc.pData			= &m_desc.m_vGravitySphereColor1;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.iSize = sizeof(_float2);
	_desc.sConstantName = "g_vMaskUVScale0";
	_desc.pData = &m_desc.m_vMaskUVScale0;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_vMaskUVScale1";
	_desc.pData = &m_desc.m_vMaskUVScale1;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_vScratchUVScale";
	_desc.pData = &m_desc.m_vScratchUVScale;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.iSize = sizeof(_float);
	_desc.sConstantName = "g_fGravitySphereRimThreshold";
	_desc.pData = &m_desc.m_fGravitySphereRimThreshold;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fGravitySphereShoreDepth";
	_desc.pData = &m_desc.m_fGravitySphereShoreDepth;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fScrollSpeed";
	_desc.pData = &m_desc.m_fScrollSpeed;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	
	_desc.sConstantName = "g_fGravitySphereAlpha";
	_desc.pData = &m_desc.m_fGravitySphereAlpha;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fShoreAlpha";
	_desc.pData = &m_desc.m_fShoreAlpha;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.sConstantName = "g_fScrollAmount";
	_desc.pData = &m_fScrollAmount;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fScratchAmount";
	_desc.pData = &m_fScratchAmount;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.sConstantName = "g_fRimIntensity0";
	_desc.pData = &m_desc.m_fRimIntensity0;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fRimIntensity1";
	_desc.pData = &m_desc.m_fRimIntensity1;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_fLayerIntensity";
	_desc.pData = &m_desc.m_fLayerIntensity;
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);

	_desc.eParamType	= CRenderer::EPARAMTYPE::PT_SRV;
	_desc.sConstantName = "g_MaskTexture";
	_desc.pData			= (*m_spGravitySphereTexture[TT_MASK])[0];
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
	_desc.sConstantName = "g_ScartchTexture";
	_desc.pData			= (*m_spGravitySphereTexture[TT_SCRATCH])[0];
	m_wpMeshRenderer.lock()->Add_Parameters(_desc);
}
