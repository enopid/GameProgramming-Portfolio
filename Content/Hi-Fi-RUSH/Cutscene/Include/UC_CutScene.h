#pragma once
#include "Client_Defines.h"
#include "UIController.h"
#include "Time_Manager_API.h"

NS_BEGIN(Client)

class CUC_CutScene : public CUIController
{
public:
	//enum ILLUSTTYPE { MIMOSA_INTRO, MIMOSA_END, KALE_INTRO, KALE_PEPP, KALE_MACA, KALE_KOR, KALE_CHAI, NONE};

private:
	explicit CUC_CutScene();
	virtual ~CUC_CutScene() = default;

public:
	HRESULT Initialize(void* pDesc, _wstring sLVLTag, _wstring sUCName) override;
	void Update(float fDeltaTime) override;
	void Late_Initialize() override;

public:
	static CUC_CutScene* Create(void* pArg, _wstring sLVLTag, _wstring sUCName);

	void Enter() override;
	void Exit() override;
	void Free() override;

public:
	void IllustImage(_int iType, _float fDutation);
	void SpeedLine(_int iType, _float fDuration);

private:
	UniqueTimer m_pIllustTime = { nullptr };
	_float m_fIllustDuration = { 1.f };
	_int m_iCurImage = 0;
};

NS_END