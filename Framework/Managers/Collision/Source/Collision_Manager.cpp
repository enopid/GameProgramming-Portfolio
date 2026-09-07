#include "Collision_Manager.h"
#include "Collidor.h"
#include "GameObject.h"
#include "GameInstance.h"


CCollision_Manager::CCollision_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{    
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

HRESULT CCollision_Manager::Initialize()
{
    Clear();
    for (size_t i = 0; i < MAXCOLLISIONMASK; i++) m_lstCorridors[i] = list<WPCollidor>();
    return S_OK;
}

HRESULT CCollision_Manager::Add_Collidor(WPCollidor wpCollidor)
{
    auto _pDesc = wpCollidor.lock()->GetDesc();
    if (_pDesc->m_iCollidorIdx >= MAXCOLLISIONMASK) return E_FAIL;
    m_lstCorridors[_pDesc->m_iCollidorIdx].push_back(wpCollidor);
    return S_OK;
}

void CCollision_Manager::LateUpdate()
{
    list<pair<WPCollidor, WPCollidor>> _lstCollisionMaskPairs, _lstBroadTestPairs, _lstMidTestPairs, _lstNarrowTestPairs;
    //Clear
    for (size_t i = 0; i < MAXCOLLISIONMASK; i++) {
        for (auto it = m_lstCorridors[i].begin(); it != m_lstCorridors[i].end(); ) {
            if (it->expired()) {
                it = m_lstCorridors[i].erase(it);
            }
            else {
                it->lock()->Clear_Manifolds();
                it++;
            }
        }
    }

    //CheckMask
    for (int _maskIdx1 = 0; _maskIdx1 < MAXCOLLISIONMASK; _maskIdx1++) for (int _maskIdx2 = 0; _maskIdx2 < MAXCOLLISIONMASK; _maskIdx2++) {
        if (!arrCollisionMask[_maskIdx1][_maskIdx2]) continue;
        for (auto _wpCollidor0 : m_lstCorridors[_maskIdx1]) for (auto _wpCollidor1 : m_lstCorridors[_maskIdx2]) {
            if (!_wpCollidor0.lock()->IsActive() || !_wpCollidor1.lock()->IsActive()) continue;
            if (_wpCollidor0.lock()->m_pOwnerObj    == _wpCollidor1.lock()->m_pOwnerObj) continue;
            _lstCollisionMaskPairs.push_back({ _wpCollidor0 , _wpCollidor1 });
        }
    }
    //BroadTest
    for (auto& [_wpCollidor0, _wpCollidor1] : _lstCollisionMaskPairs)
        if (_wpCollidor0.lock()->Check_Intersection(_wpCollidor1, CCollidor::ETESTLVL::BROAD))
            _lstBroadTestPairs.push_back({ _wpCollidor0 , _wpCollidor1 });
    //MidTest
    for (auto& [_wpCollidor0, _wpCollidor1] : _lstBroadTestPairs)
        if (_wpCollidor0.lock()->Check_Intersection(_wpCollidor1, CCollidor::ETESTLVL::MID))
            _lstMidTestPairs.push_back({ _wpCollidor0 , _wpCollidor1 });
    //NarrowTest
    for (auto& [_wpCollidor0, _wpCollidor1] : _lstMidTestPairs)
        if (_wpCollidor0.lock()->Check_Intersection(_wpCollidor1, CCollidor::ETESTLVL::NARROW))
            _lstNarrowTestPairs.push_back({ _wpCollidor0 , _wpCollidor1 });
   //Compute Manifold
   for (auto& [_wpCollidor0, _wpCollidor1] : _lstNarrowTestPairs)
        _wpCollidor0.lock()->Compute_Manifolds(_wpCollidor1);
   //Execute Enter/Stay/Exit Func
   for (size_t i = 0; i < MAXCOLLISIONMASK; i++) for (auto& _wpModel : m_lstCorridors[i])
       _wpModel.lock()->Execute_Func();

   //Solve Manifold
   int iCnt = 3;
   for (int idx = 0; idx < iCnt; idx++) {
       for (auto& [_wpCollidor0, _wpCollidor1] : _lstNarrowTestPairs)
           _wpCollidor0.lock()->Clear_Manifolds();
       for (auto& [_wpCollidor0, _wpCollidor1] : _lstNarrowTestPairs) 
           _wpCollidor0.lock()->Compute_Manifolds(_wpCollidor1);
       for (size_t i = 0; i < MAXCOLLISIONMASK; i++) for (auto& _wpModel : m_lstCorridors[i])
           _wpModel.lock()->TestSolver();
       if (idx >= iCnt - 1) break;
   }
}

HRESULT CCollision_Manager::ActiveCollision(_uint _id1, _uint _id2)
{
    if (_id1 >= MAXCOLLISIONMASK) return E_FAIL;
    if (_id2 >= MAXCOLLISIONMASK) return E_FAIL;
    arrCollisionMask[_id1][_id2] = true;

    return S_OK;
}

bool CCollision_Manager::RayCast_Collidor(_int iCollisionMask, RAY ray, _float& fInOutDist, list<pair<_float, WPCollidor>>& lstWPOutCollidor)
{
    bool _bIsHit =false;
    lstWPOutCollidor.clear();
    float _fMinDist = fInOutDist;
    for (int i = 0; i < MAXCOLLISIONMASK; i++)
    {
        if (!(iCollisionMask & (1 << i))) continue;
        for (auto& _wpCollidor : m_lstCorridors[i])
        {
            if (!_wpCollidor.lock()->IsActive()) continue;
            float _fTmpDist;
            if (_wpCollidor.lock()->RayCast(ray, _fTmpDist)) {
                if (_fTmpDist < fInOutDist) {
                    _fMinDist = min(fInOutDist, _fTmpDist);
                    lstWPOutCollidor.push_back({ _fTmpDist, _wpCollidor });
                    _bIsHit         = true;
                }
            }
        }
    }
    fInOutDist = _fMinDist;

    return _bIsHit;
}

void CCollision_Manager::Clear()
{
    for (int i = 0; i < MAXCOLLISIONMASK; i++) for (int j = 0; j < MAXCOLLISIONMASK; j++)
        arrCollisionMask[i][j] = false;
}

void CCollision_Manager::SetNames(vector<string> vecNames)
{
    m_vecNames = vecNames;
}

vector<string> CCollision_Manager::GetNames()
{
    return m_vecNames;
}

CCollision_Manager* CCollision_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    auto pInstance = new CCollision_Manager(pDevice, pContext);
    if (FAILED(pInstance->Initialize())) {
        MSG_BOX("Fail to Create Collision Manager");
        Safe_Release(pInstance);
        pInstance = nullptr;
    }
    return pInstance;
}

void CCollision_Manager::Free()
{
    __super::Free();
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
