#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CGameObject;
class CComponent;

class CTransform;
class CImageUI;
class CVideoUI;
class CButtonUI;
class CTextUI;

class CVIBuffer;
class CVIBuffer_Rect;
class CVIBuffer_UIRect;
class CVIBuffer_Cube;
class CVIBuffer_Terrain;
class CVIBuffer_Trail;
class CVIBuffer_Particle_Rect;
class CMesh;
class CVIBuffer_InsMesh;

class CShader;
class CComputeShader;

class CRenderer;
class CMeshRenderer;
class CInstancingMeshRenderer;
class CTerrainRenderer;
class CTrailRenderer;
class CParticleRenderer;
class CSkyBoxRenderer;
class CDecalRenderer;

class CUIImageRenderer;
class CUITextRenderer;
class CUIAnimator;
class CUIVideoRenderer;

class CModule_Volume;

class CModel;
class CInstancing_BaseModel;
class CCollidor;
class CDynamicShaderParam;
class CRotationComponent;
class CSpatialSound_Component;
class CObjectMoveComponent;
class CEmissiveCtrlComponent;

using SPComponent = shared_ptr	<CComponent>;
using WPComponent = weak_ptr	<CComponent>;

using WPTransform = weak_ptr	<CTransform>;
using WPRenderer  = weak_ptr	<CRenderer>;
using WPCollidor  = weak_ptr	<CCollidor>;
using WPDSP		  = weak_ptr	<CDynamicShaderParam>;
using WPShader    = weak_ptr	<CShader>;
using WPVIBuffer  = weak_ptr	<CVIBuffer>;


using WPTrailRenderer = weak_ptr	<CTrailRenderer>;

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	const static _uint MAXDESCSIZE = 50000;
	const static _uint MAXCOMNAMESIZE = 60;
private:
	const static _uint COMPONENTHEADER = 0x900DFA11;
	const static _uint COMPONENTFOOTER = 0xBAADFA11;

protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize			(void* pDesc);
	virtual HRESULT Load_Asset() { return S_OK; };
	virtual HRESULT Late_Initialize		() { return S_OK; };

	virtual void Priority_Update(_float fTimeDelta) {};
	virtual void Update			(_float fTimeDelta)	{};
	virtual void Late_Update	(_float fTimeDelta) {};


	virtual void		Free();
	virtual SPComponent Clone(void* pArg)			PURE;
	virtual void		Render_Inspector()			PURE;

protected:
	virtual HRESULT Save(void* _pDesc, _uint& _iSize) const	PURE;
	virtual HRESULT Load(void* _pDesc)						PURE;

public:
	_bool	IsLateInitialized() { return m_bIsLateInitialize; }
	HRESULT CheckLateInitialize() { 
		if (!m_bIsLateInitialize) {
			m_bIsLateInitialize = true;
			return Late_Initialize();
		}
		else 
			return S_OK; 
	};
	_int GetInitializePriority() { return m_iLateInitializePriority; }

	void			SetGameObject(CGameObject* _pGameObject);
	CGameObject*	GetGameObject();
	bool			IsUpdateComponent() { return m_bIsUpdateComponent; };

public:
	template<typename T, enable_if_t<is_base_of_v<CComponent, T>>* = nullptr>
	static T* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr) {
		T* pInstance = new T(pDevice, pContext);
		pInstance->m_sComPrototypeTag = CComponent::GetTypeName<T>();

		if (FAILED(pInstance->Initialize_Prototype(pArg)))
		{
			wstring _msg = L"Failed to Create : Component ";
			_msg += CComponent::GetTypeName<T>();
			MSG_BOXL(_msg.c_str());
			Safe_Release(pInstance);
		}

		return pInstance;
	}

	template<typename T, enable_if_t<is_base_of_v<CComponent, T>>* = nullptr>
	SPComponent CloneBase(void* pDesc) {
		auto spComponent = SPComponent(new T(*static_cast<T*>(this)), [=](CComponent* pComponent) {
			Safe_Release(pComponent);
			delete pComponent;
		});

		spComponent->m_wpComponent = dynamic_pointer_cast<CComponent>(spComponent);

		if (FAILED(spComponent->Initialize(pDesc)))
		{
			wstring _msg = L"Failed to Clone : Component ";
			_msg += CComponent::GetTypeName<T>();
			MSG_BOXL(_msg.c_str());
		}

		return spComponent;
	}

public:
	const wstring& GetTypeName()	const { return m_sComPrototypeTag; };
	void  SetTypeName(const _wstring& sNewPrototypeTag);

	const wstring& GetName()		const { return m_sComTag; };
	void  SetName		(const _wstring& sNewNameTag);
	
	void  SetInitialAdd();
	_bool IsInitialAdd() { return m_bIsInitialAdd; }

	void	SetActive(_bool bIsActive)	{ m_bIsActive = bIsActive; }
	_bool 	IsActive();
public:
	template<typename T>
	static wstring GetTypeName() { return L"Empty Component"; };

	static HRESULT SaveComponent(fstream& _fs, const CComponent* _pComponent);
	static HRESULT LoadComponent(fstream& _fs, SPComponent& spComponent);
	
