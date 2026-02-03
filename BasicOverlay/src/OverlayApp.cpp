#include "OverlayApp.h"
#include <windows.h>
#include <openvr.h>

using namespace vr;

bool OverlayApp::Init()
{
	vr::EVRInitError VRIniterr = VRInitError_None;
	VRSystem = VR_Init(&VRIniterr, VRApplication_Overlay);

	if (VRIniterr != VRInitError_None) {
		return false;
	}

	Overlay = VROverlay();
	EVROverlayError OverlayErr = Overlay->CreateOverlay("com.ashes.my_overlay", "My Overlay", &handle);

	if (OverlayErr != VROverlayError_None) {
		return false;
	}

	Overlay->SetOverlayWidthInMeters(handle, .5);

	Overlay->SetOverlayFlag(handle, VROverlayFlags_MakeOverlaysInteractiveIfVisible, true);
	Overlay->SetOverlayFlag(handle, VROverlayFlags_SendVRSmoothScrollEvents, true);
	Overlay->SetOverlayFlag(handle, VROverlayFlags_EnableClickStabilization, true);

	Overlay->SetOverlayFlag(handle, VROverlayFlags_VisibleInDashboard, false);
	Overlay->SetOverlayFlag(handle, VROverlayFlags_WantsModalBehavior, false);

	Overlay->SetOverlaySortOrder(handle, 1000);
	Overlay->SetOverlayAlpha(handle, .2f);
	Overlay->SetOverlayColor(handle, 1.0f, 1.0f, 1.0f);


	char cwd[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, cwd);
	printf("CWD: %s\n", cwd);
	char FullPath[MAX_PATH];
	snprintf(FullPath, MAX_PATH, "%s\\Assets\\whitebackground.png", cwd);

	Overlay->SetOverlayFromFile(handle, FullPath);

	Overlay->ShowOverlay(handle);


	return true;

}

void OverlayApp::Shutdown()
{
	if (VRSystem) {
		vr::VR_Shutdown();
	}
}

bool OverlayApp::UpdateOverlayPosition(float DistanceMeters)
{
	if (!VRSystem || !Overlay || handle == k_ulOverlayHandleInvalid)
		return false;
	TrackedDevicePose_t poses[k_unMaxTrackedDeviceCount];
	
	//Get Photons
	float fSecondsSinceLastVsync;
	VRSystem->GetTimeSinceLastVsync(&fSecondsSinceLastVsync, NULL);

	float fDisplayFrequency = VRSystem->GetFloatTrackedDeviceProperty(k_unTrackedDeviceIndex_Hmd, Prop_DisplayFrequency_Float);
	float fFrameDuration = 1.f / fDisplayFrequency;
	float fVsyncToPhotons = VRSystem->GetFloatTrackedDeviceProperty(k_unTrackedDeviceIndex_Hmd, Prop_SecondsFromVsyncToPhotons_Float);
		
	float fPredictedSecondsFromNow = fFrameDuration - fSecondsSinceLastVsync + fVsyncToPhotons;


	VRSystem->GetDeviceToAbsoluteTrackingPose(TrackingUniverseStanding, fPredictedSecondsFromNow, poses, k_unMaxTrackedDeviceCount);

	const TrackedDevicePose_t& hmd = poses[k_unTrackedDeviceIndex_Hmd];
	if (!hmd.bPoseIsValid)
		return false;
	const HmdMatrix34_t& hmdPose = hmd.mDeviceToAbsoluteTracking;

	HmdMatrix34_t overlayTransform = hmdPose;

	overlayTransform.m[0][3] += -hmdPose.m[0][2] * DistanceMeters;
	overlayTransform.m[1][3] += -hmdPose.m[1][2] * DistanceMeters;
	overlayTransform.m[2][3] += -hmdPose.m[2][2] * DistanceMeters;
	
	EVROverlayError te = Overlay->SetOverlayTransformAbsolute(handle, TrackingUniverseStanding, &overlayTransform);

	if (te != VROverlayError_None) {
		return false;
	}

	return true;
}
