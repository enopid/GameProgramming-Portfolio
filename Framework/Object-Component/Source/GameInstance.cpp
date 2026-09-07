#include "GameInstance.h"
#include "Graphic_Device.h"

#include "Level_Manager.h"
#include "Time_Manager_Impl.h"
#include "Input_Manager_Impl.h"
#include "Asset_Manager.h"
#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "ImGUI_Manager.h"
#include "Config_Manager.h"
#include "Collision_Manager.h"
#include "Video_Manager.h"
#include "DebugManager.h"
#include "PipeLine.h"
#include "Picking.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Sound_Manager.h"
#include "Rhythm_Manager_Impl.h"
#include "Shadow.h"
#include "GI.h"
#include "UIController.h"
#include "Sequence.h"
#include "DSP_Manager.h"
#include "ListenerUpdater.h"
#include <CameraObject.h>
#include "Level.h"

IMPLEMENT_SINGLETON(CGameInstance);

CGameInstance::CGameInstance()
{
}

#pragma region ENGINE

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_hwnd		= EngineDesc.hWnd;
	m_pDevice	= *ppDevice;
	m_pContext	= *ppContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pConfig_Manager = CConfig_Manager::Create();
	if (nullptr == m_pConfig_Manager)		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create();
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pInput_Manager = CInput_Manager::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Manager)	return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)	return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pTime_Manager = CTime_Manager::Create();
	if (nullptr == m_pTime_Manager)		return E_FAIL;

	m_pAsset_Manager = CAsset_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pAsset_Manager) return E_FAIL;
	
	m_pVideo_Manager = CVideo_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pVideo_Manager) return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager) return E_FAIL;

	m_pDebug_Manager = CDebugManager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pDebug_Manager) return E_FAIL;

	m_pImGui_Manager = CImGui_Manager::Create(m_hwnd , *ppDevice, *ppContext);
	if (nullptr == m_pImGui_Manager) return E_FAIL;
	
	m_pRenderer_Manager = CRenderer_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer_Manager) return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pCollision_Manager)	return E_FAIL;

	m_pDSP_Manager = CDSP_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pDSP_Manager)	return E_FAIL;

	m_pRhythm_Manager = CRhythm_Manager::Create();
	if (nullptr == m_pRhythm_Manager)	return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;
	
	m_pPicking = CPicking::Create(*ppDevice, *ppContext);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pShadow = { CShadow::Create() };
	if (!m_pShadow)
		return E_FAIL;

	m_pGI = { CGI::Create() };
	if (!m_pGI)
		return E_FAIL;

	m_pModuleManager = CModuleManager::Create();
	if (nullptr == m_pModuleManager)
		return E_FAIL;

	m_pSequence = { CSequence::Create() };
	if (!m_pSequence)
		return E_FAIL;

	m_pListenerUpdater = CListenerUpdater::Create();
	if (nullptr == m_pListenerUpdater)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine()
{
	if (m_bLevelChangeCall) {
		m_bLevelChangeCall = false;
		m_pLevel_Manager->Change_Level();
		m_pModuleManager->Request_Initialize();
	}

	auto _sCurrentLVL = m_pLevel_Manager->GetCurrentLevelTag();

	_float fTimeDelta = m_pTime_Manager->GetDeltaTime();

	m_FPS = (1.f - 0.1f) * m_FPS + (0.1f) * (1 / fTimeDelta);

	m_pConfig_Manager	->Update(fTimeDelta);
	m_pTime_Manager		->Update(fTimeDelta);
	m_pVideo_Manager	->Update(fTimeDelta);
	m_pSound_Manager	->Update();
	m_pDebug_Manager	->Update(fTimeDelta);
	m_pRhythm_Manager	->Update(fTimeDelta);
	m_pSequence			->Update(fTimeDelta);

	m_pInput_Manager->Update();

	m_pImGui_Manager->Update(fTimeDelta);
	m_pObject_Manager->Priority_Update(_sCurrentLVL, fTimeDelta);
	m_pPipeLine->Update();
	m_pListenerUpdater->Update(fTimeDelta);
	m_pPicking->Update();
	

	m_pObject_Manager->Update(_sCurrentLVL, fTimeDelta);

	m_pObject_Manager->Late_Update(_sCurrentLVL, fTimeDelta);

	m_pCollision_Manager->LateUpdate();
	m_pDSP_Manager		->LateUpdate();
	m_pModuleManager->Update();

	m_pCollision_Manager->LateUpdate();

	m_pLevel_Manager->Update(fTimeDelta);
	m_pShadow->Update();

	m_pRenderer_Manager->SetUP();
	m_pRenderer_Manager->SetTimeDelta(fTimeDelta); 
}




