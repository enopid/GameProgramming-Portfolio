#include "Interact_Macaron_Wall.h"
#include "BaseModel.h"
#include "GameInstance.h"
#include "AnimController.h"
#include "EffectRoot.h"

CGameObject* CInteract_Macaron_Wall::Clone(void* pArg)
{
	return CloneBase<CInteract_Macaron_Wall>(pArg);
}

HRESULT CInteract_Macaron_Wall::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents());
	return S_OK;
}

HRESULT CInteract_Macaron_Wall::Save(void* _pDesc, _uint& _iSize) const
{
	static_assert(is_trivially_copyable_v<FINTERACTWALLDESC>);
	_iSize = sizeof(FINTERACTWALLDESC);
	FINTERACTWALLDESC _desc = m_wallDesc;
	static_cast<CInteractObject::FINTERACTOBJECTDESC&>(_desc) = m_desc;
	memcpy(_pDesc, &_desc, _iSize);
	return S_OK;
}

HRESULT CInteract_Macaron_Wall::Load(void* _pDesc)
{
	m_wallDesc	= *static_cast<FINTERACTWALLDESC*>(_pDesc);
	m_desc = m_wallDesc;

	m_upDeactiveTimer->SetDuration(m_desc.m_fDeactiveTime);
	m_eCurState = m_ePrevState = ESTATE(m_desc.m_iInitState);
	return S_OK;
}

CInteract_Macaron_Wall::CInteract_Macaron_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:	CInteract_RhythmChallenge(pDevice, pContext)
	
{
	m_eInteractType = EInteractType::IT_MACARON_WALL;
}

CInteract_Macaron_Wall::CInteract_Macaron_Wall(const CInteract_Macaron_Wall& Prototype)
	:	CInteract_RhythmChallenge(Prototype),
		m_wallDesc(Prototype.m_wallDesc)
{
	m_eInteractType = EInteractType::IT_MACARON_WALL;
}

HRESULT CInteract_Macaron_Wall::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
		return S_OK;
}

HRESULT CInteract_Macaron_Wall::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	return S_OK;
}

HRESULT CInteract_Macaron_Wall::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())

	m_pUnbrokenWall = Get_ChildObject(L"GimmicWallA0");
	m_pBrokenWall	= Get_ChildObject(L"GimmicWallA1");
	m_pTriggerPoint = Get_ChildObject(L"Trigger_Point");
	
	if (m_pBrokenWall) {
		auto _lstDebris = m_pBrokenWall->GetChildrens();
		m_vecDebris = vector<CGameObject*>(_lstDebris.begin(), _lstDebris.end());
	}
	for (auto& pObj : m_vecDebris) {
		DebrisInstance tmpInfo = {};
		XMStoreFloat4x4(&tmpInfo.m_Initmat, pObj->Get_MainTransform().lock()->GetLocalMatrix());
		
		m_vecDebrisInfos.push_back(tmpInfo);
	}
	m_iNumDebris = (Engine::_uint)m_vecDebris.size();

	AddStateFunc(ESTATE::TRIGGER, EKEYACTIONSTATE::ENTER, [=](float) {
		m_pUnbrokenWall	->SetActive(false, true);
		m_pBrokenWall	->SetActive(true, true);
		for (auto& pObj : m_vecDebris) {
			pObj->Get_MainTransform().lock()->Set_Parent(nullptr, false);
		}

		Explode();
	});

	AddStateFunc(ESTATE::TRIGGER, EKEYACTIONSTATE::EXIT, [=](float) {
		m_pUnbrokenWall	->SetActive(true, true);
		m_pBrokenWall	->SetActive(false, true);
		for (auto& pObj : m_vecDebris)
		{
			pObj->Get_MainTransform().lock()->Set_Parent(m_pBrokenWall, false);
		}

		ResetInfo();

	});


	//SetCollidor
	for (size_t i = 0; i < m_iNumDebris; i++)
	{
		auto pObj		= m_vecDebris[i];
		if (auto spComp = pObj->Get_Component<CCollidor>().lock()) {
			auto spCol = ConvertWPComponent<CCollidor>(spComp).lock();
			spCol->Set_Func(EKEYACTIONSTATE::ENTER, [=](CONTACTMF_DESC CTDESC, auto pCollidor) {
				if (CTDESC.contactCount <= 0) return;
				auto& contact	= CTDESC.contacts[0];
				auto& info		= m_vecDebrisInfos[i];

				_vector vNormal = -XMLoadFloat4(&contact.vNormal);
				_vector vPos	= XMLoadFloat4(&contact.vPosition);
				_vector vVel	= XMLoadFloat3(&info.m_vVelocity);

				// 1. 속도가 표면으로 들어가는 경우만 처리
				float vDot = XMVectorGetX(XMVector3Dot(vVel, vNormal));
				if (vDot >= 0.f) return;

				// 2. 반사 (간단한 restitution 포함)
				_vector vReflect = vVel - (1.f + m_wallDesc.m_fSpeedrestitution) * vDot * vNormal;

				// 3. 마찰 느낌 (탱젠트 감쇠)
				_vector vTangent = vReflect - XMVectorGetX(XMVector3Dot(vReflect, vNormal)) * vNormal;
				vReflect -= vTangent * m_wallDesc.m_fFriction;

				float vLen = XMVectorGetX(XMVector3Length(vReflect));
				if (vLen < m_wallDesc.m_fSleepThreshold && XMVectorGetY(vNormal) >=0.9f) {
					vReflect = XMVectorZero();
					info.m_bPosLock = true;
				}

				XMStoreFloat3(&info.m_vVelocity, vReflect);

				//_vector vCOM = pObj->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
				//_vector r = vPos - vCOM;

				//// 임펄스 (반사에서 쓴 값 재활용 가능)
				//_vector vImpulse = -(1.f + m_wallDesc.m_fAngularrestitution) * vDot * vNormal;

				//// 올바른 회전
				//_vector vAngular = XMVector3Cross(r, vImpulse) / m_wallDesc.m_fAngularFriction;

				//XMStoreFloat3(&info.m_vAngularVelocity, vAngular);

			});
		}
	}

	m_pUnbrokenWall->SetActive(false, true);
	m_pBrokenWall->SetActive(true, true);
	for (auto& pObj : m_vecDebris) {
		pObj->Get_MainTransform().lock()->Set_Parent(nullptr, false);
	}
	m_pUnbrokenWall	->SetActive(true,	true);
	m_pBrokenWall	->SetActive(false,	true);
	for (auto& pObj : m_vecDebris)
	{
		pObj->Get_MainTransform().lock()->Set_Parent(m_pBrokenWall, false);
	}
	ResetInfo();

	return S_OK;
}

