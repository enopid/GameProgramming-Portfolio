#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& copy)
	:	m_desc(copy.m_desc),
		m_fCurrentTrackPosition(0.f),
		m_iNumChannels	(copy.m_vecChannels.size()),
		m_vecNotifies	(copy.m_vecNotifies)
{
	m_vecChannels.clear();
	for (size_t i = 0; i < m_iNumChannels; i++)
		m_vecChannels.push_back(copy.m_vecChannels[i]->Clone());
}

HRESULT CAnimation::Initialize(fstream& _fs)
{
	auto _sentinel = ANIMATIONHEADER;

	_fs.read((char*)&_sentinel, sizeof(ANIMATIONHEADER));
	if (_sentinel != ANIMATIONHEADER) {
		MSG_BOX("Fail to Load Animation : Invalid Animation Header!");
		return E_FAIL;
	}

	_fs.read((char*)&m_desc,			sizeof m_desc);
	_fs.read((char*)&m_iNumChannels,	sizeof m_iNumChannels);
	m_vecChannels.clear();
	m_vecChannels.reserve(m_iNumChannels);
	for (size_t i = 0; i < m_iNumChannels; i++)
		m_vecChannels.push_back(CChannel::Create(_fs));
	auto iSize = LoadVector(_fs, m_vecNotifies);
	_fs.read((char*)&_sentinel, sizeof(ANIMATIONFOOTER));
	if (_sentinel != ANIMATIONFOOTER) {
		MSG_BOX("Fail to Load Animation : Invalid Animation Footer!");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimation::Save(fstream& _fs)
{
	_fs.write((char*)&ANIMATIONHEADER, sizeof(ANIMATIONHEADER));

	_fs.write((char*)&m_desc, sizeof m_desc);

	_fs.write((char*)&m_iNumChannels, sizeof m_iNumChannels);
	for (size_t i = 0; i < m_iNumChannels; i++)
		m_vecChannels[i]->Save(_fs);
	auto iSize = SaveVector(_fs, m_vecNotifies);
	_fs.write((char*)&ANIMATIONFOOTER, sizeof(ANIMATIONFOOTER));

	return S_OK;
}

_matrix CAnimation::Update_TransformationMatrices(_float fTimeDelta, const vector<class CBone*>& Bones)
{
	if (m_vecBoneNames.empty()) {
		for (auto pBone : Bones) m_vecBoneNames.push_back(pBone->GetDesc().m_sName);
	}

	m_fCurrentTrackPosition += m_desc.m_fTickPerSecond * m_fTimeCoeff * fTimeDelta;
	if (m_fCurrentTrackPosition >= m_desc.m_fDuration) {
		if (m_desc.m_bLoop) {
			Reset();
		}
		else {
			m_fCurrentTrackPosition = m_desc.m_fDuration;
		}
		m_bIsEnd = true;
	}
	else {
		m_bIsEnd = false;
	}
	
	_matrix _matRootLocal = XMMatrixIdentity();
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		if (m_desc.m_iRootBoneIdx  !=-1 && m_vecChannels[i]->IsGivenBoneChannel(m_desc.m_iRootBoneIdx)) {
			_matRootLocal = m_vecChannels[i]->Update_RootTransformationMatrices(m_fCurrentTrackPosition, Bones, m_desc.m_bLoop);	//SRT
		}
		else
			m_vecChannels[i]->Update_TransformationMatrices(m_fCurrentTrackPosition, Bones, m_desc.m_bLoop);
	}

	return _matRootLocal;
}

_matrix CAnimation::Update_Additive_TransformationMatrices(_float fTimeDelta, const vector<CBone*>& Bones, const vector<CBone*>& refBones, _float fWeight)
{
	if (m_vecBoneNames.empty()) {
		for (auto pBone : Bones) m_vecBoneNames.push_back(pBone->GetDesc().m_sName);
	}

	m_fCurrentTrackPosition += m_desc.m_fTickPerSecond * m_fTimeCoeff * fTimeDelta;
	if (m_fCurrentTrackPosition >= m_desc.m_fDuration) {
		if (m_desc.m_bLoop) {
			Reset();
		}
		else {
			m_fCurrentTrackPosition = m_desc.m_fDuration;
		}
		m_bIsEnd = true;
	}
	else {
		m_bIsEnd = false;
	}

	_matrix _matRootLocal = XMMatrixIdentity();
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		if (m_desc.m_iRootBoneIdx != -1 && m_vecChannels[i]->IsGivenBoneChannel(m_desc.m_iRootBoneIdx)) {
			//현재는 트랜슬레이션만
			_matRootLocal = m_vecChannels[i]->Update_Additive_RootTransformationMatrices(m_fCurrentTrackPosition, Bones, refBones, m_desc.m_bLoop, fWeight);	//SRT
		}
		else
			m_vecChannels[i]->Update_Additive_TransformationMatrices(m_fCurrentTrackPosition, Bones, refBones, m_desc.m_bLoop, fWeight);
	}
	
	return _matRootLocal;
}

