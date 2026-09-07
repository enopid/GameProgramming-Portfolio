# Boss and Ozraeen

FSM 기반 보스 공통 구조와 Ozraeen 구현 코드입니다.

## 구조

```mermaid
flowchart TD
    Boss[Boss Base] --> Part[Boss Part]
    Boss --> Ozraeen[Ozraeen]
    Ozraeen --> Head[Head]
    Ozraeen --> Body[Body]
    Ozraeen --> Parts[Parts]
```

## 포함 파일

| 구현 단위 | 헤더 | 구현 |
|---|---|---|
| `Boss` | `Include/Boss.h` | `Source/Boss.cpp` |
| `Boss_Part` | `Include/Boss_Part.h` | `Source/Boss_Part.cpp` |
| `Ozraeen` | `Include/Ozraeen.h` | `Source/Ozraeen.cpp` |
| `Ozraeen_Body` | `Include/Ozraeen_Body.h` | `Source/Ozraeen_Body.cpp` |
| `Ozraeen_Head` | `Include/Ozraeen_Head.h` | `Source/Ozraeen_Head.cpp` |
| `Ozraeen_Part` | `Include/Ozraeen_Part.h` | `Source/Ozraeen_Part.cpp` |

> 전체 프로젝트의 빌드 의존성은 포함하지 않았으며, 구현 흐름을 확인하는 데 필요한 범위까지만 선별했습니다.
