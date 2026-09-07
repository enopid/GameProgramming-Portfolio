#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CEffectRoot;
NS_END

NS_BEGIN(Client)
class CKorsica_Wind : public CGameObject
{
public:
	struct FKORSICAWINDDESC {
		_float	m_fLifeTime		= 10.f;
		_float	m_fLinearTime	= 1.f;
		
		_float	m_fSpeed		= 1.f;
	};
public:
	CKorsica_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKorsica_Wind(const CKorsica_Wind& Prototype);
	virtual ~CKorsica_Wind() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	HRESULT Load_Asset() override;

	HRESULT Add_InitComponents() override;
public:
	virtual void Render_Properties() override;
	void	Shot(_vector vPos, _vector vDir);
private:
	void	Stop();
	void	Move(float fDeltaTime);
public:
	virtual void	Free() override;

	CGameObject* Clone(void* pArg) override;
	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc) override;
protected:
	WPCollidor						m_wpHitCollidor;
	WPCollidor						m_wpBodyCollidor;
	FKORSICAWINDDESC				m_desc				= {};

	UniqueTimer						m_upLinearTimer		= nullptr;
	UniqueTimer						m_upLifeTimer		= nullptr;
	_bool							m_bTurn				= false;
	_bool							m_bRightTurn		= false;
	vector<class CEnemy*>           m_vecRotTarget; 
	CEffectRoot*					m_pEffect			= nullptr;

	UniqueTimer                     m_pWindDamageTimer  = nullptr;

private:
	void                           EnemyRot();
};

NS_END
