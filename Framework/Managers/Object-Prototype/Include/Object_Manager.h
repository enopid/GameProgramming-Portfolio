#pragma once

#include "Base.h"
#include "Picking.h"

NS_BEGIN(Engine)

class CGameObject;

class CObject_Manager final : public CBase
{
protected:
	const static _uint MAXLAYERNAMESIZE = 60;
private:
	const static _uint LAYERGROUPHEADER = 0x900DC0DE;
	const static _uint LAYERGROUPFOOTER = 0xBAADC0DE;

	const static _uint OBJGROUPHEADER	= 0x900DFACE;
	const static _uint OBJGROUPFOOTER	= 0xBAADFACE;
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	CGameObject*		Get_Object(const _wstring& strLVLTag, const _wstring& strLayerTag, const _wstring& strObjectName);
	list<CGameObject*> Get_Objects(const _wstring& strLVLTag, const _wstring& strLayerTag);
	vector<CGameObject*> Get_Terrains(const _wstring& strLVLTag, const _wstring& strLayerTag);
	HRESULT				Load_Asset(const _wstring& strLVLTag);
public:
	HRESULT Initialize();
	HRESULT Add_GameObject_ToLayer(const _wstring& strPrototypeTag, const _wstring& strLVLTag, const _wstring& strLayerTag, CGameObject** _ppOut, void* pArg);

	void Priority_Update	(const _wstring& strLVLTag, _float fTimeDelta);
	void Update				(const _wstring& strLVLTag, _float fTimeDelta);
	void Late_Update		(const _wstring& strLVLTag, _float fTimeDelta);
	void Clear				(const _wstring& strLVLTag);
	void AddLevel			(const _wstring& strLVLTag);

	HRESULT	SaveObjects		(fstream& _fs, const _wstring& strLVLTag);
	HRESULT	LoadObjects		(fstream& _fs, const _wstring& strLVLTag);

	HRESULT ChangeLayerTimeScale(const _wstring& sLVLTag, const _wstring& sLayerTag, float fTimeScale);
	

	//Editor
	//Editor-Layer
	vector<_wstring>	GetLayerNames	(const _wstring& sLVLTag);
	HRESULT				AddLayer		(const _wstring& sLVLTag, const _wstring& sLayerTag);
	HRESULT				DeleteLayer		(const _wstring& sLVLTag, const _wstring& sLayerTag);
	HRESULT				RenameLayer		(const _wstring& sLVLTag, const _wstring& sOldLayerTag, const _wstring& sNewLayerTag);

	_bool				CheckNameDuplication(const _wstring& sLVLTag, const wstring& sName);
	//Editor - picking
	list<CPicking::PICKING_DESC>	RayCastObjects(RAY InWorldRay, const _wstring& sLVLTag, vector<wstring> vecLayers);

	//Editor-Object
	HRESULT			ChangeObjectLayer(const _wstring& sLVLTag, const _wstring& sNewLayerTag, CGameObject* pObject);
	HRESULT			CopyObject(const _wstring& sLVLTag, CGameObject* pObject);
	CGameObject*	Render_ImGui_Hierarchy(CGameObject* pLastSelected);


private:
	map<_wstring, map<_wstring, class CLayer*>> m_pLayers;


private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	class CLayer* Find_Layer(const _wstring& strLVLTag, const _wstring& strLayerTag);
	
public:
	static CObject_Manager* Create();
	virtual void Free() override;

};

NS_END