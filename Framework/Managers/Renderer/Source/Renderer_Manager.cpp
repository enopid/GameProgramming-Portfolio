#include"Renderer_Manager.h"
#include"GameInstance.h"
#include"ModuleManager.h"

#include"Mesh.h"
#include"MeshRenderer.h"

Engine::CRenderer_Manager::CRenderer_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Engine::Safe_AddRef(m_pDevice);
	Engine::Safe_AddRef(m_pContext);
	Engine::Safe_AddRef(m_pGameInstance);
}
HRESULT Engine::CRenderer_Manager::Initialize()
{
	Engine::_uint uiNumViewports = { 1 };
	D3D11_VIEWPORT Desc = {};
	m_pContext->RSGetViewports(&uiNumViewports, &Desc);

	// RenderTarget
	// OBJ - Common
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetDiffuseB", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetNormal", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R8G8B8A8_SNORM, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetDepth", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R32_FLOAT, Engine::_float4(1.f, 1.f, 0.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetEmissive", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R11G11B10_FLOAT, Engine::_float4(0.f, 0.f, 0.f, 1.f)));
	// OBJ - NonAnim
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetELSR", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, Engine::_float4(0.f, 0.f, 0.f, 0.f)));	// EdgeLine, Specular, Roughness
	// Glass
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetGlass", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R8_SNORM, Engine::_float4(0.f, 0.f, 0.f, 1.f)));
	// OBJ - CHR
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetV", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R8_SNORM, Engine::_float4(0.f, 0.f, 0.f, 1.f)));
	// Particle
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetAccum", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetReveal", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R8_UNORM, Engine::_float4(1.f, 1.f, 1.f, 1.f)));
	// UNLIT
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetUnlit", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	// Renderer
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetPriority", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetOutline", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R8_UNORM, Engine::_float4(0.f, 0.f, 0.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetSShadow", m_uiSShadowSize, m_uiSShadowSize, DXGI_FORMAT_R32_FLOAT, Engine::_float4(1.f, 1.f, 1.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetDShadow", m_uiDShadowSize, m_uiDShadowSize, DXGI_FORMAT_R32_FLOAT, Engine::_float4(1.f, 1.f, 1.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetLight", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetScene", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetSSR", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetAfter", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R11G11B10_FLOAT, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetEffectB", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R8_UNORM, Engine::_float4(1.f, 0.f, 0.f, 0.f)));

	// MRT
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_CHR", L"TargetDiffuseB"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_CHR", L"TargetNormal"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_CHR", L"TargetDepth"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_CHR", L"TargetEmissive"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_CHR", L"TargetV"));

	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_GLS", L"TargetGlass"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_GLS", L"TargetELSR"));

	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_OBJ", L"TargetDiffuseB"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_OBJ", L"TargetNormal"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_OBJ", L"TargetDepth"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_OBJ", L"TargetEmissive"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_OBJ", L"TargetELSR"));

	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_PTC", L"TargetAccum"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_PTC", L"TargetReveal"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_PTC", L"TargetDepth"));

	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_ULT", L"TargetUnlit"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_ULT", L"TargetDepth"));

	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_EFT", L"TargetScene"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_EFT", L"TargetEmissive"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_EFT", L"TargetEffectB"));

	//Blur 
	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"Target_Bloom", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, Engine::_float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP0"), (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP1"), (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS0A"), (Engine::_uint)(Desc.Width / 2.f), (Engine::_uint)(Desc.Height / 2.f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS1A"), (Engine::_uint)(Desc.Width / 4.f), (Engine::_uint)(Desc.Height / 4.f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS2A"), (Engine::_uint)(Desc.Width / 8.f), (Engine::_uint)(Desc.Height / 8.f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS3A"), (Engine::_uint)(Desc.Width / 16.f), (Engine::_uint)(Desc.Height / 16.f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));

	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS0B"), (Engine::_uint)(Desc.Width / 2.f), (Engine::_uint)(Desc.Height / 2.f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS1B"), (Engine::_uint)(Desc.Width / 4.f), (Engine::_uint)(Desc.Height / 4.f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS2B"), (Engine::_uint)(Desc.Width / 8.f), (Engine::_uint)(Desc.Height / 8.f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));
	CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS3B"), (Engine::_uint)(Desc.Width / 16.f), (Engine::_uint)(Desc.Height / 16.f), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));

	CHKFAIL(m_pGameInstance->Add_RenderTarget(L"TargetX", (Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)));

	m_Viewports[0] = Desc;
	for (size_t i = 0; i < 5; i++)
	{
		XMStoreFloat4x4(&m_matDSWorlds[i], XMMatrixScaling(m_Viewports[i].Width, m_Viewports[i].Height, 1.f));
		XMStoreFloat4x4(&m_matDSProjections[i], XMMatrixOrthographicLH(m_Viewports[i].Width, m_Viewports[i].Height, 0.f, 1.f));

		if (i == 4) break;

		m_Viewports[i + 1] = m_Viewports[i];
		m_Viewports[i + 1].Width /= 2;
		m_Viewports[i + 1].Height /= 2;
	}

	if (FAILED(ReadyShadowDepthStencil()))
		return E_FAIL;
	DirectX::XMStoreFloat4x4(&m_matWorld, DirectX::XMMatrixScaling(Desc.Width, Desc.Height, 1.f));
	DirectX::XMStoreFloat4x4(&m_matView, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_matProjection, DirectX::XMMatrixOrthographicLH(Desc.Width, Desc.Height, 0.f, 1.f));

	m_pShader = { Engine::CShader::Create<VTXPOSTEX>(m_pDevice, m_pContext, L"../Bin/Shaders/Shader_Deferred.hlsl") };
	if (!m_pShader)
		return E_FAIL;
	m_pSSShader = { Engine::CShader::Create<VTXPOSTEX>(m_pDevice, m_pContext, L"../Bin/Shaders/Shader_ScreenSpace.hlsl") };
	if (!m_pSSShader) return E_FAIL;

	m_pPPShader = { Engine::CShader::Create<VTXPOSTEX>(m_pDevice, m_pContext, L"../Bin/Shaders/Shader_PostProcessing.hlsl") };
	if (!m_pPPShader) return E_FAIL;

	m_pVIBuffer = { Engine::CVIBuffer_Rect::Create<CVIBuffer_Rect>(m_pDevice,m_pContext) };
	if (!m_pVIBuffer)
		return E_FAIL;

	InitRSStates();
	InitBSStates();

	m_upDistortionTimer = { m_pGameInstance->CreateTimerWithDuration(1.f,false,nullptr,false) };

	SetAdditionalWeights();

	//CHKFAIL(ReadyWholeRT());	//only whole RT

	return S_OK;
}

void CRenderer_Manager::InitRSStates()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.DepthClipEnable = TRUE;
	desc.FrontCounterClockwise = FALSE;

	desc.CullMode = D3D11_CULL_FRONT;
	m_pDevice->CreateRasterizerState(&desc, &m_pRS_CullFront);

	desc.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&desc, &m_pRS_CullBack);

	desc.CullMode = D3D11_CULL_NONE;
	m_pDevice->CreateRasterizerState(&desc, &m_pRS_CullNone);
}

void CRenderer_Manager::InitBSStates()
{
	D3D11_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;

	// 기본 (Opaque)
	desc.RenderTarget[0].BlendEnable = FALSE;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pDevice->CreateBlendState(&desc, &m_pBS_Opaque);

	desc = {};
	// Alpha Blend
	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	m_pDevice->CreateBlendState(&desc, &m_pBS_AlphaBlend);

	desc = {};
	// Additive
	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pDevice->CreateBlendState(&desc, &m_pBS_Additive);
}

