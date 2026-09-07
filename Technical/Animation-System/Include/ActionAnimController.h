#pragma once
#include "AnimController.h"
NS_BEGIN(Engine)

class CActionAnimController :  public CAnimController
{
public:
	~CActionAnimController() = default;
	CActionAnimController& operator=(const CActionAnimController&) = delete;
	CActionAnimController(const CActionAnimController&) = delete;
protected:
	CActionAnimController(const CActionAnimController& prototype, shared_ptr<CModel> spModel);
	CActionAnimController(shared_ptr<CModel>	spModel);
public:
	HRESULT				AddAnimNode(unique_ptr<CAnimNode>			upAnimNode, bool bIsStartNode = false);
	HRESULT				AddAnimTransitition(Json::Value jsonValue);
public:
	unique_ptr<CAnimController>			Clone(shared_ptr<CModel>	spModel);
	static  unique_ptr<CAnimController>	Create(Json::Value jsonValue, shared_ptr<CModel>	spModel);
};

NS_END
