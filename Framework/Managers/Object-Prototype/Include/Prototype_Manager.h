#pragma once


//#include "VIBuffer_Cube.h"
//#include "VIBuffer_Terrain.h"

#include "EmptyObject.h"
#include "TestInstancingObj.h"

#include "Transform.h"

#include "VideoUI.h"
#include "BaseModel.h"
#include "Instancing_BaseModel.h"

#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_Particle_Rect.h"
#include "VIBuffer_UIRect.h"

#include "TerrainRenderer.h"
#include "ParticleRenderer.h"
#include "SkyBoxRenderer.h"
#include "DecalRenderer.h"
#include "UIImageRenderer.h"
#include "UITextRenderer.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "Navigation.h"

#include "Collidor.h"

#include "Module_Volume.h"
#include "RotationComponent.h"
#include "SpatialSound_Component.h"
#include "ObjectMoveComponent.h"
#include "EmissiveCtrlComponent.h"

#include "BehaviorTree.h"
#include "BT_Selector.h"
#include "BT_RandomSelector.h"
#include "BT_Sequence.h"
#include "BT_Decorator.h"
#include "BT_Condition.h"
#include "BT_Task.h"

#include "UIAnimator.h"
#include "DynamicShaderParam.h"
#include "UIVideoRenderer.h"

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Prototype		(PROTOTYPE ePrototype, const _wstring& strPrototypeTag, class CBase* pPrototype, _bool _bIsClassPrototype);
	bool	IsClassPrototype	(PROTOTYPE ePrototype, const _wstring& strPrototypeTag) {
		return m_BasePrototypeNames[ENUM_TO_UINT(ePrototype)].find(strPrototypeTag) != m_BasePrototypeNames[ENUM_TO_UINT(ePrototype)].end();
	}
	CBase*	Clone_Prototype		(PROTOTYPE ePrototype, const _wstring& strPrototypeTag, void* pArg);
	SPComponent	Clone_ComponentPrototype		(const _wstring& strPrototypeTag, void* pArg);
	HRESULT	Remove_Prototype	(PROTOTYPE ePrototype, const _wstring& strPrototypeTag);
	void	Clear();
	void	Clear(const _wstring& strLVLTag);
	vector<_wstring> GetPrototypeNames(PROTOTYPE ePrototype, _bool _bOnlyIsClassPrototype);
private:
	map<_wstring, class CBase*>	m_Prototypes		[ENUM_TO_UINT(PROTOTYPE::END)];
	set<_wstring>				m_BasePrototypeNames[ENUM_TO_UINT(PROTOTYPE::END)];
	list<_wstring>				m_PrototypeNames	[ENUM_TO_UINT(PROTOTYPE::END)]; 

private:
	CBase*	Find_Prototype(PROTOTYPE ePrototype, const _wstring& strPrototypeTag);
public:
	static CPrototype_Manager* Create();
	virtual void Free() override;
};


NS_END