HRESULT Engine::CRenderer_Manager::Add_RenderGroup(RENDERGROUP eGroupID, WPRenderer wpRenderer)
{
	Engine::_uint uiIndex = { ENUM_TO_UINT(eGroupID) };
	if (ENUM_TO_UINT(RENDERGROUP::END) <= uiIndex)
		return E_FAIL;
	m_listRenderer[uiIndex].push_back(wpRenderer);
	return S_OK;
}
HRESULT Engine::CRenderer_Manager::Add_SSGroup(SCREENSPACE eGroupID, WPRenderer wpRenderer)
{
	Engine::_uint uiIndex = { ENUM_TO_UINT(eGroupID) };
	if (ENUM_TO_UINT(SCREENSPACE::END) <= uiIndex)
		return E_FAIL;
	m_listSSRenderer[uiIndex].push_back(wpRenderer);
	return S_OK;
}
HRESULT CRenderer_Manager::Remove_RenderGroup(RENDERGROUP eGroupID, WPRenderer wpRenderer)
{
	Engine::_uint uiIndex = { ENUM_TO_UINT(eGroupID) };
	if (ENUM_TO_UINT(RENDERGROUP::END) <= uiIndex)
		return E_FAIL;
	m_listRenderer[uiIndex].remove_if([=](WPRenderer _pRenderer) {return _pRenderer.lock() == wpRenderer.lock(); });
	return S_OK;
}
HRESULT Engine::CRenderer_Manager::Change_RenderGroup(RENDERGROUP eGroupID, WPRenderer wpRenderer)
{
	Engine::_uint uiIndex = { ENUM_TO_UINT(eGroupID) };
	if (ENUM_TO_UINT(RENDERGROUP::END) <= uiIndex)
		return E_FAIL;
	for (size_t i = 0; i < ENUM_TO_UINT(RENDERGROUP::END); i++) m_listRenderer[i].remove_if([=](WPRenderer _pRenderer) {return _pRenderer.lock() == wpRenderer.lock(); });
	m_listRenderer[uiIndex].push_back(wpRenderer);
	return S_OK;
}
void Engine::CRenderer_Manager::Draw()
{
	Update();

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_OBJ"))) return;
	Render(RENDERGROUP::NONBLEND_OBJECT);
	if (FAILED(m_pGameInstance->End_MRT())) return;

	if (FAILED(m_pGameInstance->Clear_RT(L"TargetGlass")))return;
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_GLS", true, false)))return;
	Render(RENDERGROUP::GLASS);
	if (FAILED(m_pGameInstance->End_MRT()))return;

	RenderOutLine();

	//Reset Chr RT
	if (FAILED(m_pGameInstance->Clear_RT(L"TargetV")))return;
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_CHR", true, false))) return;
	Render(RENDERGROUP::NONBLEND_CHARACTER);
	if (FAILED(m_pGameInstance->End_MRT())) return;

	RenderShadow();
	BindCombined();
	RenderLight();

	if (FAILED(m_pGameInstance->Begin_RT(L"TargetPriority")))
		return;
	Render(RENDERGROUP::PRIORITY);
	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	RenderCombined();
	RenderSSAO();
	ZSort();

	if (FAILED(m_pGameInstance->Clear_RT(L"TargetAccum")))
		return;
	if (FAILED(m_pGameInstance->Clear_RT(L"TargetReveal")))
		return;
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_PTC", true, false)))
		return;
	Render(RENDERGROUP::PARTICLE);
	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	if (FAILED(m_pGameInstance->Clear_RT(L"TargetUnlit")))
		return;
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_ULT", true, false)))
		return;
	Render(RENDERGROUP::UNLIT);
	RenderParticle();
	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	RenderAfterimage();
	RenderEffect();
	RenderBlend();
	RenderSSR();

	if (FAILED(m_pGameInstance->Begin_RT(L"TargetScene", true, false)))
		return;
	RenderLateBlend();
	Render(RENDERGROUP::BLEND);
	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	RenderBloom(L"TargetEmissive");
	if (FAILED(m_pGameInstance->Begin_RT(L"Target_PP0"))) return;
	RenderOL(L"TargetScene");
	if (FAILED(m_pGameInstance->End_MRT())) return;
	if (m_bExplosion)
		RenderAdditionalBlur();
	RenderPPCombine(L"Target_PP0");

	Render(RENDERGROUP::UI);

	if (FAILED(m_pGameInstance->Clear_RT(L"TargetAccum")))
		return;
	if (FAILED(m_pGameInstance->Clear_RT(L"TargetReveal")))
		return;
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_PTC", true, false)))
		return;
	Render(RENDERGROUP::LATE_PTC);
	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	if (FAILED(m_pGameInstance->Clear_RT(L"TargetUnlit")))
		return;
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_ULT", true, false)))
		return;
	Render(RENDERGROUP::LATE_EFT);
	RenderParticle();
	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	RenderLateEFT();

	Render(RENDERGROUP::LATE_UI);

	//RenderDebug();
}
void Engine::CRenderer_Manager::DrawStaticShadow()
{
	//m_uiSShadowCount = { 1 };
}
void CRenderer_Manager::Bind_RSParameters(RSPARAM_DESC _desc)
{
	ID3D11RasterizerState* pRS = nullptr;

	if (_desc.iCullType == 0)       // Cull Front
		pRS = m_pRS_CullFront;
	else if (_desc.iCullType == 1)  // Cull Back
		pRS = m_pRS_CullBack;
	else if (_desc.iCullType == 2)  // Cull None
		pRS = m_pRS_CullNone;

	m_pContext->RSSetState(pRS);

	ID3D11BlendState* pBS = nullptr;
	if (_desc.iBlendType == 0)  // Opaque
		pBS = m_pBS_Opaque;
	else if (_desc.iBlendType == 1)  // AlphaBlend
		pBS = m_pBS_AlphaBlend;
	else if (_desc.iBlendType == 2)  // Additive
		pBS = m_pBS_Additive;

	float blendFactor[4] = { 0, 0, 0, 0 };
	m_pContext->OMSetBlendState(pBS, blendFactor, 0xffffffff);
}
std::vector<Engine::_wstring>Engine::CRenderer_Manager::GetName()
{
	std::vector<Engine::_wstring>vecRMName;
	std::filesystem::path path = { m_pGameInstance->ReadConfig<Engine::_wstring>(L"Asset.Paths.RMPath") };
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		vecRMName.push_back(entry.path().filename().replace_extension());
	return vecRMName;
}
Engine::CRenderer_Manager::RM_DESC* Engine::CRenderer_Manager::GetDesc()
{
	return &m_tDesc;
}
HRESULT Engine::CRenderer_Manager::Save(const Engine::_wstring& wstrFileName)
{
	std::filesystem::path path = { m_pGameInstance->ReadConfig<Engine::_wstring>(L"Asset.Paths.RMPath") };
	std::filesystem::path tmpPath = { path };
	path += wstrFileName + L".RM";
	tmpPath += L"RMSave";
	tmpPath += L".tmp";
	std::ios_base::openmode mode = { std::ios::out | std::ios::binary | std::ios::trunc };
	std::fstream fs;
	fs.open(tmpPath, mode);
	if (!fs.is_open())
	{
		Engine::_wstring msg = path;
		msg = { L"Failed to Save File : (" + msg + L")" };
		MSG_BOXL(msg.c_str());
		return E_FAIL;
	}
	fs.write((Engine::_char*)&m_tDesc, sizeof(m_tDesc));
	fs.close();
	std::filesystem::rename(tmpPath, path);
	return S_OK;
}
HRESULT Engine::CRenderer_Manager::Load(const Engine::_wstring& wstrFileName)
{
	std::filesystem::path path = { m_pGameInstance->ReadConfig<Engine::_wstring>(L"Asset.Paths.RMPath") };
	path += wstrFileName + L".RM";
	std::ios_base::openmode mode = { std::ios::in | std::ios::binary };
	std::fstream fs;
	fs.open(path, mode);
	if (!fs.is_open())
	{
		Engine::_wstring msg = path;
		msg = { L"Failed to Load File : (" + msg + L")" };
		MSG_BOXL(msg.c_str());
		return E_FAIL;
	}
	fs.read((Engine::_char*)&m_tDesc, sizeof(m_tDesc));
	fs.close();
	ChangeWeights();
	m_f3PurDiffuse = { m_tDesc.f3LightDiffuse };
	return S_OK;
}
void CRenderer_Manager::SetTimeDelta(Engine::_float fTimeDelta)
{
	m_fTimeDelta = { fTimeDelta };
}
void CRenderer_Manager::SetAfterVelocity(Engine::_float2 f2Velocity)
{
	m_f2Velocity = { f2Velocity };
}
void CRenderer_Manager::SetKaleAfterVelocity(Engine::_float2 f2Velocity)
{
	m_f2KaleVelo = { f2Velocity };
}
//void CRenderer_Manager::SetCamDistortion(Engine::_float fDistortionStrength, Engine::_float3 f3CamVelo)
//{
//	m_fCamStrength = { fDistortionStrength };
//	m_f3CamVelo = { f3CamVelo };
//}
void CRenderer_Manager::SetDistortion(DISTORTION_DESC desc)
{
	m_tDistortionDesc = { desc };
	m_upDistortionTimer->SetDuration(m_tDistortionDesc.fDuration);
	m_upDistortionTimer->Restart();
}
void CRenderer_Manager::AddWave(WAVE_DESC Desc)
{
	m_listWave.push_back(Desc);
	m_listWaveTime.push_back(0.f);
	if (m_listWave.size() > m_uiWaveLimit)
	{
		m_listWave.pop_front();
		m_listWaveTime.pop_front();
	}
}
void Engine::CRenderer_Manager::SetColorReverse(Engine::_float fReverse)
{
	m_fReverse = { fReverse };
}

void CRenderer_Manager::ChangeWeights()
{
	for (Engine::_int i = { 0 }; i < 5; ++i)
		m_fWeights[i] = { std::expf(-(i * i) / (2.f * m_tDesc.fSigma * m_tDesc.fSigma)) };
	if (m_pPPShader)
		m_pPPShader->Bind_RawData("weights", m_fWeights, sizeof(m_fWeights));
}

void CRenderer_Manager::SetAdditionalWeights()
{
	for (Engine::_int i = { 0 }; i < 8; ++i)
	{
		m_fShallowWeights[i] = { std::expf(-(i * i) / 18.f) };
		m_fDeepWeights[i] = { std::expf(-(i * i) / 50.f) };
	}
	if (FAILED(m_pPPShader->Bind_RawData("g_fShallowWeights", m_fShallowWeights, sizeof(m_fShallowWeights))))return;
	if (FAILED(m_pPPShader->Bind_RawData("g_fDeepWeights", m_fDeepWeights, sizeof(m_fDeepWeights))))return;
}

void CRenderer_Manager::SetRMDiffuse(Engine::_float3 f3Diffuse)
{
	m_tDesc.f3LightDiffuse = { f3Diffuse };
}

void CRenderer_Manager::SetLowHP(Engine::_bool bLowHP)
{
	m_bLowHP = { bLowHP };
	if (!bLowHP)
		m_fLowHP = { 0.f };
}

void CRenderer_Manager::SetExplosion(Engine::_bool bExplosion)
{
	m_bExplosion = { bExplosion };
}

HRESULT Engine::CRenderer_Manager::ReadyShadowDepthStencil()
{
	ID3D11Texture2D* pDepthStencilTexture = { nullptr };
	D3D11_TEXTURE2D_DESC Desc = {};
	Desc.Width = Desc.Height = { m_uiSShadowSize };
	Desc.MipLevels = { 1 };
	Desc.ArraySize = { 1 };
	Desc.Format = { DXGI_FORMAT_D24_UNORM_S8_UINT };
	Desc.SampleDesc.Count = { 1 };
	Desc.BindFlags = { D3D11_BIND_DEPTH_STENCIL };
	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV[ENUM_TO_UINT(SHADOW::S)])))
		return E_FAIL;
	m_pGameInstance->Clear_RT(L"TargetSShadow");
	Safe_Release(pDepthStencilTexture);
	Desc.Width = Desc.Height = { m_uiDShadowSize };
	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV[ENUM_TO_UINT(SHADOW::D)])))
		return E_FAIL;
	Safe_Release(pDepthStencilTexture);
	return S_OK;
}
void Engine::CRenderer_Manager::ChangeViewport(Engine::_uint uiWidth, Engine::_uint uiHeight)
{
	D3D11_VIEWPORT Desc = {};
	Desc.Width = { (Engine::_float)uiWidth };
	Desc.Height = { (Engine::_float)uiHeight };
	Desc.MaxDepth = { 1.f };
	m_pContext->RSSetViewports(1, &Desc);
}
void Engine::CRenderer_Manager::ZSort()
{
	std::function function = { [](const WPRenderer& wpRenderer1, const WPRenderer& wpRenderer2) {
		auto spRenderer1 = wpRenderer1.lock();
		auto spRenderer2 = wpRenderer2.lock();
		if (!spRenderer1)return false;
		if (!spRenderer2)return true;
		return spRenderer1->GetPriority() < spRenderer2->GetPriority();
		} };
	//m_listRenderer[ENUM_TO_UINT(RENDERGROUP::PARTICLE)].sort(function);
	m_listRenderer[ENUM_TO_UINT(RENDERGROUP::UNLIT)].sort(function);
	m_listRenderer[ENUM_TO_UINT(RENDERGROUP::BLEND)].sort(function);
	m_listRenderer[ENUM_TO_UINT(RENDERGROUP::UI)].sort(function);
	m_listRenderer[ENUM_TO_UINT(RENDERGROUP::LATE_UI)].sort(function);
}
void Engine::CRenderer_Manager::RenderShadow()
{
	_uint uiNumViewports = { 1 };
	D3D11_VIEWPORT Desc = {};
	m_pContext->RSGetViewports(&uiNumViewports, &Desc);
	ID3D11ShaderResourceView* nullSRV[16] = {};
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetSShadow", false, false, m_pShadowDSV[ENUM_TO_UINT(SHADOW::S)])))
		return;
	ChangeViewport(m_uiSShadowSize, m_uiSShadowSize);
	Render(RENDERGROUP::SSHADOW);
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetSShadow", m_pShader, "g_SShadowTexture")))
		return;
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetDShadow", false, true, m_pShadowDSV[ENUM_TO_UINT(SHADOW::D)])))
		return;
	ChangeViewport(m_uiDShadowSize, m_uiDShadowSize);
	Render(RENDERGROUP::DSHADOW);
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDShadow", m_pShader, "g_DShadowTexture")))
		return;
	ChangeViewport((Engine::_uint)Desc.Width, (Engine::_uint)Desc.Height);
}
void Engine::CRenderer_Manager::BindCombined()
{
	//Shader Resource View
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDiffuseB", m_pShader, "g_DiffuseBTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetNormal", m_pShader, "g_NormalTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDepth", m_pShader, "g_DepthTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetELSR", m_pShader, "g_ELSRTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetGlass", m_pShader, "g_GlassTexture")))
		return;
	// Matrix
	Engine::_float4x4 f44 = {};
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_matView)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_matProjection)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_matCamView", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_matCamProj", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_matViewInverse", m_pGameInstance->Get_InverseTransform(D3DTS::VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_matProjectionInverse", m_pGameInstance->Get_InverseTransform(D3DTS::PROJ))))
		return;
	Engine::_matrix mat = { DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(m_pGameInstance->Get_InverseTransform(Engine::D3DTS::PROJ)),
		DirectX::XMLoadFloat4x4(m_pGameInstance->Get_InverseTransform(Engine::D3DTS::VIEW))) };
	DirectX::XMStoreFloat4x4(&f44, mat);
	if (FAILED(m_pShader->Bind_Matrix("g_matPVInverse", &f44)))
		return;
	mat = { DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(m_pGameInstance->GetShadowTransform(Engine::D3DTS::VIEW,Engine::SHADOW::S)),
		DirectX::XMLoadFloat4x4(m_pGameInstance->GetShadowTransform(Engine::D3DTS::PROJ,Engine::SHADOW::S))) };
	DirectX::XMStoreFloat4x4(&f44, mat);
	if (FAILED(m_pShader->Bind_Matrix("g_matSLightVP", &f44)))
		return;
	mat = { DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(m_pGameInstance->GetShadowTransform(Engine::D3DTS::VIEW,Engine::SHADOW::D)),
		DirectX::XMLoadFloat4x4(m_pGameInstance->GetShadowTransform(Engine::D3DTS::PROJ,Engine::SHADOW::D))) };
	DirectX::XMStoreFloat4x4(&f44, mat);
	if (FAILED(m_pShader->Bind_Matrix("g_matDLightVP", &f44)))
		return;
	mat = { DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(
		DirectX::XMLoadFloat4x4(&m_matWorld),DirectX::XMLoadFloat4x4(&m_matView)),DirectX::XMLoadFloat4x4(&m_matProjection)) };
	DirectX::XMStoreFloat4x4(&f44, mat);
	if (FAILED(m_pShader->Bind_Matrix("g_matWVP", &f44)))
		return;
	mat = { DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&m_matView),DirectX::XMLoadFloat4x4(&m_matProjection)) };
	DirectX::XMStoreFloat4x4(&f44, mat);
	if (FAILED(m_pShader->Bind_Matrix("g_matVP", &f44)))
		return;
	// float4
	if (FAILED(m_pShader->Bind_RawData("g_f4CameraPosition", m_pGameInstance->Get_CamPosition(), sizeof(Engine::_float4))))
		return;
}
void Engine::CRenderer_Manager::RenderLight()
{
	if (FAILED(m_pGameInstance->BindShutDown(m_pPPShader)))
		return;
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetLight")))
		return;
	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
void Engine::CRenderer_Manager::RenderCombined()
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetScene", false, false)))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetLight", m_pShader, "g_LightTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetPriority", m_pShader, "g_PriorityTexture")))
		return;
	//if (FAILED(m_pGameInstance->BindGI(m_pShader)))
	//	return;
	// float3
	SHADOW_DESC* Desc = { m_pGameInstance->GetShadowDesc() };
	if (FAILED(m_pShader->Bind_RawData("g_f3LightDirection", &Desc->f3LightDirection, sizeof(Engine::_float3))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_f3LightDiffuse", &m_tDesc.f3LightDiffuse, sizeof(Engine::_float3))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_f3LightAmbient", &m_tDesc.f3LightAmbient, sizeof(Engine::_float3))))
		return;
	// float
	if (FAILED(m_pShader->Bind_RawData("g_fLightIntensity", &m_tDesc.fLightIntensity, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fJudgeShadow", &m_tDesc.fJudgeShade, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fShadeBlend", &m_tDesc.fShadeBlend, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fJudgePCF", &m_tDesc.fDJudgePCF, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fSShadowBias", &m_tDesc.fSShadowBias, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fDShadowBias", &m_tDesc.fDShadowBias, sizeof(Engine::_float))))
		return;
	Engine::_float fSize = (Engine::_float)m_uiSShadowSize;
	if (FAILED(m_pShader->Bind_RawData("g_fSShadowSize", &fSize, sizeof(Engine::_float))))
		return;
	fSize = (Engine::_float)m_uiDShadowSize;
	if (FAILED(m_pShader->Bind_RawData("g_fDShadowSize", &fSize, sizeof(Engine::_float))))
		return;
	// Render
	if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::COMBINED))))
		return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
