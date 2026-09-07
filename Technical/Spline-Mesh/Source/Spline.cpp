#include "Spline.h"
#include "GameInstance.h"
#include "BaseModel.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "MagnetRail.h"

CSpline::CSpline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	
}

CSpline::CSpline(const CSpline& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CSpline::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
	return S_OK;
}

HRESULT CSpline::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	CGameObject::LoadPrototype(
		L"MagnetRail",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pMagnetRail)
	);
	m_pMagnetRail->SetLevelObject();
	m_pMagnetRail->SetSpline(this);

	return S_OK;
}

HRESULT CSpline::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())
	CreatePath();

	return S_OK;
}

void CSpline::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSpline::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	
	if (m_bRecalc)
		CalcSplineDesc();

	if (m_bRebakeRenderer)
		RebakeRenderer();

	if (BSELFDESTRUCTSEQUENCE) {
		Destroy();
	}
}

void CSpline::CalcSplineDesc()
{
	int _idx = 0;
	for (auto& _desc : m_vecSplineDescs) {
		_desc.g_vOffset.w = m_desc.m_fSegmentInterval;
		memcpy(&_desc.g_vOffset, &m_desc.m_vSegmentOffset, sizeof(_float3));

		_float _fLength = _idx * m_desc.m_fSegmentInterval;
		for (size_t i = 0; i < NUMSPLINESAMPLE-1; i++)
		{
			XMStoreFloat4(&_desc.g_vPos[i], GetPos(_fLength));
			XMStoreFloat4(&_desc.g_vTangent[i], GetDir(_fLength));

			_fLength += m_desc.m_fSegmentInterval / (NUMSPLINESAMPLE - 1);
		}
		_fLength = (_idx + 1) * m_desc.m_fSegmentInterval;
		XMStoreFloat4(&_desc.g_vPos[NUMSPLINESAMPLE-1], GetPos(_fLength));
		XMStoreFloat4(&_desc.g_vTangent[NUMSPLINESAMPLE-1], GetDir(_fLength));

		m_vecSplineSegments[_idx]->Get_MainTransform().lock()
			->Set_LocalState(STATE::POSITION,
				XMVectorSetW(XMLoadFloat4(&_desc.g_vPos[NUMSPLINESAMPLE / 2]), 1.f)
			);
		
		_idx++;
	}
	m_bRecalc = false;
}

void CSpline::RebakeRenderer()
{
	for (size_t i = 0; i < m_vecSplineSegments.size(); i++)
	{
		auto _pObject = m_vecSplineSegments[i];
		if (auto spModel = ConvertWPComponent<CModel>(_pObject->Get_Component(L"Com_Model_Tunnel")).lock()) {
			_int iNumMesh = spModel->Get_NumMeshes();
			for (size_t j = 0; j < iNumMesh; j++)
			{
				if (auto spMeshrenderer = spModel->GetMeshRenderer((Engine::_int)j)) {
					m_pGameInstance->Change_RenderGroup(RENDERGROUP::NONBLEND_OBJECT, spMeshrenderer);
					spMeshrenderer->GetMesh().lock()->SetShaderIdx(9);
					
					CRenderer::SHADERPARAM_DESC _desc;

					_desc.eParamType = CRenderer::EPARAMTYPE::PT_RAWDATA;
					_desc.pData = &m_vecSplineDescs[i].g_vOffset;
					_desc.iSize = sizeof(m_vecSplineDescs[i].g_vOffset);
					_desc.sConstantName = "g_vSplineOffset";
					spMeshrenderer->Add_Parameters(_desc);

					_desc.pData = m_vecSplineDescs[i].g_vTangent;
					_desc.iSize = sizeof(m_vecSplineDescs[i].g_vTangent);
					_desc.sConstantName = "g_vSplineTanget";
					spMeshrenderer->Add_Parameters(_desc);

					_desc.pData = m_vecSplineDescs[i].g_vPos;
					_desc.iSize = sizeof(m_vecSplineDescs[i].g_vPos);
					_desc.sConstantName = "g_vSplinePos";
					spMeshrenderer->Add_Parameters(_desc);

					m_bRebakeRenderer = false;
				}
			}
		}
		if (auto spModel = ConvertWPComponent<CModel>(_pObject->Get_Component(L"Com_Model_Rail")).lock()) {
			_int iNumMesh = spModel->Get_NumMeshes();
			for (size_t j = 0; j < iNumMesh; j++)
			{
				if (auto spMeshrenderer = spModel->GetMeshRenderer((Engine::_int)j)) {
					m_pGameInstance->Change_RenderGroup(RENDERGROUP::NONBLEND_OBJECT, spMeshrenderer);
					spMeshrenderer->GetMesh().lock()->SetShaderIdx(9);
					
					CRenderer::SHADERPARAM_DESC _desc;

					_desc.eParamType = CRenderer::EPARAMTYPE::PT_RAWDATA;
					_desc.pData = &m_vRailOffset;
					_desc.iSize = sizeof(m_vecSplineDescs[i].g_vOffset);
					_desc.sConstantName = "g_vSplineOffset";
					spMeshrenderer->Add_Parameters(_desc);

					_desc.pData = m_vecSplineDescs[i].g_vTangent;
					_desc.iSize = sizeof(m_vecSplineDescs[i].g_vTangent);
					_desc.sConstantName = "g_vSplineTanget";
					spMeshrenderer->Add_Parameters(_desc);

					_desc.pData = m_vecSplineDescs[i].g_vPos;
					_desc.iSize = sizeof(m_vecSplineDescs[i].g_vPos);
					_desc.sConstantName = "g_vSplinePos";
					spMeshrenderer->Add_Parameters(_desc);

					m_bRebakeRenderer = false;
				}
			}
		}
	}
}

