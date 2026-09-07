#include "EnemyShield.h"
#include "BaseModel.h"
#include "Bone.h"
#include "AnimController.h"
#include "GameInstance.h"
#include "EffectRoot.h"


CEnemyShield::CEnemyShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CEnemyShield::CEnemyShield(const CEnemyShield& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CEnemyShield::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

		m_upVanishTimer = m_pGameInstance->CreateTimerWithDuration(2.f, false, [=]() {SetActive(false); }, false);
    return S_OK;
}

HRESULT CEnemyShield::Late_Initialize()
{
    CHKFAIL(__super::Late_Initialize())

	SetBone();
	CHKFAIL(SetVFX())
	m_vecDebrisInfos.resize(m_vecPartBones.size(), DebrisInstance{});

	Reset();
    return S_OK;
}

void CEnemyShield::SetBone()
{
	for (size_t idx = 0; idx < 8; idx++)
	{
		wstring sTag = L"panel_break_0";
		sTag += to_wstring(idx);
		auto pBone = m_wpModel.lock()->FindBoneWithName(sTag);
		if (pBone)
			m_vecPanelBones.push_back(pBone);

		m_vecDamageStateIndices.push_back(3);
	}
	int iCnt = 0;
	while (iCnt < 3) {
		auto idx = Random(0, 7);
		if (m_vecDamageStateIndices[idx] == 3) {
			m_vecDamageStateIndices[idx] = 2;
			iCnt++;
		}
	}
	iCnt = 0;
	while (iCnt < 3) {
		auto idx = Random(0, 7);
		if (m_vecDamageStateIndices[idx] == 3) {
			m_vecDamageStateIndices[idx] = 1;
			iCnt++;
		}
	}

	for (size_t idx = 0; idx < 6; idx++)
	{
		wstring sTag = L"l_parts_0";
		sTag += to_wstring(idx);
		auto pBone = m_wpModel.lock()->FindBoneWithName(sTag);
		if (pBone)
			m_vecPartBones.push_back(pBone);
	}
	for (size_t idx = 0; idx < 6; idx++)
	{
		wstring sTag = L"r_parts_0";
		sTag += to_wstring(idx);
		auto pBone = m_wpModel.lock()->FindBoneWithName(sTag);
		if (pBone)
			m_vecPartBones.push_back(pBone);
	}

	if (auto pBone = m_wpModel.lock()->FindBoneWithName(L"shield"))
		m_pShieldBone = pBone;
	if (auto pBone = m_wpModel.lock()->FindBoneWithName(L"attach_vfx_break"))
		m_pVFXBone = pBone;
}

void CEnemyShield::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_bExplode) 
		Explode();


	if (m_upVanishTimer->IsActive()) {
		for (size_t i = 0; i < m_vecPartBones.size(); ++i)
		{
			auto& debris = m_vecDebrisInfos[i];

			_vector velocity = XMLoadFloat3(&debris.m_vVelocity);
			_vector angularVel = XMLoadFloat3(&debris.m_vAngularVelocity);

			velocity += XMVectorSet(0.f, m_shieldDesc.m_fGravity, 0.f, 0.f) * fTimeDelta;

			float damp = expf(m_shieldDesc.m_fSpeedDamp * fTimeDelta);
			float angularDamp = expf(m_shieldDesc.m_fAngularDamp * fTimeDelta);

			velocity *= damp;
			angularVel *= angularDamp;


			_matrix rotMat		= XMMatrixIdentity();
			_matrix transMat	= XMMatrixIdentity();

			float angSpeed = XMVectorGetX(XMVector3Length(angularVel));
			if (angSpeed > 0.01f)
			{
				_vector axis = XMVector3Normalize(angularVel);

				float angle = angSpeed * fTimeDelta;

				_vector dq = XMQuaternionRotationAxis(axis, angle);

				rotMat = XMMatrixRotationAxis(axis, angle);
			}
			transMat = XMMatrixTranslationFromVector(velocity * fTimeDelta);

			_matrix _matBone = m_vecPartBones[i]->GetConstraintMatrix();

			_matBone = rotMat * _matBone;
			_matBone.r[3] += velocity * fTimeDelta;
			m_vecPartBones[i]->SetConstraintMatrix(_matBone);

			XMStoreFloat3(&debris.m_vVelocity, velocity);
			XMStoreFloat3(&debris.m_vAngularVelocity, angularVel);

		}
	}
}

