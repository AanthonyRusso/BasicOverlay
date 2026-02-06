#pragma once

#include <windows.h>
#include <openvr.h>


struct NowPlaying
{
	std::wstring title;
	std::wstring artist;
};


class OverlayApp
{
public:
	bool Init();
	void Shutdown();
	bool UpdateOverlayPosition(float DistanceMeters);
	void UpdateNowPlaying();

private:



	vr::IVRSystem* VRSystem = nullptr;
	vr::IVROverlay* Overlay = nullptr;
	vr::VROverlayHandle_t handle = vr::k_ulOverlayHandleInvalid;
	char CoverPath[MAX_PATH];
	char TitlePath[MAX_PATH];
	NowPlaying CurrentPlaying;
	NowPlaying OldPlaying;

};