void Engine::CRenderer_Manager::RenderSSR()
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetSSR", false)))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetScene", m_pShader, "g_SceneTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetEmissive", m_pShader, "g_EmissiveTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDepth", m_pShader, "g_DepthTexture")))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fDensityOfDots", &m_tDesc.fDensityOfDotsInDoor, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fHalftoneRadius", &m_tDesc.fHalftoneRadius, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_uiMaxSteps", &m_tDesc.SSRDesc.uiMaxSteps, sizeof(Engine::_uint))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_uiBinarySteps", &m_tDesc.SSRDesc.uiBinarySteps, sizeof(Engine::_uint))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fMaxDistance", &m_tDesc.SSRDesc.fMaxDistance, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fStride", &m_tDesc.SSRDesc.fStride, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fThickness", &m_tDesc.SSRDesc.fThickness, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fRayBias", &m_tDesc.SSRDesc.fRayBias, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::SSR))))
		return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
void Engine::CRenderer_Manager::RenderSSAO()
{
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetScene", true, false)))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fAOBias", &m_tDesc.SSAODesc.fAOBias, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fAORadius", &m_tDesc.SSAODesc.fAORadius, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fAOThickness", &m_tDesc.SSAODesc.fAOThickness, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fDensityOfLine", &m_tDesc.SSAODesc.fDensityOfLine, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::SSAO))))
		return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
void Engine::CRenderer_Manager::RenderAfterimage()
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetAfter")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetV", m_pShader, "g_VelocityTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetScene", m_pShader, "g_SceneTexture")))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fAfterSpeed", &m_tDesc.AfterDesc.fAfterSpeed, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_f2AfterVelocity", &m_f2Velocity, sizeof(Engine::_float2))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fAfterAlpha", &m_tDesc.AfterDesc.fAfterAlpha, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_uiAfterCount", &m_uiZero, sizeof(Engine::_uint))))
		return;
	for (Engine::_uint i = { ENUM_TO_UINT(AFTER::GREEN) }; i < ENUM_TO_UINT(AFTER::END); ++i)
	{
		if (FAILED(m_pShader->Bind_RawData("g_uiAfterColor", &i, sizeof(Engine::_uint))))
			return;
		if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::AFTER))))
			return;
		if (FAILED(m_pVIBuffer->Bind_Resources()))
			return;
		if (FAILED(m_pVIBuffer->Render()))
			return;
	}
	// Kale
	if (FAILED(m_pShader->Bind_RawData("g_f2AfterVelocity", &m_f2KaleVelo, sizeof(Engine::_float2))))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_uiAfterColor", &m_uiZero, sizeof(Engine::_uint))))
		return;
	Engine::_uint uiPibo = { 0 };
	for (Engine::_uint i = { 1 }; i <= 7; ++i)
	{
		uiPibo += i;
		if (FAILED(m_pShader->Bind_RawData("g_uiAfterCount", &uiPibo, sizeof(Engine::_uint))))
			return;
		if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::AFTER))))
			return;
		if (FAILED(m_pVIBuffer->Bind_Resources()))
			return;
		if (FAILED(m_pVIBuffer->Render()))
			return;
	}
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
void Engine::CRenderer_Manager::RenderBlend()
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetScene", true, false)))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetAfter", m_pShader, "g_AfterTexture")))
		return;
	if (FAILED(m_pShader->Bind_RawData("g_fAfterAlpha", &m_tDesc.AfterDesc.fAfterAlpha, sizeof(Engine::_float))))
		return;
	if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::BLEND))))
		return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
void CRenderer_Manager::RenderLateBlend()
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetSSR", m_pShader, "g_SSRTexture")))
		return;
	if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::LATE_BLEND))))
		return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;
}
void CRenderer_Manager::RenderParticle()
{
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetAccum", m_pShader, "g_AccumTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetReveal", m_pShader, "g_RevealTexture")))
		return;
	if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::WEIGHT))))
		return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;
}
void CRenderer_Manager::RenderEffect()
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetEffectB")))
		return;
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_EFT", true, false)))
		return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetUnlit", m_pShader, "g_UnlitTexture")))
		return;
	if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::EFFECT))))
		return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;
	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}
void Engine::CRenderer_Manager::RenderOutLine()
{
	if (FAILED(m_pGameInstance->Begin_RT(TEXT("TargetOutline")))) return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDepth", m_pSSShader, "g_DepthTexture")))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("TargetNormal"), m_pSSShader, "g_NormalTexture"))) return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("TargetELSR"), m_pSSShader, "g_ELSRTexture"))) return;
	auto _OLDesc = m_tDesc.OLDesc;

	if (!m_bUseNormalOL)
		_OLDesc.vNormalEdgeProperty.x = 0.f;
	if (!m_bUseDepthOL)
		_OLDesc.vDepthEdgeProperty.x = 0.f;

	if (FAILED(m_pGameInstance->Bind_TransformInverseStateMatrix(m_pSSShader, "g_ProjMatrixInv", D3DTS::PROJ))) return;

	if (FAILED(m_pSSShader->Bind_RawData("g_vDepthEdgeProperty", &_OLDesc.vDepthEdgeProperty, sizeof(Engine::_float4))))				return;
	if (FAILED(m_pSSShader->Bind_RawData("g_vNormalEdgeProperty", &_OLDesc.vNormalEdgeProperty, sizeof(Engine::_float4))))				return;
	if (FAILED(m_pSSShader->Bind_RawData("g_vELEdgeProperty", &_OLDesc.vELEdgeProperty, sizeof(Engine::_float4))))				return;


	if (FAILED(m_pSSShader->Bind_Matrix("g_WorldMatrix", &m_matWorld)))  return;
	if (FAILED(m_pSSShader->Bind_Matrix("g_ViewMatrix", &m_matView)))    return;
	if (FAILED(m_pSSShader->Bind_Matrix("g_ProjMatrix", &m_matProjection)))    return;


	if (FAILED(m_pSSShader->Begin(ENUM_TO_UINT(SCREENSPACE::OUTLINE)))) return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))                      return;
	if (FAILED(m_pVIBuffer->Render()))                              return;

	if (FAILED(m_pGameInstance->End_MRT())) return;
}
void CRenderer_Manager::RenderOL(const wstring& sCurPPScene)
{
	if (FAILED(m_pGameInstance->Bind_RT_SRV(sCurPPScene, m_pPPShader, "g_SceneTexture"))) return;
	if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::COPY)))) return;
	if (FAILED(m_pVIBuffer->Bind_Resources())) return;
	if (FAILED(m_pVIBuffer->Render())) return;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetOutline", m_pPPShader, "g_OLTexture"))) return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetELSR", m_pPPShader, "g_ELSRTexture"))) return;

	auto _OLDesc = m_tDesc.OLDesc;
	if (FAILED(m_pPPShader->Bind_RawData("g_vEdgeColor", &_OLDesc.vEdgeColor, sizeof(Engine::_float3))))			return;

	if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_matWorld))) return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_matView))) return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_matProjection))) return;

	if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::OUTLINE)))) return;
	if (FAILED(m_pVIBuffer->Bind_Resources())) return;
	if (FAILED(m_pVIBuffer->Render())) return;

	return;
}
void Engine::CRenderer_Manager::RenderBloom(const wstring& sCurPPScene)
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	_float2 vTexelSize;
	vTexelSize.x = 1.f / m_Viewports[0].Width;
	vTexelSize.y = 1.f / m_Viewports[0].Height;

	//ExtractBloom
	if (FAILED(m_pGameInstance->Begin_RT(L"Target_PP0"))) return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDepth", m_pPPShader, "g_DepthTexture")))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(sCurPPScene, m_pPPShader, "g_SceneTexture"))) return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_matWorld)))  return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_matView)))    return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_matProjection)))    return;

	if (FAILED(m_pPPShader->Bind_RawData("g_fBloomThreshold", &m_tDesc.BloomDesc.fEmissiveThreshold, sizeof(Engine::_float))))return;
	if (FAILED(m_pPPShader->Bind_RawData("g_fBlurDepthLimit", &m_tDesc.BloomDesc.fBlurDepthLimit, sizeof(Engine::_float))))return;

	if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::BLOOMEXTRACT)))) return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))                                  return;
	if (FAILED(m_pVIBuffer->Render()))                                          return;

	if (FAILED(m_pGameInstance->End_MRT())) return;

	int idx = 0;
	//DownSample0
	for (auto [_RTTag, TextureRTTag] : {
		pair(L"Target_PP_DS0A", L"Target_PP0"),
		pair(L"Target_PP_DS1A", L"Target_PP_DS0A") ,
		pair(L"Target_PP_DS2A", L"Target_PP_DS1A") ,
		pair(L"Target_PP_DS3A", L"Target_PP_DS2A") })
	{
		idx++;
		m_pContext->RSSetViewports(1, &m_Viewports[idx]);

		m_pContext->PSSetShaderResources(0, 16, nullSRV);
		if (FAILED(m_pGameInstance->Begin_RT(_RTTag, false))) return;
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TextureRTTag, m_pPPShader, "g_TargetSampleTexture")))  return;
		m_pPPShader->Bind_RawData("g_vTexelSize", &vTexelSize, sizeof(_float2));


		if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_matDSWorlds[idx])))		return;
		if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_matView)))				return;
		if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_matDSProjections[idx]))) return;

		if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::DOWNSAMPLE))))   return;
		if (FAILED(m_pVIBuffer->Bind_Resources()))                                  return;
		if (FAILED(m_pVIBuffer->Render()))                                          return;

		if (FAILED(m_pGameInstance->End_MRT()))  return;
		vTexelSize.x *= 2.f;
		vTexelSize.y *= 2.f;
	}
	m_pContext->RSSetViewports(1, &m_Viewports[0]);
	m_pContext->PSSetShaderResources(0, 16, nullSRV);

	//Blur
	vTexelSize.x = 1.f / m_Viewports[0].Width;
	vTexelSize.y = 1.f / m_Viewports[0].Height;
	idx = 0;
	for (auto [_RTTag0, _RTTag1] : {
		pair(L"Target_PP_DS0A", L"Target_PP_DS0B"),
		pair(L"Target_PP_DS1A", L"Target_PP_DS1B") ,
		pair(L"Target_PP_DS2A", L"Target_PP_DS2B") ,
		pair(L"Target_PP_DS3A", L"Target_PP_DS3B") })
	{
		idx++;
		m_pContext->RSSetViewports(1, &m_Viewports[idx]);

		vTexelSize.x *= 2.f;
		vTexelSize.y *= 2.f;

		m_pContext->PSSetShaderResources(0, 16, nullSRV);
		if (FAILED(m_pGameInstance->Begin_RT(_RTTag1, false))) return;
		if (FAILED(m_pGameInstance->Bind_RT_SRV(_RTTag0, m_pPPShader, "g_TargetSampleTexture")))  return;
		m_pPPShader->Bind_RawData("g_vTexelSize", &vTexelSize, sizeof(_float2));

		if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_matDSWorlds[idx])))		return;
		if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_matView)))				return;
		if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_matDSProjections[idx]))) return;

		if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::BLURH))))    return;
		if (FAILED(m_pVIBuffer->Bind_Resources()))                              return;
		if (FAILED(m_pVIBuffer->Render()))                                      return;

		if (FAILED(m_pGameInstance->End_MRT()))  return;
		m_pContext->PSSetShaderResources(0, 16, nullSRV);
		/////////
		if (FAILED(m_pGameInstance->Begin_RT(_RTTag0, false)))  return;
		if (FAILED(m_pGameInstance->Bind_RT_SRV(_RTTag1, m_pPPShader, "g_TargetSampleTexture")))  return;
		m_pPPShader->Bind_RawData("g_vTexelSize", &vTexelSize, sizeof(_float2));

		if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_matDSWorlds[idx])))		return;
		if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_matView)))				return;
		if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_matDSProjections[idx]))) return;

		if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::BLURV))))    return;
		if (FAILED(m_pVIBuffer->Bind_Resources()))                              return;
		if (FAILED(m_pVIBuffer->Render()))                                      return;

		if (FAILED(m_pGameInstance->End_MRT()))  return;
	}
	m_pContext->RSSetViewports(1, &m_Viewports[0]);

	//UpSample
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Begin_RT(L"Target_Bloom", false)))  return;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_PP_DS0A", m_pPPShader, "g_UpSampleTexture0")))  return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_PP_DS1A", m_pPPShader, "g_UpSampleTexture1")))  return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_PP_DS2A", m_pPPShader, "g_UpSampleTexture2")))  return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_PP_DS3A", m_pPPShader, "g_UpSampleTexture3")))  return;

	if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_matWorld)))   return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_matView)))     return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_matProjection)))     return;

	if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::UPSAMPLE))))     return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))                                  return;
	if (FAILED(m_pVIBuffer->Render()))                                          return;

	if (FAILED(m_pGameInstance->End_MRT()))  return;
}