protected:
	ID3D11Device*			m_pDevice		= nullptr;
	ID3D11DeviceContext*	m_pContext		= nullptr;
	class CGameInstance*	m_pGameInstance = nullptr;

	_bool m_bIsInitialAdd			= false;
	_bool m_bIsLateInitialize		= false;
	_int  m_iLateInitializePriority = 0;
protected:
	WPComponent		m_wpComponent		= { };

	wstring			m_sComPrototypeTag	= { L"Empty Component" };
	wstring			m_sComTag			= { L"Empty Component" };
	CGameObject*	m_pOwnerObj			= { nullptr };
	_bool			m_bIsUpdateComponent= { false };
	_bool			m_bIsActive			= { true };
};

template<typename T, enable_if_t<is_base_of_v<CComponent, T>>* = nullptr>
weak_ptr<T> ConvertWPComponent(WPComponent wpComponent) {
	weak_ptr<T> _wpComponent;
	if (auto spComponent = wpComponent.lock()) {
		_wpComponent = static_pointer_cast<T>(spComponent);
	}
	return _wpComponent;
}

template<>
inline static wstring CComponent::GetTypeName<CTransform>()					{ return L"Transform_Component";		};
template<>
inline static wstring CComponent::GetTypeName<CVIBuffer_Rect>()				{ return L"VIBufferRect_Component";		};
template<>
inline static wstring CComponent::GetTypeName<CVIBuffer_Cube>()				{ return L"VIBufferCube_Component";		};
template<>
inline static wstring CComponent::GetTypeName<CVIBuffer_Terrain>()			{ return L"VIBufferTerrain_Component";	};
template<>
inline static wstring CComponent::GetTypeName<CVIBuffer_Trail>()			{ return L"VIBufferTrail_Component";	};
template<>
inline static wstring CComponent::GetTypeName<CVIBuffer_Particle_Rect>()	{ return L"VIBufferParticleRect_Component";	};
template<>
inline static wstring CComponent::GetTypeName<CImageUI>()					{ return L"ImageUI_Component";			};
template<>
inline static wstring CComponent::GetTypeName<CVideoUI>()			{ return L"VideoUI_Component";			};
template<>
inline static wstring CComponent::GetTypeName<CButtonUI>()			{ return L"ButtonUI_Component";			};
template<>
inline static wstring CComponent::GetTypeName<CTextUI>()			{ return L"TextUI_Component";			};
template<>
inline static wstring CComponent::GetTypeName<CModel>()				{ return L"Model_Component";			};
template<>
inline static wstring CComponent::GetTypeName<CTerrainRenderer>()	{ return L"TerrainRenderer_Component";	};
template<>
inline static wstring CComponent::GetTypeName<CSkyBoxRenderer>()	{ return L"SkyboxRenderer_Component";	};
template<>
inline static wstring CComponent::GetTypeName<CParticleRenderer>()	{ return L"ParticleRenderer_Component";	};
template<>
inline static wstring CComponent::GetTypeName<CComputeShader>()		{ return L"ComputeShader_Component";	};
template<>
inline static wstring CComponent::GetTypeName<CTrailRenderer>()		{ return L"TrailRenderer_Component";	};
template<>
inline static wstring CComponent::GetTypeName<CDecalRenderer>()		{ return L"DecalRenderer_Component";	};
template<>
inline static wstring CComponent::GetTypeName<CCollidor>()			{ return L"Collidor_Component";			};

template<>
inline static wstring CComponent::GetTypeName<CModule_Volume>() { return L"ModuleVolume_Component"; };
template<>
inline static wstring CComponent::GetTypeName<CRotationComponent>() { return L"Rotation_Component"; };
template<>
inline static wstring CComponent::GetTypeName<CSpatialSound_Component>() { return L"SpatialSound_Component"; };
template<>
inline static wstring CComponent::GetTypeName<CObjectMoveComponent>() { return L"ObjectMove_Component"; };
template<>
inline static wstring CComponent::GetTypeName<CEmissiveCtrlComponent>() { return L"EmissiveCtrl_Component"; };

template<>
inline static wstring CComponent::GetTypeName<CUIImageRenderer>() { return L"UIImageRenderer_Component"; };
template<>
inline static wstring CComponent::GetTypeName<CUITextRenderer>() { return L"UITextRenderer_Component"; };
template<>
inline static wstring CComponent::GetTypeName<CVIBuffer_UIRect>() { return L"VIBufferUIRect_Component"; };
template<>
inline static wstring CComponent::GetTypeName<CUIAnimator>() { return L"UIAnimator_Component"; };
template<>
inline static wstring CComponent::GetTypeName<CUIVideoRenderer>() { return L"UIVideoRenderer_Component"; };

/* �׽�Ʈ�� */
template<>
inline static wstring CComponent::GetTypeName<CInstancing_BaseModel>() { return L"InstancingModel_Component"; };

template<>
inline static wstring CComponent::GetTypeName<CDynamicShaderParam>() { return L"DSP_Component"; };

NS_END
