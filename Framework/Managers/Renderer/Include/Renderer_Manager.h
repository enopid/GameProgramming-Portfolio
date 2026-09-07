#pragma once
#include"Renderer.h"
#include"Time_Manager_API.h"

NS_BEGIN(Engine)
class CRenderer_Manager final :public CBase
{
public:
	enum class ENVIRONMENT { OUTDOOR, INDOOR };
	typedef struct tagBloomDesc
	{
		Engine::_float  fEmissiveIntensity	= { 1.f };
		Engine::_float	fEmissiveThreshold	= { 0.f };
		Engine::_float	fBlurDepthLimit		= { 1.f };
		Engine::_float	fSpecularValue		= { 1.f };
	} BLOOM_DESC;
	typedef struct tagOLDesc
	{
		Engine::_float3 vEdgeColor			= { 0.f, 0.f, 0.f };

		Engine::_float4 vDepthEdgeProperty	= { 1.0f, 1.5f, 0.50f, 0.02f }; // Weight, Width, Thershold, Smoothness
		Engine::_float4 vNormalEdgeProperty = { 1.0f, 1.0f, 0.70f, 0.02f };
		Engine::_float4 vELEdgeProperty		= { 1.0f, 2.0f, 1.00f, 0.26f };
	} OL_DESC;
	typedef struct tagSSRDesc
	{
		Engine::_uint uiMaxSteps = { 30 };
		Engine::_uint uiBinarySteps = { 3 };
		Engine::_float fMaxDistance = { 30.f };
		Engine::_float fStride = { 1.f };
		Engine::_float fThickness = { 0.5f };
		Engine::_float fRayBias = { 0.02f };
	}SSR_DESC;
	typedef struct tagSSAODesc
	{
		Engine::_float fAOBias = { 0.02f };
		Engine::_float fAORadius = { 0.2f };
		Engine::_float fAOThickness = { 0.3f };
		Engine::_float fDensityOfLine = { 10.f };
	}SSAO_DESC;
	typedef struct tagAfterDesc
	{
		Engine::_float fAfterSpeed = { 0.4f };
		Engine::_float fAfterAlpha = { 0.27f };
	}AFTER_DESC;
	typedef struct tagDiffuseDesc
	{
		Engine::_int	iMSS		= { -1 };
		Engine::_float3 f3Diffuse	= { 1.f,1.f,1.f };
	}DIFFUSE_DESC;
	typedef struct tagRMDesc
	{
		ENVIRONMENT eEnvironment = {};
		Engine::_float fSShadowBias = { 0.01f };
		Engine::_float fDShadowBias = { 0.01f };
		Engine::_float fJudgeShade = { 1.f };
		Engine::_float fDJudgePCF = { 0.1f };
		Engine::_float fShadeBlend = { 0.6f };
		Engine::_float fLightIntensity = { 1.f };
		Engine::_float fDensityOfDotsInDoor = { 25.f };
		Engine::_float fDensityOfDotsOutDoor = { 150.f };
		Engine::_float fHalftoneRadius = { 0.5f };
		Engine::_float fSigma			= { 2.f };
		Engine::_float3	f3LightDiffuse = { 1.f,1.f,1.f };
		Engine::_float3 f3LightAmbient = { 0.f,0.f,0.f };

		OL_DESC			OLDesc					= {};
		BLOOM_DESC		BloomDesc				= {};
		SSR_DESC		SSRDesc					= {};
		SSAO_DESC		SSAODesc				= {};
		AFTER_DESC		AfterDesc				= {};
		DIFFUSE_DESC	DiffuseDesc[20]			= {};
	}RM_DESC;
private:
	CRenderer_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer_Manager() = default;
	HRESULT Initialize();
public:
	HRESULT Add_RenderGroup(RENDERGROUP eGroupID, WPRenderer wpRenderer);
	HRESULT Add_SSGroup(SCREENSPACE eGroupID, WPRenderer wpRenderer);
	void InitRSStates();
	void InitBSStates();
	HRESULT Remove_RenderGroup(RENDERGROUP eGroupID, WPRenderer wpRenderer);
	HRESULT Change_RenderGroup(RENDERGROUP eGroupID, WPRenderer wpRenderer);
	void Draw();
	void DrawStaticShadow();

	void Bind_RSParameters(RSPARAM_DESC _desc);

	std::vector<Engine::_wstring>GetName();
	RM_DESC* GetDesc();
	HRESULT Save(const Engine::_wstring& wstrFileName);
	HRESULT Load(const Engine::_wstring& wstrFileName);

	void SetTimeDelta(Engine::_float fTimeDelta);

