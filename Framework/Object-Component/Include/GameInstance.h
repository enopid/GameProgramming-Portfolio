#pragma once

#include "Prototype_Manager.h"
#include "Config_Manager.h"
#include "ImGUI_Manager.h"
#include "Time_Manager_API.h"
#include "Input_Manager_API.h"
#include "DebugManager_Api.h"
#include "Rhythm_Manager_API.h"
#include "Renderer.h"
#include "Renderer_Manager.h"
#include "Asset.h"
#include "ComputeShader.h"
#include "Picking.h"
#include "ModuleManager.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
private:
	CGameInstance();	
	virtual ~CGameInstance() = default;

#pragma region Engine
public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void	Update_Engine();
	HRESULT Begin_Draw(const _float4& vClearColor);
	HRESULT Draw();
	HRESULT End_Draw();
	HRESULT Clear(const _wstring& strLVLTag);
	//ID3D11ShaderResourceView* GetShaderResourceView();
#pragma endregion

#pragma region Editor
	HRESULT Load_Level_InEditor(const wstring& sLVLTag,		  wstring& sLVLPrototypeTag);
	HRESULT Save_Level_InEditor(const wstring& sLVLTag, const wstring& sLVLPrototypeTag);

	HRESULT AddLayer	(const _wstring& sLVLTag, const _wstring& sLayerTag);
	HRESULT DeleteLayer	(const _wstring& sLVLTag, const _wstring& sLayerTag);
	HRESULT RenameLayer	(const _wstring& sLVLTag, const _wstring& sOldLayerTag, const _wstring& sNewLayerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
	
	HRESULT		Load_Level(const tagLevelTransitionDesc& _LVLTransitDesc);
	HRESULT		Change_Level();
	HRESULT		Create_Level(const wstring& sLVLPrototypeTag, const wstring& sLVLName);
	_wstring	GetCurrentLevelTag();
	_wstring	GetCurrentDetailLevelTag();
	tagLevelTransitionDesc	GetLevelDesc();
	class CUIController* GetCurrentLevelUC();
	class CUIController* GetUC(wstring sUCName);
	class CLevel* GetCurrentLevel();
#pragma endregion

#pragma region TIME_MANAGER
	UniqueTimer CreateTimerWithFrequency(double _frequency, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive);
	UniqueTimer CreateTimerWithDuration(double _duration, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive);
	void		SetTimeScale(_float fScale);
#pragma endregion

#pragma region Input_MANAGER
	bool				IsKeyState			(_ubyte byKeyID, EKEYACTIONSTATE _KeyState);
	bool				IsMouseBtnState		(EMOUSEBTN eMouseBtn, EKEYACTIONSTATE _KeyState);
	_long				GetMouseMoveState	(EMOUSEMOVE eMouseMove);

	UniqueKeyContext	CreateKeyContext(bool _bInitialActive);
	POINT				GetMouseCursor();
	bool				IsCursorInRect(const CUIObject::FLOAT_RECT& _rect);
	bool				IsCursorInRect(const RECT& _rect);
#pragma endregion

#pragma region Resource_MANAGER
	HRESULT				LoadAsset(SP_Asset& _spAsset, _wstring  _sName);
	vector<_wstring>	GetAssetNames(EASSETTYPE eAssetType);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT				Add_Prototype				(PROTOTYPE ePrototype, const _wstring& strPrototypeTag, class CBase* pPrototype, _bool _bIsClassPrototype = false);
	CBase*				Clone_Prototype				(PROTOTYPE ePrototype, const _wstring& strPrototypeTag, void* pArg = nullptr);
	SPComponent			Clone_ComponentPrototype(const _wstring& strPrototypeTag, void* pArg = nullptr);
	bool				IsClassPrototype				(PROTOTYPE ePrototype, const _wstring& strPrototypeTag);
	vector<_wstring>	GetPrototypeNames	(PROTOTYPE ePrototype, _bool _bOnlyIsClassPrototype);

	template<typename T>
	HRESULT AddComponentClsPrototype();
	template<typename T>
	HRESULT AddShaderClsPrototype(const wstring& sComName, const wstring& sShaderPath);
	template<typename T>
	HRESULT AddComputeShaderClsPrototype(const _wstring& sComName, const _wstring& sShaderPath);
	template<typename T>
	HRESULT AddLevelPrototype(wstring sLevelName);
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	vector<class CGameObject*>	Get_Terrains(const _wstring& strLVLTag, const _wstring& strLayerTag);
	HRESULT						ChangeLayerTimeScale(const _wstring& sLVLTag, const _wstring& sLayerTag, float fTimeScale);
	class CGameObject*			Get_Object		(const _wstring& strLVLTag, const _wstring& strLayerTag, const _wstring& strObjectName);
	list<CGameObject*>			Get_Objects		(const _wstring& strLVLTag, const _wstring& strLayerTag);
	HRESULT						Add_GameObject_ToLayer		(const _wstring& strPrototypeTag, const _wstring& strLVLTag, const _wstring& strLayerTag, CGameObject** ppOut = nullptr, void* pArg = nullptr);
	HRESULT						SaveObjects					(fstream& _fs, const _wstring& strLVLTag);
	HRESULT						LoadObjects					(fstream& _fs, const _wstring& strLVLTag);

	wstring						FindPossibleName(const _wstring& sLVLTag, const wstring& sName);

	vector<_wstring>			GetLayerNames	(const _wstring& strLVLTag);
	HRESULT						ChangeObjectLayer(const _wstring& sLVLTag, const _wstring& sNewLayerTag, CGameObject* pObject);
	HRESULT						CopyObject(const _wstring& sLVLTag, CGameObject* pObject);
	CGameObject*				Render_ImGui_Hierarchy	(CGameObject* pLastSelected);
#pragma endregion

#pragma region CONFIG_MANAGER
	HRESULT LoadConfig(const wstring& _ConfigPath);
	HRESULT LoadJson(const wstring& _ConfigPath, Json::Value& outJson);

	template<typename T>
	T ReadConfig(const wstring& _ConfigPath, T* _pOut = nullptr);
	template<typename T>
	T ReadSubConfig(const wstring& _subConfigTag, const wstring& _ConfigPath, T* _pOut = nullptr);
	template<typename T>
	vector<T> ReadArrayConfig(const wstring& _ConfigPath, vector<T>* _pOut = nullptr);
	template<typename T>
	vector<T> ReadArraySubConfig(const wstring& _subConfigTag, const wstring& _ConfigPath, vector<T>* _pOut = nullptr);
#pragma endregion

#pragma region ImGUI_MANAGER
	template<typename T>
	T* Create_ImGui_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	template<typename T>
	T* Find_ImGui_Object();

	HRESULT ImGUI_Render();
	void ImGUI_Clear();
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDERGROUP eGroupID, WPRenderer pRenderer);
	HRESULT Add_SSGroup(SCREENSPACE eGroupID, WPRenderer pRenderer);
	HRESULT Change_RenderGroup(RENDERGROUP eGroupID, WPRenderer pRenderer);
	HRESULT Remove_RenderGroup(RENDERGROUP eGroupID, WPRenderer pRenderer);
	void	Debug_Render();
	void	AddDebugRenderCall(const DEBUGRENDER_DESC* pDesc);
	void	DrawStaticShadow();
	void	BindRSParam(RSPARAM_DESC _desc);
	std::vector<Engine::_wstring>GetRMName();
	CRenderer_Manager::RM_DESC* GetRMDesc();
	HRESULT SaveRM(const Engine::_wstring& wstrFileName);
	HRESULT LoadRM(const Engine::_wstring& wstrFileName);
	void SetAfterVelocity(const Engine::_float2& f2Velocity);
	void SetKaleAfterVelocity(const Engine::_float2& f2Velocity);
	//void SetCamDistortion(Engine::_float fDistortionStrength, Engine::_float3 f3CamVelo);
	void SetDistortion(DISTORTION_DESC Desc);
	void SetExplosion(Engine::_bool bExplosion);
	void AddWave(WAVE_DESC Desc);
	void SetColorReverse(Engine::_float fReverse);
	void ChangeWeights();
	void SetRMDiffuse(Engine::_float3 f3Diffuse);
	void SetLowHP(Engine::_bool bLowHP);
