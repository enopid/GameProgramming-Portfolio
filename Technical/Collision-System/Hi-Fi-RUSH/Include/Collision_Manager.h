#pragma once

#include "Base.h"
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollision_Manager final : public CBase
{
	inline static vector<string>	m_vecNames;
	constexpr static int MAXCOLLISIONMASK = 24;
private:
	CCollision_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCollision_Manager() = default;
public:
	HRESULT Initialize();
	HRESULT Add_Collidor(WPCollidor wpCollidor);

	void	LateUpdate();
	HRESULT ActiveCollision(_uint _id1, _uint _id2);
	bool	RayCast_Collidor(_int iCollisionMask, RAY ray, _float& fInOutDist, list<pair<_float, WPCollidor>>& lstWPOutCollidor);
	void Clear();
	static void				SetNames(vector<string> vecNames);
	static vector<string>	GetNames();
private:
	ID3D11Device*			m_pDevice	= { nullptr };
	ID3D11DeviceContext*	m_pContext	= { nullptr };

	list<WPCollidor>		m_lstCorridors[MAXCOLLISIONMASK];
	_bool arrCollisionMask[MAXCOLLISIONMASK][MAXCOLLISIONMASK];
private:
public:
	static CCollision_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END