	void SetAfterVelocity(Engine::_float2 f2Velocity);
	void SetKaleAfterVelocity(Engine::_float2 f2Velocity);
	//void SetCamDistortion(Engine::_float fDistortionStrength, Engine::_float3 f3CamVelo);
	void SetDistortion(DISTORTION_DESC desc);
	void AddWave(WAVE_DESC Desc);
	void SetColorReverse(Engine::_float fReverse);
	void ChangeWeights();
	void SetAdditionalWeights();
	void SetRMDiffuse(Engine::_float3 f3Diffuse);
	void SetLowHP(Engine::_bool bLowHP);
	void SetExplosion(Engine::_bool bExplosion);
private:
	HRESULT ReadyShadowDepthStencil();
	void ChangeViewport(Engine::_uint uiWidth, Engine::_uint uiHeight);
	void ZSort();

	void RenderShadow();
	void BindCombined();
	void RenderLight();
	void RenderCombined();
	void RenderBlend();
	void RenderLateBlend();
	void RenderParticle();
	void RenderEffect();

	//SS
	void RenderOutLine();
	void RenderSSR();
	void RenderSSAO();
	void RenderAfterimage();
	//PP
	void RenderOL(const wstring& sCurPPScene);
	void RenderBloom(const wstring& sCurPPScene);
	void RenderPPCombine(const wstring& sCurPPScene);
	void RenderCopy	(const wstring& sSourceRTTag, const wstring& sTargetRTTag);
	void RenderAdditionalBlur();
	
	void RenderLateEFT();

	void Render(Engine::RENDERGROUP eGroupID);
	void RenderSS(Engine::SCREENSPACE eGroupID);
	void RenderDebug();

	Engine::_float MakeDistortionUV(Engine::_float2& f2UVPos, Engine::_float2& f2UVRange);
	Engine::_bool MakeWaveUV(Engine::_float2& f2UVPos, Engine::_float& fUVRange, Engine::_float& fUVDistance, std::list<WAVE_DESC>::iterator it1, std::list<Engine::_float>::iterator it2);
	void Update();
	Engine::_float DiffuseApproach(Engine::_float fCurrent, Engine::_float fTarget, Engine::_float fStep);

	HRESULT ReadyWholeRT();

	RM_DESC						m_tDesc;
	ID3D11Device*				m_pDevice																							= { nullptr };
	ID3D11DeviceContext*		m_pContext																							= { nullptr };
	class CGameInstance*		m_pGameInstance																						= { nullptr };
	class CShader*				m_pShader																							= { nullptr };
	class CShader*				m_pSSShader																							= { nullptr };
	class CShader*				m_pPPShader																							= { nullptr };
	class CVIBuffer_Rect*		m_pVIBuffer																							= { nullptr };
	std::list<WPRenderer>		m_listRenderer[ENUM_TO_UINT(RENDERGROUP::END)], m_listSSRenderer[ENUM_TO_UINT(SCREENSPACE::END)];
	//Engine::_uint				m_uiSShadowCount																					= { 1 };
	Engine::_uint				m_uiSShadowSize{ 16384 }, m_uiDShadowSize{ 4096 };	//8192, 16384
	Engine::_float4x4			m_matWorld, m_matView, m_matProjection;
	Engine::_float4x4			m_matDSWorlds[5], m_matDSProjections[5];
	D3D11_VIEWPORT				m_Viewports[5];

	ID3D11RasterizerState*		m_pRS_CullFront		= nullptr;
	ID3D11RasterizerState*		m_pRS_CullBack		= nullptr;
	ID3D11RasterizerState*		m_pRS_CullNone		= nullptr;

	ID3D11BlendState*			m_pBS_Opaque		= nullptr;
	ID3D11BlendState*			m_pBS_AlphaBlend	= nullptr;
	ID3D11BlendState*			m_pBS_Additive		= nullptr;

	ID3D11DepthStencilView*		m_pShadowDSV[ENUM_TO_UINT(SHADOW::END)]{};

	//Afterimage
	enum class AFTER { GREEN = 1, RED, BLUE, END };
	Engine::_float2				m_f2Velocity		= {};
	Engine::_float2				m_f2KaleVelo		= {};
	Engine::_uint				m_uiZero			= {};

	//Color Reverse
	Engine::_float				m_fReverse = {};

	//Distortion
	DISTORTION_DESC				m_tDistortionDesc	= {};
	UniqueTimer					m_upDistortionTimer = nullptr;

	//Wave
	std::list<WAVE_DESC>		m_listWave = {};
	std::list<Engine::_float>	m_listWaveTime = {};
	Engine::_float				m_fTimeDelta = {};
	static constexpr Engine::_uint	m_uiWaveLimit = { 5 };