list<ANIMNOTIFY_DESC>	CAnimation::Update_AnimationsNotifies() {
	list<ANIMNOTIFY_DESC> _list;

	//Propagte Notify
	while (m_iCurNotifyIdx < m_vecNotifies.size()) {
		if (m_vecNotifies[m_iCurNotifyIdx].m_fPosition * m_desc.m_fDuration <= m_fCurrentTrackPosition) {
			_list.push_back(m_vecNotifies[m_iCurNotifyIdx]);
			m_iCurNotifyIdx++;
		}
		else {
			break;
		}
	}

	return _list;
}

list<ANIMNOTIFY_DESC> CAnimation::Reset_Notifies()
{
	list<ANIMNOTIFY_DESC> _list;

	//Propagte Notify
	while (m_iCurNotifyIdx < m_vecNotifies.size()) {
		if (m_vecNotifies[m_iCurNotifyIdx].m_bIsStart == false) {
			_list.push_back(m_vecNotifies[m_iCurNotifyIdx]);
		}
		m_iCurNotifyIdx++;
	}

	return _list;
}

list<ANIMNOTIFY_DESC> CAnimation::Reset()
{
	m_fCurrentTrackPosition = 0.f;
	for (auto& pChannel : m_vecChannels) pChannel->Reset();

	auto _list = Reset_Notifies();
	m_iCurNotifyIdx = 0;
	m_bIsEnd = false;
	return _list;
}