#pragma endregion

#pragma region COLLISION
	HRESULT		Add_Collidor(WPCollidor wpCollidor);
	HRESULT		ActiveCollision(_uint _id1, _uint _id2);
	bool		RayCast_Collidor(_int iCollisionMask, RAY ray, _float& fInOutDist, list<pair<_float, WPCollidor>>& lstWPOutCollidor);
#pragma endregion
#pragma region DSP
	void				Render_DSPManager();
	HRESULT				AddDSPComponent(WPDSP wpDSP);
	HRESULT				AddDSP(string sDSPTag, string sConstantName);
	HRESULT				SetDSP(string sDSPTag, string sConstantName, DSPVALUEDESC desc);
	DSPVALUEDESC		GetDSP(string sDSPTag, string sConstantName);
	HRESULT				BindDSP(class CMeshRenderer* pMeshRenderer, class CShader* pShader);
	vector<string>		GetDSPNames();
	vector<string>		GetDSPParamNames(string sDSPTag);
#pragma endregion

#pragma region Video_MANAGER
public:
	HRESULT			Bind_VideoTexture(class CShader* m_pShader, const _char* pConstantName);
	VIDEOINFO_DESC  GetVideoDesc();
	vector<wstring> GetVideoNames();

	HRESULT LoadVideo(const wstring& sFilePath);
	HRESULT SetTime(_float fTime);
	void    SetLoop(_bool bIsLoop);
	void    SetClip(_bool bIsClip, _float fSrtTime, _float fendTime);
	void    Play_Video();                 
	void    Pause_Video();
	void    Restart_Video();              
	void    Reset_Video();                