HRESULT CGameInstance::Begin_Draw(const _float4& vClearColor)
{
	m_pGraphic_Device->Clear_BackBuffer_View(&vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
 	m_pRenderer_Manager->Draw();
	m_pDebug_Manager->Render();

	CHKFAIL(m_pLevel_Manager->Render());
	
	if (IsKeyState(DIK_F, EKEYACTIONSTATE::STAY)) {
		_tchar pLevelName[100];
		GetWindowText(m_hwnd, pLevelName, 100);
		wstring sLevelName = pLevelName;
		sLevelName += L"   fps :\t" + to_wstring(m_FPS);
		SetWindowText(m_hwnd, sLevelName.c_str());
	}

	CHKFAIL(ImGUI_Render());

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	m_pGraphic_Device->Present();

 	return S_OK;
}

HRESULT CGameInstance::Clear(const _wstring& strLVLTag)
{
	m_pObject_Manager	->Clear(strLVLTag);
	m_pPrototype_Manager->Clear(strLVLTag);
	m_pLevel_Manager	->Clear(strLVLTag);
	m_pDSP_Manager->Clear();

	return S_OK;
}
//ID3D11ShaderResourceView* Engine::CGameInstance::GetShaderResourceView()
//{
//	return m_pGraphic_Device->GetDepthStencilShaderResourceView();
//}
HRESULT CGameInstance::Load_Level_InEditor(const wstring& sLVLTag, wstring& sLVLPrototypeTag)
{
	auto _curLevelTag = m_pLevel_Manager->GetCurrentLevelTag();
	m_pObject_Manager->Clear(_curLevelTag);
	CHKFAIL(m_pLevel_Manager->Load_Level_InEditor(sLVLTag, sLVLPrototypeTag))
	m_pObject_Manager->Load_Asset(_curLevelTag);

	return S_OK;
}

HRESULT CGameInstance::Save_Level_InEditor(const wstring& sLVLTag, const wstring& sLVLPrototypeTag)
{
	auto _curLevelTag = m_pLevel_Manager->GetCurrentLevelTag();
	CHKFAIL(m_pLevel_Manager->Save_Level_InEditor(sLVLTag, sLVLPrototypeTag))
	return S_OK;
}

HRESULT CGameInstance::AddLayer(const _wstring& sLVLTag, const _wstring& sLayerTag)
{
	return m_pObject_Manager->AddLayer(sLVLTag, sLayerTag);
}

HRESULT CGameInstance::DeleteLayer(const _wstring& sLVLTag, const _wstring& sLayerTag)
{
	return m_pObject_Manager->DeleteLayer(sLVLTag, sLayerTag);
}

HRESULT CGameInstance::RenameLayer(const _wstring& sLVLTag, const _wstring& sOldLayerTag, const _wstring& sNewLayerTag)
{
	return m_pObject_Manager->RenameLayer(sLVLTag, sOldLayerTag, sNewLayerTag);
}


#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Load_Level(const tagLevelTransitionDesc& _LVLTransitDesc)
{
	m_pObject_Manager->AddLevel(_LVLTransitDesc.nextLevelTag);
	m_pLevel_Manager->Load_Level(_LVLTransitDesc);
	m_pObject_Manager->Load_Asset(_LVLTransitDesc.nextLevelTag);
	return S_OK;
}

HRESULT CGameInstance::Change_Level()
{
	m_bLevelChangeCall = true;
	m_pSequence->ClearSequnceFunc();
	return S_OK;
}
HRESULT CGameInstance::Create_Level(const wstring& sLVLPrototypeTag, const wstring& sLVLName)
{
	m_pObject_Manager->AddLevel(sLVLName);
	return m_pLevel_Manager->Create_Level(sLVLPrototypeTag, sLVLName);
}
_wstring CGameInstance::GetCurrentLevelTag()
{
	return m_pLevel_Manager->GetCurrentLevelTag();
}
_wstring CGameInstance::GetCurrentDetailLevelTag()
{
	return m_pLevel_Manager->GetCurrentDetailLevelTag();
}
tagLevelTransitionDesc CGameInstance::GetLevelDesc()
{
	return m_pLevel_Manager->GetLevelDesc();
}

CUIController* CGameInstance::GetCurrentLevelUC()
{
	return m_pLevel_Manager->GetCurrentLevelUC();
}

CUIController* CGameInstance::GetUC(wstring sUCName)
{
	auto pLevelUC = m_pLevel_Manager->GetCurrentLevelUC();
	if (!pLevelUC) return nullptr;
	auto pUC = pLevelUC->GetUIController(sUCName);
	return pUC;
}

CLevel* CGameInstance::GetCurrentLevel()
{
	return m_pLevel_Manager->GetCurrentLevelPtr();
}

#pragma endregion

#pragma region Time_MANAGER
UniqueTimer CGameInstance::CreateTimerWithFrequency(double _frequency, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive)
{
	return m_pTime_Manager->CreateTimerWithFrequency(_frequency, _bIsLoop, _func, _bInitialActive);
}
UniqueTimer CGameInstance::CreateTimerWithDuration(double _duration, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive)
{
	return m_pTime_Manager->CreateTimerWithDuration(_duration, _bIsLoop, _func, _bInitialActive);
}
void CGameInstance::SetTimeScale(_float fScale)
{
	fScale = max(0.001f, fScale);
	m_pTime_Manager->SetTimeScale(fScale);
}
#pragma endregion

#pragma region Input_MANAGER
bool CGameInstance::IsKeyState(_ubyte byKeyID, EKEYACTIONSTATE eKeyState) { 
	return m_pInput_Manager->IsKeyState(byKeyID, eKeyState); }

bool CGameInstance::IsMouseBtnState(EMOUSEBTN eMouseBtn, EKEYACTIONSTATE eKeyState)
{
	return m_pInput_Manager->IsMouseBtnState(eMouseBtn, eKeyState);
}

_long CGameInstance::GetMouseMoveState(EMOUSEMOVE eMouseMove)
{
	return m_pInput_Manager->GetMouseMoveState(eMouseMove);
}

UniqueKeyContext CGameInstance::CreateKeyContext(bool _bInitialActive) { return m_pInput_Manager->CreateKeyContext(_bInitialActive); }

POINT CGameInstance::GetMouseCursor() {
	POINT _mousePT;
	GetCursorPos(&_mousePT);
	ScreenToClient(m_hwnd, &_mousePT);
	return _mousePT;
}

bool CGameInstance::IsCursorInRect(const CUIObject::FLOAT_RECT& _rect) {
	POINT _mousePT = GetMouseCursor();
	if (_rect.right < _mousePT.x || _rect.left > _mousePT.x) return false;
	if (_rect.bottom < _mousePT.y || _rect.top > _mousePT.y) return false;
	return true;
}
bool CGameInstance::IsCursorInRect(const RECT& _rect) {
	POINT _mousePT = GetMouseCursor();
	if (_rect.right < _mousePT.x || _rect.left > _mousePT.x) return false;
	if (_rect.bottom < _mousePT.y || _rect.top > _mousePT.y) return false;
	return true;
}
HRESULT CGameInstance::LoadAsset(SP_Asset& _spAsset, _wstring _sName)
{
	return m_pAsset_Manager->LoadAsset(_spAsset, _sName);
}
vector<_wstring> CGameInstance::GetAssetNames(EASSETTYPE eAssetType)
{
	return m_pAsset_Manager->GetAssetNames(eAssetType);
}
#pragma endregion


#pragma region PROTOTYPE_MANAGER

HRESULT CGameInstance::Add_Prototype(PROTOTYPE ePrototype, const _wstring& strPrototypeTag, class CBase* pPrototype, _bool _bIsClassPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(ePrototype, strPrototypeTag, pPrototype, _bIsClassPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototype, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototype, strPrototypeTag, pArg);
}

SPComponent CGameInstance::Clone_ComponentPrototype(const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_ComponentPrototype(strPrototypeTag, pArg);
}

bool CGameInstance::IsClassPrototype(PROTOTYPE ePrototype, const _wstring& strPrototypeTag)
{
	return m_pPrototype_Manager->IsClassPrototype(ePrototype, strPrototypeTag);
}

vector<_wstring> CGameInstance::GetPrototypeNames(PROTOTYPE ePrototype, _bool _bOnlyIsClassPrototype)
{
	return m_pPrototype_Manager->GetPrototypeNames(ePrototype, _bOnlyIsClassPrototype);
}



#pragma endregion

#pragma region OBJECT_MANAGER

vector<CGameObject*> CGameInstance::Get_Terrains(const _wstring& strLVLTag, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_Terrains(strLVLTag, strLayerTag);
}

HRESULT CGameInstance::ChangeLayerTimeScale(const _wstring& sLVLTag, const _wstring& sLayerTag, float fTimeScale)
{
	return m_pObject_Manager->ChangeLayerTimeScale(sLVLTag, sLayerTag, fTimeScale);
}

CGameObject* CGameInstance::Get_Object(const _wstring& strLVLTag, const _wstring& strLayerTag, const _wstring& strObjectName)
{
	return m_pObject_Manager->Get_Object(strLVLTag, strLayerTag, strObjectName);
}

list<CGameObject*> CGameInstance::Get_Objects(const _wstring& strLVLTag, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_Objects(strLVLTag, strLayerTag);
}

HRESULT CGameInstance::Add_GameObject_ToLayer(const _wstring& strPrototypeTag, const _wstring& strLVLTag, const _wstring& strLayerTag, CGameObject** ppOut, void* pArg)
{
	return m_pObject_Manager->Add_GameObject_ToLayer(strPrototypeTag, strLVLTag, strLayerTag, ppOut, pArg);
}

HRESULT CGameInstance::SaveObjects(fstream& _fs, const _wstring& strLVLTag)
{
	return m_pObject_Manager->SaveObjects(_fs, strLVLTag);
}

HRESULT CGameInstance::LoadObjects(fstream& _fs, const _wstring& strLVLTag)
{
	return m_pObject_Manager->LoadObjects(_fs, strLVLTag);
}

wstring CGameInstance::FindPossibleName(const _wstring& sLVLTag, const wstring& sName)
{
	if (m_pObject_Manager->CheckNameDuplication(sLVLTag, sName)) {
		int idx = 1;
		while (true)  {
			wstring _tmpName = sName + L"(" + to_wstring(idx++) + L")";
			if (!m_pObject_Manager->CheckNameDuplication(sLVLTag, _tmpName)) return _tmpName;
		}
	}
	else {
		return sName;
	}
	return wstring();
}

vector<_wstring> CGameInstance::GetLayerNames(const _wstring& strLVLTag)
{
	return m_pObject_Manager->GetLayerNames(strLVLTag);
}

HRESULT CGameInstance::ChangeObjectLayer(const _wstring& sLVLTag, const _wstring& sNewLayerTag, CGameObject* pObject)
{
	return m_pObject_Manager->ChangeObjectLayer(sLVLTag, sNewLayerTag, pObject);
}

HRESULT CGameInstance::CopyObject(const _wstring& sLVLTag, CGameObject* pObject)
{
	return m_pObject_Manager->CopyObject(sLVLTag, pObject);
}

CGameObject* CGameInstance::Render_ImGui_Hierarchy(CGameObject* pLastSelected)
{
	return m_pObject_Manager->Render_ImGui_Hierarchy(pLastSelected);
}

HRESULT CGameInstance::LoadConfig(const wstring& _ConfigPath)
{
	return m_pConfig_Manager->LoadConfig(_ConfigPath);
}

HRESULT CGameInstance::LoadJson(const wstring& _ConfigPath, Json::Value& outJson)
{
	return m_pConfig_Manager->LoadJson(_ConfigPath, outJson);
}

HRESULT CGameInstance::ImGUI_Render()
{
	return m_pImGui_Manager->Render();
}

void CGameInstance::ImGUI_Clear()
{
	return m_pImGui_Manager->Clear();
}

#pragma endregion

#pragma region RENDERER
HRESULT CGameInstance::Add_RenderGroup(RENDERGROUP eGroupID, WPRenderer pRenderer)
{
	return m_pRenderer_Manager->Add_RenderGroup(eGroupID, pRenderer);
}
HRESULT CGameInstance::Add_SSGroup(SCREENSPACE eGroupID, WPRenderer pRenderer)
{
	return m_pRenderer_Manager->Add_SSGroup(eGroupID, pRenderer);
}
HRESULT CGameInstance::Change_RenderGroup(RENDERGROUP eGroupID, WPRenderer pRenderer)
{
	return m_pRenderer_Manager->Change_RenderGroup(eGroupID, pRenderer);
}

HRESULT CGameInstance::Remove_RenderGroup(RENDERGROUP eGroupID, WPRenderer pRenderer)
{
	return m_pRenderer_Manager->Remove_RenderGroup(eGroupID, pRenderer);
}

void CGameInstance::Debug_Render()
{
	m_pDebug_Manager->Render();
}

void CGameInstance::AddDebugRenderCall(const DEBUGRENDER_DESC* pDesc)
{
 	m_pDebug_Manager->AddDebugRenderCall(pDesc);
}

void CGameInstance::DrawStaticShadow()
{
	m_pRenderer_Manager->DrawStaticShadow();
}

void CGameInstance::BindRSParam(RSPARAM_DESC _desc)
{
	m_pRenderer_Manager->Bind_RSParameters(_desc);
}

std::vector<Engine::_wstring>Engine::CGameInstance::GetRMName()
{
	return m_pRenderer_Manager->GetName();
}

Engine::CRenderer_Manager::RM_DESC* Engine::CGameInstance::GetRMDesc()
{
	return m_pRenderer_Manager->GetDesc();
}

HRESULT Engine::CGameInstance::SaveRM(const Engine::_wstring& wstrFileName)
{
	return m_pRenderer_Manager->Save(wstrFileName);
}

HRESULT Engine::CGameInstance::LoadRM(const Engine::_wstring& wstrFileName)
{
	return m_pRenderer_Manager->Load(wstrFileName);
}

void CGameInstance::SetAfterVelocity(const Engine::_float2& f2Velocity)
{
	m_pRenderer_Manager->SetAfterVelocity(f2Velocity);
}

void CGameInstance::SetKaleAfterVelocity(const Engine::_float2& f2Velocity)
{
	m_pRenderer_Manager->SetKaleAfterVelocity(f2Velocity);
}

//void CGameInstance::SetCamDistortion(Engine::_float fDistortionStrength, Engine::_float3 f3CamVelo)
//{
//	m_pRenderer_Manager->SetCamDistortion(fDistortionStrength, f3CamVelo);
//}

void CGameInstance::SetDistortion(DISTORTION_DESC Desc)
{
	m_pRenderer_Manager->SetDistortion(Desc);
}

void CGameInstance::SetExplosion(Engine::_bool bExplosion)
{
	m_pRenderer_Manager->SetExplosion(bExplosion);
}

void CGameInstance::AddWave(WAVE_DESC Desc)
{
	m_pRenderer_Manager->AddWave(Desc);
}

void CGameInstance::SetColorReverse(Engine::_float fReverse)
{
	m_pRenderer_Manager->SetColorReverse(fReverse);
}

void CGameInstance::ChangeWeights()
{
	m_pRenderer_Manager->ChangeWeights();
}

void CGameInstance::SetRMDiffuse(Engine::_float3 f3Diffuse)
{
	m_pRenderer_Manager->SetRMDiffuse(f3Diffuse);
}

void CGameInstance::SetLowHP(Engine::_bool bLowHP)
{
	m_pRenderer_Manager->SetLowHP(bLowHP);
}

#pragma endregion
#pragma region COLLISION
HRESULT CGameInstance::Add_Collidor(WPCollidor wpCollidor)
{
	return m_pCollision_Manager->Add_Collidor(wpCollidor);
}

HRESULT CGameInstance::ActiveCollision(_uint _id1, _uint _id2)
{
	return m_pCollision_Manager->ActiveCollision(_id1, _id2);
}

bool CGameInstance::RayCast_Collidor(_int iCollisionMask, RAY ray, _float& fInOutDist, list<pair<_float, WPCollidor>>& lstWPOutCollidor)
{
	return  m_pCollision_Manager->RayCast_Collidor(iCollisionMask, ray, fInOutDist, lstWPOutCollidor);
}

#pragma endregion
#pragma region DSP
void CGameInstance::Render_DSPManager()
{
	return m_pDSP_Manager->Render_Manager();
}
HRESULT CGameInstance::AddDSPComponent(WPDSP wpDSP)
{
	return m_pDSP_Manager->AddDSPComponent(wpDSP);
}
HRESULT CGameInstance::AddDSP(string sDSPTag, string sConstantName)
{
	return m_pDSP_Manager->AddDSP(sDSPTag, sConstantName);
}

HRESULT CGameInstance::SetDSP(string sDSPTag, string sConstantName, DSPVALUEDESC desc)
{
	return m_pDSP_Manager->SetDSP(sDSPTag, sConstantName, desc);
}

DSPVALUEDESC CGameInstance::GetDSP(string sDSPTag, string sConstantName)
{
	return m_pDSP_Manager->GetDSP(sDSPTag, sConstantName);
}

HRESULT CGameInstance::BindDSP(CMeshRenderer* pMeshRenderer, CShader* pShader)
{
	return m_pDSP_Manager->BindDSP(pMeshRenderer, pShader);
}

vector<string> CGameInstance::GetDSPNames()
{
	return m_pDSP_Manager->GetDSPNames();
}

vector<string> CGameInstance::GetDSPParamNames(string sDSPTag)
{
	return m_pDSP_Manager->GetDSPParamNames(sDSPTag);
}

#pragma endregion
#pragma region Video

HRESULT CGameInstance::Bind_VideoTexture(CShader* m_pShader, const _char* pConstantName)
{
	return m_pVideo_Manager->Bind_VideoTexture(m_pShader, pConstantName);
}

VIDEOINFO_DESC CGameInstance::GetVideoDesc()
{
	return m_pVideo_Manager->GetVideoDesc();
}

vector<wstring> CGameInstance::GetVideoNames()
{
	return m_pVideo_Manager->GetVideoNames();
}

HRESULT CGameInstance::LoadVideo(const wstring& sFilePath)
{
	return m_pVideo_Manager->Load(sFilePath);
}

HRESULT CGameInstance::SetTime(_float fTime)
{
	return m_pVideo_Manager->SetTime(fTime);
}

void CGameInstance::SetLoop(_bool bIsLoop)
{
	return m_pVideo_Manager->SetLoop(bIsLoop);
}

void CGameInstance::SetClip(_bool bIsClip, _float fSrtTime, _float fendTime)
{
	return m_pVideo_Manager->SetClip(bIsClip, fSrtTime, fendTime);
}

void CGameInstance::Play_Video()
{
	return m_pVideo_Manager->Play();
}

void CGameInstance::Pause_Video()
{
	return m_pVideo_Manager->Pause();
}

void CGameInstance::Restart_Video()
{
	return m_pVideo_Manager->Restart();
}

void CGameInstance::Reset_Video()
{
	return m_pVideo_Manager->Reset();
}

#pragma endregion


#pragma region PIPELINE

void CGameInstance::Set_Transform(D3DTS eTransformState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

const _float4x4* CGameInstance::Get_Transform(D3DTS eTransformState)
{
	return m_pPipeLine->Get_Transform(eTransformState);
}

const _float4x4* CGameInstance::Get_InverseTransform(D3DTS eTransformState)
{
	return m_pPipeLine->Get_InverseTransform(eTransformState);
}

const _float4x4* CGameInstance::Get_VP()
{
	return m_pPipeLine->Get_VP();
}

const _float4* CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

const _float4* CGameInstance::Get_PrevCamPosition()
{
	return m_pPipeLine->Get_PrevCamPosition();
}

const _float4* CGameInstance::Get_CamUp()
{
	return m_pPipeLine->Get_CamUp();
}

const _float4* CGameInstance::Get_CamLook()
{
	return m_pPipeLine->Get_CamLook();
}

const _float CGameInstance::Get_CamFOV()
{
	return m_pPipeLine->Get_CamFOV();
}

RAY CGameInstance::GetViewportRay()
{
	return m_pPipeLine->GetViewportRay();
}

_vector CGameInstance::ConvertViewportPos(_vector _vWorldPos)
{
	return m_pPipeLine->ConvertViewportPos(_vWorldPos);
}

const BoundingFrustum* CGameInstance::Get_Frustum()
{
	return m_pPipeLine->Get_Frustum();
}

HRESULT CGameInstance::Bind_CamPosition(CShader* pShader, const _char* pConstantName)
{
	return m_pPipeLine->Bind_CamPosition(pShader, pConstantName);
}

HRESULT CGameInstance::Bind_TransformStateMatrix(CShader* pShader, const _char* pConstantName, D3DTS eTransformState)
{
	return m_pPipeLine->Bind_TransformStateMatrix(pShader, pConstantName, eTransformState);
}

HRESULT CGameInstance::Bind_TransformInverseStateMatrix(CShader* pShader, const _char* pConstantName, D3DTS eTransformState)
{
	return m_pPipeLine->Bind_TransformInverseStateMatrix(pShader, pConstantName, eTransformState);
}

#pragma endregion

#pragma region LIGHT_MANAGER
vector<_wstring> CGameInstance::GetLightMapNames()
{
	return m_pLight_Manager->GetLightNames();
}
HRESULT CGameInstance::SaveLightMap(const wstring& sFileName)
{
	return m_pLight_Manager->SaveLightMap(sFileName);
}
HRESULT CGameInstance::LoadLightMap(const wstring& sFileName)
{
	return m_pLight_Manager->LoadLightMap(sFileName);
}
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

GLOBALLIGHT_DESC* CGameInstance::Get_GlobalLightDesc()
{
	return m_pLight_Manager->Get_GlobalLightDesc();
}

ID3D11ShaderResourceView* CGameInstance::GetLUT()
{
	return m_pLight_Manager->GetLUT();
}

_uint CGameInstance::Get_LightSize()
{
	return m_pLight_Manager->Get_LightSize();
}

CLight* CGameInstance::Get_Light(_uint iIndex)
{
	return m_pLight_Manager->Get_Light(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
HRESULT CGameInstance::Delete_Light(const Engine::_char* cName)
{
	return m_pLight_Manager->Delete_Light(cName);
}
CLight* CGameInstance::Add_Dynamic_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Dynamic_Light(LightDesc);
}
void CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	m_pLight_Manager->Render(pShader, pVIBuffer);
}
HRESULT CGameInstance::Add_Chasing_Light(const Engine::_vector& vPos, CHASING_TYPE eType, Engine::_uint uiShape, Engine::_uint uiShutDown)
{
	return m_pLight_Manager->Add_Chasing_Light(vPos, eType, uiShape, uiShutDown);
}
HRESULT CGameInstance::BindShutDown(CShader* pShader)
{
	return m_pLight_Manager->BindShutDown(pShader);
}
void CGameInstance::SetSamurai(const Engine::_vector& vPos)
{
	m_pLight_Manager->Set_Samurai_Spot(vPos);
}
#pragma endregion

#pragma region Atmosphere
vector<_wstring> CGameInstance::GetAtmosphereNames()
{
	vector<_wstring> m_vecAtmosphereNames;
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.AtmospherePath");
	for (const auto& entry : filesystem::recursive_directory_iterator(_path))
		m_vecAtmosphereNames.push_back(entry.path().filename().replace_extension());
	return m_vecAtmosphereNames;
}
HRESULT CGameInstance::LoadAtmosphere(_wstring sFileName)
{
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.AtmospherePath");
	_path += sFileName + L".ATMO";

	ios_base::openmode      _mode = ios::in | ios::binary;

	fstream _fs;
	_fs.open(_path, _mode);
	if (!_fs.is_open()) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create load file : ( " + _sMSG + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	_uint	_iSize;

	_fs.read((char*)&_iSize, sizeof _iSize);
	_fs.read((char*)&m_atmosphereDesc, _iSize);

	_fs.close();

	return S_OK;
}
HRESULT CGameInstance::SaveAtmosphere(_wstring sFileName)
{
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.AtmospherePath");
	std::filesystem::path   _tmpPath = _path;
	_path += sFileName + L".ATMO";
	_tmpPath += L"ATMOSave";
	_tmpPath += L".tmp";

	ios_base::openmode      _mode = ios::out | ios::binary | ios::trunc;

	fstream _fs;
	_fs.open(_tmpPath, _mode);
	if (!_fs.is_open()) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create save file : ( " + _sMSG + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	_uint iSize = sizeof m_atmosphereDesc;
	_fs.write((char*)&iSize, sizeof iSize);
	_fs.write((char*)&m_atmosphereDesc, sizeof m_atmosphereDesc);

	_fs.close();

	filesystem::rename(_tmpPath, _path);

	return S_OK;
}
ATMOSPHERE_DESC* CGameInstance::GetAtmosphereDesc()
{
	return &m_atmosphereDesc;
}
HRESULT CGameInstance::Play_SFX(wstring sSoundTag, float fVolume, int fPriority, float fEndPoint, bool bReset, bool bLoop)
{
	return m_pSound_Manager->Play_SFX(sSoundTag, fVolume, fPriority, fEndPoint, bReset, bLoop);
}
HRESULT CGameInstance::Play_3D_EnvSound(const _wstring& sSoundTag, const _float3& vWorldPosition, const _float3& vWorldVelocity, _float fVolume, _float fMinDist, _float fMaxDist, _bool bIsLoop, _int iPriority)
{
	return m_pSound_Manager->Play_3D_EnvSound(sSoundTag, vWorldPosition, vWorldVelocity, fVolume, iPriority, fMinDist, fMaxDist, bIsLoop);
}
HRESULT CGameInstance::Play_3D_Env_NonRandom(const _wstring& sSoundTag, const _float3& vWorldPosition, const _float3& vWorldVelocity, _float fVolume, _float fMinDist, _float fMaxDist, _bool bIsLoop, _int iPriority)
{
	return m_pSound_Manager->Play_3D_Env_NonRandom(sSoundTag, vWorldPosition, vWorldVelocity, fVolume, iPriority, fMinDist, fMaxDist, bIsLoop);
}
HRESULT CGameInstance::Play_VOX(wstring sSoundTag, float fVolume, int fPriority)
{
	return m_pSound_Manager->Play_VOX(sSoundTag, fVolume, fPriority);
}
HRESULT CGameInstance::Play_Music(wstring sSoundTag, float fVolume, _bool bIsLoop)
{
	return m_pSound_Manager->Play_Music(sSoundTag, fVolume, bIsLoop);
}
HRESULT CGameInstance::Stop_Group(ESOUNDTYPE eSoundType)
{
	return m_pSound_Manager->Stop_Group(eSoundType);
}
FMOD::Sound* CGameInstance::Get_Sound(ESOUNDTYPE eSoundType, wstring sSoundTag)
{
	return m_pSound_Manager->Get_Sound(eSoundType, sSoundTag);
}
HRESULT CGameInstance::Stop_Sound(ESOUNDTYPE eSoundType, wstring sSoundTag)
{
	return m_pSound_Manager->Stop_Sound(eSoundType, sSoundTag);
}

HRESULT CGameInstance::Play_Music_CrossFade(const _wstring& sSoundTag, _float fTargetVolume, _bool bIsLoop, _float fFadeDuration)
{
	return m_pSound_Manager->Play_Music_CrossFade(sSoundTag, fTargetVolume, bIsLoop, fFadeDuration);
}

HRESULT CGameInstance::Stop_Music()
{
	return m_pSound_Manager->Stop_Music();
}

_bool CGameInstance::Is_Music_Playing() const
{
	return m_pSound_Manager->Is_Music_Playing();
}

_bool CGameInstance::Is_Channel_Playing(FMOD::Channel* pChannel) const
{
	return m_pSound_Manager->Is_Channel_Playing(pChannel);
}

FMOD::Channel* CGameInstance::Get_CurrentMusicChannel() const
{
	return m_pSound_Manager->Get_CurrentMusicChannel();
}

const _wstring& CGameInstance::Get_CurrentBGMTag() const
{
	return m_pSound_Manager->Get_CurrentBGMTag();
}

vector<_wstring>& CGameInstance::Get_EnvSoundNames()
{
	return m_pSound_Manager->Get_EnvSoundNames();
}

vector<_wstring> CGameInstance::Get_SoundNames(ESOUNDTYPE eSoundType)
{
	return m_pSound_Manager->Get_SoundNames(eSoundType);
}

void CGameInstance::Update_3DListener(const _float3& vListenerPosition, const _float3& vListenerVelocity, const _float3& vListenerUp, const _float3& vListenerLook)
{
	m_pSound_Manager->Update_Listener(vListenerPosition, vListenerVelocity, vListenerUp, vListenerLook);
}

#pragma endregion

#pragma region TARGET_MANAGER

HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, eFormat, vClearColor);
}
HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}
HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, bool UseDS, bool bClearRenderTarget)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, UseDS, bClearRenderTarget);
}
HRESULT CGameInstance::Begin_RT(const _wstring& strRTTag, bool UseDS, bool bClearRenderTarget, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_RT(strRTTag, UseDS, bClearRenderTarget, pDSV);
}
HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}
HRESULT CGameInstance::Clear_RT(const _wstring& strRTTag)
{
	return m_pTarget_Manager->Clear_RT(strRTTag);
}