HRESULT CAnimation::Render_Animation_Property()
{
	//desc info

	ImGui::Text("Name :");
	ImGui::SameLine(200);
	ImGui::Text(ws2s(wstring(m_desc.m_sAnimationName)).data());

	ImGui::Text("Duration :");
	ImGui::SameLine(200);
	ImGui::Text(to_string(m_desc.m_fDuration).data());

	ImGui::Text("Tick :");
	ImGui::SameLine(200);
	ImGui::Text(to_string(m_desc.m_fTickPerSecond).data());

	ImGui::Text("Num Channel:");
	ImGui::SameLine(200);
	ImGui::Text(to_string(m_iNumChannels).data());

	ImGui::PushItemWidth(200);
	ImGui::Combo("RootMotionType",	&m_desc.m_iRootMotionType, ROOTMOTIONTYPES, 3);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	ImGui::Combo("LoopDeltaType",	&m_desc.m_iLoopDeltaType, LOOPDELTATYPES, 3);
	ImGui::PopItemWidth();

	ImGui::Separator();
	//Current Info
	ImGui::Checkbox("IsLoop", &m_desc.m_bLoop);
	if (ImGui::SliderFloat("CurPos", &m_fCurrentTrackPosition, 0.f, m_desc.m_fDuration)) {
		for (auto& pChannel : m_vecChannels) pChannel->Reset();
	}

	ImGui::Separator();
	if (ImGui::Button("Add Notify")) {
		ANIMNOTIFY_DESC _desc;
		_desc.m_fPosition = m_fCurrentTrackPosition;
		AddNotify(_desc);
	}

	static string sAnimNotifyRenameText = "";
	_int iDeleteCollidorIdx = -1;
	for (int i = 0; i < m_vecNotifies.size(); i++) {
		string sName = ws2s(wstring(m_vecNotifies[i].m_sName));

		ImGui::PushID(i);
		bool _bOpen = ImGui::TreeNodeEx(sName.c_str(), ImGuiTreeNodeFlags_Framed);
		ImGui::PopID();

		if (ImGui::BeginPopupContextItem())
		{
			ImGui::InputText("##1", &sAnimNotifyRenameText);
			ImGui::SameLine();
			if (ImGui::Button("Rename")) {
				m_vecNotifies[i].m_sName;
				wstring wsName = s2ws(sAnimNotifyRenameText);
				memcpy(m_vecNotifies[i].m_sName, wsName.c_str(), sizeof m_vecNotifies[i].m_sName);
			}
			if (ImGui::MenuItem("Delete")) {
				iDeleteCollidorIdx = i;
			}
			ImGui::EndPopup();
		}

		if (_bOpen)
		{
			ImGui::Checkbox("Is StartNotify", &m_vecNotifies[i].m_bIsStart);
			ImGui::DragFloat("Duration", &m_vecNotifies[i].m_fDuration, 0.01f, 0.f, 20.f, "%.3f s");
			ImGui::DragFloat("Position", &m_vecNotifies[i].m_fPosition, 0.01f, 0.f, m_desc.m_fDuration, "%.3f");

			if (m_vecBoneNames.size()) {
				string  _sCurItem = ws2s(m_vecBoneNames[m_vecNotifies[i].m_iBoneIdx]);
				if (ImGui::BeginCombo("Bone", _sCurItem.c_str()))
				{
					for (int j = 0; j < m_vecBoneNames.size(); ++j)
					{
						bool isSelected = (m_vecNotifies[i].m_iBoneIdx == j);
						string  _sItem = ws2s(m_vecBoneNames[j]);
						if (ImGui::Selectable(_sItem.c_str(), isSelected))
							m_vecNotifies[i].m_iBoneIdx = j;

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			ImGui::TreePop();
		}
	}

	if (iDeleteCollidorIdx != -1) {
		if (iDeleteCollidorIdx < m_vecNotifies.size()) {
			m_vecNotifies[iDeleteCollidorIdx] = m_vecNotifies.back();  // 맨 뒤 값을 가져옴
			m_vecNotifies.pop_back();         // 마지막 원소 제거
		}
		sort(m_vecNotifies.begin(), m_vecNotifies.end(), [](const ANIMNOTIFY_DESC& _desc0, const ANIMNOTIFY_DESC& _desc1) {
			return _desc0.m_fPosition < _desc1.m_fPosition;
			}
		);
	}

	/*float fAvg(0), fMax(0), fsd(0);
	for (auto pChannel : m_vecChannels) {
		fAvg += pChannel->GetKeyFrames();
		fMax = max(pChannel->GetKeyFrames(), fMax);
	}
	fAvg /= m_vecChannels.size();

	for (auto pChannel : m_vecChannels) {
		fsd += (fAvg - pChannel->GetKeyFrames()) * (fAvg - pChannel->GetKeyFrames());
	}
	fsd /= m_vecChannels.size();
	fsd = sqrt(fsd);

	ImGui::Text("AvgFrame :");
	ImGui::SameLine(200);
	ImGui::Text(to_string(fAvg).data());

	ImGui::Text("MaxFrame :");
	ImGui::SameLine(200);
	ImGui::Text(to_string(fMax).data());

	ImGui::Text("Frame SD :");
	ImGui::SameLine(200);
	ImGui::Text(to_string(fsd).data());*/

	return S_OK;
}

void CAnimation::AddNotify(ANIMNOTIFY_DESC& _desc)
{
	m_vecNotifies.push_back(_desc);
	sort(m_vecNotifies.begin(), m_vecNotifies.end(), [](const ANIMNOTIFY_DESC& _desc0, const ANIMNOTIFY_DESC& _desc1) {
		return _desc0.m_fPosition < _desc1.m_fPosition;
		}
	);
}

void CAnimation::SetRootBone(_uint i)
{
	m_desc.m_iRootBoneIdx = i;
}

_float CAnimation::GetRatio()
{
	if (IsEnd()) return 1.f;
	return m_fCurrentTrackPosition / m_desc.m_fDuration;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

CAnimation* CAnimation::Create(fstream& _fs)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(_fs)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();
	for (auto& pChannel : m_vecChannels)
		Safe_Release(pChannel);
	m_vecChannels.clear();
}
