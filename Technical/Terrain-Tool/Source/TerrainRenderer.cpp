#include "GameInstance.h"
#include "Asset.h"
#include "TerrainRenderer.h"
#include "VIBuffer_Terrain.h"

CTerrainRenderer::CTerrainRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRenderer (pDevice, pContext)
{
}

CTerrainRenderer::CTerrainRenderer(const CTerrainRenderer& Prototype)
	:	CRenderer		( Prototype )
{
}

HRESULT CTerrainRenderer::Initialize_Prototype(void* pDesc)
{
	CHKFAIL(__super::Initialize_Prototype(pDesc))

	return S_OK;
}

HRESULT CTerrainRenderer::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg))

	m_iShaderPassIdx = m_eViewMode;

	return S_OK;
}

SPComponent CTerrainRenderer::Clone(void* pArg)
{
	return CloneBase<CTerrainRenderer>(pArg);
}


void CTerrainRenderer::Free()
{
	__super::Free();
}

void CTerrainRenderer::Render_Inspector()
{
	__super::Render_Inspector();

	ImGui::Combo("ViewMode", &m_eViewMode, VIEWNAMES, 3);
	
}

HRESULT CTerrainRenderer::Save(void* _pDesc, _uint& _iSize) const
{
	_iSize = 0;
	return S_OK;
}

HRESULT CTerrainRenderer::Load(void* _pDesc)
{
	return S_OK;
}

HRESULT CTerrainRenderer::AddRenderGroup()
{
	WPRenderer wpRenderer = dynamic_pointer_cast<CRenderer>(m_wpComponent.lock());
	
	//Select RenderGroup
	// Èì depth prepass´Â »ý·«
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, wpRenderer);

	return S_OK;
}

HRESULT CTerrainRenderer::Bind_Resources()
{
	auto _pObject = static_cast<CGameObject*>(m_pOwnerObj);
	if (!_pObject) return E_FAIL;

	if (!m_wpTransformCom.lock())	m_wpTransformCom	= _pObject->Get_MainTransform();
	if (!m_wpShaderCom.lock())		m_wpShaderCom		= dynamic_pointer_cast<CShader>		(_pObject->Get_Component(L"Com_TerrainShader_Main").lock());;
	if (!m_wpVIbufferCom.lock()) {
		m_wpVIbufferCom = dynamic_pointer_cast<CVIBuffer>	(_pObject->Get_Component(L"Com_VIBuffer_Terrain").lock());
		m_wpTerrain		= dynamic_pointer_cast<CVIBuffer_Terrain>(m_wpVIbufferCom.lock());
	}

	m_iShaderPassIdx	= m_eViewMode;
	m_bWireframeOverlay = (m_eViewMode == WIREFRAME);

	CHKFAIL(__super::Bind_Resources())


	if (auto _spTerrain = m_wpTerrain.lock()) {
		auto _vScale = _spTerrain->GetSplatScale();
		m_wpShaderCom.lock()->Bind_RawData("g_vSplatScale", &_vScale, sizeof _float4);
		m_wpShaderCom.lock()->Bind_ShaderResourceView("g_SplatMapTexture", _spTerrain->GetSplatMap());
		if (auto _spAsset = _spTerrain->Get_TileAsset(0)) {
			auto pAsset = static_cast<CTextureAsset*>(_spAsset.get());
			
			ID3D11ShaderResourceView* pSRVs[3];
			for (size_t i = 0; i < 3; i++) pSRVs[i] = (*pAsset)[i];
			m_wpShaderCom.lock()->Bind_ShaderResourceViewArray("g_SplatTexture0", pSRVs, 3);
		}
		if (auto _spAsset = _spTerrain->Get_TileAsset(1)) {
			auto pAsset = static_cast<CTextureAsset*>(_spAsset.get());

			ID3D11ShaderResourceView* pSRVs[3];
			for (size_t i = 0; i < 3; i++) pSRVs[i] = (*pAsset)[i];
			m_wpShaderCom.lock()->Bind_ShaderResourceViewArray("g_SplatTexture1", pSRVs, 3);
		}
		if (auto _spAsset = _spTerrain->Get_TileAsset(2)) {
			auto pAsset = static_cast<CTextureAsset*>(_spAsset.get());

			ID3D11ShaderResourceView* pSRVs[3];
			for (size_t i = 0; i < 3; i++) pSRVs[i] = (*pAsset)[i];
			m_wpShaderCom.lock()->Bind_ShaderResourceViewArray("g_SplatTexture2", pSRVs, 3);
		}
		if (auto _spAsset = _spTerrain->Get_TileAsset(3)) {
			auto pAsset = static_cast<CTextureAsset*>(_spAsset.get());

			ID3D11ShaderResourceView* pSRVs[3];
			for (size_t i = 0; i < 3; i++) pSRVs[i] = (*pAsset)[i];
			m_wpShaderCom.lock()->Bind_ShaderResourceViewArray("g_SplatTexture3", pSRVs, 3);
		}
	}
		

	CHKFAIL(m_pGameInstance->Bind_TransformStateMatrix(m_wpShaderCom.lock().get(), "g_ViewMatrix", D3DTS::VIEW))
	CHKFAIL(m_pGameInstance->Bind_TransformStateMatrix(m_wpShaderCom.lock().get(), "g_ProjMatrix", D3DTS::PROJ))
	CHKFAIL(m_pGameInstance->Bind_CamPosition(m_wpShaderCom.lock().get(), "g_camPosition"))
	return S_OK;
}

weak_ptr<CVIBuffer_Terrain> CTerrainRenderer::Get_Terrain()
{
	return m_wpTerrain;
}
