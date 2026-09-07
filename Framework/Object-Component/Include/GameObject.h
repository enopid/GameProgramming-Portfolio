#pragma once

#include "Base.h"
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
private:
	constexpr static float	COLWIDTH = 100.f;

protected:
	const static _uint MAXDESCSIZE		= 50000;
	const static _uint MAXOBJNAMESIZE	= 60;
private:
	const static _uint OBJECTHEADER		= 0x900DF00D;
	const static _uint OBJECTFOOTER		= 0xBAADF00D;

	const static _uint OBJGROUPHEADER	= 0x900DFACE;
	const static _uint OBJGROUPFOOTER	= 0xBAADFACE;

	const static _uint COMGROUPHEADER	= 0x900DFEE1;
	const static _uint COMGROUPFOOTER	= 0xBAADFEE1;
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Load_Asset();
	virtual HRESULT Late_Initialize();

	virtual void	Priority_Update(_float fTimeDelta);
	virtual void	Update(_float fTimeDelta);
	virtual void	Late_Update(_float fTimeDelta);

	virtual void			Free() override;
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Render_Properties() {};
	virtual HRESULT Add_InitComponents() PURE;
public:
	virtual HRESULT Save(void* _pDesc, _uint& _iSize) const	PURE;
	virtual HRESULT Load(void* _pDesc)						PURE;

public:
	WPTransform			Get_MainTransform();
	WPComponent			Get_Component	(const _wstring& sComponentTag);

	template<typename T, enable_if_t<is_base_of_v<CComponent, T>>* = nullptr>
	vector<WPComponent> Get_Components() {
		wstring _sTypeName = CComponent::GetTypeName<T>();
		auto it = m_vecComponents.find(_sTypeName);
		vector<WPComponent> vecComponents;
		if (it != m_vecComponents.end()) {
			for (SPComponent& spComponent : (it->second)) vecComponents.push_back(spComponent);
		}
		return vecComponents;
	}
	template<typename T, enable_if_t<is_base_of_v<CComponent, T>>* = nullptr>
	WPComponent Get_Component() {
		wstring _sTypeName = CComponent::GetTypeName<T>();
		auto it = m_vecComponents.find(_sTypeName);
		if (it== m_vecComponents.end() || (it->second).empty())
			return WPComponent();
		else
			return (it->second)[0];
	}

	template<typename T, enable_if_t<is_base_of_v<CComponent, T>>* = nullptr>
	HRESULT Add_Component(const _wstring& sComponentTag, void* pArg = nullptr) {
		wstring _sTypeName = CComponent::GetTypeName<T>();
		if (FAILED(Add_Component(_sTypeName, sComponentTag, pArg))) {
			wstring _sMSG = L"Fail to add Component : " + _sTypeName;
			MSG_BOXL(_sMSG.c_str());
			return E_FAIL;
		}
		return S_OK;
	}

	HRESULT Add_Component(const _wstring& sPrototypeTag, const _wstring& sComponentTag, void* pArg = nullptr);

	CGameObject*		Get_ChildObject(const _wstring& sNameTag);
	list<CGameObject*>	GetChildrens() { return m_listChildrenObjects; };
	CGameObject*		GetParent() { return m_pParentObject; };
	HRESULT				SetParent(CGameObject* _pParent);
protected:
	HRESULT Add_Component(SPComponent pComponent,			void* pArg = nullptr);
public:

	_bool	IsDestroyFlag() const { return m_bDestroyFlag; }
	void	SetDestroyFlag();

	template<typename T, enable_if_t<is_base_of_v<CGameObject, T>>* = nullptr>
	static T* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr) {
		T* pInstance = new T(pDevice, pContext);

		if (FAILED(pInstance->Initialize_Prototype(pArg)))
		{
			MSG_BOX("Failed to Create : GameObject");
			Safe_Release(pInstance);
		}

		return pInstance;
	}

	template<typename T, enable_if_t<is_base_of_v<CGameObject, T>>* = nullptr>
	CGameObject* CloneBase(void* pDesc) {
		T* pInstance = new T(*static_cast<T*>(this));
		if (FAILED(pInstance->Initialize(pDesc)))
		{
			wstring _msg = L"Failed to Clone : CGameObject";
			MSG_BOXL(_msg.c_str());
			Safe_Release(pInstance);
		}
		if (pDesc) pInstance->Load(pDesc);

		return pInstance;
	}
public:
	//Editor
	_wstring GetName()		{ return m_sOBJName; }
	_wstring GetLayerName() { return m_sOBJLayertag; }

	HRESULT ChangeName	(const _wstring& sNewNameTag);
	HRESULT ChangeLayer (const _wstring& sNewLayerTag);
	HRESULT ChangeLevel	(const _wstring& sNewLevelTag);
	HRESULT SetTypeName (const _wstring& sNewTypeTag);

	void	SetLevelObject()	{ m_bIslevelObj = true; }
	_bool	IsLevelObject()		{ return m_bIslevelObj; }

	virtual void  SetActive(_bool bIsActive, _bool IsRecursive = false);
	void SetSectionActive(_bool bIsActive, _bool IsRecursive = false);
	_bool IsActive()		{ return m_bIsActive && m_bIsSectionActive; }
	_bool IsSectionActive() { return m_bIsSectionActive; }
	_bool IsRootObject()	{ return m_pParentObject == nullptr; }

	CGameObject*	Render_ImGui_Hierarchy(class CGameObject* pLastSelected);
	void			Render_Inspector();
	void			Render_Info();
	void			Render_Components();
public:
	virtual HRESULT			Reset();
public:
	HRESULT			SavePrototype();
	static HRESULT	LoadPrototype(const _wstring& sPrototypeTag, const _wstring& sLVLTag, CGameObject** _ppObject = nullptr);
public:
	static HRESULT	SaveObject(fstream& _fs, CGameObject* _pObject);
	static HRESULT	LoadObject(fstream& _fs, CGameObject** ppOut, wstring _sOBJLeveltag);

protected:
	ID3D11Device* m_pDevice					= nullptr;
	ID3D11DeviceContext* m_pContext			= nullptr;
	class CGameInstance* m_pGameInstance	= nullptr;

protected:
	list<CGameObject*>				m_listChildrenObjects;
	CGameObject* m_pParentObject = { nullptr };

	map<_wstring, vector<SPComponent>>	m_vecComponents;
	list<WPComponent>					m_listUpdateComponents;

	wstring								m_sOBJTypeName	= { L"GameObject" };
	wstring								m_sOBJName		= { L"Empty Object" };
	wstring								m_sOBJLayertag	= { L"Empty Layer" };
	wstring								m_sOBJLeveltag	= { L"Empty Level" };
	

	_bool								m_bIsLoadAsset		= { false };
	_bool								m_bIsPrefab			= { false };
	_bool								m_bIsLateInitialize = { false };
	_bool								m_bIslevelObj		= { false };
	_bool								m_bIsActive			= { true };
	_bool								m_bIsSectionActive	= { true };

	_bool								m_bDestroyFlag		= { false };
	WPTransform							m_wpMainTransformCom = {};
};

NS_END