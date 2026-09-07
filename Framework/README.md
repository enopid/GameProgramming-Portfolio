# Hi-Fi RUSH Framework

Hi-Fi RUSH 모작에서 팀원이 공통으로 사용한 DirectX 11 프레임워크입니다. 이 영역의 매니저와 프레임워크, 오브젝트·컴포넌트 기본 구조는 직접 설계하고 구현했습니다.

## 오브젝트·컴포넌트

- `GameObject`를 기준으로 공통 생명 주기 정의
- `Component`를 조합해 렌더링과 동작 기능 구성
- Prototype을 이용한 생성과 복제
- 오브젝트와 컴포넌트의 저장·불러오기

## 우선 공개 매니저

- `Time_Manager`: 타이머 등록·갱신과 핸들 수명 관리
- `Asset_Manager`, `Asset`: 에셋 생성·공유와 소유권 관리
- `Collision_Manager`: 콜라이더 등록과 충돌 처리 단계 관리
- `Object_Manager`, `Prototype_Manager`: 레벨별 오브젝트 보관과 복제 관리
- `Level_Manager`: 레벨 전환과 레벨 단위 수명 관리
- `Renderer_Manager`: 렌더 그룹과 순서 관리
- `Input_Manager`: 입력 컨텍스트와 입력 상태 관리
- `Rhythm_Manager`: 음악 재생 위치를 기준으로 비트와 판정 시점 관리
