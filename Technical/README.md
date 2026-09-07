# 기술 구현

기술문서에 수록한 아홉 가지 구현입니다. 검토 후 각 항목 아래에 핵심 코드와 별도 설명 문서를 배치합니다.

## Animation System

Kena 모작에서 이동·조준·공격을 동시에 합성하기 위해 3D Blend Space, Additive Animation, Aim Offset을 지원하는 다중 레이어 애니메이션 시스템을 구현했습니다.

공개 후보: `Animation`, `AnimController`, `AnimNode`, `AnimTransition`, `LayerAnimNode`, `ActionAnimController`, `ActionAnimNode` 계열

## Behaviour Tree

Kena 모작의 몬스터 공통 상태와 개별 행동을 계층적으로 조합하기 위해 Composite·Decorator·Task 노드와 런타임 실행 흐름 시각화를 구현했습니다.

공개 후보: `BehaviourTree`, `BT_Task`, `BT_Decorator` 및 Composite 노드 계열

## Collision System

Kena에서는 캐릭터 본을 따라 갱신되는 캡슐 콜라이더를 구현했습니다. Hi-Fi RUSH에서는 매니폴드 연산을 반복하는 Iterative Solver와 다단계 충돌 처리 구조를 적용했습니다.

공개 후보: 본 기반 콜라이더, `Collision_Manager`, 매니폴드와 반복 충돌 해소 코드

## PBR Rendering

Kena의 Deferred Rendering 환경에 Cook–Torrance BRDF 기반 직접광과 Irradiance Map, Prefiltered Environment Map, BRDF LUT를 이용한 Split-Sum IBL을 구현했습니다.

공개 후보: 직접광·Irradiance·Prefilter·BRDF LUT 생성 및 적용 셰이더

## Cartoon Outline

Hi-Fi RUSH에서 Inverted Hull과 깊이·노멀 차이를 이용한 후처리 외곽선을 오브젝트 특성에 맞춰 조합하고, 거리와 시야각 변화에 따른 두께를 보정했습니다.

공개 후보: 외곽선 렌더 패스와 Depth·Normal Edge Detection 셰이더

## Hybrid Camera

Toodee and Topdee에서 낮은 FOV로 2D 평면감을 만들고, 전환 비율에 따라 FOV·카메라 기울기·거리를 계산해 3D 탑 뷰로 전환했습니다.

공개 후보: `HybridCamera`, `BaseCamera`

## Spline Mesh

Hi-Fi RUSH의 레일 콘텐츠를 위해 Catmull–Rom 경로, 거리 기반 위치·방향 계산, Vertex Shader 기반 메시 변형을 구현했습니다.

공개 후보: `Spline`과 경로 생성·샘플링 코드, 메시 변형 셰이더

## Terrain Tool

Kena의 레벨 제작을 위해 Raise·Flatten·Smooth 브러시와 Height 기반 재질 블렌딩, PBR Splat Map 편집을 하나의 도구로 구현했습니다.

공개 후보: `TerrainRenderer`, `VIBuffer_Terrain`, Height·Splat 편집 코드와 셰이더

## Network Synchronization

Celeste에 네트워크 통신을 연동하고 동일 스테이지 선택 전파, 시퀀스 검증, 최근 두 스냅샷 기반 보간, 데드 레커닝과 상태 기반 보정을 구현했습니다.

공개 범위: `CelesteSession`, `CelesteSessionManager`, `ServerPacketHandler`, `NetworkManager`, `ServerSession`, `RemotePlayer`, 프로토콜과 패킷 처리 코드
