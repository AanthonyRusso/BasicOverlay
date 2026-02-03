#pragma once

#include <openvr.h>
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
	
};

