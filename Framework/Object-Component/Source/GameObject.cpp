#include "GameObject.h"
#include "GameInstance.h"
#include "Transform.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :   m_pDevice       (pDevice),
        m_pContext      (pContext),
        m_pGameInstance (CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& Prototype)
    :   m_pDevice       (Prototype.m_pDevice),
        m_pContext      (Prototype.m_pContext),
        m_pGameInstance (CGameInstance::GetInstance()),
        m_sOBJName      (Prototype.m_sOBJName),
        m_sOBJTypeName  (Prototype.m_sOBJTypeName),
        m_bIsPrefab     (Prototype.m_bIsPrefab)
{
    for (auto& [_sTypeName, _vecComps] : Prototype.m_vecComponents) {
        auto it = m_vecComponents.find(_sTypeName);
        if (it == m_vecComponents.end()) {
            m_vecComponents.emplace(_sTypeName, vector<SPComponent>());
            it = m_vecComponents.find(_sTypeName);
        }

        for (auto _pComponent : _vecComps) {
            Add_Component(_pComponent->Clone(nullptr));
        }
    }

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

WPTransform	CGameObject::Get_MainTransform() {
    if (!m_wpMainTransformCom.lock()) {
        m_wpMainTransformCom = dynamic_pointer_cast<CTransform>(Get_Component(L"Com_Transform_Main").lock());
        m_wpMainTransformCom.lock()->SetInitialAdd();
    }
    return m_wpMainTransformCom;
}

WPComponent CGameObject::Get_Component(const _wstring& strComponentTag)
{
    for (auto& [_, _vecComponents] : m_vecComponents) 
        for (auto _pComponent : _vecComponents)
        if (_pComponent->GetName() == strComponentTag) 
            return _pComponent;
    
    return WPComponent();
}

HRESULT CGameObject::Initialize_Prototype(void* pArg) 
{
    return S_OK;
}

HRESULT CGameObject::Initialize(void* pDesc)
{
    if (!pDesc) {
    }
    return S_OK;
}

HRESULT CGameObject::Late_Initialize()
{
    m_wpMainTransformCom = dynamic_pointer_cast<CTransform>(Get_Component(L"Com_Transform_Main").lock());
    m_wpMainTransformCom.lock()->SetInitialAdd();

    return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
    list<SPComponent> _lstLateInitializeComps;
    for (auto& [_, vecComponents] : m_vecComponents) for (auto& pComponent : vecComponents) {
        if (!pComponent->IsLateInitialized()) _lstLateInitializeComps.push_back(pComponent);
    }
    _lstLateInitializeComps.sort([=](SPComponent& _spComp0, SPComponent& _spComp1) {
        return _spComp0->GetInitializePriority() < _spComp0->GetInitializePriority();
    });
    if (!m_bIsLoadAsset) Load_Asset();
    for (auto& pComponent : _lstLateInitializeComps) pComponent->CheckLateInitialize();
    if (!m_bIsLateInitialize) {
        m_bIsLateInitialize = true;
        Late_Initialize();
    }

    if (!m_bIsActive || !m_bIsSectionActive) return;
    for (auto it = m_listUpdateComponents.begin(); it != m_listUpdateComponents.end(); ) {
        if (auto sp = (*it).lock()) {
            sp->Priority_Update(fTimeDelta);
            it++;
        }
        else {
            it = m_listUpdateComponents.erase(it);
        }
    }
}

void CGameObject::Update(_float fTimeDelta)
{
    if (!m_bIsActive || !m_bIsSectionActive) return;
    for (auto it = m_listUpdateComponents.begin(); it != m_listUpdateComponents.end(); ) {
        if (auto sp = (*it).lock()) {
            sp->Update(fTimeDelta);
            it++;
        }
        else {
            it = m_listUpdateComponents.erase(it);
        }
    }
}

void CGameObject::Late_Update(_float fTimeDelta)
{
    if (!m_bIsActive || !m_bIsSectionActive) return;
    for (auto it = m_listUpdateComponents.begin(); it != m_listUpdateComponents.end(); ) {
        if (auto sp = (*it).lock()) {
            sp->Late_Update(fTimeDelta);
            it++;
        }
        else {
            it = m_listUpdateComponents.erase(it);
        }
    }
}
void	CGameObject::SetDestroyFlag() {
    SetParent(nullptr);
    m_bDestroyFlag = true;
    auto _listChildCopy = m_listChildrenObjects;
    for (auto pChild : _listChildCopy) pChild->SetDestroyFlag();
}

HRESULT CGameObject::SetParent(CGameObject* _pParent)
{
    if (m_pParentObject && m_pParentObject == _pParent) {
        MSG_BOX("fail to set parent : already parent");
        return E_FAIL;
    }

    bool _bIsChild = false;
    stack<CGameObject*> st;
    st.push(this);
    while (st.size() && !_bIsChild) {
        auto _curNode = st.top();
        st.pop();
        for (auto _childNode : _curNode->m_listChildrenObjects) {
            if (_childNode == _pParent) {
                _bIsChild = true;
                break;
            }
            st.push(_childNode);
        }
    }
    if (_bIsChild) {
        MSG_BOX("fail to set parent : parent is descendant");
        return E_FAIL;
    }

    if (m_pParentObject) 
        m_pParentObject->m_listChildrenObjects.remove(this);
    

    m_pParentObject = _pParent;
    if (_pParent) {
        _pParent->m_listChildrenObjects.push_back(this);
    }

    if (m_sOBJTypeName.substr(0, 9) != L"Object_UI")
        Get_MainTransform().lock()->Set_Parent(m_pParentObject, true);

    return S_OK;
}

HRESULT CGameObject::SaveObject(fstream& _fs, CGameObject* _pObject)
{
    if (!_pObject) {
        MSG_BOX("Fail to save Object : try to Save Null Object");
        return E_FAIL;
    }
    _fs.write((char*)&OBJECTHEADER, sizeof(OBJECTHEADER));

    //Declare Obj Info
    _uint   _iSize;
    _char   _pDesc[MAXDESCSIZE];
    _bool   _bIsPrefab = _pObject->m_bIsPrefab;
    wchar_t _sOBJName           [MAXOBJNAMESIZE];
    wchar_t _sOBJPrototypeName  [MAXOBJNAMESIZE];
    wchar_t _sOBJLayertag       [MAXOBJNAMESIZE];
    
    //Load OBJ Info
    lstrcpynW(_sOBJName,            _pObject->m_sOBJName.c_str(),       MAXOBJNAMESIZE);
    lstrcpynW(_sOBJPrototypeName,   _pObject->m_sOBJTypeName.c_str(),   MAXOBJNAMESIZE);
    lstrcpynW(_sOBJLayertag,        _pObject->m_sOBJLayertag.c_str(),   MAXOBJNAMESIZE);
    CHKFAIL(_pObject->Save(_pDesc, _iSize))
    if (MAXDESCSIZE < _iSize) {
        wstring _sText = L"desc size is too big : " + to_wstring(_iSize) + L"Byte";
        MSG_BOXL(_sText.c_str());
        return E_FAIL;
    }

    _fs.write((char*)_sOBJName,             sizeof(wchar_t) * MAXOBJNAMESIZE);
    _fs.write((char*)_sOBJPrototypeName,    sizeof(wchar_t) * MAXOBJNAMESIZE);
    _fs.write((char*)_sOBJLayertag,         sizeof(wchar_t) * MAXOBJNAMESIZE);
    _fs.write((char*)&_iSize,               sizeof(_iSize));
    _fs.write(_pDesc,                       _iSize);
    _fs.write((char*)&_bIsPrefab,           sizeof(_bool)                   );
    
    //Save Component info if Non-Prefab
    _fs.write((char*)&COMGROUPHEADER, sizeof(COMGROUPHEADER));
    if (!_bIsPrefab) {
        for (auto [_, _componentVec] : _pObject->m_vecComponents) for (auto _pComp : _componentVec)
            CHKFAIL(CComponent::SaveComponent(_fs, _pComp.get()))
    }
    _fs.write((char*)&COMGROUPFOOTER, sizeof(COMGROUPFOOTER));

    //Save Children
    _fs.write((char*)&OBJGROUPHEADER,   sizeof(OBJGROUPHEADER));
    for (CGameObject* _pChildObject : _pObject->m_listChildrenObjects) { 
        if (_pChildObject->IsLevelObject()) continue;
        CHKFAIL(SaveObject(_fs, _pChildObject)) 
    }
    _fs.write((char*)&OBJGROUPFOOTER,   sizeof(OBJGROUPFOOTER));

    _fs.write((char*)&OBJECTFOOTER, sizeof(OBJECTFOOTER));
    return S_OK;
}

HRESULT CGameObject::LoadObject(fstream& _fs, CGameObject** ppOut, wstring _sOBJLeveltag)
{
    auto _sentinel = OBJECTHEADER;
    _fs.read((char*)&_sentinel, sizeof(OBJECTHEADER));
    if (_sentinel != OBJECTHEADER) {
        MSG_BOX("Fail to Load Object : Invalid OBJ Header!");
        return E_FAIL;
    }

    //Declare Obj Info
    wchar_t _sOBJName           [MAXOBJNAMESIZE];
    wchar_t _sOBJPrototypeName  [MAXOBJNAMESIZE];
    wchar_t _sOBJLayerName      [MAXOBJNAMESIZE];
    _uint   _iSize;
    _char   _pDesc[MAXDESCSIZE];
    _bool   _bIsPrefab;
    ZeroMemory(_pDesc, MAXDESCSIZE);

    _fs.read((char*)_sOBJName,          sizeof(wchar_t) * MAXOBJNAMESIZE);
    _fs.read((char*)_sOBJPrototypeName, sizeof(wchar_t) * MAXOBJNAMESIZE);
    _fs.read((char*)_sOBJLayerName,     sizeof(wchar_t) * MAXOBJNAMESIZE);
    _fs.read((char*)&_iSize,            sizeof(_iSize));
    _fs.read(_pDesc,                    _iSize);
    _fs.read((char*)&_bIsPrefab,        sizeof(_bool));
    CGameObject* _pGameObject = { nullptr };
    
    if (_iSize==0)  CGameInstance::GetInstance()->Add_GameObject_ToLayer(_sOBJPrototypeName, _sOBJLeveltag, _sOBJLayerName, &_pGameObject, nullptr);
    else            CGameInstance::GetInstance()->Add_GameObject_ToLayer(_sOBJPrototypeName, _sOBJLeveltag, _sOBJLayerName, &_pGameObject, _pDesc);

    _pGameObject->ChangeName(_sOBJName);
    _pGameObject->m_bIsPrefab = _bIsPrefab;
    
    _fs.read((char*)&_sentinel, sizeof(COMGROUPHEADER));
    if (_sentinel != COMGROUPHEADER) {
        MSG_BOX("Fail to Load Object : Invalid COMPGROUP Header!");
        return E_FAIL;
    }
    if (!_bIsPrefab) { //Prototype Object Case
        while (!CheckHeader(_fs, COMGROUPFOOTER)) {
            SPComponent _pComponent = nullptr;
            CHKFAIL(CComponent::LoadComponent(_fs, _pComponent))
            _pGameObject->Add_Component(_pComponent);
        }
    }
    _fs.read((char*)&_sentinel, sizeof(COMGROUPFOOTER));
    if (_sentinel != COMGROUPFOOTER) {
        MSG_BOX("Fail to Load Object : Invalid COMPGROUP FOOTER!");
        return E_FAIL;
    }

    //Load Children
    _fs.read((char*)&_sentinel, sizeof(OBJGROUPHEADER));
    if (_sentinel != OBJGROUPHEADER) {
        MSG_BOX("Fail to Load Object : Invalid OBJChild Header!");
        return E_FAIL;
    }

    CGameObject* _pChildOut = nullptr;
    while (!CheckHeader(_fs, OBJGROUPFOOTER)) { 
        LoadObject(_fs, &_pChildOut, _sOBJLeveltag);
        _pChildOut->SetParent(_pGameObject);
    }

    _fs.read((char*)&_sentinel, sizeof(OBJGROUPFOOTER));
    if (_sentinel != OBJGROUPFOOTER) {
        MSG_BOX("Fail to Load Object : Invalid OBJChild Footer!");
        return E_FAIL;
    }
    
    _fs.read((char*)&_sentinel, sizeof(OBJECTFOOTER));
    if (_sentinel != OBJECTFOOTER) {
        MSG_BOX("Fail to Load Object : Invalid OBJ Footer!");
        return E_FAIL;
    }
    if(ppOut) *ppOut = _pGameObject;

    return S_OK;
}

HRESULT CGameObject::SavePrototype()
{
    std::filesystem::path   _path = m_pGameInstance->ReadConfig<wstring>(L"Prototype.Paths.OBJ_PrototypePath");
    std::filesystem::path   _tmpPath = _path;

    _path += m_sOBJName + L".dat";
    _tmpPath += L"ObjectSave";
    _tmpPath += L".tmp";

    ios_base::openmode      _mode = ios::out | ios::binary | ios::trunc;
    
    fstream _fs;
    _fs.open(_tmpPath, _mode);
    if (!_fs.is_open()) {
        wstring _sMSG = _tmpPath;
        _sMSG = L"Fail to create save file : ( " + _sMSG + L" )";
        MSG_BOXL(_sMSG.c_str());
        return E_FAIL;
    }
    CHKFAIL(SaveObject(_fs, this));
    _fs.close();

    filesystem::rename(_tmpPath, _path);


    return S_OK;
}

HRESULT CGameObject::LoadPrototype(const _wstring& sPrototypeTag, const _wstring& sLVLTag, CGameObject** _ppObject)
{
    std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Prototype.Paths.OBJ_PrototypePath");
    _path += sPrototypeTag + L".dat";

    ios_base::openmode      _mode = ios::in | ios::binary;
    fstream _fs;
    _fs.open(_path, _mode);
    if (!_fs.is_open()) {
        wstring _sMSG = _path;
        _sMSG = L"Fail to create read file : ( " + _sMSG + L" )";
        MSG_BOXL(_sMSG.c_str());
        return E_FAIL;
    }

    if (FAILED(CGameObject::LoadObject(_fs, _ppObject, sLVLTag))) {
        wstring _sMSG = _path;
        _sMSG = L"Fail to create Object from file : ( " + _sMSG + L" )";
        MSG_BOXL(_sMSG.c_str());
        return E_FAIL;
    }

    _fs.close();


    return S_OK;
}


HRESULT CGameObject::ChangeLayer(const _wstring& sNewLayerTag)
{
    m_sOBJLayertag = sNewLayerTag;
    return S_OK;
}

HRESULT CGameObject::ChangeName(const _wstring& sNewNameTag)
{
    m_sOBJName = sNewNameTag;
    return S_OK;
}

HRESULT CGameObject::ChangeLevel(const _wstring& sNewLevelTag)
{
    m_sOBJLeveltag = sNewLevelTag;
    return S_OK;
}

HRESULT CGameObject::SetTypeName(const _wstring& sNewTypeTag)
{
    m_sOBJTypeName = sNewTypeTag;
    return S_OK;
}

void CGameObject::SetActive(_bool bIsActive, _bool IsRecursive)
{
    m_bIsActive = bIsActive;
    for (auto& [_, vecComps] : m_vecComponents)  for (auto& pComp :vecComps)
    {
        pComp->SetActive(bIsActive);
    }
    if (IsRecursive) {
        for (auto pObject : m_listChildrenObjects) {
            pObject->SetActive(bIsActive, IsRecursive);
        }
    }
}

void CGameObject::SetSectionActive(_bool bIsActive, _bool IsRecursive)
{
    m_bIsSectionActive = bIsActive;

    if (IsRecursive)
    {
        for (auto pObject : m_listChildrenObjects)
            pObject->SetSectionActive(bIsActive, true);
    }
}

CGameObject* CGameObject::Render_ImGui_Hierarchy(CGameObject* pLastSelected)
{
    static string   sObjecctRenameText = "";

    CGameObject* pSelected = nullptr;

    ImGuiTreeNodeFlags          eFlag = ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (this == pLastSelected)  eFlag |= ImGuiTreeNodeFlags_Selected;

    string strLabel = ws2s(m_sOBJName);
    
    if (!m_bIsActive) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    _bool bTreeOpen = ImGui::TreeNodeEx(strLabel.c_str(), eFlag);
    if (!m_bIsActive) ImGui::PopStyleColor();

    if (ImGui::BeginPopupContextItem())
    {
        ImGui::InputText("##7", &sObjecctRenameText);
        ImGui::SameLine();
        if (ImGui::Button("Rename")) {
            ChangeName(s2ws(sObjecctRenameText));
        }
        if (ImGui::MenuItem("Remove Parent")) {
            SetParent(nullptr);
        }
        if (ImGui::MenuItem("Copy")) {
            if (FAILED(m_pGameInstance->CopyObject(L"Level_Editor", this))) {
                MSG_BOX("Fail to Copy Object");
            }
        }
        if (ImGui::MenuItem("Save Prototype")) {
            if (FAILED(SavePrototype())) {
                MSG_BOX("Fail to Save Object");
            }
        }
        if (ImGui::MenuItem("Delete")) {
            if (pLastSelected!=this)
                SetDestroyFlag();
        }
        ImGui::EndPopup();
    }
    
    if (ImGui::BeginDragDropSource())
    {
        CGameObject* tmpObjectPtr = this;
        ImGui::SetDragDropPayload("OBJECT_PAYLOAD", &tmpObjectPtr, sizeof(tmpObjectPtr));
        ImGui::Text("Dragging...");
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_PAYLOAD"))
        {
            CGameObject* tmpObjectPtr = *static_cast<CGameObject**>(payload->Data);
            tmpObjectPtr->SetParent(this);
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemClicked()) pSelected = this;
    if (bTreeOpen) {
        
        list<CGameObject*> _copyList = m_listChildrenObjects;
        _copyList.sort([](CGameObject* pGameObject1, CGameObject* pGameObject2) {
            return pGameObject1->GetName() < pGameObject2->GetName(); });
        _int id = 0;
        for (auto pObject : _copyList)
        {
            ImGui::PushID(id++);
            if (auto pNewSelected = pObject->Render_ImGui_Hierarchy(pLastSelected)) pSelected = pNewSelected;
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    return pSelected;
}

void CGameObject::Render_Info()
{
    string _sName       = ws2s(m_sOBJName);
    string _sProtoName  = ws2s(m_sOBJTypeName);
    string _sLayerName  = ws2s(m_sOBJLayertag);
    
    ImGui::BeginChild("##3", ImVec2(0, 120), true);
    ImGui::Text("Object Info");
    ImGui::Separator();

    ImGui::Text("Name");
    ImGui::SameLine(COLWIDTH);
    ImGui::Text(_sName.c_str());

    ImGui::Text("Prototype");
    ImGui::SameLine(COLWIDTH);
    ImGui::Text(_sProtoName.c_str());

    ImGui::Text("Layer");
    ImGui::SameLine(COLWIDTH);
    ImGui::Text(_sLayerName.c_str());

    ImGui::EndChild();
}

void CGameObject::Render_Inspector()
{
    static _bool g_bIsRecursive = false;
    if (ImGui::Checkbox("Is Active", &m_bIsActive)) {
        SetActive(m_bIsActive, g_bIsRecursive);
    }
    ImGui::SameLine();
    ImGui::Checkbox("Is Recursive", &g_bIsRecursive);

    Render_Info();
    Render_Properties();
    Render_Components();
}

void CGameObject::Render_Components()
{
    static string sComponentRenameText = "";
    for (auto& pair : m_vecComponents)
    {
        string sComClsName = ws2s(pair.first);
        _uint pDeleteComponentIdx = -1;
        if (ImGui::TreeNodeEx(sComClsName.c_str(), ImGuiTreeNodeFlags_Framed))
        {
            for (int i = 0; i < pair.second.size(); i++)
            {
                string sComName = ws2s(pair.second[i]->GetName());

                if (pair.second[i]->IsInitialAdd()) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
                bool open = ImGui::TreeNodeEx(
                    sComName.c_str(),
                    ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_Bullet
                );
                if (pair.second[i]->IsInitialAdd()) ImGui::PopStyleColor();

                if (!(pair.second[i]->IsInitialAdd()) && ImGui::BeginPopupContextItem())
                {
                    ImGui::InputText("##7", &sComponentRenameText);
                    ImGui::SameLine();
                    if (ImGui::Button("Rename")) {
                        pair.second[i]->SetName(s2ws(sComponentRenameText));
                    }
                    if (ImGui::MenuItem("Delete")) {
                        pDeleteComponentIdx = i;
                    }
                    ImGui::EndPopup();
                }

                if (open) {
                    ImGui::TreePush(sComName.c_str());
                    pair.second[i]->Render_Inspector();
                    ImGui::TreePop();
                }

            }
            ImGui::TreePop();

            if (pDeleteComponentIdx != -1) {
                (pair.second)[pDeleteComponentIdx] = nullptr;
                (pair.second).erase((pair.second).begin() + pDeleteComponentIdx);
            }
        }

    }
}


HRESULT CGameObject::Add_Component(const _wstring& sPrototypeTag, const _wstring& sComponentTag, void* pArg)
{
    auto it = m_vecComponents.find(sPrototypeTag);
    if (it == m_vecComponents.end()) {
        m_vecComponents.emplace(sPrototypeTag, vector<SPComponent>());
        it = m_vecComponents.find(sPrototypeTag);
    }
    for (auto& spCom : it->second) if (spCom->GetName() == sComponentTag) return S_OK;
    
    SPComponent pComponent = m_pGameInstance->Clone_ComponentPrototype(sPrototypeTag, pArg);

    if (!pComponent) return E_FAIL;

    pComponent->SetName(sComponentTag);
    pComponent->SetTypeName(sPrototypeTag);
    pComponent->SetGameObject(this);
        
    (it->second).push_back(pComponent);
    if (pComponent->IsUpdateComponent()) m_listUpdateComponents.push_back(pComponent);

    return S_OK;
}

CGameObject* CGameObject::Get_ChildObject(const _wstring& sNameTag)
{
    for (auto pObject : m_listChildrenObjects) {
        if (pObject->GetName() == sNameTag)                     return pObject;
        if (auto pChild = pObject->Get_ChildObject(sNameTag))   return pChild;
    }
    return nullptr;
}

HRESULT CGameObject::Add_Component(SPComponent pComponent, void* pArg)
{
    auto sPrototypeTag = pComponent->GetTypeName();
    if (m_vecComponents.end() == m_vecComponents.find(sPrototypeTag)) { m_vecComponents.emplace(sPrototypeTag, vector<SPComponent>()); }

    for (auto& spCom : m_vecComponents.find(sPrototypeTag)->second) if (spCom->GetName() == pComponent->GetName()) return S_OK;

    m_vecComponents.find(sPrototypeTag)->second.push_back(pComponent);
    if (pComponent->IsUpdateComponent()) m_listUpdateComponents.push_back(pComponent);
    pComponent->SetGameObject(this);

    return S_OK;
}

HRESULT CGameObject::Add_InitComponents()
{
    Add_Component<CTransform>(L"Com_Transform_Main");
    auto spMainTransform = dynamic_pointer_cast<CTransform>(Get_Component(L"Com_Transform_Main").lock());
    spMainTransform->SetInitialAdd();
    m_wpMainTransformCom = spMainTransform;

    return S_OK;
}

HRESULT CGameObject::Load_Asset()
{
    HRESULT hr = S_OK;
    for (auto [_, _componentVec] : m_vecComponents) for (auto spComponent : _componentVec)
    {
        if (FAILED(spComponent->Load_Asset())) hr = E_FAIL;
    }
    m_bIsLoadAsset = true;
    return hr;
}

void CGameObject::Free()
{
    __super::Free();    

    for (auto [_, _componentVec] : m_vecComponents) _componentVec.clear();
    m_vecComponents.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}

HRESULT CGameObject::Reset()
{
    return S_OK;
}