HRESULT CGameInstance::Bind_RT_SRV(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_SRV(strTargetTag, pShader, pConstantName);
}

HRESULT Engine::CGameInstance::Copy_RT(const _wstring& strTakeTarget, const _wstring& strGiveTarget)
{
	return m_pTarget_Manager->Copy_RT(strTakeTarget, strGiveTarget);
}

HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_MRT(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_MRT(strMRTTag, pShader, pVIBuffer);
}
HRESULT CGameInstance::Render_RT(const _wstring& strRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_RT(strRTTag, pShader, pVIBuffer);
}
#pragma endregion

#pragma region RHYTHM
void CGameInstance::AddBeatNotifiy(float fSrtBeat, float fEndBeat, wstring sNotyfyTag)
{
	m_pRhythm_Manager->AddBeatNotifiy(fSrtBeat, fEndBeat, sNotyfyTag);
}
void CGameInstance::AddImpulseBeatNotifiy(float fSrtBeat, wstring sNotyfyTag)
{
	m_pRhythm_Manager->AddImpulseBeatNotifiy(fSrtBeat, sNotyfyTag);
}
UniqueBeatListener CGameInstance::CreateBeatListener(wstring sNotifyTag, function<void(float)> funcUpdate, function<void(void)> funcEnter, function<void(void)> funcExit, _bool IsOnce)
{
	return m_pRhythm_Manager->CreateBeatListener(sNotifyTag, funcUpdate, funcEnter, funcExit, IsOnce);
}
void CGameInstance::SetBPM(_float _fBPM)
{
	m_pRhythm_Manager->SetBPM(_fBPM);
}
_float CGameInstance::GetBPM()
{
	return m_pRhythm_Manager->GetBPM();
}
_float CGameInstance::GetElapsedBeat()
{
	return m_pRhythm_Manager->GetElapsedBeat();
}
bool CGameInstance::IsOnBeat(initializer_list<_float> fBeatLst)
{
	return m_pRhythm_Manager->IsOnBeat(fBeatLst);
}
void CGameInstance::ResetBeat()
{
	return m_pRhythm_Manager->ResetBeat();
}
#pragma endregion