#pragma endregion

#pragma region PIPELINE
	void					Set_Transform(D3DTS eTransformState, _fmatrix TransformMatrix);
	const _float4x4*		Get_Transform(D3DTS eTransformState);
	const _float4x4*		Get_InverseTransform(D3DTS eTransformState);
	const _float4x4*		Get_VP();
	const _float4*			Get_CamPosition();
	const _float4*			Get_PrevCamPosition();
	const _float4*			Get_CamUp();
	const _float4*			Get_CamLook();
	const _float			Get_CamFOV();
	RAY						GetViewportRay();
	_vector					ConvertViewportPos(_vector _vWorldPos);
	const BoundingFrustum*	Get_Frustum();
	HRESULT					Bind_CamPosition(class CShader* pShader, const _char* pConstantName);
	HRESULT					Bind_TransformStateMatrix(class CShader* pShader, const _char* pConstantName, D3DTS eTransformState);
	HRESULT					Bind_TransformInverseStateMatrix(class CShader* pShader, const _char* pConstantName, D3DTS eTransformState);

#pragma endregion

#pragma region LIGHT_MANAGER
	vector<_wstring>	GetLightMapNames();
	HRESULT				SaveLightMap(const wstring& sFileName);
	HRESULT				LoadLightMap(const wstring& sFileName);
	const LIGHT_DESC*	Get_LightDesc(_uint iIndex);
	GLOBALLIGHT_DESC*	Get_GlobalLightDesc();
	ID3D11ShaderResourceView* GetLUT();
	_uint				Get_LightSize();
	class CLight*		Get_Light(_uint iIndex);
	HRESULT				Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT				Delete_Light(const Engine::_char* cName);
	CLight*				Add_Dynamic_Light(const LIGHT_DESC& LightDesc);
	void				Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT				Add_Chasing_Light(const Engine::_vector& vPos, CHASING_TYPE eType, Engine::_uint uiShape = 0, Engine::_uint uiShutDown = 1);
	HRESULT				BindShutDown(class CShader* pShader);
	void				SetSamurai(const Engine::_vector& vPos);
#pragma endregion