void CRenderer_Manager::RenderPPCombine(const wstring& sCurPPScene)
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Begin_RT(sCurPPScene, true, false)))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Bloom"), m_pPPShader, "g_BloomTexture"))) return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDiffuseB", m_pPPShader, "g_BTexture")))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetEffectB", m_pPPShader, "g_EBTexture")))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetNormal", m_pPPShader, "g_NormalTexture")))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDepth", m_pPPShader, "g_DepthTexture")))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetEmissive", m_pPPShader, "g_EmissiveTexture")))return;

	if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_matWorld))) return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_matView))) return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_matProjection))) return;
	if (FAILED(m_pGameInstance->Bind_TransformInverseStateMatrix(m_pPPShader, "g_ViewMatrixInv", D3DTS::VIEW)))return;
	if (FAILED(m_pGameInstance->Bind_TransformInverseStateMatrix(m_pPPShader, "g_ProjMatrixInv", D3DTS::PROJ)))return;

	Engine::_uint uiEnv = { ENUM_TO_UINT(m_tDesc.eEnvironment) };
	if (FAILED(m_pPPShader->Bind_RawData("g_uiEnvironment", &uiEnv, sizeof(Engine::_uint))))return;
	if (m_tDesc.eEnvironment == ENVIRONMENT::OUTDOOR)
	{
		if (FAILED(m_pPPShader->Bind_RawData("g_fDensityOfDots", &m_tDesc.fDensityOfDotsOutDoor, sizeof(Engine::_float))))return;
	}
	else if (FAILED(m_pPPShader->Bind_RawData("g_fDensityOfDots", &m_tDesc.fDensityOfDotsInDoor, sizeof(Engine::_float))))return;
	if (FAILED(m_pPPShader->Bind_RawData("g_fHalftoneRadius", &m_tDesc.fHalftoneRadius, sizeof(Engine::_float))))return;
	if (FAILED(m_pPPShader->Bind_RawData("g_fBloomIntensity", &m_tDesc.BloomDesc.fEmissiveIntensity, sizeof(Engine::_float))))return;
	if (FAILED(m_pPPShader->Bind_RawData("g_fSpecularValue", &m_tDesc.BloomDesc.fSpecularValue, sizeof(Engine::_float))))return;

	if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::COMBINED)))) return;
	if (FAILED(m_pVIBuffer->Bind_Resources())) return;
	if (FAILED(m_pVIBuffer->Render())) return;
	if (FAILED(m_pGameInstance->End_MRT()))return;

	// Color Reverse, Distortion, Wave
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Bind_RT_SRV(sCurPPScene, m_pPPShader, "g_SceneTexture")))return;
	if (FAILED(m_pPPShader->Bind_RawData("g_fReverse", &m_fReverse, sizeof(Engine::_float))))return;
	if (FAILED(m_pPPShader->Bind_RawData("g_fLowHP", &m_fLowHP, sizeof(Engine::_float))))return;
	//if (FAILED(m_pPPShader->Bind_RawData("g_fTime", &m_fTimeDelta, sizeof(Engine::_float))))return;
	//if (FAILED(m_pPPShader->Bind_RawData("g_fCamDistortionStrength", &m_fCamStrength, sizeof(Engine::_float))))return;
	//if (FAILED(m_pPPShader->Bind_RawData("g_vCamVelo", &m_f3CamVelo, sizeof(Engine::_float3))))return;

	Engine::_float2 f2UVPos = {};
	Engine::_float2 f2UVRange = {};
	Engine::_float fTry = MakeDistortionUV(f2UVPos, f2UVRange);
	if (FAILED(m_pPPShader->Bind_RawData("g_vDistortionPos", &f2UVPos, sizeof(Engine::_float2))))return;
	if (FAILED(m_pPPShader->Bind_RawData("g_vDistortionRange", &f2UVRange, sizeof(Engine::_float2))))return;
	Engine::_float fStrength = { m_tDistortionDesc.fStrength * (1.f - (Engine::_float)m_upDistortionTimer->GetElapsedRatio()) * fTry };
	if (FAILED(m_pPPShader->Bind_RawData("g_fDistortionStrength", &fStrength, sizeof(Engine::_float))))return;

	Engine::_float4 f4WavePos[m_uiWaveLimit] = {};
	Engine::_float4 f4WaveParam[m_uiWaveLimit] = {};
	Engine::_uint uiIndex = {};
	auto time = m_listWaveTime.begin();
	for (auto it = m_listWave.begin(); it != m_listWave.end();)
	{
		if (uiIndex >= m_uiWaveLimit)
			break;
		*time += m_fTimeDelta;
		Engine::_float2 f2UVPos = {};
		Engine::_float fUVRange = {};
		Engine::_float fUVDistance = {};
		if (!MakeWaveUV(f2UVPos, fUVRange, fUVDistance, it, time))
		{
			++it;
			++time;
			continue;
		}
		else if (fUVDistance < FLT_EPSILON)
		{
			it = m_listWave.erase(it);
			time = m_listWaveTime.erase(time);
			continue;
		}
		f4WavePos[uiIndex] = { f2UVPos.x,f2UVPos.y,0.f,0.f };
		f4WaveParam[uiIndex] = { fUVRange,it->fStrength,fUVDistance,0.f };
		++uiIndex;
		++it;
		++time;
	}
	if (FAILED(m_pPPShader->Bind_RawData("g_vWavePosXY", f4WavePos, sizeof(f4WavePos))))
		return;
	if (FAILED(m_pPPShader->Bind_RawData("g_vWaveParamRSD", f4WaveParam, sizeof(f4WaveParam))))
		return;

	if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::FINAL))))return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))return;
	if (FAILED(m_pVIBuffer->Render()))return;
}
void CRenderer_Manager::RenderCopy(const wstring& sSourceRTTag, const wstring& sTargetRTTag)
{
	if (FAILED(m_pGameInstance->Begin_RT(sTargetRTTag))) return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(sSourceRTTag, m_pPPShader, "g_SceneTexture"))) return;

	if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_matWorld)))  return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_matView)))    return;
	if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_matProjection)))    return;

	if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::COPY))))     return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))                              return;
	if (FAILED(m_pVIBuffer->Render()))                                      return;

	if (FAILED(m_pGameInstance->End_MRT()))                                 return;
	return;
}
void CRenderer_Manager::RenderAdditionalBlur()
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Begin_RT(L"TargetX", false)))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_PP0", m_pPPShader, "g_SceneTexture")))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDepth", m_pPPShader, "g_DepthTexture")))return;
	if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::BLURX))))return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))return;
	if (FAILED(m_pVIBuffer->Render()))return;
	if (FAILED(m_pGameInstance->End_MRT()))return;

	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Begin_RT(L"Target_PP0", false)))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetX", m_pPPShader, "g_SceneTexture")))return;
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetDepth", m_pPPShader, "g_DepthTexture")))return;
	if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::BLURY))))return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))return;
	if (FAILED(m_pVIBuffer->Render()))return;
	if (FAILED(m_pGameInstance->End_MRT()))return;


}
void CRenderer_Manager::RenderLateEFT()
{
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"TargetUnlit", m_pShader, "g_UnlitTexture")))
		return;
	if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::LATE_EFT))))
		return;
	if (FAILED(m_pVIBuffer->Bind_Resources()))
		return;
	if (FAILED(m_pVIBuffer->Render()))
		return;
}
void Engine::CRenderer_Manager::Render(Engine::RENDERGROUP eGroupID)
{
	for (auto it = m_listRenderer[ENUM_TO_UINT(eGroupID)].begin(); it != m_listRenderer[ENUM_TO_UINT(eGroupID)].end();)
	{
		auto spRenderer = it->lock();
		if (spRenderer)
		{
			if (!spRenderer->IsActive())
			{
				it++;
				continue;
			}
			if (!spRenderer->GetModuleVisible() || !spRenderer->GetSectionVisible())
			{
				it++;
				continue;
			}

			if (RENDERGROUP::SSHADOW == eGroupID || RENDERGROUP::DSHADOW == eGroupID)
			{
				if (RENDERGROUP::DSHADOW == eGroupID)
					int a = 10;
				if (FAILED(spRenderer->RenderShadow(eGroupID == RENDERGROUP::SSHADOW ? SHADOW::S : SHADOW::D)))
					MSG_BOX("Failed to RenderShadow");
			}
			else if (spRenderer->BroadFrustumTest() && spRenderer->MidFrustumTest())
			{
				//if (spRenderer->IsActive() && spRenderer->IsCage())
				//	int i = 10;

				if (FAILED(spRenderer->Render()))
					MSG_BOX("Failed to Render");
			}
			++it;
		}
		else it = { m_listRenderer[ENUM_TO_UINT(eGroupID)].erase(it) };
	}
}
void Engine::CRenderer_Manager::RenderSS(Engine::SCREENSPACE eGroupID)
{
	for (auto it = m_listSSRenderer[ENUM_TO_UINT(eGroupID)].begin(); it != m_listSSRenderer[ENUM_TO_UINT(eGroupID)].end();)
	{
		auto spRenderer = it->lock();
		if (spRenderer)
		{
			++it;
		}
		else it = { m_listSSRenderer[ENUM_TO_UINT(eGroupID)].erase(it) };
	}
}
void Engine::CRenderer_Manager::RenderDebug()
{
	Engine::_float4x4 f44 = {};
	Engine::_matrix mat = { DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(
		DirectX::XMLoadFloat4x4(&m_matWorld),DirectX::XMLoadFloat4x4(&m_matView)),DirectX::XMLoadFloat4x4(&m_matProjection)) };
	DirectX::XMStoreFloat4x4(&f44, mat);
	if (FAILED(m_pShader->Bind_Matrix("g_matWVP", &f44)))
		return;
	static Engine::_bool bRenderMRT{ false };
	static Engine::_bool bRenderRT{ false };
	static Engine::_bool bLastIsRT{ false };
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_matView))) return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_matProjection))) return;
	//if (m_pGameInstance->IsKeyState(DIK_1, EKEYACTIONSTATE::ENTER)) { bRenderMRT ^= true; bLastIsRT = { false }; }
	if (m_pGameInstance->IsKeyState(DIK_3, EKEYACTIONSTATE::ENTER)) { bRenderRT ^= true; bLastIsRT = { true }; }
	if (bRenderMRT)
		m_fDebugU = { 0.f };
	else if (bRenderRT) {
		m_fDebugU -= 0.01f;
		if (m_fDebugU < 0.f)
			m_fDebugU = { 0.f };
	}
	else {
		m_fDebugU += 0.01f;
		if (m_fDebugU > 1.f)
			m_fDebugU = { 1.f };
	}
	if (FAILED(m_pShader->Bind_RawData("g_fDebugU", &m_fDebugU, sizeof(Engine::_float))))return;
	//if (!bLastIsRT)
	//{
	//	if (FAILED(m_pGameInstance->Render_MRT(L"MRT_WHOLE", m_pShader, m_pVIBuffer)))return;
	//}
	//else 
	if (FAILED(m_pGameInstance->Render_RT(L"TargetV", m_pShader, m_pVIBuffer)))return;
}
Engine::_float CRenderer_Manager::MakeDistortionUV(Engine::_float2& f2UVPos, Engine::_float2& f2UVRange)
{
	if (!m_upDistortionTimer->IsActive())
		return 0.f;
	Engine::_matrix matView = { DirectX::XMLoadFloat4x4(m_pGameInstance->Get_Transform(Engine::D3DTS::VIEW)) };
	const Engine::_float4x4* pProj = { m_pGameInstance->Get_Transform(D3DTS::PROJ) };
	Engine::_matrix matProj = { DirectX::XMLoadFloat4x4(pProj) };
	Engine::_vector vViewPos = { DirectX::XMVector3TransformCoord(m_tDistortionDesc.vPosition, matView) };
	Engine::_float fViewZ = { DirectX::XMVectorGetZ(vViewPos) };
	if (fViewZ < FLT_EPSILON)
		return 0.f;
	Engine::_float4 f4ClipPosition = {};
	DirectX::XMStoreFloat4(&f4ClipPosition, DirectX::XMVector4Transform(vViewPos, matProj));
	if (f4ClipPosition.w < FLT_EPSILON)
		return 0.f;
	f2UVRange = { m_tDistortionDesc.fRange * pProj->_11 * 0.5f / fViewZ,m_tDistortionDesc.fRange * pProj->_22 * 0.5f / fViewZ };
	f2UVPos = { f4ClipPosition.x / f4ClipPosition.w * 0.5f + 0.5f,-f4ClipPosition.y / f4ClipPosition.w * 0.5f + 0.5f };
	return 1.f;
}
Engine::_bool CRenderer_Manager::MakeWaveUV(Engine::_float2& f2UVPos, Engine::_float& fUVRange, Engine::_float& fUVDistance, std::list<WAVE_DESC>::iterator it1, std::list<Engine::_float>::iterator it2)
{
	Engine::_matrix matView = { DirectX::XMLoadFloat4x4(m_pGameInstance->Get_Transform(Engine::D3DTS::VIEW)) };
	const Engine::_float4x4* pProj = { m_pGameInstance->Get_Transform(D3DTS::PROJ) };
	Engine::_matrix matProj = { DirectX::XMLoadFloat4x4(pProj) };
	Engine::_vector vViewPos = { DirectX::XMVector3TransformCoord(it1->vPosition,matView) };
	Engine::_float fViewZ = { DirectX::XMVectorGetZ(vViewPos) };
	if (fViewZ < FLT_EPSILON)
		return false;
	Engine::_float4 f4ClipPosition = {};
	DirectX::XMStoreFloat4(&f4ClipPosition, DirectX::XMVector4Transform(vViewPos, matProj));
	if (f4ClipPosition.w < FLT_EPSILON)
		return false;
	fUVRange = { it1->fRange * pProj->_11 * 0.5f / fViewZ };
	fUVDistance = { (it1->fMaxDist - (it1->fSpeed) * (*it2)) * pProj->_11 * 0.5f / fViewZ };
	f2UVPos = { f4ClipPosition.x / f4ClipPosition.w * 0.5f + 0.5f,-f4ClipPosition.y / f4ClipPosition.w * 0.5f + 0.5f };
	return true;
}
void CRenderer_Manager::Update()
{
	CModuleManager::SECTION_DESC* desc = { m_pGameInstance->Get_CurrentSectionDesc() };
	if (!desc)
		return;
	Engine::_int iCurMS = { desc->tSectionIdxRef.iModuleIndex * 10 + desc->tSectionIdxRef.iSectionIndex };
	Engine::_int iCurS = { (Engine::_int)m_pGameInstance->GetCurrentSequenceIndex() };
	Engine::_int iBestMS = { -1 };
	Engine::_int iBestSPriority = { 99 };
	for (size_t i = { 0 }; i < 20; ++i)
	{
		if (m_tDesc.DiffuseDesc[i].iMSS < 1)
			break;
		Engine::_int iDescMS = { m_tDesc.DiffuseDesc[i].iMSS / 10 };
		Engine::_int iDescS = { m_tDesc.DiffuseDesc[i].iMSS % 10 };
		if (iDescMS > iCurMS)
			continue;
		Engine::_int iSPriority = { 99 };
		if (iDescS == iCurS)
			iSPriority = { 0 };
		else if (iDescS == 1)
			iSPriority = { 1 };
		else continue;
		Engine::_bool bBetter = { false };
		if (iDescMS > iBestMS)
			bBetter = { true };
		else if (iDescMS == iBestMS && iSPriority < iBestSPriority)
			bBetter = { true };
		if (bBetter)
		{
			iBestMS = { iDescMS };
			iBestSPriority = { iSPriority };
			m_f3PurDiffuse = { m_tDesc.DiffuseDesc[i].f3Diffuse };
		}
		if (m_bLowHP)
			m_fLowHP = { cosf(m_pGameInstance->GetElapsedBeat() * DirectX::XM_PIDIV2) * 0.5f + 0.5f };
	}
	if (iCurS == static_cast<Engine::_int>(Engine::SEQUENCE::SAMURAI))
	{
		m_tDesc.f3LightDiffuse.x = { DiffuseApproach(m_tDesc.f3LightDiffuse.x, m_f3PurDiffuse.x, 0.1f) };
		m_tDesc.f3LightDiffuse.y = { DiffuseApproach(m_tDesc.f3LightDiffuse.y, m_f3PurDiffuse.y, 0.1f) };
		m_tDesc.f3LightDiffuse.z = { DiffuseApproach(m_tDesc.f3LightDiffuse.z, m_f3PurDiffuse.z, 0.1f) };
	}
	else
	{
		m_tDesc.f3LightDiffuse.x = { DiffuseApproach(m_tDesc.f3LightDiffuse.x, m_f3PurDiffuse.x, 0.004f) };
		m_tDesc.f3LightDiffuse.y = { DiffuseApproach(m_tDesc.f3LightDiffuse.y, m_f3PurDiffuse.y, 0.004f) };
		m_tDesc.f3LightDiffuse.z = { DiffuseApproach(m_tDesc.f3LightDiffuse.z, m_f3PurDiffuse.z, 0.004f) };
	}
}
Engine::_float CRenderer_Manager::DiffuseApproach(Engine::_float fCurrent, Engine::_float fTarget, Engine::_float fStep)
{
	Engine::_float fDiff = { fTarget - fCurrent };
	if (std::fabsf(fDiff) <= fStep)
		return fTarget;
	return fCurrent + (fDiff < 0.f ? -fStep : fStep);
}
HRESULT CRenderer_Manager::ReadyWholeRT()
{
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetDiffuseB"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetNormal"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetEmissive"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetELSR"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetV"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetGlass"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetOutline"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetDepth"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetSShadow"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetDShadow"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetLight"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetPriority"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetSSR"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"TargetAfter"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"Target_PP_DS0A"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"Target_PP_DS1A"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"Target_PP_DS2A"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"Target_PP_DS3A"));
	CHKFAIL(m_pGameInstance->Add_MRT(L"MRT_WHOLE", L"Target_Bloom"));
	// GBuffer
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetDiffuseB", 155.f, 115.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetNormal", 155.f, 285.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetEmissive", 155.f, 455.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetELSR", 155.f, 625.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetV", 155.f, 795.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetGlass", 155.f, 965.f, 192.f, 108.f));
	// OutLine
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetOutline", 385.f, 115.f, 192.f, 108.f));
	// DepthBuffer
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetDepth", 615.f, 115.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetSShadow", 615.f, 285.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetDShadow", 615.f, 455.f, 192.f, 108.f));
	// Light
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetLight", 845.f, 115.f, 192.f, 108.f));
	// Sky Sphere
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetPriority", 1075.f, 115.f, 192.f, 108.f));
	// Screen Space
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetSSR", 1305.f, 115.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"TargetAfter", 1305.f, 285.f, 192.f, 108.f));
	// Scaling
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"Target_PP_DS0A", 1535.f, 115.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"Target_PP_DS1A", 1535.f, 285.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"Target_PP_DS2A", 1535.f, 455.f, 192.f, 108.f));
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"Target_PP_DS3A", 1535.f, 625.f, 192.f, 108.f));
	// Bloom
	CHKFAIL(m_pGameInstance->Ready_RT_Debug(L"Target_Bloom", 1765.f, 115.f, 192.f, 108.f));
	return S_OK;
}
Engine::CRenderer_Manager* Engine::CRenderer_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	auto pInstance = new CRenderer_Manager(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : Renderer_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void Engine::CRenderer_Manager::Free()
{
	__super::Free();
	for (size_t i = { 0 }; i < ENUM_TO_UINT(RENDERGROUP::END); ++i)
		m_listRenderer[i].clear();
	for (size_t i = { 0 }; i < ENUM_TO_UINT(SCREENSPACE::END); ++i)
		m_listSSRenderer[i].clear();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pSSShader);
	Safe_Release(m_pPPShader);
	for (size_t i = { 0 }; i < ENUM_TO_UINT(SHADOW::END); ++i)
		Safe_Release(m_pShadowDSV[i]);
	Safe_Release(m_pRS_CullFront);
	Safe_Release(m_pRS_CullBack);
	Safe_Release(m_pRS_CullNone);
	Safe_Release(m_pBS_Opaque);
	Safe_Release(m_pBS_AlphaBlend);
	Safe_Release(m_pBS_Additive);
}

