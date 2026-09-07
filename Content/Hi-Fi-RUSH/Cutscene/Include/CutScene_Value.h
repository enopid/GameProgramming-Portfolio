#pragma once
#include"Asset.h"
namespace Client
{
	static Engine::_bool m_bTrash = { false };

	static Engine::_uint m_uiOne = { 1 };
	static Engine::_uint m_uiTwo = { 2 };

	static Engine::_float m_fTrash = { 0.f };

	static Engine::_float3 m_vWhite = { 1.f,1.f,1.f };
	static Engine::_float3 m_vGray = { 0.6f,0.6f,0.6f };
	static Engine::_float3 m_vDouble = { 2.f,2.f,2.f };
	static Engine::_float3 m_vPlatformMarkColor = { 0.431f,0.51f,0.62f };
	static Engine::_float3 m_vKaleCoreRed = { 1.f,0.25f,0.f };
	static Engine::_float3 m_vKaleCorePlus = { 0.f,0.2f,0.f };
	static Engine::_float3 m_vKaleWeaponBlue = { 0.4f,0.831f,1.f };
	static Engine::_float3 m_vKaleWeaponSky = { 0.718f,0.992f,1.f };
	static Engine::_float3 m_vKaleWeaponRed = { 1.f, 0.361f, 0.384f };
	static Engine::_float3 m_vKaleWeaponOrange = { 1.f,0.953f,0.373f };
	static Engine::_float3 m_vGuitarSky	= { 0.f,0.f,0.f };
	static Engine::_float3 m_vOriginBlue = { 0.f,0.f,0.f };

	static Engine::SP_TextureAsset m_spPlatform;
	static Engine::SP_TextureAsset m_spKaleCore;
	static Engine::SP_TextureAsset m_spKaleCoreRed;
	static Engine::SP_TextureAsset m_spArmCore;
	static Engine::SP_TextureAsset m_spArmCircle;
}