void CSpline::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_vRailOffset.w = m_desc.m_fSegmentInterval;
}

HRESULT CSpline::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents())

	return S_OK;
}

void CSpline::Render_Properties()
{
	__super::Render_Properties();
	static bool bAutoRebake = true;
	ImGui::PushItemWidth(100);
	if (ImGui::DragInt("iNumSmaple", &m_desc.m_iSampleCnt, 1.f, 1, 100)) {
		CreatePath();
	}
	ImGui::DragInt("iNumSegments",		&m_desc.m_iSegmentCnt, 1.f, 1, 100);
	ImGui::DragFloat3("SegmentOffset",		reinterpret_cast<_float*>(&m_desc.m_vSegmentOffset),	0.01f);
	ImGui::DragFloat("SegmentInterval",	reinterpret_cast<_float*>(&m_desc.m_fSegmentInterval),	0.01f);
	ImGui::Checkbox("IncludeTunnel", &m_desc.m_bIncludeTunnel);
	if (ImGui::Button("Rebake Path")) {
		CreatePath();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Auto", &bAutoRebake);
	if (bAutoRebake) {
		CreatePath();
	}
	if (ImGui::Button("Rebake Model")) {
		RebakeModel();
	}
	//Obstacles
	ImGui::Separator();

	if (ImGui::Button("Refrresh Obstacle")) {
		RebakeObstcle();
	}

	if (ImGui::Button("Add Obstacle")) {
		if (m_desc.m_iNumObstacles <= 20 - 1) {
			m_desc.m_arrObstacles[m_desc.m_iNumObstacles++] = RAILOBSTACLEDESC{};
		}
	}

	int iDeleteSeqIdx = -1;
	for (size_t i = 0; i < m_desc.m_iNumObstacles; i++)
	{
		ImGui::PushID((Engine::_int)i);
		string sTag = "# " + to_string(i) + " : ";
		ImGui::Text(sTag.c_str());
		ImGui::SameLine();
		ImGui::DragInt("Type : ", &m_desc.m_arrObstacles[i].iObtacleType, 0.f, 0, 2);
		ImGui::SameLine();
		ImGui::DragFloat("Length : ", &m_desc.m_arrObstacles[i].fRailLength, 0.f, 0.f, 1000.f);
		

		if (ImGui::Button("Delete")) {
			iDeleteSeqIdx = (Engine::_int)i;
		}
		ImGui::PopID();
	}
	if (iDeleteSeqIdx != -1) {
		m_desc.m_iNumObstacles--;
		for (_int i = iDeleteSeqIdx; i < 20 - 1; i++)
			m_desc.m_arrObstacles[i] = m_desc.m_arrObstacles[i + 1];
	}

	ImGui::PopItemWidth();

	//Debug
	static const int	DEBUGSAMPLE = 100;
	static float		DEBUGLENGTH = 0.f;
	DEBUGLINE_DESC _desc;
	_desc.eColor[0] = Colors::White;
	for (int i = 0; i < DEBUGSAMPLE; i++)
	{
		auto _vSrt = GetPos(m_fMaxLength * (i + 0) / DEBUGSAMPLE);
		auto _vDst = GetPos(m_fMaxLength * (i + 1) / DEBUGSAMPLE);


		XMStoreFloat3(&_desc.vSrt, _vSrt);
		XMStoreFloat3(&_desc.vDst, _vDst);
		m_pGameInstance->AddDebugRenderCall(&_desc);
	}
	ImGui::SliderFloat("Length", &DEBUGLENGTH, 0.f, m_fMaxLength);

	DEBUGPOINT_DESC _ptDesc;
	_ptDesc.fSize = 0.3f;
	_ptDesc.eColor[0] = Colors::Red;
	for (size_t i = 0; i < m_vecPathPoints.size(); i++)
	{
		memcpy(&_ptDesc.vLocalMatrix.m[3], &m_vecPathPoints[i], sizeof(_float3));
		m_pGameInstance->AddDebugRenderCall(&_ptDesc);
	}

	_ptDesc.eColor[0] = Colors::Yellow;
	auto _vPos = GetPos(DEBUGLENGTH);
	XMStoreFloat3(reinterpret_cast<_float3*>(_ptDesc.vLocalMatrix.m[3]), _vPos);
	m_pGameInstance->AddDebugRenderCall(&_ptDesc);
}

void CSpline::CreatePath()
{
	m_bRecalc = true;

	//트랜스폼 컴포넌트의 형태로 관리하는 제어점의 위치를 저장하는 벡터
	m_vecPathPoints.clear();
	auto _vecTransforms = Get_Components<CTransform>();
	for (size_t i = 1; i < _vecTransforms.size(); i++) {
		_float4 tmpPos;
		ConvertWPComponent<CTransform>(_vecTransforms[i]).lock()->Set_Parent(this, true);
		XMStoreFloat4(&tmpPos, ConvertWPComponent<CTransform>(_vecTransforms[i]).lock()->Get_WorldState(STATE::POSITION));
		m_vecPathPoints.push_back(tmpPos);
	}
	if (m_vecPathPoints.empty()) return;

	//제어점을 CatmullRom으로 보간한 곡선 기준으로 구간마다 특정 개수만큼 샘플링하여 전체 경로 상에서의 (진행 비율, 진행 거리)를 관리하는 벡터
	m_vecSamples.clear();
	auto _vPrevPos = XMLoadFloat4(&m_vecPathPoints[0]);
	_float fAccRatio(0.f), fAccLength(0.f);
	for (_int seg = 0; seg < m_vecPathPoints.size() - 1; seg++)
	{
		fAccRatio = (_float)seg;
		auto P0 = XMLoadFloat4(&m_vecPathPoints[clamp(seg - 1, 0, int(m_vecPathPoints.size()) - 1)]);
		auto P1 = XMLoadFloat4(&m_vecPathPoints[clamp(seg + 0, 0, int(m_vecPathPoints.size()) - 1)]);
		auto P2 = XMLoadFloat4(&m_vecPathPoints[clamp(seg + 1, 0, int(m_vecPathPoints.size()) - 1)]);
		auto P3 = XMLoadFloat4(&m_vecPathPoints[clamp(seg + 2, 0, int(m_vecPathPoints.size()) - 1)]);

		fAccLength += XMVectorGetX(XMVector3Length(P1 - _vPrevPos));
		m_vecSamples.push_back({ fAccRatio, fAccLength });
		fAccRatio += 1.f / m_desc.m_iSampleCnt;
		_vPrevPos = P1;
		//두 제어점 사이의 공간을 m_desc.m_iSampleCnt만큼 샘플링을 시행
		for (int i = 0; i < m_desc.m_iSampleCnt; i++)
		{
			auto _vPos = XMVectorCatmullRom( P0, P1, P2, P3, (i + 1.f) / m_desc.m_iSampleCnt);
			fAccLength += XMVectorGetX(XMVector3Length(_vPos - _vPrevPos));
			m_vecSamples.push_back({ fAccRatio, fAccLength });
			fAccRatio  += 1.f / m_desc.m_iSampleCnt;
			_vPrevPos = _vPos;
		}
	}
	m_fMaxLength = fAccLength;

	for (size_t i = 0; i < m_desc.m_iNumObstacles; i++)
	{
		auto _fLength = m_desc.m_arrObstacles[i].fRailLength;
		if (i >= m_vecObstacles.size() || !m_vecObstacles[i]) return;

		m_vecObstacles[i]->Get_MainTransform().lock()->Set_LocalState(STATE::POSITION, GetPos(_fLength));
		m_vecObstacles[i]->Get_MainTransform().lock()->Aim(GetDir(_fLength));
	}
}

_vector CSpline::GetPos(_float fLength)
{
	_vector _vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	auto it = std::upper_bound(m_vecSamples.begin(), m_vecSamples.end(), fLength,
		[](float length, const auto& sample) {return length < sample.second; }
	);

	if (it != m_vecSamples.begin())  --it;
	_int _iSampleIdx = static_cast<_int>(it - m_vecSamples.begin());

	if (m_vecPathPoints.size() >= 2) {

		if (m_vecSamples.size() <= _iSampleIdx + 1) { _vPos = XMLoadFloat4(&m_vecPathPoints.back()); }
		else {
			auto t0 = m_vecSamples[_iSampleIdx + 0].first;
			auto t1 = m_vecSamples[_iSampleIdx + 1].first;
			auto l0 = m_vecSamples[_iSampleIdx + 0].second;
			auto l1 = m_vecSamples[_iSampleIdx + 1].second;

			_int seg = int(t0);
			_float fRatio = t0 - seg;
			fRatio += (t1 - t0) * (fLength - l0) / (l1 - l0 + 0.0001f);

			auto P0 = XMLoadFloat4(&m_vecPathPoints[clamp(seg - 1, 0, int(m_vecPathPoints.size()) - 1)]);
			auto P1 = XMLoadFloat4(&m_vecPathPoints[clamp(seg + 0, 0, int(m_vecPathPoints.size()) - 1)]);
			auto P2 = XMLoadFloat4(&m_vecPathPoints[clamp(seg + 1, 0, int(m_vecPathPoints.size()) - 1)]);
			auto P3 = XMLoadFloat4(&m_vecPathPoints[clamp(seg + 2, 0, int(m_vecPathPoints.size()) - 1)]);

			_vPos = XMVectorCatmullRom(P0, P1, P2, P3, fRatio);
		}
	}
	return _vPos;
}

_vector CSpline::GetDir(_float fLength, _float fDelta)
{
	fLength = min(fLength, m_fMaxLength);
	return XMVectorSetW(XMVector3Normalize(GetPos(max(fLength + fDelta, 0.f)) - GetPos(max(fLength - fDelta, 0.f))), 0.f);
}

_float CSpline::GetMaxLength()
{
	return m_fMaxLength;
}

void CSpline::RebakeModel()
{
	for (auto pObject : m_vecSplineSegments)
	{
		pObject->SetDestroyFlag();
	}
	m_vecSplineSegments.clear();
	m_vecSplineDescs.clear();
	m_vecSplineDescs.resize(m_desc.m_iSegmentCnt);

	//create
	CModel::MODEL_DESC _desc;
	for (size_t i = 0; i < m_desc.m_iSegmentCnt; i++)
	{
		CGameObject* _pObject;
		m_pGameInstance->Add_GameObject_ToLayer(
			L"Object_Empty", 
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			L"Layer_GEO", &_pObject);
		_pObject->Add_InitComponents();
		if (m_desc.m_bIncludeTunnel) {
			lstrcpy(_desc.m_sModelAssetName, L"Tunnel_Export");
			_pObject->Add_Component<CModel>(L"Com_Model_Tunnel", &_desc);
		}
		lstrcpy(_desc.m_sModelAssetName, L"Rail_Export");
		_pObject->Add_Component<CModel>(L"Com_Model_Rail",	&_desc);
		_pObject->Add_Component(L"Shader_Mesh_Component", L"Com_MeshShader_Main");
		_pObject->Load_Asset();
		_pObject->SetLevelObject();
		
		m_vecSplineSegments.push_back(_pObject);
	}
	m_bRebakeRenderer = true;
}

void CSpline::RebakeObstcle()
{
	if (m_desc.m_iNumObstacles > 20) return;

	for (auto pObject : m_vecObstacles)
	{
		pObject->SetDestroyFlag();
	}
	m_vecObstacles.clear();

	for (size_t i = 0; i < m_desc.m_iNumObstacles; i++)
	{
		int _idx = clamp(m_desc.m_arrObstacles[i].iObtacleType, 0, 2);
		CGameObject* pObject = nullptr;
		wstring sPrototypeTag = L"Prototype_Rail_Obstacle" + to_wstring(_idx);
		CGameObject::LoadPrototype(
			sPrototypeTag,
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			&pObject
		);
		pObject->SetLevelObject();

		m_vecObstacles.push_back(pObject);
	}
}

void CSpline::Destroy()
{
	for (auto pSegment : m_vecSplineSegments)
	{
		pSegment->SetDestroyFlag();
	}
	m_vecSplineSegments.clear();
	for (auto pObstacle : m_vecObstacles)
	{
		pObstacle->SetDestroyFlag();
	}
	m_vecObstacles.clear();
	m_pMagnetRail->SetDestroyFlag();
	m_pMagnetRail = nullptr;
	SetDestroyFlag();
}

void CSpline::Free()
{
	__super::Free();
}

CGameObject* CSpline::Clone(void* pArg)
{
	return CloneBase<CSpline>(pArg);
}

HRESULT CSpline::Save(void* _pDesc, _uint& _iSize) const
{
	static_assert(is_trivially_copyable_v<FSPLINEDESC>);
	_iSize = sizeof(FSPLINEDESC);
	memcpy(_pDesc, &m_desc, _iSize);
	return S_OK;
}

HRESULT CSpline::Load(void* _pDesc)
{
	auto _iSize = sizeof(FSPLINEDESC);
	memcpy(&m_desc, _pDesc, _iSize);

	RebakeModel();
	RebakeObstcle();
	return S_OK;
}
