#include "Object_Manager.h"
#include "Layer.h"

#include "GameInstance.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

vector<CGameObject*> CObject_Manager::Get_Terrains(const _wstring& strLVLTag, const _wstring& strLayerTag)
{
	auto _pLayer = Find_Layer(strLVLTag, strLayerTag);
	if (!_pLayer) return vector<CGameObject*>();
	return _pLayer->Get_Terrains();
}

CGameObject* CObject_Manager::Get_Object(const _wstring& strLVLTag, const _wstring& strLayerTag, const _wstring& strObjectName)
{
	auto _pLayer = Find_Layer(strLVLTag, strLayerTag);
	if (!_pLayer) return nullptr;
	auto _pObject = _pLayer->Get_Object(strObjectName);
	return _pObject;
}

list<CGameObject*> CObject_Manager::Get_Objects(const _wstring& strLVLTag, const _wstring& strLayerTag)
{
	list<CGameObject*> _Objlst;
	auto _pLayer = Find_Layer(strLVLTag, strLayerTag);
	if (!_pLayer) return _Objlst;
	_Objlst = _pLayer->Get_Objects();
	return _Objlst;
}

HRESULT CObject_Manager::Load_Asset(const _wstring& strLVLTag)
{
	HRESULT hr = S_OK;
	auto _itLevelLayer = m_pLayers.find(strLVLTag);
	if (_itLevelLayer == m_pLayers.end()) return E_FAIL;
	for (auto [_, _pLayer] : _itLevelLayer->second)
		if (FAILED(_pLayer->Load_Asset())) hr = E_FAIL;
	return hr;
}

HRESULT CObject_Manager::Initialize()
{
	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_ToLayer(const _wstring& strPrototypeTag, const _wstring& strLVLTag, const _wstring& strLayerTag, CGameObject** ppOut, void* pArg)
{
	auto _itLevelLayer = m_pLayers.find(strLVLTag);
	if (_itLevelLayer == m_pLayers.end()) {
		wstring _sMSG = L"Fail to Create Layer : ";
		_sMSG += strLVLTag;         
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, strPrototypeTag, pArg));
	if (nullptr == pGameObject) return E_FAIL;


	CLayer* pLayer = Find_Layer(strLVLTag, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create(strLayerTag);
		m_pLayers.find(strLVLTag)->second.emplace(strLayerTag, pLayer);
	}

	pLayer->Add_GameObject(pGameObject);
	pGameObject->ChangeLevel(strLVLTag);
	if(ppOut) *ppOut = pGameObject;

	return S_OK;
}

void CObject_Manager::Priority_Update(const _wstring& strLVLTag, _float fTimeDelta)
{
	auto _itLevelLayer = m_pLayers.find(strLVLTag);
	if (_itLevelLayer == m_pLayers.end()) return;

	for (auto [_, _pObject] : _itLevelLayer->second) _pObject->Priority_Update(fTimeDelta);
}

void CObject_Manager::Update(const _wstring& strLVLTag, _float fTimeDelta)
{
	auto _itLevelLayer = m_pLayers.find(strLVLTag);
	if (_itLevelLayer == m_pLayers.end()) return;

	for (auto [_, _pObject] : _itLevelLayer->second) _pObject->Update(fTimeDelta);
}

void CObject_Manager::Late_Update(const _wstring& strLVLTag, _float fTimeDelta)
{
	auto _itLevelLayer = m_pLayers.find(strLVLTag);
	if (_itLevelLayer == m_pLayers.end()) return;

	for (auto [_, _pObject] : _itLevelLayer->second) _pObject->Late_Update(fTimeDelta);
}

CLayer* CObject_Manager::Find_Layer(const _wstring& strLVLTag, const _wstring& strLayerTag)
{
	auto _itLevelLayer = m_pLayers.find(strLVLTag);
	if (_itLevelLayer == m_pLayers.end()) return nullptr;
	
	auto _itLayer = _itLevelLayer->second.find(strLayerTag);
	if(_itLayer == _itLevelLayer->second.end()) return nullptr;

	return _itLayer->second;
}