#pragma region
HRESULT Engine::CGameInstance::InitializeShadow()
{
	return m_pShadow->InitializeShadow();
}
const Engine::_float4x4* Engine::CGameInstance::GetShadowTransform(Engine::D3DTS eTransformState, Engine::SHADOW eShadow)
{
	return m_pShadow->GetShadowTransform(eTransformState, eShadow);
}
const Engine::_float4x4* CGameInstance::GetShadowVP(Engine::SHADOW eShadow)
{
	return m_pShadow->GetShadowVP(eShadow);
}
void Engine::CGameInstance::UpdateShadow()
{
	m_pShadow->Update();
}
std::vector<Engine::_wstring>Engine::CGameInstance::GetShadowName()
{
	return m_pShadow->GetShadowName();
}
Engine::SHADOW_DESC* Engine::CGameInstance::GetShadowDesc()
{
	return m_pShadow->GetShadowDesc();
}
HRESULT Engine::CGameInstance::SaveShadow(const Engine::_wstring& wstrFileName)
{
	return m_pShadow->SaveShadow(wstrFileName);
}
HRESULT Engine::CGameInstance::LoadShadow(const Engine::_wstring& wstrFileName)
{
	return m_pShadow->LoadShadow(wstrFileName);
}
#pragma endregion

#pragma region
HRESULT Engine::CGameInstance::BindGI(Engine::CShader* pShader)
{
	return m_pGI->BindGI(pShader);
}
void Engine::CGameInstance::SetGI(Engine::GI eGI, Engine::_float fLerp)
{
	m_pGI->SetGI(eGI, fLerp);
}
#pragma endregion