void CInteract_Macaron_Wall::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInteract_Macaron_Wall::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (!m_bForceReset) {
		if		(m_eCurState == ESTATE::ACTIVE)		Trigger();
		else if (m_eCurState == ESTATE::TRIGGER)	Deactivate();
		else if (m_eCurState == ESTATE::DEACTIVE) {
			Activate();
			m_bForceReset = true;
			//나의 패배를 인정한다
			//쓰레기같은 코드를 치게 만든 너의 승리다  
		}
	}

	if (m_eCurState == ESTATE::TRIGGER) {
		for (size_t i = 0; i < m_iNumDebris; ++i)
		{
			auto& debris = m_vecDebrisInfos[i];

			if (debris.m_fLifeTIme >= m_wallDesc.m_fLifeTime) {
				m_vecDebris[i]->SetActive(false);
				continue;
			}
			debris.m_fLifeTIme += fTimeDelta;

			auto transform = m_vecDebris[i]->Get_MainTransform().lock();

			_vector pos = transform->Get_WorldState(STATE::POSITION);

			_vector velocity	= XMLoadFloat3(&debris.m_vVelocity);
			_vector angularVel	= XMLoadFloat3(&debris.m_vAngularVelocity);

			velocity += XMVectorSet(0.f, m_wallDesc.m_fGravity, 0.f, 0.f) * fTimeDelta;

			float damp			= expf(m_wallDesc.m_fSpeedDamp	* fTimeDelta);
			float angularDamp	= expf(m_wallDesc.m_fAngularDamp* fTimeDelta);

			velocity	*= damp;
			angularVel	*= angularDamp;

			pos			+= velocity * fTimeDelta;

			float angSpeed = XMVectorGetX(XMVector3Length(angularVel));

			if (angSpeed > 0.01f)
			{
				_vector axis = XMVector3Normalize(angularVel);

				float angle = angSpeed * fTimeDelta;

				_vector dq = XMQuaternionRotationAxis(axis, angle);

				transform->Turn(axis, angle);
			}

			/*
			

			float wLen = XMVectorGetX(XMVector3Length(angularVel));
			if (wLen < m_wallDesc.m_fSleepThreshold) {
				angularVel	= XMVectorZero();
			}
			*/

			XMStoreFloat3(&debris.m_vVelocity,		velocity);
			XMStoreFloat3(&debris.m_vAngularVelocity, angularVel);

			if (!debris.m_bPosLock)
				transform->Set_LocalState(STATE::POSITION, pos);
		}

	}
}

