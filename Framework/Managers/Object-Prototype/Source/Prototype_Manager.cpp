#include "Prototype_Manager.h"
#include "GameObject.h"
#include "Component.h"
#include "Level.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize()
{

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(PROTOTYPE ePrototype, const _wstring& strPrototypeTag, CBase* pPrototype, _bool _bIsClassPrototype)
{
	if (nullptr != Find_Prototype(ePrototype, strPrototypeTag)) {
		wstring _sMSG = L"Already Used Prototype Tag : ";
		_sMSG += strPrototypeTag;
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	switch (ePrototype)
	{
	case Engine::PROTOTYPE::GAMEOBJECT:
		static_cast<CGameObject*>	(pPrototype)->SetTypeName(strPrototypeTag);
		break;
	case Engine::PROTOTYPE::COMPONENT:
		static_cast<CComponent*>	(pPrototype)->SetTypeName(strPrototypeTag);
		break;
	case Engine::PROTOTYPE::LEVEL:
		static_cast<CLevel*>		(pPrototype);
		break;
	default:
		return E_FAIL;
		break;
	}
		
	m_Prototypes[ENUM_TO_UINT(ePrototype)].emplace(strPrototypeTag, pPrototype);
	if(_bIsClassPrototype) m_BasePrototypeNames[ENUM_TO_UINT(ePrototype)].insert(strPrototypeTag);
	return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE ePrototype, const _wstring& strPrototypeTag, void* pArg)
{
	CBase*	pPrototype = Find_Prototype(ePrototype, strPrototypeTag);
	if (nullptr == pPrototype) return nullptr;

	CBase* pCloneObject = { nullptr };

	if		(PROTOTYPE::GAMEOBJECT	== ePrototype)
		pCloneObject = dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
	else if (PROTOTYPE::LEVEL		== ePrototype)
		pCloneObject = dynamic_cast<CLevel*		>(pPrototype)->Clone(pArg);

	return pCloneObject;
}

SPComponent CPrototype_Manager::Clone_ComponentPrototype(const _wstring& strPrototypeTag, void* pArg)
{

	CComponent* pPrototype = static_cast<CComponent*>(Find_Prototype(PROTOTYPE::COMPONENT, strPrototypeTag));
	if (nullptr == pPrototype) return nullptr;

	return (pPrototype)->Clone(pArg);
}

HRESULT CPrototype_Manager::Remove_Prototype(PROTOTYPE ePrototype, const _wstring& strPrototypeTag)
{
	auto _pBase = Find_Prototype(ePrototype, strPrototypeTag);
	if (!_pBase) {
		wstring _sMSG = L"Prototype Tag to remove doesn't exist : ";
		_sMSG += strPrototypeTag;
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	m_Prototypes[ENUM_TO_UINT(ePrototype)].erase(strPrototypeTag);
	Safe_Release(_pBase);

	return S_OK;
}

void CPrototype_Manager::Clear()
{
	for (size_t i = 0; i < ENUM_TO_UINT(PROTOTYPE::END); i++)
	{
		for (auto& Pair : m_Prototypes[i]) Safe_Release(Pair.second);
		m_Prototypes[i].clear();
	}
}

void CPrototype_Manager::Clear(const _wstring& strLVLTag)
{
}

CBase* CPrototype_Manager::Find_Prototype(PROTOTYPE ePrototype, const _wstring& strPrototypeTag)
{
	auto	iter = m_Prototypes[ENUM_TO_UINT(ePrototype)].find(strPrototypeTag);
	if (iter == m_Prototypes[ENUM_TO_UINT(ePrototype)].end()) return nullptr;
	return iter->second;
}

vector<_wstring> CPrototype_Manager::GetPrototypeNames(PROTOTYPE ePrototype, _bool _bOnlyIsClassPrototype)
{
	vector<wstring> _vecNames;
	for (auto[_sName,_] : m_Prototypes[ENUM_TO_UINT(ePrototype)])
	{
		if (!_bOnlyIsClassPrototype || IsClassPrototype(ePrototype, _sName)) {
			_vecNames.push_back(_sName);
		}
		
	}
	return _vecNames;
}

CPrototype_Manager* CPrototype_Manager::Create()
{
	CPrototype_Manager* pInstance = new CPrototype_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPrototype_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPrototype_Manager::Free()
{
	__super::Free();

	Clear();
}