#pragma region PICKING
_bool CGameInstance::RayCastObject(RAY InWorldRay, CGameObject* pGameObject, CPicking::PICKING_DESC* pDesc)
{
	return m_pPicking->RayCastObject(InWorldRay, pGameObject, pDesc);
}

HRESULT CGameInstance::RayToLocalSpace(RAY InWorldRay, CGameObject* pGameObject, RAY& outLocalRay)
{
	return m_pPicking->RayToLocalSpace(InWorldRay, pGameObject, outLocalRay);
}

_bool CGameInstance::RayCastInLocalSpace(RAY InLoaclRay, _fvector pPointA, _fvector pPointB, _fvector pPointC, _float3* pLocalOut, _float* pLocalDist)
{
	return m_pPicking->RayCastInLocalSpace(InLoaclRay, pPointA, pPointB, pPointC, pLocalOut, pLocalDist);
}

list<CPicking::PICKING_DESC> CGameInstance::RayCastObjects(RAY InWorldRay, const _wstring& sLVLTag, vector<wstring> vecLayers)
{
	list<CPicking::PICKING_DESC> lisPickingDescs;
	lisPickingDescs = m_pObject_Manager->RayCastObjects(InWorldRay, sLVLTag, vecLayers);

	return lisPickingDescs;
}

