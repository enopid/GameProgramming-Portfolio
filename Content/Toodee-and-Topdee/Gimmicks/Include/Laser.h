#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CLaser : public CGameObject
{
	constexpr static _uint MAXREFLECTION = 20;
public:
	struct FLaserDesc {
		_float3 vInitPos;
		_float3	vDir;
	};

protected:
	CLaser(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLaser(const CLaser& Prototype);
	virtual ~CLaser() = default;
public:
	virtual HRESULT Initialize_Prototype()				override; /* 원형을 위한 초기화(서버, 파일) */
	virtual HRESULT Initialize(void* pArg)				override; /* 사본객체를 위한 초기화 */
	virtual void	Priority_Update(_float fTimeDelta)	override;
	virtual void	Update(_float fTimeDelta)			override;
	virtual void	Late_Update(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

	HRESULT Ready_Components();
public:
	static CLaser* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
public:
	void SetActive(_bool _bIsActive) { m_bIsActive = _bIsActive; };
	bool IsActive()					{ return m_bIsActive; };

private:
	void Shot();
	bool Move();

private:
	CTexture*		m_pTextureCom		= { nullptr };
	CTexture*		m_pTextureMaskCom	= { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CTransform*		m_pPivotTransformCom = { nullptr };
	CVIBuffer_Rect*	m_pVIBufferCom		= { nullptr };

	vector<pair<_float3, _float3>>	m_vecReflectInfos;
	vector<wstring>					m_vecLayerMasks;

private:
	_float3		m_vInitPos	= {};
	_float3		m_vCurDIr	= {};
	_bool		m_bIsActive = {true};
	_bool		m_bReflect;
private:
	class CPSystem_Dust* m_pSystemDust = { nullptr };
};

NS_END