void CObject_Manager::Clear(const _wstring& strLVLTag)
{
	auto it = m_pLayers.find(strLVLTag);
	if (it == m_pLayers.end()) return;
	
	for (auto& [_, _pLayer] : it->second) {
		Safe_Release(_pLayer);
	}

	(it->second).clear();
}

void CObject_Manager::AddLevel(const _wstring& strLVLTag)
{
	auto it = m_pLayers.find(strLVLTag);
	if (it != m_pLayers.end()) return;

	m_pLayers.emplace(strLVLTag, map<_wstring, class CLayer*>());
}

HRESULT CObject_Manager::SaveObjects(fstream& _fs, const _wstring& strLVLTag)
{
	auto it = m_pLayers.find(strLVLTag);
	if (it == m_pLayers.end()) {
		wstring _sMSG = L"Fail to Save Objects : invalid LVL tag ( " + strLVLTag + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	_fs.write((char*)&LAYERGROUPHEADER, sizeof(LAYERGROUPHEADER));
	for (auto [_sLayerName, _] : it->second) {
		_fs.write((char*)_sLayerName.c_str(), sizeof(wchar_t) * MAXLAYERNAMESIZE);
	}
	_fs.write((char*)&LAYERGROUPFOOTER, sizeof(LAYERGROUPFOOTER));


	_fs.write((char*)&OBJGROUPHEADER, sizeof(OBJGROUPHEADER));
	for (auto [_, _pLayer] : it->second) _pLayer->SaveLayer(_fs);
	_fs.write((char*)&OBJGROUPFOOTER, sizeof(OBJGROUPFOOTER));

	return S_OK;
}

HRESULT CObject_Manager::LoadObjects(fstream& _fs, const _wstring& strLVLTag)
{

	auto _sentinel = LAYERGROUPHEADER;
	auto it = m_pLayers.find(strLVLTag);
	if (it == m_pLayers.end()) {
		wstring _sMSG = L"Fail to Load Objects : already exist LVL tag ( " + strLVLTag + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}
	m_pLayers.emplace(strLVLTag, map<_wstring, class CLayer*>());

	_fs.read((char*)&_sentinel, sizeof(LAYERGROUPHEADER));
	if (_sentinel != LAYERGROUPHEADER) { MSG_BOX("Fail to Load LayerGroup : Invalid Header!"); return E_FAIL; }
	wchar_t _sLayerName[MAXLAYERNAMESIZE];
	while (!CheckHeader(_fs, LAYERGROUPFOOTER)) {
		_fs.read((char*)_sLayerName, sizeof(wchar_t) * MAXLAYERNAMESIZE);
		auto& _LVLLayer = (m_pLayers.find(strLVLTag)->second);
		if (_LVLLayer.find(_sLayerName) == _LVLLayer.end()) _LVLLayer.emplace(_sLayerName, CLayer::Create(_sLayerName));
	}
	_fs.read((char*)&_sentinel, sizeof(LAYERGROUPFOOTER));
	if (_sentinel != LAYERGROUPFOOTER) { MSG_BOX("Fail to Load LayerGroup : Invalid Footer!"); return E_FAIL; }

	_fs.read((char*)&_sentinel, sizeof(OBJGROUPHEADER));
	if (_sentinel != OBJGROUPHEADER) { MSG_BOX("Fail to Load ObjGroup : Invalid Header!"); return E_FAIL; }
	while (!CheckHeader(_fs, OBJGROUPFOOTER)) {
		CLayer::LoadLayer(_fs, strLVLTag);
	}
	_fs.read((char*)&_sentinel,	sizeof(OBJGROUPFOOTER));
	if (_sentinel != OBJGROUPFOOTER) { MSG_BOX("Fail to Load ObjGroup : Invalid Footer!"); return E_FAIL; }

	return S_OK;
}

HRESULT CObject_Manager::ChangeLayerTimeScale(const _wstring& sLVLTag, const _wstring& sLayerTag, float fTimeScale)
{
	auto itLVLLayer = m_pLayers.find(sLVLTag);
	if (itLVLLayer == m_pLayers.end()) {
		MSG_BOX("Fail to Find Layer : Invalid Level Tag");
		return E_FAIL;
	}
	auto it = (itLVLLayer->second).find(sLayerTag);
	if (it == (itLVLLayer->second).end()) {
		MSG_BOX("Fail to Find Layer : Invalid Layer Tag");
		return E_FAIL;
	}
	it->second->ChangeLayerTimeScale(fTimeScale);

	return S_OK;
}

vector<_wstring> CObject_Manager::GetLayerNames(const _wstring& strLVLTag)
{
	vector<wstring> _vecNames;

	auto it = m_pLayers.find(strLVLTag);
	if (it != m_pLayers.end()) {
		for (auto [_sName, _] : m_pLayers[strLVLTag]) _vecNames.push_back(_sName);
	}
	return _vecNames;
}

HRESULT CObject_Manager::AddLayer(const _wstring& sLVLTag, const _wstring& sLayerTag)
{
	auto itLVLLayer = m_pLayers.find(sLVLTag);
	if (itLVLLayer == m_pLayers.end()) {
		MSG_BOX("Fail to Add Layer : Invalid Level Tag");
		return E_FAIL;
	}
	auto it = (itLVLLayer->second).find(sLayerTag);
	if (it != (itLVLLayer->second).end()) {
		MSG_BOX("Fail to Add Layer : already used Layer Tag");
		return E_FAIL;
	}
	(itLVLLayer->second).emplace(sLayerTag, CLayer::Create(sLayerTag));

	return S_OK;
}

HRESULT CObject_Manager::DeleteLayer(const _wstring& sLVLTag, const _wstring& sLayerTag)
{
	auto itLVLLayer = m_pLayers.find(sLVLTag);
	if (itLVLLayer == m_pLayers.end()) {
		MSG_BOX("Fail to Add Layer : Invalid Level Tag");
		return E_FAIL;
	}
	auto it = (itLVLLayer->second).find(sLayerTag);
	if (it == (itLVLLayer->second).end()) {
		MSG_BOX("Fail to Add Layer : invalid Layer Tag");
		return E_FAIL;
	}
	Safe_Release((itLVLLayer->second).find(sLayerTag)->second);
	(itLVLLayer->second).erase(sLayerTag);

	return S_OK;
}

HRESULT CObject_Manager::RenameLayer(const _wstring& sLVLTag, const _wstring& sOldLayerTag, const _wstring& sNewLayerTag)
{
	auto itLVLLayer = m_pLayers.find(sLVLTag);
	if (itLVLLayer == m_pLayers.end()) {
		MSG_BOX("Fail to Rename Layer : Invalid Level Tag");
		return E_FAIL;
	}
	auto itOldLayer = (itLVLLayer->second).find(sOldLayerTag);
	if (itOldLayer == (itLVLLayer->second).end()) {
		MSG_BOX("Fail to Rename Layer : invalid OldLayer Tag");
		return E_FAIL;
	}
	auto itNewLayer = (itLVLLayer->second).find(sNewLayerTag);
	if (itNewLayer != (itLVLLayer->second).end()) {
		MSG_BOX("Fail to Rename Layer : already used Layer Tag");
		return E_FAIL;
	}
	(itOldLayer->second)->RenameLayer(sNewLayerTag);
	(itLVLLayer->second).emplace(sNewLayerTag, (itOldLayer->second));
	(itLVLLayer->second).erase(sOldLayerTag);

	return S_OK;
}

_bool CObject_Manager::CheckNameDuplication(const _wstring& sLVLTag, const wstring& sName)
{
	auto itLVLLayer = m_pLayers.find(sLVLTag);
	if (itLVLLayer == m_pLayers.end()) {
		MSG_BOX("Fail to Check Name Duplication : Invalid Level Tag");
		return false;
	}

	for (auto pLayer : itLVLLayer->second)
		if (pLayer.second->CheckNameDuplication(sName)) return true;

	return false;


}

list<CPicking::PICKING_DESC> CObject_Manager::RayCastObjects(RAY InWorldRay, const _wstring& sLVLTag, vector<wstring> vecLayers)
{
	list<CPicking::PICKING_DESC> listPickingDescs;

	auto LVLIt = m_pLayers.find(sLVLTag);
	if (LVLIt == m_pLayers.end()) {
		MSG_BOX("Unexisting Lvl Tag");
		return listPickingDescs;
	}
	
	if (vecLayers.empty()) {
		for (auto [_sLayerTag, _] : LVLIt->second) {
			vecLayers.push_back(_sLayerTag);
		}
	}

	for (auto sLayerTag : vecLayers)
	{
		auto LayerIt = LVLIt->second.find(sLayerTag);
		if (LayerIt != (LVLIt->second).end()) {
			listPickingDescs.merge(LayerIt->second->RayCastObjects(InWorldRay));
		}
		else {
			MSG_BOX("Unexisting Layer Tag");
		}
	}

	

	return listPickingDescs;
}

HRESULT CObject_Manager::ChangeObjectLayer(const _wstring& sLVLTag, const _wstring& sNewLayerTag, CGameObject* pObject)
{
	auto itLVLLayer = m_pLayers.find(sLVLTag);
	if (itLVLLayer == m_pLayers.end()) {
		MSG_BOX("Fail to Change Layer : Invalid Level Tag");
		return E_FAIL;
	}
	auto itOldLayer = (itLVLLayer->second).find(pObject->GetLayerName());
	if (itOldLayer == (itLVLLayer->second).end()) {
		MSG_BOX("Fail to Change Layer : invalid OldLayer Tag");
		return E_FAIL;
	}
	auto itNewLayer = (itLVLLayer->second).find(sNewLayerTag);
	if (itNewLayer == (itLVLLayer->second).end()) {
		MSG_BOX("Fail to Change Layer : already NewLayer Tag");
		return E_FAIL;
	}

	(itNewLayer->second)->Add_GameObject(pObject);
	Safe_AddRef(pObject);
	(itOldLayer->second)->Remove_GameObject(pObject);

	return S_OK;
}


HRESULT CObject_Manager::CopyObject(const _wstring& sLVLTag, CGameObject* pObject)
{
	auto itLVLLayer = m_pLayers.find(sLVLTag);
	if (itLVLLayer == m_pLayers.end()) {
		MSG_BOX("Fail to Copy Object : Invalid Level Tag");
		return E_FAIL;
	}
	auto itLayer = (itLVLLayer->second).find(pObject->GetLayerName());
	if (itLayer == (itLVLLayer->second).end()) {
		MSG_BOX("Fail to Copy Object : invalid Layer Tag");
		return E_FAIL;
	}
	auto pCopyObject = (itLayer->second)->Copy_GameObject(pObject);
	pCopyObject->SetParent(pObject->GetParent());
	for (CGameObject* pChild : pObject->GetChildrens()) {
		auto itChildLayer = (itLVLLayer->second).find(pObject->GetLayerName());
		if (itChildLayer == (itLVLLayer->second).end()) {
			MSG_BOX("Fail to Copy Object : invalid Layer Tag");
			return E_FAIL;
		}
		auto pCopyCjildObject = (itChildLayer->second)->Copy_GameObject(pChild);
		pCopyCjildObject->SetParent(pCopyObject);
	}
	return S_OK;
}

CGameObject* CObject_Manager::Render_ImGui_Hierarchy(CGameObject* pLastSelected)
{
	CGameObject* pSelected = nullptr;

	
	wstring _curLVLTag = m_pGameInstance->GetCurrentLevelTag();
	auto LVLit = m_pLayers.find(_curLVLTag);
	if (LVLit == m_pLayers.end()) {
		wstring _sMSG = L"Fail to render hierarchy : Fail to find LVLTag ( ";
		_sMSG += _curLVLTag;
		_sMSG += L" )";
		MSG_BOXL(_sMSG.c_str());
		return nullptr;
	}
	auto pCurLayers = LVLit->second;

	int _id = 0;
	for (auto [_, pLayer] : pCurLayers)
	{
		ImGui::PushID(_id++);
		if (auto pNewSelected = pLayer->Render_ImGui_Hierarchy(pLastSelected)) pSelected = pNewSelected;
		ImGui::PopID();
	}
	return pSelected;
}

CObject_Manager* CObject_Manager::Create()
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (auto& [_, _pLevelLayer] : m_pLayers) {
		for (auto& [_, _pLayer] : _pLevelLayer) Safe_Release(_pLayer);
		_pLevelLayer.clear();
	}
	m_pLayers.clear();
	Safe_Release(m_pGameInstance);
}
