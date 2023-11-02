// [NOTE]: This was the original code before further improvements, THIS FILE IS NOT INCLUDED IN THE SOLUTION AVAILABLE

// MorseCodeTesting.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <string>
#include <windows.h>
#include <thread>
#include <mutex>

struct MorseTreeCode {
    char character;

    std::unique_ptr<MorseTreeCode> dit;
    std::unique_ptr<MorseTreeCode> dash;

    MorseTreeCode(char Character) : character(Character), dit(nullptr), dash(nullptr) {}

    bool IsCodeValid() const { return character != '\0'; }

    char GetCharacter() const { return character; }

    MorseTreeCode* MoveToDit() const { return dit.get(); }

    MorseTreeCode* MoveToDash() const { return dash.get(); }

    void AddDit(char newChar) {
        dit = std::make_unique<MorseTreeCode>(newChar);
        //return dit;
    }

    void AddDash(char newChar) {
        dash = std::make_unique<MorseTreeCode>(newChar);
        //return dash;
    }
};

std::unique_ptr<MorseTreeCode> BuildTree()
{
    auto root = std::make_unique<MorseTreeCode>('\0');

    #pragma region Left-Branch

    root->dit = new MorseTreeCode('E');
    
    #pragma region Left-B-L
    root->dit->dit = new MorseTreeCode('I');

    root->dit->dit->dit = new MorseTreeCode('S');

    root->dit->dit->dit->dit = new MorseTreeCode('H');

    root->dit->dit->dit->dit->dit = new MorseTreeCode('5');

    root->dit->dit->dit->dit->dash = new MorseTreeCode('4');

    root->dit->dit->dit->dash = new MorseTreeCode('V');

    root->dit->dit->dit->dash->dash = new MorseTreeCode('3');

    root->dit->dit->dash = new MorseTreeCode('U');

    root->dit->dit->dash->dash = new MorseTreeCode('\0');

    root->dit->dit->dash->dash->dash = new MorseTreeCode('2');

    root->dit->dit->dash->dit = new MorseTreeCode('F');
    #pragma endregion

    #pragma region Left-B-R

    root->dit->dash = new MorseTreeCode('A');

    #pragma region Right-B-L
    root->dit->dash->dit = new MorseTreeCode('R');

    root->dit->dash->dit->dash = new MorseTreeCode('\0');

    root->dit->dash->dit->dash->dit = new MorseTreeCode('+');

    root->dit->dash->dit->dit = new MorseTreeCode('L');
    #pragma endregion

    #pragma region Right-B-R
    root->dit->dash->dash = new MorseTreeCode('W');

    root->dit->dash->dash->dit = new MorseTreeCode('P');

    root->dit->dash->dash->dash = new MorseTreeCode('J');

    root->dit->dash->dash->dash->dash = new MorseTreeCode('1');
    #pragma endregion

    #pragma endregion

    #pragma endregion

    #pragma region Right-Branch
    
    root->dash = new MorseTreeCode('T');
   
    #pragma region Right-B-L

    root->dash->dit = new MorseTreeCode('N');

    root->dash->dit->dit = new MorseTreeCode('D');
    root->dash->dit->dit->dit = new MorseTreeCode('B');
    root->dash->dit->dit->dit->dit = new MorseTreeCode('6');
    root->dash->dit->dit->dit->dash = new MorseTreeCode('=');

    root->dash->dit->dit->dash = new MorseTreeCode('X');
    root->dash->dit->dit->dash->dit = new MorseTreeCode('/');

    root->dash->dit->dash = new MorseTreeCode('K');
    root->dash->dit->dash->dit = new MorseTreeCode('C');
    root->dash->dit->dash->dash = new MorseTreeCode('Y');
    #pragma endregion

    #pragma region Right-B-R
    root->dash->dash = new MorseTreeCode('M');

    root->dash->dash->dit = new MorseTreeCode('G');
    root->dash->dash->dit->dit = new MorseTreeCode('Z');
    root->dash->dash->dit->dit->dit = new MorseTreeCode('7');
    root->dash->dash->dit->dash = new MorseTreeCode('Q');

    root->dash->dash->dash = new MorseTreeCode('O');
    root->dash->dash->dash->dit = new MorseTreeCode('\0');
    root->dash->dash->dash->dit->dit = new MorseTreeCode('8');
    root->dash->dash->dash->dash = new MorseTreeCode('\0');
    root->dash->dash->dash->dash->dit = new MorseTreeCode('9');
    root->dash->dash->dash->dash->dash = new MorseTreeCode('0');

    #pragma endregion

    #pragma endregion

    return root;
}

using namespace std;

const int SPACE_KEY = VK_SPACE;
const int DOT_DURATION = 300;
const unsigned int SignificateBit_KEYDOWN = 0x8000;

bool LoopLock = false;

auto startTime = std::chrono::steady_clock::now();

std::unique_ptr<bool[]> morseBuffer(new bool[5]);
int morsePointer = 0;

long long getTimerDuration()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
}

char ProcessMorseBuffer()
{
    auto tree = BuildTree();

    auto move = tree.get();

    for (size_t i = 0; i < morsePointer; i++)
    {
        move = !morseBuffer[i] ? move->MoveToDit() : move->MoveToDash();
    }

    if (move == nullptr) return '\0';

    return move->IsCodeValid() ? move->GetCharacter() : '\0';
}

std::mutex parseMutexLock;

void Reset_And_Parse_MorseBuffer()
{
    std::lock_guard<std::mutex> lock(parseMutexLock);
    char output = ProcessMorseBuffer();

    morseBuffer[0] = false;
    morseBuffer[1] = false;
    morseBuffer[2] = false;
    morseBuffer[3] = false;
    morseBuffer[4] = false;

    if (output != '\0')
    {
        printf("%*c\n", 10 - morsePointer, output);
        Beep(700, 100);
    }
    else
    {
        printf("%*s\n", 16 - morsePointer, "INVALID");
        Beep(900, 150);
    }

    morsePointer = 0;
}

void Process_OnKeyUp()
{
    int forceBufferDurationLimit = DOT_DURATION * 6;

    while (true)
    {
        if (LoopLock)
        {
            LoopLock = false;

            auto duration = getTimerDuration();

            morseBuffer[morsePointer] = duration < DOT_DURATION ? false : true;

            Beep(500, !morseBuffer[morsePointer] ? 100 : 400);
            printf(!morseBuffer[morsePointer] ? "." : "-");
            morsePointer += 1;

            if (morsePointer == 5) { Reset_And_Parse_MorseBuffer(); }
        }
        else
        {
            if (morsePointer > 0 && (getTimerDuration() > forceBufferDurationLimit))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                Reset_And_Parse_MorseBuffer();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void Process_OnKeyDown()
{
    bool KeyDownLock = false;
    bool KEYUP = false;

    while (true)
    {
        KEYUP = !(GetAsyncKeyState(SPACE_KEY) & SignificateBit_KEYDOWN);
        
        if (!KEYUP && !KeyDownLock)
        {
            KeyDownLock = true;
            startTime = std::chrono::steady_clock::now();
        }
        
        if (KEYUP && KeyDownLock) 
        { 
            KeyDownLock = false;
            if(!LoopLock) LoopLock = true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

int main()
{
    std::thread pressUp(Process_OnKeyUp);
    std::thread pressDown(Process_OnKeyDown);

    pressUp.join(); pressDown.join();
    
    return 0;
}