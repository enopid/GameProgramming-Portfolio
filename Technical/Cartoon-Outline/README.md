# Cartoon Outline

Inverted Hull과 깊이·노멀 기반 후처리 외곽선에 사용한 셰이더입니다.

## 실행 결과

![시야각 변화에 따른 외곽선 보정](Media/outline-fov.gif)

## 포함 파일

| 구현 단위 | 헤더 | 구현 |
|---|---|---|
| `Shader_Defines` | — | `Shaders/Shader_Defines.hlsli` |
| `Shader_PostProcessing` | — | `Shaders/Shader_PostProcessing.hlsl` |
| `Shader_VtxMesh` | — | `Shaders/Shader_VtxMesh.hlsl` |

> 전체 프로젝트의 빌드 의존성은 포함하지 않았으며, 구현 흐름을 확인하는 데 필요한 범위까지만 선별했습니다.
