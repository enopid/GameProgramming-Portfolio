#include "Korsica_Wind.h"
#include "GameInstance.h"
#include "Enemy.h"

#include "EffectRoot.h"

CKorsica_Wind::CKorsica_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	
}

CKorsica_Wind::CKorsica_Wind(const CKorsica_Wind& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CKorsica_Wind::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
	return S_OK;
}

HRESULT CKorsica_Wind::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	m_upLinearTimer = 
	m_pGameInstance->CreateTimerWithDuration(1.f, false, [=]() {
		m_bTurn = true;
	}, false);
	m_upLifeTimer =
	m_pGameInstance->CreateTimerWithDuration(1.f, false, [=]() {
		Stop();
	}, false);


	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH4000_Attack_Hold",
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_pEffect))
	)
	m_pEffect->SetLevelObject();
	m_pEffect->Stop();


	CCharacter::FDamageInfo _damageInfo = {};
	_damageInfo.eAttackType = CCharacter::EAttackType::KORSICA_WHIRL_ATK;
	_damageInfo.fDamage = 4.f;
	_damageInfo.pSourceObj = this;


	m_pWindDamageTimer = m_pGameInstance->CreateTimerWithDuration(0.5f, true, [=]()
		{
			if (m_vecRotTarget.size()) 
			{
				for (auto& pEnemy : m_vecRotTarget)
				{
					if (!pEnemy)
						continue;

					pEnemy->OnDamage(_damageInfo);

					
				}

			}
		}, false);

	return S_OK;
}

HRESULT CKorsica_Wind::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())
	m_wpBodyCollidor = ConvertWPComponent<CCollidor>(Get_Component(L"Com_Collidor_PXBody")).lock();
	if (m_wpBodyCollidor.lock()) {
		m_wpBodyCollidor.lock()->Set_Func(EKEYACTIONSTATE::ENTER, [=](CONTACTMF_DESC _mfDesc, CCollidor* pCollidor) {
			auto _vNormal = XMLoadFloat4(&_mfDesc.contacts[0].vNormal);
			auto _vLook = Get_MainTransform().lock()->Get_WorldState(STATE::LOOK);
			_vNormal = XMVectorSetY(_vNormal, 0.f);
			if (XMVectorGetX(XMVector3Length(_vNormal)) < 0.01f) return;
			_vNormal = XMVector3Normalize(_vNormal);
			if (XMVectorGetX(XMVector3Dot(_vNormal, _vLook)) > 0.0f) {
				Get_MainTransform().lock()->Aim(XMVector3Reflect(_vLook, _vNormal));
				m_bTurn = true;
			}
		});
	}

	m_wpHitCollidor = ConvertWPComponent<CCollidor>(Get_Component(L"Com_Collidor_Atk")).lock();
	if (m_wpHitCollidor.lock()) {
		m_wpHitCollidor.lock()->Set_Func(EKEYACTIONSTATE::ENTER, [=](CONTACTMF_DESC _mfDesc, CCollidor* pCollidor) {
			//Check Enemyw
			//Turn
			m_bTurn = true;
			auto pEnemy = dynamic_cast<CEnemy*>(pCollidor->GetGameObject());
			if (pEnemy && !pEnemy->IsBoss() && m_vecRotTarget.size() <= 2)
			{
				auto iter = std::find(
					m_vecRotTarget.begin(),
					m_vecRotTarget.end(),
					pEnemy
				);

				if (iter == m_vecRotTarget.end())
				{
					m_vecRotTarget.push_back(pEnemy);
					PrintDebug("Korsica Wind Hold Enter!");
				}
			}
		});
	}
	
	m_wpBodyCollidor.lock()->SetActive(false);
	m_wpHitCollidor.lock()->SetActive(false);

	return S_OK;
}

void CKorsica_Wind::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CKorsica_Wind::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	Move(fTimeDelta);
	
}

void CKorsica_Wind::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	EnemyRot();
}

HRESULT CKorsica_Wind::Load_Asset()
{
	CHKFAIL(__super::Load_Asset())

	return S_OK;
}


HRESULT CKorsica_Wind::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents())

	return S_OK;
}



