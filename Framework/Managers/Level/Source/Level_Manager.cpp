#include "Level_Manager.h"

#include "Level.h"
#include "GameInstance.h"
#include "UIController.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Change_Level()
{
	//현재 레벨의 삭제
	if (!m_curLVLTag.empty()) m_pGameInstance->Clear(m_curLVLTag);
	if (0 != Safe_Release(m_pCurrentLevel)) return E_FAIL;
	m_pCurrentLevel = nullptr;

	//레벨의 전환
	m_pCurrentLevel = m_pNextLevel;
	m_curLVLTag		= m_TransitDesc.nextLevelTag;

	return S_OK;
}

HRESULT CLevel_Manager::Load_Level(tagLevelTransitionDesc _desc)
{
	m_TransitDesc = _desc;
	m_pNextLevel = static_cast<CLevel*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::LEVEL, _desc.nextLevelTag, reinterpret_cast<void*>(&m_TransitDesc)));
	m_pNextLevel->AddInitObject();

	return S_OK;
}

HRESULT CLevel_Manager::Create_Level(const wstring& sLVLPrototypeTag, wstring sLVLName)
{
	LVLTRANSIT_DESC _desc;
	if (!m_pGameInstance->IsClassPrototype(PROTOTYPE::LEVEL, sLVLPrototypeTag)) { 
		MSG_BOX("Fail to Create Level : Non-Class Prototype");
		return E_FAIL; 
	}
	CLevel* _pLevel = static_cast<CLevel*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::LEVEL, sLVLPrototypeTag, reinterpret_cast<void*>(&_desc)));
	if (!_pLevel) {
		MSG_BOX("Fail to Create Level : Fail to clone prototype");
		return E_FAIL;
	}
	_pLevel->m_sLVLName = sLVLName;
	CHKFAIL(_pLevel->SavePrototype());

	CHKFAIL(m_pGameInstance->Add_Prototype(PROTOTYPE::LEVEL, sLVLName, _pLevel))

	return S_OK;
}

HRESULT CLevel_Manager::Load_Level_InEditor(const wstring& sLVLName, wstring& sLVLPrototypeName)
{
	if (!m_pCurrentLevel) return E_FAIL;
	m_pCurrentLevel->AddInitObject();
	CHKFAIL(m_pCurrentLevel->LoadLevelObjects_InEditor(sLVLName, sLVLPrototypeName))

	return S_OK;
}

HRESULT CLevel_Manager::Save_Level_InEditor(const wstring& sLVLName, const wstring& sLVLPrototypeName)
{
	if (!m_pCurrentLevel) return E_FAIL;
	CHKFAIL(m_pCurrentLevel->SaveLevelObjects_InEditor(sLVLName, sLVLPrototypeName))
	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	return m_pCurrentLevel->Render();	
}

_wstring CLevel_Manager::GetCurrentLevelTag()
{
	if (!m_pCurrentLevel) return _wstring();
	return m_pCurrentLevel->GetLevelName();
}

_wstring CLevel_Manager::GetCurrentDetailLevelTag()
{
	if (!m_pCurrentLevel) return _wstring();
	return m_pCurrentLevel->GetDetailName();
}

tagLevelTransitionDesc CLevel_Manager::GetLevelDesc()
{
	return m_TransitDesc;
}

void CLevel_Manager::Clear(const _wstring& strLVLTag)
{

}


CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCurrentLevel);
}

CUIController* CLevel_Manager::GetCurrentLevelUC()
{
	return m_pCurrentLevel->GetUIController();
}

CLevel* CLevel_Manager::GetCurrentLevelPtr()
{
	return m_pCurrentLevel;
}
