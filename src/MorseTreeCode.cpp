#include "MorseCodeTree.h"

std::unique_ptr<bool[]> morseBuffer(new bool[5]);
int morsePointer = 0;

MorseTreeCode::MorseTreeCode(char Character) : character(Character), dit(nullptr), dash(nullptr) {}

bool MorseTreeCode::IsCodeValid() const { return character != '\0'; }

char MorseTreeCode::GetCharacter() const { return character; }
MorseTreeCode* MorseTreeCode::MoveToDit() const { return dit.get(); }

MorseTreeCode* MorseTreeCode::MoveToDash() const { return dash.get(); }

void MorseTreeCode::AddDit(char newChar) { dit = std::make_unique<MorseTreeCode>(newChar); }

void MorseTreeCode::AddDash(char newChar) { dash = std::make_unique<MorseTreeCode>(newChar); }

std::unique_ptr<MorseTreeCode> BuildTree()
{
    auto root = std::make_unique<MorseTreeCode>('\0');

#pragma region Left-Branch

    root->AddDit('E');

#pragma region Left-B-L
    root->dit->AddDit('I');

    root->dit->dit->AddDit('S');

    root->dit->dit->dit->AddDit('H');

    root->dit->dit->dit->dit->AddDit('5');

    root->dit->dit->dit->dit->AddDash('4');

    root->dit->dit->dit->AddDash('V');

    root->dit->dit->dit->dash->AddDash('3');

    root->dit->dit->AddDash('U');

    root->dit->dit->dash->AddDash('\0');

    root->dit->dit->dash->dash->AddDash('2');

    root->dit->dit->dash->AddDit('F');
#pragma endregion

#pragma region Left-B-R

    root->dit->AddDash('A');

#pragma region Right-B-L
    root->dit->dash->AddDit('R');

    root->dit->dash->dit->AddDash('\0');

    root->dit->dash->dit->dash->AddDit('+');

    root->dit->dash->dit->AddDit('L');
#pragma endregion

#pragma region Right-B-R
    root->dit->dash->AddDash('W');

    root->dit->dash->dash->AddDit('P');

    root->dit->dash->dash->AddDash('J');

    root->dit->dash->dash->dash->AddDash('1');
#pragma endregion

#pragma endregion

#pragma endregion

#pragma region Right-Branch

    root->AddDash('T');

#pragma region Right-B-L

    root->dash->AddDit('N');

    root->dash->dit->AddDit('D');
    root->dash->dit->dit->AddDit('B');
    root->dash->dit->dit->dit->AddDit('6');
    root->dash->dit->dit->dit->AddDash('=');

    root->dash->dit->dit->AddDash('X');
    root->dash->dit->dit->dash->AddDit('/');

    root->dash->dit->AddDash('K');
    root->dash->dit->dash->AddDit('C');
    root->dash->dit->dash->AddDash('Y');
#pragma endregion

#pragma region Right-B-R
    root->dash->AddDash('M');

    root->dash->dash->AddDit('G');
    root->dash->dash->dit->AddDit('Z');
    root->dash->dash->dit->dit->AddDit('7');
    root->dash->dash->dit->AddDash('Q');

    root->dash->dash->AddDash('O');
    root->dash->dash->dash->AddDit('\0');
    root->dash->dash->dash->dit->AddDit('8');
    root->dash->dash->dash->AddDash('\0');
    root->dash->dash->dash->dash->AddDit('9');
    root->dash->dash->dash->dash->AddDash('0');

#pragma endregion

#pragma endregion

    return root;
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

void ProcessInput(MorseType __type)
{
    bool processBuffer = true;

    switch (__type)
    {
        case MorseType::DIT:
            morseBuffer[morsePointer] = false;
            break;

        case MorseType::DASH:
            morseBuffer[morsePointer] = true;
            break;

        case MorseType::PROCESS:
            if (morsePointer > 0) Reset_And_Parse_MorseBuffer();
            return;
    }

    if (processBuffer)
    {
        Beep(500, !morseBuffer[morsePointer] ? 100 : 400);
        
        printf(!morseBuffer[morsePointer] ? "." : "-");
        
        morsePointer += 1;
        
        if (morsePointer == 5) Reset_And_Parse_MorseBuffer();
    }
}

void Reset_And_Parse_MorseBuffer()
{
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