void CEnemyShield::Render_Properties()
{
	__super::Render_Properties();

	if (ImGui::Button("Damage")) {
		Damage();
	}
	ImGui::SameLine();
	if (ImGui::Button("Hit")) {
		Hit();
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		Reset();
	}

	ImGui::BeginChild("Wall", ImVec2(0, 230), true);

	ImGui::DragFloat("Gravity", &m_shieldDesc.m_fGravity);
	ImGui::DragFloat("Damp Speed", &m_shieldDesc.m_fSpeedDamp);
	ImGui::DragFloat("Damp Angular", &m_shieldDesc.m_fAngularDamp);
	ImGui::DragFloat("Sleep Threshold", &m_shieldDesc.m_fSleepThreshold);
	ImGui::DragFloat("Life Time", &m_shieldDesc.m_fLifeTime);
	ImGui::DragFloat("Impulse Strangth", &m_shieldDesc.m_fImpulseStrength);
	ImGui::DragFloat("Impulse Falloff", &m_shieldDesc.m_fImpulseFalloff);
	ImGui::DragFloat("Directional bias", &m_shieldDesc.m_fDirectionalBias);

	ImGui::DragFloat2("Range Speed", reinterpret_cast<_float*>(&m_shieldDesc.m_vInitialSpeedRange));
	ImGui::DragFloat2("Range Angular", reinterpret_cast<_float*>(&m_shieldDesc.m_vAngularSpeedRange));


	ImGui::DragFloat("Linear  Resistitution", &m_shieldDesc.m_fSpeedrestitution);
	ImGui::DragFloat("Angular Resistitution", &m_shieldDesc.m_fAngularrestitution);
	ImGui::DragFloat("Linear	Friction", &m_shieldDesc.m_fFriction);
	ImGui::DragFloat("Angular Friction", &m_shieldDesc.m_fAngularFriction);

	ImGui::EndChild();


}

CGameObject* CEnemyShield::Clone(void* pArg)
{
	
    return CloneBase<CEnemyShield>(pArg);
}

void CEnemyShield::Damage()
{
	if (iDamageState >= 4) return;
	else {
		iDamageState++;
		if (iDamageState == 4) {
			Destroy();
		}
		for (size_t i = 0; i < 8; i++)
		{
			m_vecPanelBones[i]->SetRemove(m_vecDamageStateIndices[i] <= iDamageState);
		}
		//Particle
		PlayVFX(ESV_DEBRIS, m_pVFXBone, false, false);
	}
}

void CEnemyShield::Hit()
{
	//Noise
}

HRESULT CEnemyShield::Reset()
{
	CHKFAIL(__super::Reset())
	SetActive(true);
	iDamageState = 0;
	for (size_t i = 0; i < 8; i++)
	{
		m_vecPanelBones[i]->SetRemove(m_vecDamageStateIndices[i] <= iDamageState);
	}
	for (auto pPartBone : m_vecPartBones)
	{
		pPartBone->SetRemove(true);
	}
	m_pShieldBone->SetRemove(false);
	m_upVanishTimer->Reset();


	for (auto& pInfo : m_vecDebrisInfos)
	{
		pInfo.m_vVelocity = { 0.f, 0.f, 0.f };
		pInfo.m_vAngularVelocity = { 0.f, 0.f, 0.f };
		pInfo.m_bPosLock = false;
	}
	for (auto pPartBone : m_vecPartBones)
	{
		pPartBone->SetRemoveConstraint(false);
	}
	m_bExplode = false;

	return S_OK;
}

void CEnemyShield::Destroy()
{
	for (auto pPartBone : m_vecPartBones)
	{
		pPartBone->SetRemove(false);
	}
	m_pShieldBone->SetRemove(true);
	m_upVanishTimer->Restart();
	m_bExplode = true;

	//SetActive(false);
}

HRESULT CEnemyShield::SetVFX()
{
	m_vecEffects.resize(EENEMYSHIELDVFX::ESV_END);

	CHKFAILMSG(
		LoadVFX(
			ESV_DEBRIS,
			L"VFX_Kale_em7510_BreakBarrier"),
		"Fail to load VFX : Hit Debris"
	);

	return S_OK;
}

