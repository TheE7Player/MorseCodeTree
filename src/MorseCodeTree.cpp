#include "MorseCodeTree.h"

const int DOT_DURATION = 300;

int main()
{
    auto MorseCodeReceiver = new SpacebarReceiver(ProcessInput, DOT_DURATION);

    MorseCodeReceiver->WaitReceiver();

    return 0;
}