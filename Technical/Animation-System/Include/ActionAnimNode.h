#pragma once
#include "AnimNode.h"

NS_BEGIN(Engine)

class ENGINE_DLL CActionAnimNode final : public CAnimNode
{
public:
	virtual ~CActionAnimNode() = default;
protected:
	CActionAnimNode(shared_ptr<CModel>	spModel);
	CActionAnimNode(const CActionAnimNode& prototype, shared_ptr<CModel>	spModel);

public:
	// CAnimNode을(를) 통해 상속됨
	void Update(float _fDeltaTime) override;
	void UpdateCTX(const ANIMCTX_DESC& ctx) override;
public:
	bool CanExit(ANIMTRANSITION_DESC _desc) override;
	void Enter(ANIMTRANSITION_DESC _desc)	override;
	void Exit(ANIMTRANSITION_DESC _desc)	override;
	void Reset() override;
public:
	unique_ptr<CAnimNode> Clone(shared_ptr<CModel> spModel) override;
	static  unique_ptr<CActionAnimNode>	Create(Json::Value jsonValue, shared_ptr<CModel>	spModel);
private:
	ANIMCLIP_DESC m_animClipDesc;

	// CAnimNode을(를) 통해 상속됨
	HRESULT Initialize(Json::Value jsonValue) override;

	_bool		m_bHasExitTime = false;
	_float		m_fRatio = 0.f;


	// CAnimNode을(를) 통해 상속됨
	void SetRootMotionScale(float fXScale, float fYScale) override;

	// CAnimNode을(를) 통해 상속됨
};

NS_END