void CEnemyShield::Explode()
{
	m_bExplode = false;

	auto _triggerDesc = m_pVFXBone->GetDesc();
	_vector vTriggerPos = XMLoadFloat4(
		reinterpret_cast<_float4*>(_triggerDesc.m_CombinedTransformationMatrix.m[3]));

	for (auto pPartBone : m_vecPartBones)
	{
		pPartBone->SetRemoveConstraint(true);
	}

	for (size_t i = 0; i < m_vecPartBones.size(); i++)
	{
		auto& debris = m_vecDebrisInfos[i];

		auto _debrisDesc = m_vecPartBones[i]->GetDesc();
		_vector vDebrisPos = XMLoadFloat4(
			reinterpret_cast<_float4*>(_debrisDesc.m_CombinedTransformationMatrix.m[3]));
		_vector vToDebris	= vDebrisPos - vTriggerPos;

		float dist = XMVectorGetX(XMVector3Length(vToDebris));
		_vector dir = XMVector3Normalize(vToDebris);

		float falloff = 1.0f / (1.0f + dist * m_shieldDesc.m_fImpulseFalloff);
		_vector randVec = XMVector3Normalize(XMVectorSet(
			Random(-1.f, 1.f),
			Random(0.f, 1.f),
			Random(-1.f, 1.f),
			0.f));

		float bias = m_shieldDesc.m_fDirectionalBias;
		_vector finalDir = XMVector3Normalize(
			dir * bias + randVec * (1.f - bias)
		);

		float speed = Random(
			m_shieldDesc.m_vInitialSpeedRange.x,
			m_shieldDesc.m_vInitialSpeedRange.y
		);
		speed *= m_shieldDesc.m_fImpulseStrength * falloff;

		_vector velocity = finalDir * speed;
		XMStoreFloat3(&debris.m_vVelocity, velocity);

		//AngVel
		_vector randAxis = XMVector3Normalize(XMVectorSet(
			Random(-1.f, 1.f),
			Random(-1.f, 1.f),
			Random(-1.f, 1.f),
			0.f));

		float angularSpeed = Random(
			m_shieldDesc.m_vAngularSpeedRange.x,
			m_shieldDesc.m_vAngularSpeedRange.y
		);

		_vector angularVel = randAxis * angularSpeed;
		XMStoreFloat3(&debris.m_vAngularVelocity, angularVel);
	}
}


HRESULT CEnemyShield::LoadVFX(int iEffectIdx, wstring sPrototypeTag)
{
	if (iEffectIdx >= m_vecEffects.size()) return E_FAIL;
	CGameObject::LoadPrototype(
		sPrototypeTag,
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_vecEffects[iEffectIdx])
	);
	if (!m_vecEffects[iEffectIdx]) return E_FAIL;

	m_vecEffects[iEffectIdx]->Stop();
	m_vecEffects[iEffectIdx]->SetLevelObject();
	return S_OK;
}

void CEnemyShield::PlayVFX(int iEffectIdx, _vector vPos)
{
	if (iEffectIdx >= m_vecEffects.size()) return;

	if (m_vecEffects[iEffectIdx]) {
		m_vecEffects[iEffectIdx]->Set_Pos(vPos);
		m_vecEffects[iEffectIdx]->Restart();
	}
}

void CEnemyShield::PlayVFX(int iEffectIdx, _vector vPos, _vector vDir)
{
	if (iEffectIdx >= m_vecEffects.size()) return;

	if (m_vecEffects[iEffectIdx]) {
		m_vecEffects[iEffectIdx]->Set_Pos(vPos);
		m_vecEffects[iEffectIdx]->Set_Dir(vDir);
		m_vecEffects[iEffectIdx]->Restart();
	}
}

void CEnemyShield::PlayVFX(int iEffectIdx, _vector vDir, CBone* pBone)
{
	if (iEffectIdx >= m_vecEffects.size()) return;

	if (m_vecEffects[iEffectIdx] && pBone) {
		auto _desc = pBone->GetDesc();
		auto _vPos = XMLoadFloat4(reinterpret_cast<_float4*>(&_desc.m_CombinedTransformationMatrix.m[3]));
		m_vecEffects[iEffectIdx]->Set_Dir(vDir);
		m_vecEffects[iEffectIdx]->Restart();
	}
}

void CEnemyShield::PlayVFX(int iEffectIdx, CBone* pBone, bool bTrack, bool bUseDir)
{
	if (iEffectIdx >= m_vecEffects.size()) return;

	if (m_vecEffects[iEffectIdx] && pBone) {
		if (bTrack) {
			m_vecEffects[iEffectIdx]->Set_Bone(pBone);
		}
		else {
			auto _desc = pBone->GetDesc();
			auto _vPos = XMLoadFloat4(reinterpret_cast<_float4*>(&_desc.m_CombinedTransformationMatrix.m[3]));
			m_vecEffects[iEffectIdx]->Set_Pos(_vPos);
			if (bUseDir) {
				auto _vDir = XMLoadFloat4(reinterpret_cast<_float4*>(&_desc.m_CombinedTransformationMatrix.m[0]));
				m_vecEffects[iEffectIdx]->Set_Dir(_vDir);
			}
		}
		m_vecEffects[iEffectIdx]->Restart();
	}
}

HRESULT CEnemyShield::Save(void* _pDesc, _uint& _iSize) const
{
	static_assert(is_trivially_copyable_v<FWEAPONSHIELDDESC>);
	_iSize = sizeof(FWEAPONSHIELDDESC);
	FWEAPONSHIELDDESC _desc = m_shieldDesc;
	static_cast<CWeapon::FWEAPONDESC&>(_desc) = m_desc;
	memcpy(_pDesc, &_desc, _iSize);
	return S_OK;
}

HRESULT CEnemyShield::Load(void* _pDesc)
{
	m_shieldDesc = *static_cast<FWEAPONSHIELDDESC*>(_pDesc);
	m_desc = m_shieldDesc;

	return S_OK;
}
