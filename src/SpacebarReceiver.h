#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <memory>

enum class MorseType { DIT, DASH, PROCESS };

const int SPACE_KEY = 0x20; //VK_SPACE (Windows: <windows.h>)
const unsigned int SignificateBit_KEYDOWN = 0x8000;

using PressDurationCallback = void (*)(MorseType); // Function pointer

struct SpacebarReceiver {
	SpacebarReceiver(PressDurationCallback callback, int DashDetectionTime, int DashDelayWaitSeconds = 1) :
		inputCallback(callback), 
		DashDuration(DashDetectionTime), 
		DashDurationDelay(DashDelayWaitSeconds)
	{
		keyboardThread = std::make_unique<std::thread>(&SpacebarReceiver::StartReceiver, this);
	}

	PressDurationCallback inputCallback;
	
	std::atomic<bool> keyPressDown;
	std::unique_ptr<std::thread> keyboardThread;

	const int DashDuration;
	const int DashDurationDelay;

	std::unique_ptr <std::chrono::time_point<std::chrono::steady_clock>> pressStartTime;
	
	void StartReceiver();

public:
	void WaitReceiver() {
		if (keyboardThread == nullptr) return;
		if (keyboardThread.get()->joinable()) keyboardThread.get()->join();
	}
};