list<CPicking::PICKING_DESC> CGameInstance::PickObjects(const _wstring& sLVLTag, vector<wstring> vecLayers)
{
	return m_pPicking->PickObjects(sLVLTag, vecLayers);
}

#pragma endregion

#pragma region MODULE_MANAGER
void CGameInstance::ModuleMgr_Update() {
	m_pModuleManager->Update();
}
void CGameInstance::ModuleMgr_Request_Initialize() {
	m_pModuleManager->Request_Initialize();
}
const _wstring& CGameInstance::Get_CurrentModuleName() const {
	return m_pModuleManager->Get_CurrentModuleName();
}
const _int CGameInstance::Get_CurrentModuleIndex() {
	return m_pModuleManager->Get_CurrentModuleIndex();
}
const _int CGameInstance::Get_CurrentSectionIndex() {
	return m_pModuleManager->Get_CurrentSectionIndex();
}

CModuleManager::MODULE_DESC* CGameInstance::Find_ModuleDesc_ByName(const _wstring& strModuleName) {
	return m_pModuleManager->Find_ModuleDesc_ByName(strModuleName);
}
CModuleManager::SECTION_DESC* CGameInstance::Find_SectionDesc_ByName(const _wstring& strModuleName, const _wstring& strSectionName) {
	return m_pModuleManager->Find_SectionDesc_ByName(strModuleName, strSectionName);
}