void CRenderer_Manager::SetUP()
{
	if (m_pGameInstance->IsKeyState(DIK_NUMPADPERIOD, EKEYACTIONSTATE::ENTER)) {
		m_iState += 1;
		m_iState %= 6;
	}

	CMesh::SETFORCEIGNORE(L"NC", m_iState < 1);
	CMesh::SETFORCEIGNORE(L"EL", m_iState < 2);
	CMeshRenderer::SETOLIGNORE(m_iState >= 3);
	m_bUseNormalOL = (m_iState >= 4);
	m_bUseDepthOL = (m_iState >= 5);
}











//#include "Renderer_Manager.h"
//#include "GameObject.h"
//#include "GameInstance.h"
//#include "Renderer.h"
//
//
//CRenderer_Manager::CRenderer_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//    : m_pDevice { pDevice }
//    , m_pContext { pContext }
//    , m_pGameInstance{ CGameInstance::GetInstance() }
//{
//    Safe_AddRef(m_pGameInstance);
//    Safe_AddRef(m_pContext);
//    Safe_AddRef(m_pDevice);
//}
//
//HRESULT CRenderer_Manager::Initialize()
//{
//    //RS
//    D3D11_RASTERIZER_DESC _RSdesc{};
//    _RSdesc.FillMode = D3D11_FILL_SOLID;
//    _RSdesc.FrontCounterClockwise = FALSE;
//    _RSdesc.DepthClipEnable = TRUE;
//    _RSdesc.CullMode = D3D11_CULL_BACK;
//    m_pDevice->CreateRasterizerState(&_RSdesc, &m_pRS_Solid[CULLMODE_BACK][FILLMODE_SOLID]);
//    _RSdesc.CullMode = D3D11_CULL_FRONT;
//    m_pDevice->CreateRasterizerState(&_RSdesc, &m_pRS_Solid[CULLMODE_FRONT][FILLMODE_SOLID]);
//    _RSdesc.CullMode = D3D11_CULL_NONE;
//    m_pDevice->CreateRasterizerState(&_RSdesc, &m_pRS_Solid[CULLMODE_NONE][FILLMODE_SOLID]);
//
//    ZeroMemory(&_RSdesc, sizeof _RSdesc);
//    
//    _RSdesc.FillMode = D3D11_FILL_WIREFRAME;
//    _RSdesc.CullMode = D3D11_CULL_NONE;   // ���̾�� ���� �ø� ��
//    _RSdesc.DepthBias               = -1;
//    _RSdesc.SlopeScaledDepthBias    = -1.0f;
//    _RSdesc.DepthBiasClamp          = 0.0f;
//    _RSdesc.DepthClipEnable         = TRUE;
//    _RSdesc.CullMode = D3D11_CULL_BACK;
//    m_pDevice->CreateRasterizerState(&_RSdesc, &m_pRS_Solid[CULLMODE_BACK][FILLMODE_WIREFRAME]);
//    _RSdesc.CullMode = D3D11_CULL_FRONT;
//    m_pDevice->CreateRasterizerState(&_RSdesc, &m_pRS_Solid[CULLMODE_FRONT][FILLMODE_WIREFRAME]);
//    _RSdesc.CullMode = D3D11_CULL_NONE;
//    m_pDevice->CreateRasterizerState(&_RSdesc, &m_pRS_Solid[CULLMODE_NONE][FILLMODE_WIREFRAME]);
//
//    //BS
//    D3D11_BLEND_DESC _BSdesc;
//
//    ZeroMemory(&_BSdesc, sizeof _BSdesc);
//    _BSdesc.RenderTarget[0].BlendEnable           = TRUE;
//    _BSdesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
//    _BSdesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
//    _BSdesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
//    _BSdesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
//    _BSdesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
//    _BSdesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
//    _BSdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//    m_pDevice->CreateBlendState(&_BSdesc, &m_pBS_AlphaBlend);
//    
//    ZeroMemory(&_BSdesc, sizeof _BSdesc);
//    _BSdesc.RenderTarget[0].BlendEnable           = FALSE;
//    _BSdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//    m_pDevice->CreateBlendState(&_BSdesc, &m_pBS_Opaque);
//
//    D3D11_DEPTH_STENCIL_DESC _DSSdesc;
//
//    ZeroMemory(&_DSSdesc, sizeof _DSSdesc);
//    _DSSdesc.DepthEnable    = FALSE;
//    _DSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
//    _DSSdesc.DepthFunc      = D3D11_COMPARISON_ALWAYS;
//    m_pDevice->CreateDepthStencilState(&_DSSdesc, &m_pDSS[0]);
//
//    ZeroMemory(&_DSSdesc, sizeof _DSSdesc);
//    _DSSdesc.DepthEnable    = TRUE;
//    _DSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
//    _DSSdesc.DepthFunc      = D3D11_COMPARISON_LESS;
//    m_pDevice->CreateDepthStencilState(&_DSSdesc, &m_pDSS[1]);
//
//    ZeroMemory(&_DSSdesc, sizeof _DSSdesc);
//    _DSSdesc.DepthEnable    = TRUE;
//    _DSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//    _DSSdesc.DepthFunc      = D3D11_COMPARISON_LESS;
//    m_pDevice->CreateDepthStencilState(&_DSSdesc, &m_pDSS[2]);
//
//
//    //RenderTarger
//    _uint               iNumViewports = { 1 };
//    D3D11_VIEWPORT      ViewportDesc{};
//
//    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
//    
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"),           ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT,        _float4(0.f, 0.f, 0.f, 0.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_MRAOS"),             ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM,        _float4(0.f, 1.f, 1.f, 0.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_EMISSIVE"),          ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 0.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"),            ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM,    _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"),             ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT,    _float4(1.f, 1.f, 0.f, 1.f)))
//    
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"   ),          ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_SceneColor" ),       ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 0.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom" ),            ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 0.f)))
//    
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP0" ),              ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP1" ),              ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PPSCENE0" ),          ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PPSCENE1" ),          ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS0A" ),           ViewportDesc.Width * 0.500f,    ViewportDesc.Height * 0.500f,  DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS1A" ),           ViewportDesc.Width * 0.250f,    ViewportDesc.Height * 0.250f,  DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS2A" ),           ViewportDesc.Width * 0.125f,    ViewportDesc.Height * 0.125f,  DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS3A" ),           ViewportDesc.Width * 0.0625f,   ViewportDesc.Height * 0.0625f, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS0B" ),           ViewportDesc.Width * 0.500f,    ViewportDesc.Height * 0.500f,  DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS1B" ),           ViewportDesc.Width * 0.250f,    ViewportDesc.Height * 0.250f,  DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS2B" ),           ViewportDesc.Width * 0.125f,    ViewportDesc.Height * 0.125f,  DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_PP_DS3B" ),           ViewportDesc.Width * 0.0625f,   ViewportDesc.Height * 0.0625f, DXGI_FORMAT_R16G16B16A16_FLOAT,    _float4(0.f, 0.f, 0.f, 1.f)))
//
//
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"),    TEXT("Target_Diffuse")))
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"),    TEXT("Target_MRAOS")))
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"),    TEXT("Target_EMISSIVE")))
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"),    TEXT("Target_Normal")))
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_GameObject"),    TEXT("Target_Depth")))
//
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"),      TEXT("Target_Shade")))
//
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_Scene"),         TEXT("Target_SceneColor")))
//
//    /*
//   
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_DiffuseLight"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_SpecularLight"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f)))
//    CHKFAIL(m_pGameInstance->Add_RenderTarget(TEXT("Target_AmbientLight"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f)))
//
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_DiffuseLight")))
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_SpecularLight")))
//    CHKFAIL(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_AmbientLight")))
//
//    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_DiffuseLight"), 180.f, 540.f, 360.f, 360.f)))       return E_FAIL;
//    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_SpecularLight"),540.f, 540.f, 360.f, 360.f)))       return E_FAIL;
//    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_AmbientLight"), 900.f, 540.f,   360.f,  360.f)))    return E_FAIL;
//    
//    */
//
//    m_pShader = CShader::Create<VTXPOSTEX>(m_pDevice, m_pContext, TEXT("../Bin/Shaders/Shader_Deferred.hlsl"));
//    if (nullptr == m_pShader) return E_FAIL;
//    m_pPPShader = CShader::Create<VTXPOSTEX>(m_pDevice, m_pContext, TEXT("../Bin/Shaders/Shader_PostProcessing.hlsl"));
//    if (nullptr == m_pPPShader) return E_FAIL;
//    m_pSSShader = CShader::Create<VTXPOSTEX>(m_pDevice, m_pContext, TEXT("../Bin/Shaders/Shader_ScreenSpace.hlsl"));
//    if (nullptr == m_pSSShader) return E_FAIL;
//
//    m_pVIBuffer = CVIBuffer_Rect::Create<CVIBuffer_Rect>(m_pDevice, m_pContext);
//    if (nullptr == m_pVIBuffer) return E_FAIL;
//
//    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
//    XMStoreFloat4x4(&m_ViewMatrix,  XMMatrixIdentity());
//    XMStoreFloat4x4(&m_ProjMatrix,  XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));
//
//    XMStoreFloat4x4(&m_PhotoWorldMatrices[0],
//        XMMatrixScaling(ViewportDesc.Width * 0.2f, ViewportDesc.Height * 0.25f, 1.f) *
//        XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), 3.14159 * -0.166f) *
//        XMMatrixTranslation(ViewportDesc.Width * 0.37f, ViewportDesc.Height * 0.16f, 0.f)
//    );
//
//
//    XMStoreFloat4x4(&m_PhotoWorldMatrices[1],
//        XMMatrixScaling(ViewportDesc.Width * 0.2f, ViewportDesc.Height * 0.25f, 1.f) *
//        XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), 3.14159 * 0.09f) *
//        XMMatrixTranslation(ViewportDesc.Width * 0.22f, ViewportDesc.Height * 0.10f, 0.f)
//    );
//
//
//    XMStoreFloat4x4(&m_PhotoWorldMatrices[2],
//        XMMatrixScaling(ViewportDesc.Width * 0.2f, ViewportDesc.Height * 0.25f, 1.f) *
//        XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), 3.14159 * -0.09f) *
//        XMMatrixTranslation(ViewportDesc.Width * 0.37f, ViewportDesc.Height * -0.15f, 0.f)
//    );
//
//
//    XMStoreFloat4x4(&m_PhotoWorldMatrices[3],
//        XMMatrixScaling(ViewportDesc.Width * 0.2f, ViewportDesc.Height * 0.25f, 1.f) *
//        XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), 3.14159 * 0.02f) *
//        XMMatrixTranslation(ViewportDesc.Width * 0.21f, ViewportDesc.Height * -0.25f, 0.f)
//    );
//    
//    for (size_t i = 0; i < 5; i++)
//    {
//        XMStoreFloat4x4(&m_WorldMatrices[i],    XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
//        XMStoreFloat4x4(&m_ViewMatrices[i],     XMMatrixIdentity());
//        XMStoreFloat4x4(&m_ProjMatrices[i],     XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));
//        m_ViewPorts[i] = ViewportDesc;
//        ViewportDesc.Width /= 2;
//        ViewportDesc.Height /= 2;
//    }
//
//
//    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"),  180.f, 180.f,   360.f, 360.f))) return E_FAIL;
//    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_MRAOS"),    540.f, 180.f,   360.f, 360.f))) return E_FAIL;
//    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_EMISSIVE"), 900.f, 180.f,   360.f, 360.f))) return E_FAIL;
//    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"),   180.f, 540.f,   360.f, 360.f))) return E_FAIL;
//    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Depth"),    540.f, 540.f,   360.f, 360.f))) return E_FAIL;
//
//    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"),        180.f, 180.f,   360.f,  360.f)))    return E_FAIL;
//
//    return S_OK;
//} 
//
//HRESULT CRenderer_Manager::Add_RenderGroup(RENDERGROUP eGroupID, WPRenderer pRenderer)
//{   
//    if (eGroupID >= RENDERGROUP::END) return E_FAIL;
//    m_Renderers[ENUM_TO_UINT(eGroupID)].push_back(pRenderer);
//
//    return S_OK;
//}
//
//HRESULT CRenderer_Manager::Add_SSGroup(SCREENSPACE eGroupID, WPRenderer pRenderer)
//{
//    if (eGroupID >= SCREENSPACE::END) return E_FAIL;
//    m_SSRenderers[ENUM_TO_UINT(eGroupID)].push_back(pRenderer);
//    
//    return S_OK;
//}
//
//HRESULT CRenderer_Manager::Change_RenderGroup(RENDERGROUP eGroupID, WPRenderer pRenderer)
//{
//    if (eGroupID >= RENDERGROUP::END) return E_FAIL;
//    for (size_t i = 0; i < ENUM_TO_UINT(RENDERGROUP::END); i++) m_Renderers[i].remove_if([=](WPRenderer _pRenderer) {
//        return _pRenderer.lock() == pRenderer.lock();
//    });
//    m_Renderers[ENUM_TO_UINT(eGroupID)].push_back(pRenderer);
//
//    return S_OK;
//}
//
//
//void CRenderer_Manager::Draw()
//{
//    ID3D11ShaderResourceView* nullSRV[16] = {};
//    auto pDesc = m_pGameInstance->Get_GlobalLightDesc();
//
//    Render_NonBlend();
//    Render_LightAcc();
//    
//    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Scene")))) return; 
//    m_pContext->PSSetShaderResources(0, 16, nullSRV);
//    Render_Priority();
//    m_pContext->PSSetShaderResources(0, 16, nullSRV);
//    m_pShader->Bind_RawData("g_fEmissiveIntensity", &pDesc->m_fEmissiveIntensity, sizeof(pDesc->m_fEmissiveIntensity));
//    Render_Combined();
//    m_pContext->PSSetShaderResources(0, 16, nullSRV);
//    Render_SSD();
//    m_pContext->PSSetShaderResources(0, 16, nullSRV);
//    if (FAILED(m_pGameInstance->End_MRT())) return;
//
//    //SetGlobalLight
//    m_pPPShader->Bind_RawData("g_fExposure",                &pDesc->m_fExposure,        sizeof(pDesc->m_fExposure));
//    m_pPPShader->Bind_RawData("g_fGamma",                   &pDesc->m_fGamma,           sizeof(pDesc->m_fGamma));
//    m_pPPShader->Bind_RawData("g_fBloomIntensity",          &pDesc->m_fBloomIntensity,  sizeof(pDesc->m_fBloomIntensity));
//    m_pPPShader->Bind_RawData("g_fBloomThreshold",          &pDesc->m_fBloomThreshold,  sizeof(pDesc->m_fBloomThreshold));
//    auto pSRV       = m_pGameInstance->GetLUT();
//    bool bUseLUT    = pSRV != nullptr;
//    m_pPPShader->Bind_RawData("g_bUseLUT", &bUseLUT, sizeof(bUseLUT));
//    if (pSRV) m_pPPShader->Bind_ShaderResourceView("g_LUTTexture", pSRV);
//    
//
//    auto pAtmoDesc = m_pGameInstance->GetAtmosphereDesc();
//    auto sCurPPScene    = Render_Copy(L"Target_SceneColor", L"Target_PPSCENE0");
//    if (pAtmoDesc->m_bUseFog) sCurPPScene = Render_Fog(sCurPPScene);
//
//    wstring sRTTag1 = (L"Target_PPSCENE0" == sCurPPScene) ? L"Target_PPSCENE1" : L"Target_PPSCENE0";
//    if (FAILED(m_pGameInstance->Begin_RT(sRTTag1))) return;
//
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(sCurPPScene, m_pPPShader, "g_SceneTexture"))) return;
//
//    if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))  return;
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))    return;
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))    return;
//
//
//    if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::COPY))))     return;
//    if (FAILED(m_pVIBuffer->Bind_Resources()))                              return;
//    if (FAILED(m_pVIBuffer->Render()))                                      return;
//
//    m_pContext->PSSetShaderResources(0, 16, nullSRV);
//    Render_Unlit();
//    m_pContext->PSSetShaderResources(0, 16, nullSRV);
//    Render_Blend();
//    if (FAILED(m_pGameInstance->End_MRT())) return;
//    sCurPPScene = sRTTag1;
//
//    Render_Bloom(sCurPPScene);
//    Render_PPCombine(sCurPPScene);
//    
//
//    m_pGameInstance->Debug_Render();
//
//    Render_UI();  
//
//
//    Render_Debug();
//}
//
//HRESULT CRenderer_Manager::SetRenderState(_uint iCullModeIdx, _uint iFillModeIdx)
//{
//    m_pContext->RSSetState(m_pRS_Solid[iCullModeIdx][iFillModeIdx]);
//    return S_OK;
//}
//
//void CRenderer_Manager::Render_Priority()
//{
//    float blendFactor[4] = { 0, 0, 0, 0 };
//    m_pContext->OMSetBlendState(m_pBS_Opaque, blendFactor, 0xffffffff);
//    m_pContext->OMSetDepthStencilState(m_pDSS[0], 0);
//
//    Render_Group(RENDERGROUP::PRIORITY);
//}
//
//
//void CRenderer_Manager::Render_NonBlend()
//{
//    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
//    m_pContext->OMSetBlendState         (m_pBS_Opaque,      blendFactor, 0xffffffff);
//    m_pContext->OMSetDepthStencilState  (m_pDSS[2],   0);
//    SetRenderState(CULLMODE_BACK, FILLMODE_SOLID);
//
//    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObject")))) return;
//    Render_Group(RENDERGROUP::NONBLEND);
//    if (FAILED(m_pGameInstance->End_MRT())) return;
//}
//
//void CRenderer_Manager::Render_Unlit()
//{
//
//    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
//    m_pContext->OMSetBlendState(m_pBS_Opaque, blendFactor, 0xffffffff);
//    m_pContext->OMSetDepthStencilState(m_pDSS[2], 0);
//    SetRenderState(CULLMODE_BACK, FILLMODE_SOLID);
//
//    Render_Group(RENDERGROUP::UNLIT);
//}
//
////Z-depth
//void CRenderer_Manager::Render_Blend()
//{
//    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
//    m_pContext->OMSetBlendState         (m_pBS_AlphaBlend,  blendFactor, 0xffffffff);
//    m_pContext->OMSetDepthStencilState  (m_pDSS[1],   0);
//
//    //sort
//    auto& listRenderer = m_Renderers[ENUM_TO_UINT(RENDERGROUP::BLEND)];
//    listRenderer.sort([](const WPRenderer& pRenderer1, const WPRenderer& pRenderer2) {
//        auto spRenderer1 = pRenderer1.lock();
//        auto spRenderer2 = pRenderer2.lock();
//        if (!spRenderer1 && spRenderer2)     return false;
//        if (!spRenderer1) return false;
//        if (!spRenderer2) return true;
//        return spRenderer1->GetPriority() < spRenderer2->GetPriority();
//    });
//
//    Render_Group(RENDERGROUP::BLEND);
//}
//
////UI Priority
//void CRenderer_Manager::Render_UI()
//{
//    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
//    m_pContext->OMSetBlendState(m_pBS_AlphaBlend, blendFactor, 0xffffffff);
//    m_pContext->OMSetDepthStencilState(m_pDSS[0], 0);
//
//    auto& listRenderer = m_Renderers[ENUM_TO_UINT(RENDERGROUP::UI)];
//    listRenderer.sort([](const WPRenderer& pRenderer1, const WPRenderer& pRenderer2) {
//        auto spRenderer1 = pRenderer1.lock();
//        auto spRenderer2 = pRenderer2.lock();
//        if (!spRenderer1 && spRenderer2)     return false;
//        if (!spRenderer1) return false;
//        if (!spRenderer2) return true;
//        return spRenderer1->GetPriority() < spRenderer2->GetPriority();
//    });
//
//    Render_Group(RENDERGROUP::UI);
//}
//
//void CRenderer_Manager::Render_LightAcc()
//{
//    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc")))) return;
//
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Normal"),      m_pShader, "g_NormalTexture")))     return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"),       m_pShader, "g_DepthTexture")))      return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_MRAOS"),       m_pShader, "g_MRAOSTexture")))      return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_EMISSIVE"),    m_pShader, "g_EmissiveTexture")))   return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Diffuse"),     m_pShader, "g_DiffuseTexture")))    return;
//
//    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",  &m_WorldMatrix)))   return;
//    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",   &m_ViewMatrix)))    return;
//    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",   &m_ProjMatrix)))    return;
//
//    if (FAILED(m_pGameInstance->Bind_TransformInverseStateMatrix(m_pShader, "g_ViewMatrixInv", D3DTS::VIEW))) return;
//    if (FAILED(m_pGameInstance->Bind_TransformInverseStateMatrix(m_pShader, "g_ProjMatrixInv", D3DTS::PROJ))) return;
//    if (FAILED(m_pGameInstance->Bind_CamPosition(m_pShader,                 "g_vCamPosition"))) return;
//
//    if (FAILED(m_pVIBuffer->Bind_Resources())) return;
//
//    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);
//
//    if (FAILED(m_pGameInstance->End_MRT())) return;
//}
//
//void CRenderer_Manager::Render_Combined()
//{
//
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Diffuse"),     m_pShader,  "g_DiffuseTexture"))) return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_EMISSIVE"),    m_pShader,  "g_EmissiveTexture")))   return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Shade"),       m_pShader,  "g_ShadeTexture"))) return;
//    //if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Specular"),    m_pShader,  "g_SpecularTexture"))) return;
//
//    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix",  &m_WorldMatrix))) return;
//    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix",   &m_ViewMatrix))) return;
//    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix",   &m_ProjMatrix))) return;
//
//    if (FAILED(m_pShader->Begin(ENUM_TO_UINT(DEFERRED::COMBINED)))) return;
//    if (FAILED(m_pVIBuffer->Bind_Resources())) return;
//    if (FAILED(m_pVIBuffer->Render())) return;
//
//}
//
//void CRenderer_Manager::Render_SSD()
//{
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"), m_pSSShader, "g_DepthTexture"))) return;
//
//    if (FAILED(m_pSSShader->Bind_Matrix("g_WorldMatrix",        &m_WorldMatrix))) return;
//    if (FAILED(m_pSSShader->Bind_Matrix("g_ViewMatrix",         &m_ViewMatrix))) return;
//    if (FAILED(m_pSSShader->Bind_Matrix("g_ProjMatrix",         &m_ProjMatrix))) return;
//
//    if (FAILED(m_pGameInstance->Bind_TransformInverseStateMatrix(m_pSSShader, "g_ViewMatrixInv", D3DTS::VIEW))) return;
//    if (FAILED(m_pGameInstance->Bind_TransformInverseStateMatrix(m_pSSShader, "g_ProjMatrixInv", D3DTS::PROJ))) return;
//
//    Render_SSGroup(SCREENSPACE::SSD);
//}
//
//wstring CRenderer_Manager::Render_Copy(const wstring& sSourceRTTag, const wstring& sTargetRTTag) 
//{
//    if (FAILED(m_pGameInstance->Begin_RT(sTargetRTTag))) return L"";
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(sSourceRTTag, m_pPPShader, "g_SceneTexture"))) return L"";
//
//    if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))  return L"";
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))    return L"";
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))    return L"";
//
//
//    if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::COPY))))     return L"";
//    if (FAILED(m_pVIBuffer->Bind_Resources()))                              return L"";
//    if (FAILED(m_pVIBuffer->Render()))                                      return L"";
//
//    if (FAILED(m_pGameInstance->End_MRT()))                                 return L"";
//    return sTargetRTTag;
//
//}
//
//wstring CRenderer_Manager::Render_Fog(const wstring& sSourceRTTag)
//{
//    wstring sRTTag0 = sSourceRTTag;
//    wstring sRTTag1 = (L"Target_PPSCENE0" == sRTTag0) ? L"Target_PPSCENE1" : L"Target_PPSCENE0";
//
//    if (FAILED(m_pGameInstance->Begin_RT(sRTTag1))) return L"";
//
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"),       m_pSSShader, "g_DepthTexture"))) return L"";
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(sSourceRTTag,               m_pSSShader, "g_SceneTexture"))) return L"";
//
//    if (FAILED(m_pSSShader->Bind_Matrix("g_WorldMatrix",&m_WorldMatrix)))   return L"";
//    if (FAILED(m_pSSShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))    return L"";
//    if (FAILED(m_pSSShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))    return L"";
//
//    auto pDesc = m_pGameInstance->GetAtmosphereDesc();
//    if (FAILED(m_pSSShader->Bind_RawData("g_bUseDensity",   &pDesc->m_bUseDensity   , sizeof pDesc->m_bUseDensity))) return L"";
//    if (FAILED(m_pSSShader->Bind_RawData("g_vFogColor",     &pDesc->m_vFogColor     , sizeof pDesc->m_vFogColor     ))) return L"";
//    if (FAILED(m_pSSShader->Bind_RawData("g_fFogStart",     &pDesc->m_fFogSrt       , sizeof pDesc->m_fFogSrt       ))) return L"";
//    if (FAILED(m_pSSShader->Bind_RawData("g_fFogEnd",       &pDesc->m_fFogEnd       , sizeof pDesc->m_fFogEnd       ))) return L"";
//    if (FAILED(m_pSSShader->Bind_RawData("g_fFogDensity",   &pDesc->m_fFogDensity   , sizeof pDesc->m_fFogDensity   ))) return L"";
//
//
//    if (FAILED(m_pSSShader->Begin(ENUM_TO_UINT(SCREENSPACE::FOG)))) return L"";
//    if (FAILED(m_pVIBuffer->Bind_Resources()))                      return L"";
//    if (FAILED(m_pVIBuffer->Render()))                              return L"";
//
//    if (FAILED(m_pGameInstance->End_MRT())) return L"";
//
//    return sRTTag1;
//}
//
//void CRenderer_Manager::Render_Bloom(const wstring& sCurPPScene)
//{
//    ID3D11ShaderResourceView* nullSRV[16] = {};
//    _float2 vTexelSize;
//    vTexelSize.x = 1.f / 1280;
//    vTexelSize.y = 1.f / 720;
//
//    //ExtractBloom
//    if (FAILED(m_pGameInstance->Begin_RT(L"Target_PP0"))) return;
//
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(sCurPPScene, m_pPPShader, "g_SceneTexture"))) return;
//
//    if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))  return;
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))    return;
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))    return;
//
//    if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::BLOOMEXTRACT)))) return;
//    if (FAILED(m_pVIBuffer->Bind_Resources()))                                  return;
//    if (FAILED(m_pVIBuffer->Render()))                                          return;
//
//    if (FAILED(m_pGameInstance->End_MRT())) return;
//
//    int idx = 0;
//    //DownSample0
//    for (auto [_RTTag, TextureRTTag] : {
//        pair(L"Target_PP_DS0A", L"Target_PP0"),
//        pair(L"Target_PP_DS1A", L"Target_PP_DS0A") , 
//        pair(L"Target_PP_DS2A", L"Target_PP_DS1A") , 
//        pair(L"Target_PP_DS3A", L"Target_PP_DS2A")})
//    {
//        idx++;
//        m_pContext->RSSetViewports(1, &m_ViewPorts[idx]);
//
//        m_pContext->PSSetShaderResources(0, 16, nullSRV);
//        if (FAILED(m_pGameInstance->Begin_RT(_RTTag, false))) return;
//        if (FAILED(m_pGameInstance->Bind_RT_SRV(TextureRTTag, m_pPPShader, "g_TargetSampleTexture")))  return;
//        m_pPPShader->Bind_RawData("g_vTexelSize", &vTexelSize, sizeof(_float2));
//
//
//        if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix",    &m_WorldMatrices[idx])))    return;
//        if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix",     &m_ViewMatrices[idx])))     return;
//        if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix",     &m_ProjMatrices[idx])))     return;
//
//        if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::DOWNSAMPLE))))   return;
//        if (FAILED(m_pVIBuffer->Bind_Resources()))                                  return;
//        if (FAILED(m_pVIBuffer->Render()))                                          return;
//
//        if (FAILED(m_pGameInstance->End_MRT()))  return;
//        vTexelSize.x *= 2.f;
//        vTexelSize.y *= 2.f;
//    }
//    m_pContext->RSSetViewports(1, &m_ViewPorts[0]);
//    m_pContext->PSSetShaderResources(0, 16, nullSRV);
//
//    //Blur
//    vTexelSize.x = 1.f / 1280;
//    vTexelSize.y = 1.f / 720;
//    idx = 0;
//    for (auto [_RTTag0, _RTTag1] : {
//        pair(L"Target_PP_DS0A", L"Target_PP_DS0B"),
//        pair(L"Target_PP_DS1A", L"Target_PP_DS1B") ,
//        pair(L"Target_PP_DS2A", L"Target_PP_DS2B") ,
//        pair(L"Target_PP_DS3A", L"Target_PP_DS3B") })
//    {
//        idx++;
//        m_pContext->RSSetViewports(1, &m_ViewPorts[idx]);
//
//        vTexelSize.x *= 2.f;
//        vTexelSize.y *= 2.f;
//
//        m_pContext->PSSetShaderResources(0, 16, nullSRV);
//        if (FAILED(m_pGameInstance->Begin_RT(_RTTag1, false))) return;
//        if (FAILED(m_pGameInstance->Bind_RT_SRV(_RTTag0, m_pPPShader, "g_TargetSampleTexture")))  return;
//        m_pPPShader->Bind_RawData("g_vTexelSize", &vTexelSize, sizeof(_float2));
//        
//        if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix",    &m_WorldMatrices[idx])))    return;
//        if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix",     &m_ViewMatrices[idx])))     return;
//        if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix",     &m_ProjMatrices[idx])))     return;
//
//        if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::BLURH))))    return;
//        if (FAILED(m_pVIBuffer->Bind_Resources()))                              return;
//        if (FAILED(m_pVIBuffer->Render()))                                      return;
//
//        if (FAILED(m_pGameInstance->End_MRT()))  return;
//        m_pContext->PSSetShaderResources(0, 16, nullSRV);
//        /////////
//        if (FAILED(m_pGameInstance->Begin_RT(_RTTag0, false)))  return;
//        if (FAILED(m_pGameInstance->Bind_RT_SRV(_RTTag1, m_pPPShader, "g_TargetSampleTexture")))  return;
//        m_pPPShader->Bind_RawData("g_vTexelSize", &vTexelSize, sizeof(_float2));
//        
//        if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix",    &m_WorldMatrices[idx])))    return;
//        if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix",     &m_ViewMatrices[idx])))     return;
//        if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix",     &m_ProjMatrices[idx])))     return;
//
//        if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::BLURV))))    return;
//        if (FAILED(m_pVIBuffer->Bind_Resources()))                              return;
//        if (FAILED(m_pVIBuffer->Render()))                                      return;
//
//        if (FAILED(m_pGameInstance->End_MRT()))  return;
//    }
//    m_pContext->RSSetViewports(1, &m_ViewPorts[0]);
//
//    //UpSample
//    m_pContext->PSSetShaderResources(0, 16, nullSRV);
//    if (FAILED(m_pGameInstance->Begin_RT(L"Target_Bloom", false)))  return;
//
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_PP_DS0A", m_pPPShader, "g_UpSampleTexture0")))  return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_PP_DS1A", m_pPPShader, "g_UpSampleTexture1")))  return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_PP_DS2A", m_pPPShader, "g_UpSampleTexture2")))  return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_PP_DS3A", m_pPPShader, "g_UpSampleTexture3")))  return;
//
//
//    if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))   return;
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))     return;
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))     return;
//
//    if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::UPSAMPLE))))     return;
//    if (FAILED(m_pVIBuffer->Bind_Resources()))                                  return;
//    if (FAILED(m_pVIBuffer->Render()))                                          return;
//
//    if (FAILED(m_pGameInstance->End_MRT()))  return;
//}
//
//void CRenderer_Manager::Render_PPCombine(const wstring& sCurPPScene)
//{
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(sCurPPScene,            m_pPPShader, "g_SceneTexture"))) return;
//    if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Bloom"),   m_pPPShader, "g_BloomTexture"))) return;
//
//    if (FAILED(m_pPPShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix))) return;
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix))) return;
//    if (FAILED(m_pPPShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix))) return;
//
//    if (FAILED(m_pPPShader->Begin(ENUM_TO_UINT(POSTPROCESSING::COMBINED)))) return;
//    if (FAILED(m_pVIBuffer->Bind_Resources())) return;
//    if (FAILED(m_pVIBuffer->Render())) return;
//}
//
//void CRenderer_Manager::Render_Debug()
//{
//
//    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix))) return;
//    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix))) return;
//
//    static bool bRenderOBJ(false), bRenderLight(false);
//    
//    if (m_pGameInstance->IsKeyState(DIK_1, EKEYACTIONSTATE::ENTER)) bRenderOBJ    ^= true;
//    if (m_pGameInstance->IsKeyState(DIK_2, EKEYACTIONSTATE::ENTER)) bRenderLight  ^= true;
//
//    if (bRenderOBJ)     if (FAILED(m_pGameInstance->Render_MRT(TEXT("MRT_GameObject"),  m_pShader, m_pVIBuffer))) return;
//    if (bRenderLight)   if (FAILED(m_pGameInstance->Render_MRT(TEXT("MRT_LightAcc"),    m_pShader, m_pVIBuffer))) return;
//}
//
//void CRenderer_Manager::Render_Group(Engine::RENDERGROUP eRenderGroup)
//{
//    auto& listRenderer = m_Renderers[ENUM_TO_UINT(eRenderGroup)];
//    for (auto it = listRenderer.begin(); it != listRenderer.end(); )
//    {
//        auto spRenderer = it->lock();
//
//        if (spRenderer) {
//
//            if (!spRenderer->BroadFrustumTest() || !spRenderer->MidFrustumTest()) {
//                it++;
//                continue;
//            }
//            if (spRenderer->IsOverrayWireFrame()) {
//                SetRenderState(CULLMODE_NONE, FILLMODE_WIREFRAME);
//                if (FAILED(spRenderer->Render())) {
//                    MSG_BOX("Fail toRender");
//                }
//                SetRenderState(CULLMODE_BACK, FILLMODE_SOLID);
//            }
//            else {
//                if (FAILED(spRenderer->Render())) {
//                    MSG_BOX("Fail toRender");
//                }
//            }
//            ++it;
//        }
//        else {
//            it = listRenderer.erase(it);
//        }
//    }
//}
//
//void CRenderer_Manager::Render_SSGroup(Engine::SCREENSPACE eRenderGroup)
//{
//    auto& listRenderer = m_SSRenderers[ENUM_TO_UINT(eRenderGroup)];
//    for (auto it = listRenderer.begin(); it != listRenderer.end(); )
//    {
//        auto spRenderer = it->lock();
//
//        if (spRenderer) {
//            if (FAILED(spRenderer->Render(m_pSSShader, m_pVIBuffer))) {
//                MSG_BOX("Fail to Render");
//            }
//            ++it;
//        }
//        else {
//            it = listRenderer.erase(it);
//        }
//    }
//}
//
//CRenderer_Manager* CRenderer_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//    auto pInstance = new CRenderer_Manager(pDevice, pContext);
//    if (FAILED(pInstance->Initialize())) {
//        MSG_BOX("Fail to Create Renderer Manager");
//        Safe_Release(pInstance);
//        pInstance = nullptr;
//    }
//    return pInstance;
//}
//
//void CRenderer_Manager::Free()
//{
//    __super::Free();
//
//    for (size_t i = 0; i < ENUM_TO_UINT(RENDERGROUP::END); i++) m_Renderers[i].clear();
//
//    Safe_Release(m_pDevice);
//    Safe_Release(m_pContext);
//    Safe_Release(m_pGameInstance);
//    
//    for (int eCullMode= CULLMODE_BACK; eCullMode != CULLMODE_END; eCullMode++) for (int eFillMode = FILLMODE_SOLID; eFillMode != FILLMODE_END; eFillMode++)
//    {
//        Safe_Release(m_pRS_Solid[eCullMode][eFillMode]);
//    }
//
//    Safe_Release(m_pBS_Opaque);
//    Safe_Release(m_pBS_AlphaBlend);
//
//    for (size_t i = 0; i < 3; i++)
//    {
//        Safe_Release(m_pDSS[i]);
//    }
//
//    Safe_Release(m_pVIBuffer);
//    Safe_Release(m_pShader);
//    Safe_Release(m_pPPShader);
//    Safe_Release(m_pSSShader);
//}
//
