# Game Programming Portfolio

DirectX 9·11과 WinAPI 환경에서 직접 설계하고 구현한 게임 프로그래밍 포트폴리오입니다.

전체 프로젝트 대신 담당 범위가 분명한 핵심 코드만 선별합니다. 원작 리소스, 외부 라이브러리, 빌드 산출물과 팀원이 작성한 코드는 포함하지 않습니다.

## 기술 구현

기술 문서에 나오는 콘텐츠만 정리했습니다.

| 기술 | 프로젝트 | 내용 |
|---|---|---|
| [Animation System](Technical/Animation-System/README.md) | Kena | 3D Blend Space<br>Additive Animation<br>Aim Offset<br>Multi-Layer Pose |
| [Behaviour Tree](Technical/Behaviour-Tree/README.md) | Kena | Composite·Decorator·Task<br>공통 상태 Subtree<br>Stick 적용 예시<br>실행 흐름 시각화 |
| [Collision System](Technical/Collision-System/README.md) | Kena<br>Hi-Fi RUSH | 본 기반 Capsule Collider<br>Manifold<br>Iterative Solver<br>다단계 충돌 검사 |
| [PBR Rendering](Technical/PBR-Rendering/README.md) | Kena | Cook–Torrance BRDF<br>Deferred Rendering<br>Split-Sum IBL<br>IBL 사전 계산 텍스처 |
| [Cartoon Outline](Technical/Cartoon-Outline/README.md) | Hi-Fi RUSH | Inverted Hull<br>Depth·Normal Edge Detection<br>거리·시야각 두께 보정 |
| [Hybrid Camera](Technical/Hybrid-Camera/README.md) | Toodee and Topdee | 단일 투영 체계<br>FOV·각도 보간<br>표시 범위 기반 거리 계산<br>2D·3D 시점 전환 |
| [Spline Mesh](Technical/Spline-Mesh/README.md) | Hi-Fi RUSH | Catmull–Rom 경로<br>거리 기반 샘플링<br>위치·방향 계산<br>Vertex Shader 변형 |
| [Terrain Tool](Technical/Terrain-Tool/README.md) | Kena | Raise·Flatten·Smooth<br>Height 기반 레이어 블렌딩<br>PBR Splat Map |
| [Network Synchronization](Technical/IOCP-Network/README.md) | Celeste | 패킷 송수신<br>스테이지별 선택 전파<br>Snapshot Interpolation<br>Dead Reckoning·상태 보정 |

## 프로젝트 콘텐츠

기술 구현과 중복되는 항목은 제외하고 프로젝트 고유 콘텐츠만 정리했습니다.

| 기술 | 프로젝트 | 내용 |
|---|---|---|
| [Script Cutscene](Content/Hi-Fi-RUSH/Cutscene/README.md) | Hi-Fi RUSH | Act 단위 스크립트<br>카메라·대사·UI<br>이펙트·사운드 제어 |
| [Partner Combat](Content/Hi-Fi-RUSH/Partner-Combat/README.md) | Hi-Fi RUSH | 페퍼민트: 배리어·레이저<br>코르시카: 화염·회오리<br>마카롱: 실드·중력장<br>실드·배리어 파괴 연출 |
| [Partner Gimmicks](Content/Hi-Fi-RUSH/Partner-Gimmicks/README.md) | Hi-Fi RUSH | 페퍼민트: Aim Offset·원거리 조작<br>마카롱: 리듬 판정·파괴 벽<br>코르시카: 리듬 판정·기믹 연동 |
| [Locomotion·Hair](Content/Celeste/Locomotion-and-Hair/README.md) | Celeste | 이동·점프·벽타기<br>2단 대시<br>다관절 머리카락 |
| [Auto Tiling](Content/Celeste/Auto-Tiling/README.md) | Celeste | 스테이지 에디터<br>인접 타일 Bitmask<br>Atlas 자동 선택<br>연결 타일 갱신 |
| [Theme Effects](Content/Celeste/Theme-Effects/README.md) | Celeste | 얼음·용암 색조<br>아지랑이<br>경계 전이<br>Parallax Scrolling |
| [Theme Gimmicks](Content/Celeste/Theme-Gimmicks/README.md) | Celeste | 테마별 기믹 전환<br>충돌 기반 테마 변경<br>원작 외 보스전 |
| [Input Correction](Content/Celeste/Input-Correction/README.md) | Celeste | Coyote Time<br>Jump Buffer·Variable Jump<br>Dash 입력 지연 |
| [Sprite Stack](Content/Toodee-and-Topdee/Sprite-Stack/README.md) | Toodee and Topdee | 파츠별 깊이 배치<br>회전·위치 보정<br>Billboard<br>2D 스프라이트 입체화 |
| [Bat·Pig](Content/Toodee-and-Topdee/Monsters/README.md) | Toodee and Topdee | 2D 좌우 순찰<br>3D 플레이어 추적<br>전방 지면 감지 |
| [Boss·Ozraeen](Content/Toodee-and-Topdee/Boss/README.md) | Toodee and Topdee | FSM 보스 공통 클래스<br>상태·패턴 그룹<br>피격·연출 분리 |
| [Gimmick Objects](Content/Toodee-and-Topdee/Gimmicks/README.md) | Toodee and Topdee | Laser: 연속 반사<br>Fire Turret: Object Pool<br>Lightning: Ray Casting<br>Hole: 낙하 판정<br>Box: Grid·재귀 탐색 |

## Hi-Fi RUSH 프레임워크

| 기술 | 프로젝트 | 내용 |
|---|---|---|
| [Object·Component](Framework/Object-Component/README.md) | Hi-Fi RUSH | 공통 생명 주기<br>Component 조합<br>Prototype 생성·복제<br>저장·불러오기 |
| [Time Manager](Framework/Managers/Time/README.md) | Hi-Fi RUSH | Timer 등록·갱신<br>Handle 기반 수명 관리 |
| [Asset Manager](Framework/Managers/Asset/README.md) | Hi-Fi RUSH | Asset 생성·공유<br>소유권·수명 관리 |
| [Collision Manager](Framework/Managers/Collision/README.md) | Hi-Fi RUSH | Collider 등록<br>충돌 후보 분류<br>판정·해소 단계 관리 |
| [Object·Prototype Manager](Framework/Managers/Object-Prototype/README.md) | Hi-Fi RUSH | Prototype 복제<br>Level·Layer 단위 관리 |
| [Level Manager](Framework/Managers/Level/README.md) | Hi-Fi RUSH | Level 전환<br>Level 종속 객체 수명 관리 |
| [Renderer Manager](Framework/Managers/Renderer/README.md) | Hi-Fi RUSH | Render Group 등록<br>Pass·우선순위 관리 |
| [Input Manager](Framework/Managers/Input/README.md) | Hi-Fi RUSH | Input Context<br>Key State 관리 |
| [Rhythm Manager](Framework/Managers/Rhythm/README.md) | Hi-Fi RUSH | 음악 재생 위치<br>Beat 계산<br>리듬 판정 시점 관리 |

## 공개 범위

- 직접 작성한 C++ 및 셰이더 코드만 수록합니다.
- `ThirdParty`, 빌드 산출물, 원작 리소스와 팀원 코드는 제외합니다.
- 전체 프로젝트 의존성을 제거하므로 일부 코드는 이 저장소만으로 실행되지 않을 수 있습니다.