#pragma region Atmosphere
	vector<_wstring>	GetAtmosphereNames();
	HRESULT				LoadAtmosphere(_wstring sFileName);
	HRESULT				SaveAtmosphere(_wstring sFileName);
	ATMOSPHERE_DESC*	GetAtmosphereDesc();
#pragma endregion

#pragma region SOUND_MANAGER
	HRESULT			Play_SFX	(wstring sSoundTag, float fVolume = 1.f, int fPriority = 0, float fEndPoint = -1.f, bool bReset = true, bool bLoop = false);
	HRESULT			Play_3D_EnvSound	(const _wstring& sSoundTag, const _float3& vWorldPosition, const _float3& vWorldVelocity, _float fVolume, _float fMinDist = 1.f, _float fMaxDist = 8.f, _bool bIsLoop = false, _int iPriority = 256);
	HRESULT			Play_3D_Env_NonRandom(const _wstring& sSoundTag, const _float3& vWorldPosition, const _float3& vWorldVelocity, _float fVolume, _float fMinDist = 1.f, _float fMaxDist = 8.f, _bool bIsLoop = false, _int iPriority = 256);
	HRESULT			Play_VOX	(wstring sSoundTag, float fVolume = 1.f, int fPriority = 0);
	HRESULT			Play_Music	(wstring sSoundTag, float fVolume = 1.f, _bool bIsLoop = true);
	HRESULT			Stop_Group	(ESOUNDTYPE eSoundType);
	FMOD::Sound*	Get_Sound	(ESOUNDTYPE eSoundType, wstring sSoundTag);
	HRESULT			Stop_Sound	(ESOUNDTYPE eSoundType, wstring sSoundTag);

	HRESULT			Play_Music_CrossFade(const _wstring& sSoundTag, _float fTargetVolume = 1.f, _bool bIsLoop = true, _float fFadeDuration = 1.f);

	HRESULT Stop_Music();
	_bool Is_Music_Playing() const;
	_bool Is_Channel_Playing(FMOD::Channel* pChannel) const;
	FMOD::Channel* Get_CurrentMusicChannel() const;
	const _wstring& Get_CurrentBGMTag() const;

	vector<_wstring>&	Get_EnvSoundNames();
	vector<_wstring>	Get_SoundNames(ESOUNDTYPE eSoundType);

	void Update_3DListener(const _float3& vListenerPosition, const _float3& vListenerVelocity, const _float3& vListenerUp, const _float3& vListenerLook);
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag,	bool UseDS = true, bool bClearRenderTarget = true);
	HRESULT Begin_RT(const _wstring& strRTTag, bool UseDS = true, bool bClearRenderTarget = true, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Clear_RT(const _wstring& strRTTag);
	HRESULT Bind_RT_SRV(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RT(const _wstring& strTakeTarget, const _wstring& strGiveTarget);

	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Render_RT(const _wstring& strRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer);
#pragma endregion

#pragma region 
	void		AddBeatNotifiy(float fSrtBeat, float fEndBeat, wstring sNotyfyTag);
	void		AddImpulseBeatNotifiy(float fSrtBeat, wstring sNotyfyTag);
	UniqueBeatListener		CreateBeatListener(wstring sNotifyTag, function<void(float)> funcUpdate, function<void(void)> funcEnter, function<void(void)> funcExit, _bool IsOnce);

	void	SetBPM(_float _fBPM);
	_float	GetBPM();
	_float	GetElapsedBeat();

	bool	IsOnBeat(initializer_list<_float> fBeatLst = {});
	void	ResetBeat();
#pragma

#pragma region SHADOW
	HRESULT InitializeShadow();
	const Engine::_float4x4* GetShadowTransform(Engine::D3DTS eTransformState, Engine::SHADOW eShadow);
	const Engine::_float4x4* GetShadowVP(Engine::SHADOW eShadow);
	void UpdateShadow();
	std::vector<Engine::_wstring>GetShadowName();
	Engine::SHADOW_DESC* GetShadowDesc();
	HRESULT SaveShadow(const Engine::_wstring& wstrFileName);
	HRESULT LoadShadow(const Engine::_wstring& wstrFileName);
#pragma endregion

#pragma region GI
	HRESULT BindGI(class CShader* pShader);
	void SetGI(Engine::GI eGI, Engine::_float fLerp);
#pragma endregion

#pragma region PICKING
	
	_bool							RayCastObject			(RAY InWorldRay, class CGameObject* pGameObject, CPicking::PICKING_DESC* pDesc);									
	HRESULT							RayToLocalSpace			(RAY InWorldRay, class CGameObject* pGameObject, RAY& outLocalRay);													
	_bool							RayCastInLocalSpace		(RAY InLocalRay, _fvector pPointA, _fvector pPointB, _fvector pPointC, _float3* pLocalOut, _float* pLocalDist);		
	list<CPicking::PICKING_DESC>	RayCastObjects	(RAY InWorldRay, const _wstring& sLVLTag, vector<wstring> vecLayers);						
	list<CPicking::PICKING_DESC>	PickObjects		(const _wstring& sLVLTag, vector<wstring> vecLayers);							
#pragma endregion

#pragma region MODULE_MANAGER
	void ModuleMgr_Update();
	void ModuleMgr_Request_Initialize();
	const _wstring& Get_CurrentModuleName() const;
	const _int Get_CurrentModuleIndex();
	const _int Get_CurrentSectionIndex();

	const _int Get_ModuleIndex_ByName(const _wstring& strModuleName) const;

	CModuleManager::MODULE_DESC* Find_ModuleDesc_ByName(const _wstring& strModuleName);
	class CGameObject* FindModule_ByName(const _wstring& strModuleName);

	CModuleManager::SECTION_DESC* Find_SectionDesc_ByName(const _wstring& strModuleName, const _wstring& strSectionName);
	class CGameObject* FindSection_ByName(const _wstring& strModuleName, const _wstring& strSectionName);

	vector<class CGameObject*> FindObj_IntoModule(const _wstring& strModuleName, const _wstring& strObjectName);
	vector<weak_ptr<class CRenderer>> FindRenderers_PerObj_IntoModule(const _wstring& strModuleName, const _wstring& strObjectName);

	CModuleManager::SECTION_DESC* Get_CurrentSectionDesc();
#pragma endregion

#pragma region SEQUENCE
	void SetSequence(Engine::_uint uiSequenceIndex = 1);
	Engine::_uint GetCurrentSequenceIndex();
	Engine::_float GetCurrentSequenceTime();
	Engine::_float GetPreviousSequenceTime();
	void AddSequenceFunc(Engine::_uint uiSequenceIndex, Engine::_bool bEnter, std::function<void()>func);
#pragma endregion

private:
	class CGraphic_Device*		m_pGraphic_Device		= { nullptr };
	class CLevel_Manager*		m_pLevel_Manager		= { nullptr };
	class CTime_Manager*		m_pTime_Manager			= { nullptr };
	class CInput_Manager*		m_pInput_Manager		= { nullptr };
	class CAsset_Manager*		m_pAsset_Manager		= { nullptr };
	class CVideo_Manager*		m_pVideo_Manager		= { nullptr };
	class CDebugManager*		m_pDebug_Manager		= { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager	= { nullptr };
	class CObject_Manager*		m_pObject_Manager		= { nullptr };
	class CImGui_Manager*		m_pImGui_Manager		= { nullptr };
	class CConfig_Manager*		m_pConfig_Manager		= { nullptr };
	class CRenderer_Manager*	m_pRenderer_Manager		= { nullptr };
	class CCollision_Manager*	m_pCollision_Manager	= { nullptr };
	class CDSP_Manager*			m_pDSP_Manager			= { nullptr };
	class CPipeLine*			m_pPipeLine				= { nullptr };
	class CLight_Manager*		m_pLight_Manager		= { nullptr };
	class CTarget_Manager*		m_pTarget_Manager		= { nullptr };
	class CSound_Manager*		m_pSound_Manager		= { nullptr };
	class CRhythm_Manager*		m_pRhythm_Manager		= { nullptr };
	class CShadow*				m_pShadow				= { nullptr };
	class CGI*					m_pGI					= { nullptr };
	class CPicking*				m_pPicking				= { nullptr };
	class CFrustum*				m_pFrustum				= { nullptr };
	class CModuleManager*		m_pModuleManager		= { nullptr };
	class CSequence*			m_pSequence				= { nullptr };
	class CListenerUpdater*		m_pListenerUpdater		= { nullptr };
	
	ATMOSPHERE_DESC				m_atmosphereDesc = {};

	HWND						m_hwnd		= {};
	ID3D11Device*				m_pDevice	= { nullptr };
	ID3D11DeviceContext*		m_pContext	= { nullptr };
	_float						m_FPS = 60.f;
public:
	_bool	m_bLevelChangeCall = false;
	void Release_Engine();
	virtual void Free() override;

};

NS_END

template<typename T>
inline T* CGameInstance::Create_ImGui_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return m_pImGui_Manager->Create_ImGui_Object<T>(pDevice, pContext);
}

template<typename T>
inline T* CGameInstance::Find_ImGui_Object()
{
	return m_pImGui_Manager->Find_ImGui_Object<T>();
}


template<typename T>
inline HRESULT CGameInstance::AddComponentClsPrototype() {
	return Add_Prototype(PROTOTYPE::COMPONENT, CComponent::GetTypeName<T>(), CComponent::Create<T>(m_pDevice, m_pContext), true);
}

template<typename T>
inline HRESULT CGameInstance::AddShaderClsPrototype(const wstring& sComName, const wstring& sShaderPath) {
	return Add_Prototype(PROTOTYPE::COMPONENT, sComName, CShader::Create<T>(m_pDevice, m_pContext, sShaderPath.c_str()), true);
}

template<typename T>
inline HRESULT CGameInstance::AddComputeShaderClsPrototype(const _wstring& sComName, const _wstring& sShaderPath) {
	return Add_Prototype(PROTOTYPE::COMPONENT, sComName, T::Create(m_pDevice, m_pContext, sShaderPath.c_str()), true);
}

template<typename T>
inline HRESULT CGameInstance::AddLevelPrototype(wstring sLevelName) {
	return Add_Prototype(PROTOTYPE::LEVEL, sLevelName, T::Create(m_pDevice, m_pContext, &sLevelName), true);
}

template<typename T>
inline T CGameInstance::ReadConfig(const wstring& _ConfigPath, T* _pOut)
{
	if (_pOut) {
		*_pOut = m_pConfig_Manager->GetValue<T>(ws2s(_ConfigPath));
		return *_pOut;
	}
	return m_pConfig_Manager->GetValue<T>(ws2s(_ConfigPath));
}

template<typename T>
inline T CGameInstance::ReadSubConfig(const wstring& _subConfigTag, const wstring& _ConfigPath, T* _pOut)
{
	if (_pOut) {
		*_pOut = m_pConfig_Manager->GetValue<T>(ws2s(_ConfigPath), _subConfigTag);
		return *_pOut;
	}
	return m_pConfig_Manager->GetValue<T>(ws2s(_ConfigPath), _subConfigTag);
}

template<typename T>
inline vector<T> CGameInstance::ReadArrayConfig(const wstring& _ConfigPath, vector<T>* _pOut)
{
	if (_pOut) {
		*_pOut = m_pConfig_Manager->GetArray<T>(ws2s(_ConfigPath));
		return *_pOut;
	}
	return m_pConfig_Manager->GetArray<T>(ws2s(_ConfigPath));
}

template<typename T>
inline vector<T> CGameInstance::ReadArraySubConfig(const wstring& _subConfigTag, const wstring& _ConfigPath, vector<T>* _pOut)
{
	if (_pOut) {
		*_pOut = m_pConfig_Manager->GetArray<T>(ws2s(_ConfigPath), _subConfigTag);
		return *_pOut;
	}
	return m_pConfig_Manager->GetArray<T>(ws2s(_ConfigPath), _subConfigTag);
}
