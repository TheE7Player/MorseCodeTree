#include "SpacebarReceiver.h"
#include <Windows.h>

using Clock = std::chrono::time_point<std::chrono::steady_clock>;

void SpacebarReceiver::StartReceiver()
{
    bool KEYUP = false;

    std::unique_ptr<Clock> timeLastPress = nullptr;
    std::unique_ptr<Clock> pressEndTime = nullptr;
    std::unique_ptr<long long> durationTime = nullptr;

	while (true)
	{
        KEYUP = !(GetAsyncKeyState(SPACE_KEY) & SignificateBit_KEYDOWN);   

        if (!KEYUP && !keyPressDown)
        {
            keyPressDown.store(true);

            pressStartTime = std::make_unique<Clock>(std::chrono::steady_clock::now());         
        }
        
        pressEndTime = std::make_unique<Clock>(std::chrono::steady_clock::now());
        
        if (pressStartTime != nullptr)
        {
            durationTime = std::make_unique<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(*pressEndTime - *pressStartTime).count());
            
            if (keyPressDown && (*durationTime > SpacebarReceiver::DashDuration || KEYUP))
            {
                keyPressDown.store(false);
                MorseType type = *durationTime < SpacebarReceiver::DashDuration ? MorseType::DIT : MorseType::DASH;
                    
                pressStartTime.release();
                inputCallback(type);
                timeLastPress = std::make_unique<Clock>(std::chrono::steady_clock::now());
            }
        }
        else if (timeLastPress != nullptr)
        {
            durationTime = std::make_unique<long long>(std::chrono::duration_cast<std::chrono::seconds>(*pressEndTime - *timeLastPress).count());

            if (*durationTime > 1)
            {
                inputCallback(MorseType::PROCESS);
                timeLastPress.release();
            }
        }

        pressEndTime.release();
        durationTime.release();

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
};