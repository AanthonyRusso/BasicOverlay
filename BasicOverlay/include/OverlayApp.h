#pragma once

#include <openvr.h>

struct NowPlaying
{
	std::wstring title;
	std::wstring artist;
	std::wstring album;
};


class OverlayApp
{
public:
	bool Init();
	void Shutdown();
	bool UpdateOverlayPosition(float DistanceMeters);

private:


	vr::IVRSystem* VRSystem = nullptr;
	vr::IVROverlay* Overlay = nullptr;
	vr::VROverlayHandle_t handle = vr::k_ulOverlayHandleInvalid;
	NowPlaying CurrentPlaying;
	
};