	//Blur
	Engine::_float				m_fWeights[5] = {};

	//Additional Blur
	Engine::_float				m_fShallowWeights[8] = {};
	Engine::_float				m_fDeepWeights[8] = {};
	Engine::_bool				m_bExplosion = { false };

	//Camera Distortion
	//Engine::_float m_fCamStrength = {};
	//Engine::_float3 m_f3CamVelo = {};

	//Sequence
	Engine::_float3				m_f3PurDiffuse	= { 0.f,0.f,0.f };

	//HP
	Engine::_bool				m_bLowHP	= {};
	Engine::_float				m_fLowHP	= {};

	//Debug
	Engine::_float				m_fDebugU = { 1.f };
public:
	static CRenderer_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void		SetUP();
private:
	virtual void Free()override;
private:
	_int		m_iState       = 5; // 0:
	_bool		m_bUseNormalOL = true;
	_bool		m_bUseDepthOL  = true;
};
NS_END

//#include "Renderer.h"
//#include "Base.h"
//#include "Time_Manager_API.h"
//#include "Asset.h"
//
//NS_BEGIN(Engine)
//
//class CRenderer_Manager final : public CBase
//{
//	enum ECullMode {
//		CULLMODE_BACK,
//		CULLMODE_FRONT,
//		CULLMODE_NONE,
//		CULLMODE_END
//	};
//	enum EFillMode {
//		FILLMODE_SOLID,
//		FILLMODE_WIREFRAME,
//		FILLMODE_END
//	};
//private:
//	CRenderer_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual ~CRenderer_Manager() = default;
//
//public:
//	HRESULT Initialize();
//	HRESULT Add_RenderGroup		(RENDERGROUP eGroupID, WPRenderer pRenderer);
//	HRESULT Add_SSGroup			(SCREENSPACE eGroupID, WPRenderer pRenderer);
//	HRESULT Change_RenderGroup	(RENDERGROUP eGroupID, WPRenderer pRenderer);
//	void	Draw();
//
//	HRESULT SetRenderState(_uint iCullModeIdx, _uint iFillModeIdx);
//
//private:
//	ID3D11Device*			m_pDevice		= { nullptr };
//	ID3D11DeviceContext*	m_pContext		= { nullptr };
//	class CGameInstance*	m_pGameInstance	= { nullptr };
//
//	list<WPRenderer>			m_Renderers[ENUM_TO_UINT(RENDERGROUP::END)];
//	list<WPRenderer>			m_SSRenderers[ENUM_TO_UINT(SCREENSPACE::END)];
//private:
//	class CShader*					m_pShader	= { nullptr };
//	class CShader*					m_pSSShader	= { nullptr };
//	class CShader*					m_pPPShader	= { nullptr };
//	class CVIBuffer_Rect*			m_pVIBuffer = { nullptr };
//	
//private:
//	_float4x4		m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
//	_float4x4		m_WorldMatrices[5], m_ViewMatrices[5], m_ProjMatrices[5];
//	_float4x4		m_PhotoWorldMatrices[4];
//	D3D11_VIEWPORT	m_ViewPorts[5];
//private:
//	void Render_NonBlend();
//	void Render_LightAcc();
//
//	void Render_Priority();
//	void Render_Combined();
//	void Render_SSD();
//	void Render_Unlit();
//	void Render_Blend();
//	void Render_UI();
//
//	//SS
//	wstring Render_Copy	(const wstring& sSourceRTTag, const wstring& sTargetRTTag);
//	wstring Render_Fog	(const wstring& sSourceRTTag);
//
//	//PP
//	void	Render_Bloom(const wstring& sCurPPScene);
//	void	Render_PPCombine(const wstring& sCurPPScene);
//	
//	void Render_Debug();
//
//	void Render_Group(Engine::RENDERGROUP eRenderGroup);
//	void Render_SSGroup(Engine::SCREENSPACE eRenderGroup);
//public:
//	static CRenderer_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual void Free() override;
//private:
//	ID3D11RasterizerState*		m_pRS_Solid[CULLMODE_END][FILLMODE_END];
//
//	ID3D11BlendState*			m_pBS_Opaque	= nullptr;
//	ID3D11BlendState*			m_pBS_AlphaBlend= nullptr;
//
//	//Depth Write X, Depth Test X
//	//Depth Write X, Depth Test O
//	//Depth Write O, Depth Test O
//	ID3D11DepthStencilState*	m_pDSS[3] = { nullptr , nullptr ,nullptr };
//};
//
//NS_END