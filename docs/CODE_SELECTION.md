# 공개 코드 선정표

실제 파일 복사 전에 공개 범위를 확인하기 위한 문서입니다.

## 확인한 원본

| 프로젝트 | 원본 경로 |
|---|---|
| Celeste | `D:\Jusin_Portfolio\1.2D_Personal\Code\00.CELESTE\00.CELESTE\Celeste` |
| Toodee and Topdee | `D:\Jusin_Portfolio\2.SR_Team\Code\SR_Project\SR_Project\251021_텍스쳐\Framework` |
| Kena | `E:\Kena\02.Code\Kena` |
| Hi-Fi RUSH | `C:\Users\davin\Documents\Codex\2026-07-19\repo-work\work\AtomicHeart` |

## 복사 원칙

- 기술 문서의 9개 기술은 시스템 코드와 셰이더를 기술별 폴더로 복사
- 개인 프로젝트는 포트폴리오에 명시한 콘텐츠 코드 복사
- Toodee and Topdee는 포트폴리오에 명시한 콘텐츠만 복사
- Hi-Fi RUSH는 매니저·프레임워크·오브젝트·컴포넌트 기본 구조와 포트폴리오 콘텐츠 복사
- 헤더와 구현 파일을 한 쌍으로 유지
- 전체 프로젝트 의존 코드가 필요하면 바로 공개하지 않고 `Dependencies` 후보로 기록

## 제외

- `ThirdParty`, `Bin`, `Binaries`, `Intermediate`, `.vs`, 빌드 산출물
- 모델·텍스처·음원·영상 등 원작 또는 외부 리소스
- 팀원이 작성한 코드
- 계정 정보, 절대 경로, 실제 서버 주소

## 확인이 필요한 항목

- [ ] Kena 몬스터 AI는 행동 트리 시스템만 공개할지, 대표 몬스터 사용 예시도 공개할지
- [ ] Hi-Fi RUSH 파트너 이펙트 코드까지 포함할지
- [x] Celeste의 `ServerCore`는 외부 기반 코드이므로 공개 범위에서 제외
- [ ] Toodee and Topdee의 Ozraeen 파츠 코드 전체가 본인 작성분인지
- [ ] 결과 GIF를 저장소에 넣을지 외부 영상 링크만 제공할지

Hi-Fi RUSH 매니저는 기존 우선 공개 목록에 `Rhythm_Manager`만 추가합니다. 그 외 매니저는 현재 공개 대상에서 제외합니다.
