#pragma once

/* 1. 현재 활성화된 레벨을 보유하고 반복적으로 업데이트와 렌더를 호출해준다. */
/* 2. 레벨을 교체 시에 알아서 기존 레벨을 삭제하는 역활을 수행한다. */
/* 3. 기존 레벨 용 게임 자원(게임리소스, 게임오브젝트)들을 삭제 해준다. */

#include "Base.h"

NS_BEGIN(Engine)

class CLevel_Manager final : public CBase
{
public:
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT		Change_Level();
	HRESULT		Load_Level(tagLevelTransitionDesc _desc);
	HRESULT		Create_Level(const wstring& sLVLPrototypeTag, wstring sLVLName);
	_wstring	GetCurrentLevelTag();
	_wstring	GetCurrentDetailLevelTag();
	tagLevelTransitionDesc	GetLevelDesc();
	void		Update(_float fTimeDelta);
	HRESULT		Render();
	void		Clear(const _wstring& strLVLTag);

	HRESULT Load_Level_InEditor(const wstring& sLVLName, wstring& sLVLPrototypeName);
	HRESULT Save_Level_InEditor(const wstring& sLVLName, const wstring& sLVLPrototypeName);
	
	class CUIController* GetCurrentLevelUC();
	class CLevel* GetCurrentLevelPtr();
private:
	_wstring				m_curLVLTag				= { L"" };
	tagLevelTransitionDesc	m_TransitDesc	= {};
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CLevel*			m_pNextLevel	= { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

NS_END