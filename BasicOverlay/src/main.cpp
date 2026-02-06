#include <openvr.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <windows.h>
#include "OverlayApp.h"

int main() {

	OverlayApp app;

	bool success = app.Init();

	if (!success) {
		std::printf("Failed to initiate overlay :c");
		return 1;
	}


	bool MoveOverlaySuccess;

	while (true) {
		MoveOverlaySuccess = app.UpdateOverlayPosition(0.5f);
		app.UpdateNowPlaying();
		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}

	app.Shutdown();

	return 0;

}