const _int CGameInstance::Get_ModuleIndex_ByName(const _wstring& strModuleName) const {
	return m_pModuleManager->Get_ModuleIndex_ByName(strModuleName);
}
class CGameObject* CGameInstance::FindModule_ByName(const _wstring& strModuleName) {
	return m_pModuleManager->FindModule_ByName(strModuleName);
}

class CGameObject* CGameInstance::FindSection_ByName(const _wstring& strModuleName, const _wstring& strSectionName) {
	return m_pModuleManager->FindSection_ByName(strModuleName, strSectionName);
}
vector<class CGameObject*> CGameInstance::FindObj_IntoModule(const _wstring& strModuleName, const _wstring& strObjectName) {
	return m_pModuleManager->FindObj_IntoModule(strModuleName, strObjectName);
}
vector<weak_ptr<class CRenderer>> CGameInstance::FindRenderers_PerObj_IntoModule(const _wstring& strModuleName, const _wstring& strObjectName) {
	return m_pModuleManager->FindRenderers_PerObj_IntoModule(strModuleName, strObjectName);
}

CModuleManager::SECTION_DESC* CGameInstance::Get_CurrentSectionDesc()
{
	return m_pModuleManager->Get_CurrentSectionDesc();
}

void CGameInstance::SetSequence(Engine::_uint uiSequenceIndex)
{
	m_pSequence->SetSequence(uiSequenceIndex);
}

