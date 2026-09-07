#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CModel;
class CCameraHandler;
class CEffectRoot;
NS_END

NS_BEGIN(Client)
class CCutScene : public CGameObject
{
	const static int MAXCUTSCENE = 30;
	enum EACTOR : unsigned long long {
		BACKDANCER1,
		BACKDANCER2,
		BACKDANCER3,
		BACKDANCER4,
		BACKDANCER5,
		BACKDANCER6,

		MIMOSA,

		PLAYER,

		GUITAR,
		GUITARORIGIN,
		GUITAR_BAR,
		TRASHGUITAR,

		MIMOSA2,	//Mimosa2
		MIMOSA_MIKE,//Mike

		MUSICIAN1,	//drum
		MUSICIAN2,	//trombone
		MUSICIAN3,	
		MUSICIAN4,	
		MUSICIAN5,
		MUSICIAN6,	//trumpet
		MUSICIAN7,
		MUSICIAN8,	
		MUSICIAN9,

		CAT_808,
		BALL_808,
		BATTLE_808,

		ANTENA1,
		ANTENA2,
		ANTENA3,
		ANTENA4,
		ANTENA5,
		ANTENA6,
		ANTENA7,
		ANTENA8,
		PLATFORM,

		CHAI_101,

		PEPPERMINT,
		MACARON,
		KORSICA,

		SUIT_KALE,
		KALE_TRANIT,
		PHASE1_KALE,
		KALE_WEAPON,
		KALE_SPECTRA,
		KALE_BLINDCLOTH,
		KALE_CLOAK,
		KALE_BARRIER_ARM,
		KALE_FIRE_ARM,
		KALE_SHELD_ARM1,
		KALE_SHELD_ARM2,
		KALE_BEFORE_ELECTRIC,
		KALE_AFTER_ELECTRIC,
		KALE_LAST,

		PEPP_WEAPON_L,
		PEPP_WEAPON_R,
		PEPP_WEAPON_SHOTGUN,
		KOR_WEAPON_L, 
		KOR_WEAPON_R, 

		EACTOREND
	};
public:
	struct FSEQINSTDESC {
		_float	fDuration	= 1.f;
		unsigned long long	iActorMask = 1ull << (EACTOREND - 1);
	};
	struct FCUTSCENEDESC {
		_int			m_iNumSequence = 0;
		FSEQINSTDESC	m_seqInstances[MAXCUTSCENE];
		wchar_t			sVOXName[60];
		_int			m_iSeqIdx = 30;
	};
	struct FSUBTITLEDESC {
		_int		iActIdx;
		_float		fTime;
		_float		fDuration;
		_wstring	strSpeaker;
		_wstring	strDialogue;
	};
	struct FTALKBOXDESC {
		_int		iActIdx;
		_float		fTime;
		_float		fDuration;
		_int		iSpeakerIdx;
		_int		iEmotion;
		_wstring	strDialogue;
	};
	struct FILLUSTEDESC {
		_int		iActIdx;
		_float		fTime;
		_float		fDuration;
		_int		iIllustIdx;
	};
	struct FSPOTLIGHTDESC {
		_int		iActIdx;
		_float		fTime;
		_int		iActorIdx;
		_bool		bStart;
	};
	struct FVFXDESC {
		_int		iActIdx;
		_float		fTime;

		_int		iActorIdx;
		string		sVFXTag;
		string		sBoneTag;
		_bool		bDirLock	= false;
		_bool		bStop		= false;
		_bool		bNonBone	= false;
	};
	struct FVOXDESC {
		_int		iActIdx;
		_float		fTime;
		string		sVOXTag;
		_float		fVolume = 1.f;
	};
	struct FRGDESC {
		_int		iActIdx;
		_float		fTime;
		_int		iRGIdx;
		_float		fDuration = 1.f;
	};
	struct FATTACHDESC 
	{
		_int       iActIdx;

		_float     fTime;
		_float     fDuration;

		_int       iTargetActorIdx;
		_int       iWeaponActorIdx;

		_wstring   strTargetActorBoneName;
		_wstring   strWeaponActorBoneName;
	};
	struct FBONESCALEDESC 
	{
		_int       iActIdx;
		_int       iTargetActorIdx;

		_float     fTime;

		_wstring   strTargetActorBoneName;

