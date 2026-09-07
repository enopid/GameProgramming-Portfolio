#pragma once
#include "Base.h"
#include "AnimNode.h"
#include "AnimTransition.h"

//목적
//모델의 애니메이션 전이는 해당 컴포를 통해서만 일어남
//모델을 소유자는 절대로 전이를 직접 호출 ㄴㄴ
//일종의 그룹 개념 그룹내에서도 애니메이션이 돌다가 그룹간 전이조건 만족시 바로 탈출
// 
//초기화
//일단 모델컴포 소유 - Late Initialize 에서 가져옴
//
//기능
// 1. 애니메이션 추가 - 여러 종류 존재
//	1 - 1. Begin - Loop - End - 시작,  루프, 끝을 하나의 단위로 가짐
//  1 - 2. TransitAnim
//  1 - 3. LayerAnimation - 클립을 여러개 가지고 있되 특정 인덱스에 따라 다른 클립 재생
// 2. AnimGroup
//	2 - 1. 그룹 내부 구성 요소가 그룹이거나 노드일 수 있음 / 혼재는 안됨
// 3. 전이조건 추가
//	3 - 1. 조건에따른 전이
//  3 - 2. 그룹간 전이 조건 존재
// 4. 그룹 활성화 비활성화
//  4 - 1. 활성화 진입시 시작 상태 지정
//  4 - 2. 또 비활성화시 전이 애니메이션 지정 가능
// 5. AdditiveLayer 적용
//	5 - 1. 특정 Ref 애니메이션 활성화/비활성화

// Comp -> 그룹 -> 컨트롤러 -> Node
// 그룹이 애님 노드를 생성
// 


NS_BEGIN(Engine)
class CModel;

class ENGINE_DLL CAnimController  : public CBase
{
public:
	~CAnimController() = default;
	CAnimController& operator=(const CAnimController&) = delete;
	CAnimController(const CAnimController&) = delete;
protected:
	CAnimController(const CAnimController& prototype, shared_ptr<CModel> spModel);
	CAnimController(shared_ptr<CModel>	spModel);
public:
	HRESULT				PlayAnimNode	(int NodeIdx, float fBlendTime = 0.2f);
	_bool				CheckAnimNodeEnd(int NodeIdx);
	void				ResetAnimNode(int NodeIdx);
	_int				GetCurrentAnimNode();
	HRESULT				AddAnimNode			(unique_ptr<CAnimNode>			upAnimNode, bool bIsStartNode = false);
	HRESULT				AddAnimTransitition	(Json::Value jsonValue);

	void				SetRootMotionScale(string sTag, float fXScale, float fYScale);
	HRESULT				Initialize(Json::Value jsonValue);

	void				Update(_float fDeltaTime);
	void				UpdateCTX(const ANIMCTX_DESC& ctx);
	bool				CanExit(_bool bHasExitTIme, _float fRatio);

	void				Enter();
	void				Exit();

	void				SetActive(_bool bIsActive);
	_bool				IsActive() { return m_bIsActive; };

	void				BindExitFunc	(function<void(void)> pFunc);
	void				BindEnterFunc	(function<void(void)> pFunc);
	void				SetLayerIdx		(const string& sLayerTag, int idx);
public:
	unique_ptr<CAnimController>			Clone(shared_ptr<CModel>	spModel);
	static  unique_ptr<CAnimController>	Create(Json::Value jsonValue, shared_ptr<CModel>	spModel);
protected:
	void				CheckTransition(const ANIMCTX_DESC& ctx);
	_int				FindAnimNode(const string& sIDtag);
protected:
	weak_ptr<CModel>					m_wpModel;
	_bool								m_bIsActive = false;

	vector<unique_ptr<CAnimNode>>		m_vecAnimNodes;
	vector<unique_ptr<CAnimTransition>>	m_vecTransitions;
	vector<vector<int>>					m_tableTransitions;
	vector<int>							m_vecAnyStateTransitions;

	map<string, list<class CLayerAnimNode*>>		m_mapLayerAnimNodes;
	map<string, list<class CLayerActionAnimNode*>>	m_mapLayerActionAnimNodes;

	_int								m_iStartNodeIdx		= -1;
	_int								m_iCurrentNodeIdx	= -1;

	//NextNodeInfo
	ANIMTRANSITION_DESC					m_transitDesc;
	function<void(void)>				m_pExitFunc		= nullptr;
	function<void(void)>				m_pEnterFunc	= nullptr;
};

NS_END

