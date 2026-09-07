#pragma once
#include "Client_Defines.h"
#include "UIController.h"
#include "Time_Manager_API.h"

NS_BEGIN(Client)

class CUC_RhythmChallenge_Korsica : public CUIController
{
private:
	enum EUITYPE {
		UT_DISK,
		UT_DISK_CURSOR,
		UT_DISK_SUCCESS0,
		UT_DISK_SUCCESS1,
		UT_DISK_SUCCESS2,
		UT_DISK_SUCCESS3,

		UT_COUNTDOWN,
		UT_COUNTDOWN0,
		UT_COUNTDOWN1,

		UT_NOTE,
		UT_NOTE0,
		UT_NOTE1,
		UT_NOTE2,
		UT_NOTE3,

		UT_FAIL,
		UT_FAILMARKER,
		UT_HIT,
		UT_YEAH,

		UT_END
	};
private:
	explicit CUC_RhythmChallenge_Korsica();
	virtual ~CUC_RhythmChallenge_Korsica() = default;

public:
	HRESULT Initialize(void* pDesc, _wstring sLVLTag, _wstring sUCName) override;
	void Update(float fDeltaTime) override;
	void Late_Initialize() override;

public:
	static CUC_RhythmChallenge_Korsica* Create(void* pArg, _wstring sLVLTag, _wstring sUCName);

	void Enter() override;
	void Exit() override;
	void Free() override;
public:
	
	void	OnChallenge(_uint iBeatMask);
	_bool	CheckOnChallenge() { return m_iCnt >= 4; };
	_bool	CheckHit();
	_bool	CheckMiss();
	_bool	CheckSuccess();
	_int	GetHitNoteNum() { return m_iHitNoteNum; };

	void Success();
	void Fail();
	void Hit();
private:
	void NoteAppear	();
	void SetNote	();
	void UpdateNote	();
	void Countdown	();
	void TurnCursor	();
private:
	_int			m_iNoteIndices[4]	= {-1, -1, -1, -1};
	_int			m_iHitNoteNum		= 0;
	_int			m_iCnt				= -1;
	_uint			m_iBeatMask			= 0;
	_uint			m_iPhase			= 0;
	_float			m_fBeatOffset		= 0.4f;
	UniqueTimer		m_upFailTimer		= nullptr;
	UniqueTimer		m_upHitTimer		= nullptr;
	UniqueTimer		m_upSuccessTimer	= nullptr;
};

NS_END