void CKorsica_Wind::Render_Properties()
{
	__super::Render_Properties();

	if (ImGui::DragFloat("Linear Time", &m_desc.m_fLinearTime, 0.1f, 0.01f, m_desc.m_fLifeTime)) {
		m_upLinearTimer->SetDuration(m_desc.m_fLinearTime);
	}
	if (ImGui::DragFloat("Life Time", &m_desc.m_fLifeTime, 0.1f, m_desc.m_fLinearTime, 10.f)) {
		m_upLifeTimer->SetDuration(m_desc.m_fLifeTime);
	}

	ImGui::DragFloat("Speed", &m_desc.m_fSpeed);

	ImGui::Checkbox("bTurn",		&m_bTurn);
	ImGui::Checkbox("bRightTurn", &m_bRightTurn);

	if (ImGui::Button("Shot")) {
		Shot(
			Get_MainTransform().lock()->Get_WorldState(STATE::POSITION),
			Get_MainTransform().lock()->Get_WorldState(STATE::LOOK)
		);
	}
}

void CKorsica_Wind::Shot(_vector vPos, _vector vDir)
{
	SetActive(true);
	m_bTurn = false;
	m_upLinearTimer	->Restart();
	m_upLifeTimer	->Restart();
	Get_MainTransform().lock()->Set_LocalState(STATE::POSITION, vPos);
	Get_MainTransform().lock()->Aim(vDir);
	if (m_pEffect) m_pEffect->Restart();

	if (auto spCollidor = m_wpHitCollidor.lock())
		spCollidor->SetActive(true);
	if (auto spCollidor = m_wpBodyCollidor.lock())
		spCollidor->SetActive(true);
	m_pWindDamageTimer->Restart();
}

void CKorsica_Wind::Stop()
{
	SetActive(false);
	for (auto pEnemy : m_vecRotTarget)
	{
		pEnemy->KorsicaHoldEnd(); 
		PrintDebug("Korsica Wind Hold Exit!");
	}
	m_vecRotTarget.clear();
	m_bTurn = false;
	m_upLinearTimer	->Reset();
	m_upLifeTimer	->Reset();
	if (m_pEffect) m_pEffect->DissolveStop();

	if (auto spCollidor = m_wpHitCollidor.lock())
		spCollidor->SetActive(false);
	if (auto spCollidor = m_wpBodyCollidor.lock())
		spCollidor->SetActive(false);
	m_pWindDamageTimer->SetActive(false);
}

void CKorsica_Wind::Move(float fDeltaTime)
{
	if (!m_upLifeTimer->IsActive()) return;

	m_pEffect->Set_Pos(Get_MainTransform().lock()->Get_WorldState(STATE::POSITION));
	Get_MainTransform().lock()->Translate(0.f, 0.f, m_desc.m_fSpeed * fDeltaTime);
	if (m_bTurn) {
		auto _vLook = Get_MainTransform().lock()->Get_LocalState(STATE::LOOK);
		auto	_matRot = XMMatrixRotationY(
				( m_bRightTurn ? +1.f : -1.f ) * fDeltaTime *
			XM_2PI / max(0.01f, m_desc.m_fLifeTime) );
		auto _vNxtLook= XMVectorSetW(XMVector4Transform(_vLook, _matRot), 0.f);
		Get_MainTransform().lock()->Aim(XMVector3Normalize(XMVectorSetY(_vNxtLook, 0.f)));
	}
}

void CKorsica_Wind::Free()
{
	__super::Free();
}

CGameObject* CKorsica_Wind::Clone(void* pArg)
{
	return CloneBase<CKorsica_Wind>(pArg);
}

HRESULT CKorsica_Wind::Save(void* _pDesc, _uint& _iSize) const
{
	static_assert(is_trivially_copyable_v<FKORSICAWINDDESC>);
	_iSize = sizeof(FKORSICAWINDDESC);
	memcpy(_pDesc, &m_desc, _iSize);
	return S_OK;
}

HRESULT CKorsica_Wind::Load(void* _pDesc)
{
	auto _iSize = sizeof(FKORSICAWINDDESC);
	memcpy(&m_desc, _pDesc, _iSize);
	
	m_upLinearTimer	->SetDuration(m_desc.m_fLinearTime	);
	m_upLifeTimer	->SetDuration(m_desc.m_fLifeTime	);

	return S_OK;
}

void CKorsica_Wind::EnemyRot()
{
	if (m_vecRotTarget.empty()) return;

	_vector vPos = m_wpHitCollidor.lock()->Get_Center();

	for (auto& pEnemy : m_vecRotTarget) 
	{
		if (!pEnemy->IsActive())
			continue;
		pEnemy->SetPos(vPos);
	}	
}
