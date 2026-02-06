#include "OverlayApp.h"
#include <windows.h>
#include <cstdio>
#include <openvr.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <thread>
#include <atomic>

using namespace vr;

bool OverlayApp::Init()
{


	std::system("python \"src\\getname.py\"");

	vr::EVRInitError VRIniterr = VRInitError_None;
	VRSystem = VR_Init(&VRIniterr, VRApplication_Overlay);

	if (VRIniterr != VRInitError_None) {
		return false;
	}

	printf("VR Initialized \n");

	Overlay = VROverlay();
	EVROverlayError OverlayErr = Overlay->CreateOverlay("com.anton.my_overlay", "My Overlay", &handle);

	if (OverlayErr != VROverlayError_None) {
		return false;
	}
	printf("Overlay Intialized \n");

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
	snprintf(CoverPath, MAX_PATH, "%s\\ChangingAssets\\cover.jpg", cwd);

	snprintf(TitlePath, MAX_PATH, "%s\\ChangingAssets\\now_playing.txt", cwd);

	Overlay->SetOverlayFromFile(handle, CoverPath);

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



void OverlayApp::UpdateNowPlaying()
{
	static auto next = std::chrono::steady_clock::now();

	if (std::chrono::steady_clock::now() < next)
	{
		return;
	}
	next = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
	std::thread([]()
		{
			std::system("python \"src\\getname.py\"");
		}).detach();

	std::wifstream  file(TitlePath);
	if (!file.is_open()) 
	{
		return;
	}

	OldPlaying.artist = CurrentPlaying.artist;
	OldPlaying.title = CurrentPlaying.title;

	std::getline(file, CurrentPlaying.title);
	std::getline(file, CurrentPlaying.artist);

	if (OldPlaying.artist != CurrentPlaying.artist and OldPlaying.title != CurrentPlaying.title)
	{
		wprintf(L"TITLE : [%ls]\n", CurrentPlaying.title.c_str());
		wprintf(L"ARTIST: [%ls]\n", CurrentPlaying.artist.c_str());
		Overlay->SetOverlayFromFile(handle, CoverPath);
	}	



}
