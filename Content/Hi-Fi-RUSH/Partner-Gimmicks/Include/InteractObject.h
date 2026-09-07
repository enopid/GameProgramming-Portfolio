#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CAnimController;
NS_END

NS_BEGIN(Client)
class CEmpty_FSM;


class CInteractObject abstract : public CGameObject
{
public:
	enum class ESTATE {
		DEACTIVE,
		ACTIVE,
		TRIGGER
	};
public:
	struct FINTERACTOBJECTDESC {
		_bool	m_bIsMaster			= false;
		_int	m_iGroupIdx			= 0;
		_int	m_iInitState		= 0;
		_float	m_fDeactiveTime		= 0.f;
		_int	m_iAdditionalType	= 0;
	};
public:
	CInteractObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteractObject(const CInteractObject& Prototype);
	virtual ~CInteractObject() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update	(_float fTimeDelta);
	virtual void Update				(_float fTimeDelta);
	virtual void Late_Update		(_float fTimeDelta);

	virtual void	Render_Properties() override;
public:
	void			Activate();
	void			Trigger();
	void			Deactivate();
	bool			IsMaster();
	ESTATE			GetState();
	EInteractType	GetInteractType();
	_int			GetAdditionalType() { return m_desc.m_iAdditionalType; }
	_float			GetDeActiveTime() { return m_desc.m_fDeactiveTime; }
public:
	virtual void	Free() override;
	virtual HRESULT	Reset() override;

	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc) override;
protected:
	HRESULT Add_InitComponents() override;

	virtual void	Set_Animator(wstring sAnimatorPath);
	HRESULT	AddStateFunc(ESTATE eState, EKEYACTIONSTATE eKeyAction, function<void(float)> pFunc);
private:
	void    BindSameGroup();
	void    BindSameGroup(class CInteractObject* pObject);
	void    RemoveGroup(class CInteractObject* pObject);
	void	SetBBFromAnimJson(const Json::Value& root, ANIMCTX_DESC& m_ctxDesc);
protected:
	unique_ptr<CAnimController>	m_upAnimController;
	ANIMCTX_DESC				m_ctxDesc;
	weak_ptr<CModel>			m_wpModel;

	vector<CInteractObject*>	m_vecInteractObjects;
	FINTERACTOBJECTDESC			m_desc;

	ESTATE						m_eCurState			= ESTATE::DEACTIVE;
	ESTATE						m_ePrevState		= ESTATE::DEACTIVE;
	
	UniqueTimer					m_upDeactiveTimer	= nullptr;
	_bool						m_bDeactivateTimer  = false;

	list<function<void(float)>>		m_vecFuncs[3][3];

	_float3 m_vCurEmissiveColor{};

	EInteractType				m_eInteractType = EInteractType::IT_NONE;

};
NS_END