void CInteract_Macaron_Wall::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CInteract_Macaron_Wall::Render_Properties()
{
	__super::Render_Properties();
	ImGui::BeginChild("Wall", ImVec2(0, 230), true);

	ImGui::DragFloat("Gravity", &m_wallDesc.m_fGravity);
	ImGui::DragFloat("Damp Speed", &m_wallDesc.m_fSpeedDamp);
	ImGui::DragFloat("Damp Angular", &m_wallDesc.m_fAngularDamp);
	ImGui::DragFloat("Sleep Threshold", &m_wallDesc.m_fSleepThreshold);
	ImGui::DragFloat("Life Time", &m_wallDesc.m_fLifeTime);
	ImGui::DragFloat("Impulse Strangth", &m_wallDesc.m_fImpulseStrength);
	ImGui::DragFloat("Impulse Falloff", &m_wallDesc.m_fImpulseFalloff);
	ImGui::DragFloat("Directional bias", &m_wallDesc.m_fDirectionalBias);
	
	ImGui::DragFloat2("Range Speed",	reinterpret_cast<_float*>(&m_wallDesc.m_vInitialSpeedRange));
	ImGui::DragFloat2("Range Angular",	reinterpret_cast<_float*>(&m_wallDesc.m_vAngularSpeedRange));


	ImGui::DragFloat("Linear  Resistitution", &m_wallDesc.m_fSpeedrestitution);
	ImGui::DragFloat("Angular Resistitution", &m_wallDesc.m_fAngularrestitution);
	ImGui::DragFloat("Linear	Friction", &m_wallDesc.m_fFriction);
	ImGui::DragFloat("Angular Friction", &m_wallDesc.m_fAngularFriction);

	ImGui::EndChild();
}

void CInteract_Macaron_Wall::Explode()
{
	_vector vTriggerPos = m_pTriggerPoint->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);

	m_pGameInstance->Play_SFX(L"MacaronWallBreak");

	for (size_t i = 0; i < m_iNumDebris; i++)
	{
		auto& debris = m_vecDebrisInfos[i];

		_vector vDebrisPos = m_vecDebris[i]->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
		_vector vToDebris = vDebrisPos - vTriggerPos;

		float dist = XMVectorGetX(XMVector3Length(vToDebris));
		_vector dir = XMVector3Normalize(vToDebris);

		float falloff = 1.0f / (1.0f + dist * m_wallDesc.m_fImpulseFalloff);
		_vector randVec = XMVector3Normalize(XMVectorSet(
			Random(-1.f, 1.f),
			Random(0.f, 1.f), 
			Random(-1.f, 1.f),
			0.f));

		float bias = m_wallDesc.m_fDirectionalBias;
		_vector finalDir = XMVector3Normalize(
			dir * bias + randVec * (1.f - bias)
		);

		float speed = Random(
			m_wallDesc.m_vInitialSpeedRange.x,
			m_wallDesc.m_vInitialSpeedRange.y
		);
		speed *= m_wallDesc.m_fImpulseStrength * falloff;

		_vector velocity = finalDir * speed;
		XMStoreFloat3(&debris.m_vVelocity, velocity);

		//AngVel
		_vector randAxis = XMVector3Normalize(XMVectorSet(
			Random(-1.f, 1.f),
			Random(-1.f, 1.f),
			Random(-1.f, 1.f),
			0.f));

		float angularSpeed = Random(
			m_wallDesc.m_vAngularSpeedRange.x,
			m_wallDesc.m_vAngularSpeedRange.y
		);

		_vector angularVel = randAxis * angularSpeed;
		XMStoreFloat3(&debris.m_vAngularVelocity, angularVel);

		debris.m_fLifeTIme = 0.f;
	}
}

void CInteract_Macaron_Wall::ResetInfo()
{
	int idx = 0;
	for (auto& pObj : m_vecDebris)
	{
		m_vecDebrisInfos[idx].m_vVelocity			= { 0.f, 0.f, 0.f };
		m_vecDebrisInfos[idx].m_vAngularVelocity	= { 0.f, 0.f, 0.f };
		m_vecDebrisInfos[idx].m_fLifeTIme			= 0.f;
		m_vecDebrisInfos[idx].m_bPosLock			= false;
		pObj->Get_MainTransform().lock()->Copy_LocalMatrix(XMLoadFloat4x4(&(m_vecDebrisInfos[idx++].m_Initmat)));
	}
}

void CInteract_Macaron_Wall::Free()
{
	__super::Free();
}