		_bool      isRemove;
	};
	struct FCSFUNCDESC 
	{
		_int					iActIdx;
		_float					fTime;
		function<void(void)>	pFunc = nullptr;
	};
	struct FSPEEDLINEDESC 
	{
		_int					iActIdx;
		_float					fTime;
		_int					iSLIdx = 0;
		_bool					bIsStart = false;
	};
public:
	CCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCutScene(const CCutScene& Prototype);
	virtual ~CCutScene() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	HRESULT Add_InitComponents() override;

	void	AddActor(wstring sActorTag, wstring sPrototypeTag, EACTOR eActorType);
public:
	virtual void Render_Properties() override;
	void	Play();
	void	Stop();
	void	Restart();
	void	Play(int _iActIdx);
	bool	IsEnd();
	void	SetAct();

	void    ChangeLevel_Kale();

	void	SetWeapon(const FATTACHDESC& desc);
public:
	virtual void	Free() override;

	CGameObject* Clone(void* pArg) override;
	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc) override;

public:
	static _bool Get_IsCutScene()  { return IsCutScene; }

	void	AddCSFunc(FCSFUNCDESC _desc);
private:
	void	LoadSubtitle	(wstring sSubtitleJsonTag);
	void	LoadTalks		(wstring sSubtitleJsonTag);
	void	LoadIllust		(wstring sSubtitleJsonTag);
	void	LoadTransition	(wstring sSubtitleJsonTag);
	void	LoadSpolights	(wstring sSubtitleJsonTag);
	void	LoadVFXs		(wstring sSubtitleJsonTag);
	void	LoadVOXs		(wstring sSubtitleJsonTag);
	void	LoadAttach		(wstring sSubtitleJsonTag);
	void	LoadBoneScale 	(wstring sSubtitleJsonTag);
	void	LoadRG			(wstring sSubtitleJsonTag);
	void	LoadSpeedLines	(wstring sSubtitleJsonTag);
	void	PlayVFX(FVFXDESC _desc);
	void    UpdateAttach();
	void	ReadyEmissive();
	void	ChangeKale();
	void	SetExplosion(bool bExplosion);
	void	ChangeOrigin();
	void	ChangeTrash();
private:
	class Actor*						m_vecActors[EACTOREND];
	CCameraHandler*						m_pCameraHandler = nullptr;
	_int								m_iActIdx = -1;
	FCUTSCENEDESC						m_desc = {};
	UniqueTimer							m_upSeqTimer = nullptr;
	_bool								m_bSpecificSeq = false;


	unsigned long long					m_iSeqActorMask		= 0ull;
	//subtitle
	vector<FSUBTITLEDESC>				m_vecSubtitles;
	_int								m_iCurSubtitleIdx	= 0;
	vector<FTALKBOXDESC>				m_vecTalks;
	_int								m_iCurTalkIdx	= 0;
	vector<FILLUSTEDESC>				m_vecIllusts;
	_int								m_iCurIllustIdx		= 0;
	vector<FILLUSTEDESC>				m_vecTransitions;
	_int								m_iCurTransitionIdx = 0;
	vector<FSPOTLIGHTDESC>				m_vecSpotlights;
	_int								m_iCurSpotlightIdx	= 0;
	vector<FVFXDESC>					m_vecVFXs;
	_int								m_iCurVFXIdx		= 0;
	vector<FVOXDESC>					m_vecVOXs;
	_int								m_iCurVOXIdx		= 0;
	vector<FATTACHDESC>                 m_vecAttach;
	_int                                m_iCurAttachIdx     = 0;
	vector<FRGDESC>						m_vecRGs;
	_int                                m_iCurRGIdx			= 0;
	vector<FBONESCALEDESC>              m_vecBoneScale;
	_int                                m_iCurBoneScaleIdx  = 0;
	vector<FCSFUNCDESC>					m_vecCSFuncs;
	_int                                m_iCSFuncIdx		= 0;
	vector<FSPEEDLINEDESC>				m_vecSLDescs;
	_int                                m_iSLDescIdx		= 0;
	
	_int                                m_iGamePlayBPM	= {};
	class CUC_Message*					m_pUCSubtitle	= nullptr;
	class CUC_Talkbox*					m_pUCTalkbox	= nullptr;
	class CUC_CutRhythmGame*			m_pUCRhythmGame	= nullptr;

	_bool								m_bReadyInit	= false;
	_bool								m_bIsExploded	= false;

	inline static     _bool             IsCutScene		= false;

	UniqueTimer							m_upTrashTimer = { nullptr };

	CEffectRoot*					m_pSpeedLines[4] = {};
};
NS_END


