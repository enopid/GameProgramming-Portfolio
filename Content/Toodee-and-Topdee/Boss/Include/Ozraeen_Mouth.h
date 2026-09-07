#pragma once
#include "Ozraeen_Part.h"
class COzraeen_Mouth final : public COzraeen_Part
{
private:
	COzraeen_Mouth(LPDIRECT3DDEVICE9 pGraphic_Device);
	COzraeen_Mouth(const COzraeen_Mouth& Prototype);
	virtual ~COzraeen_Mouth() = default;
public:
	virtual HRESULT Initialize_Prototype()				override; /* 원형을 위한 초기화(서버, 파일) */
	virtual HRESULT Initialize(void* pArg)				override; /* 사본객체를 위한 초기화 */
	virtual void	Priority_Update(_float fTimeDelta)	override;
	virtual void	Update(_float fTimeDelta)			override;
	virtual void	Late_Update(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;
public:
	HRESULT Ready_Components()		override;
public:
	static COzraeen_Mouth*	Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject*			Clone(void* pArg)			override;
	virtual void			Free()						override;
private:
	_uint m_iCurTextureIdx = {};
};

