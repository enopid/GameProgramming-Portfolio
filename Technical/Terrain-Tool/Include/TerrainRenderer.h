#pragma once

#include "Renderer.h"
#include "DebugManager_Api.h"


// 기본적으로 mesh의 참조를 가질지		- 
// 아님 Mesh의 카피를 가질지 선택가능
NS_BEGIN(Engine)
class CVIBuffer_Terrain;

class ENGINE_DLL CTerrainRenderer final : public CRenderer
{
	inline static const char* const VIEWNAMES[3] = { "WIREFRAME", "SPLATMAP", "SHADED" };
	enum EVIEWMODE { WIREFRAME,SPLATMAP, SHADED };

	friend class CComponent;
public:
	typedef struct tagTerrainDesc
	{
		_tchar		m_sSplatTexture0Name[60] = L"";
	} TERRAIN_DESC;
private:
	CTerrainRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrainRenderer(const CTerrainRenderer& Prototype);
	virtual ~CTerrainRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype(void* _pDesc);
	virtual HRESULT Initialize(void* pArg);

	void Render_Inspector()	override;
public:
	virtual SPComponent Clone(void* pArg)	override;
	virtual void Free()						override;

	weak_ptr<CVIBuffer_Terrain> Get_Terrain();
private:
	// CComponent을(를) 통해 상속됨					
	HRESULT Save(void* _pDesc, _uint& _iSize) const	override;
	HRESULT Load(void* _pDesc)						override;

	// CRenderer을(를) 통해 상속됨
	HRESULT AddRenderGroup() override;
	HRESULT Bind_Resources() override;
private:
	weak_ptr<CVIBuffer_Terrain> m_wpTerrain;
	_bool		m_bListOpen	= false;
	int			m_eViewMode = SHADED;
};

NS_END