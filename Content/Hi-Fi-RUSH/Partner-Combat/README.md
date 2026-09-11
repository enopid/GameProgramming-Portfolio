# Partner Combat

페퍼민트·코르시카·마카롱의 일반 공격과 차지 공격에 사용한 코드입니다.

## 클래스 구조

```mermaid
classDiagram
    CCharacter <|-- CPartner
    CPartner <|-- CPartner_Character_Peppermint
    CPartner <|-- CPartner_Character_Korsica
    CPartner <|-- CPartner_Character_Macaron
    CFSM <|-- CPartnerState_FSM
    CFSM <|-- CPartnerBattle_FSM
    CProjectile <|-- CProjectile_Peppermint_Bullet
    CGameObject <|-- CKorsica_Wind
    CWeapon <|-- CEnemyShield
    CGameObject <|-- CEnemyBarrier
    CGameObject <|-- CGravitySphere
    CPartner *-- CAnimController : unique_ptr array
    CPartner *-- CPartnerState_FSM : unique_ptr
    CPartner *-- CPartnerBattle_FSM : unique_ptr
    CPartner *-- CPartnerGimmic_FSM : unique_ptr
    CPartner_Character_Macaron --> CGravitySphere : raw references
```

`<|--` 상속 · `*--` 소유 · `-->` 비소유 참조

## 실행 결과

### 페퍼민트 전투

![페퍼민트 전투](Media/partner-combat.gif)

### 코르시카 전투

![코르시카 전투](Media/korsica-combat.gif)

### 마카롱 전투

![마카롱 전투](Media/macaron-combat.gif)

## 전체 시연 영상

[Hi-Fi RUSH 담당 구간 보기](https://youtu.be/093V56PwwEg?t=163)

## 포함 파일

| 구현 단위 | 헤더 | 구현 |
|---|---|---|
| `EnemyBarrier` | `Include/EnemyBarrier.h` | `Source/EnemyBarrier.cpp` |
| `EnemyShield` | `Include/EnemyShield.h` | `Source/EnemyShield.cpp` |
| `FSM_PartnerBattle` | `Include/FSM_PartnerBattle.h` | `Source/FSM_PartnerBattle.cpp` |
| `FSM_PartnerState` | `Include/FSM_PartnerState.h` | `Source/FSM_PartnerState.cpp` |
| `GravitySphere` | `Include/GravitySphere.h` | `Source/GravitySphere.cpp` |
| `Korsica_Wind` | `Include/Korsica_Wind.h` | `Source/Korsica_Wind.cpp` |
| `Partner` | `Include/Partner.h` | `Source/Partner.cpp` |
| `Partner_Character_Korsica` | `Include/Partner_Character_Korsica.h` | `Source/Partner_Character_Korsica.cpp` |
| `Partner_Character_Macaron` | `Include/Partner_Character_Macaron.h` | `Source/Partner_Character_Macaron.cpp` |
| `Partner_Character_Peppermint` | `Include/Partner_Character_Peppermint.h` | `Source/Partner_Character_Peppermint.cpp` |
| `Projectile_Peppermint_Bullet` | `Include/Projectile_Peppermint_Bullet.h` | `Source/Projectile_Peppermint_Bullet.cpp` |

> 전체 프로젝트의 빌드 의존성은 포함하지 않았으며, 구현 흐름을 확인하는 데 필요한 범위까지만 선별했습니다.