Engine::_uint CGameInstance::GetCurrentSequenceIndex()
{
	return m_pSequence->GetCurrentSequenceIndex();
}

Engine::_float CGameInstance::GetCurrentSequenceTime()
{
	return m_pSequence->GetCurSequenceTime();
}

Engine::_float CGameInstance::GetPreviousSequenceTime()
{
	return m_pSequence->GetPreSequenceTime();
}

void CGameInstance::AddSequenceFunc(Engine::_uint uiSequenceIndex, Engine::_bool bEnter, std::function<void()> func)
{
	m_pSequence->AddSequenceFunc(uiSequenceIndex, bEnter, std::move(func));
}

#pragma endregion

void CGameInstance::Release_Engine()
{
	DestroyInstance();

	Safe_Release(m_pSequence);
	Safe_Release(m_pGI);
	Safe_Release(m_pShadow);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pDSP_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pLevel_Manager);

	Safe_Release(m_pVideo_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pDebug_Manager);
	Safe_Release(m_pAsset_Manager);
	Safe_Release(m_pImGui_Manager);
	Safe_Release(m_pConfig_Manager);

	Safe_Release(m_pTime_Manager);
	Safe_Release(m_pInput_Manager);
	Safe_Release(m_pRhythm_Manager);

	Safe_Release(m_pGraphic_Device);

	Safe_Release(m_pModuleManager);
	Safe_Release(m_pListenerUpdater);

}

void CGameInstance::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
