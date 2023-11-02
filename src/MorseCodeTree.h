#pragma once
#include <memory>
#include "SpacebarReceiver.h"
#include <Windows.h>

struct MorseTreeCode {
    char character;

    std::unique_ptr<MorseTreeCode> dit;
    std::unique_ptr<MorseTreeCode> dash;

    MorseTreeCode(char Character);

    bool IsCodeValid() const;

    char GetCharacter() const;

    MorseTreeCode* MoveToDit() const;
    MorseTreeCode* MoveToDash() const;

    void AddDit(char newChar);
    void AddDash(char newChar);
};

std::unique_ptr<MorseTreeCode> BuildTree();

char ProcessMorseBuffer();
void Reset_And_Parse_MorseBuffer();
void ProcessInput(MorseType __type);