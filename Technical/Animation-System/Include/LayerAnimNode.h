#pragma once
#include "AnimNode.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLayerAnimNode final : public CAnimNode
{
public:
	virtual ~CLayerAnimNode() = default;
protected:
	CLayerAnimNode(shared_ptr<CModel>	spModel);
	CLayerAnimNode(const CLayerAnimNode& prototype, shared_ptr<CModel>	spModel);

public:
	// CAnimNode을(를) 통해 상속됨
	void Update(float _fDeltaTime) override;
	void UpdateCTX(const ANIMCTX_DESC& ctx) override;
public:
	bool CanExit(ANIMTRANSITION_DESC _desc) override;
	void Enter(ANIMTRANSITION_DESC _desc)	override;
	void Exit(ANIMTRANSITION_DESC _desc)	override;
	bool IsEnd()							override;
	void Reset() override;

	void	SetLayerIdx(int i);
	string	GetLayerTag();
public:
	unique_ptr<CAnimNode> Clone(shared_ptr<CModel> spModel) override;
	static  unique_ptr<CLayerAnimNode>	Create(Json::Value jsonValue, shared_ptr<CModel>	spModel);
private:
	vector<ANIMCLIP_DESC>	m_vecAnimClipDesc;
	string					m_sLayerIdxTag = "";
	_int					m_iLayerIdx		= 0;
	_int					m_iNxtLayerIdx	= 0;

	// CAnimNode을(를) 통해 상속됨
	HRESULT Initialize(Json::Value jsonValue) override;

	_bool		m_bHasExitTime = false;
	_float		m_fRatio = 0.f;


	// CAnimNode을(를) 통해 상속됨
	void SetRootMotionScale(float fXScale, float fYScale) override;

	// CAnimNode을(를) 통해 상속됨
};

NS_END

