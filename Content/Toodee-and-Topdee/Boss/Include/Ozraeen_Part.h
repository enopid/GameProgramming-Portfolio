#pragma once
#include "Boss_Part.h"

NS_BEGIN(Client)
class CBoss;

class COzraeen_Part abstract : public CBoss_Part
{
protected:
	COzraeen_Part(LPDIRECT3DDEVICE9 pGraphic_Device);
	COzraeen_Part(const COzraeen_Part& Prototype);
	virtual ~COzraeen_Part() = default;
public:
	virtual HRESULT SetParent(CBoss* ppGameObject) override;
	HRESULT SetParentTransform(CTransform* pTransformCom);
	void Flip();
protected:
	void InheritPositon_Begin();
	void InheritPositon_End();
	void BillBoard();
protected:
	class CTransform* m_pParentTransform	= {nullptr};
	class CTransform* m_pBaseTransform		= {nullptr};
	_float3 m_vOffset;
	bool	m_bFlip = false;
};
NS_END

