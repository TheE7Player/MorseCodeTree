# Morse Code Project [C++]
> Do not use this guide as a definite C++ tutorial, this project is to experiment with the language with an interesting solution to a problem. The code provided may or may not use techniques or skillsets that experienced C++ developers may implement or prove upon.

Goal: To provide a C++ solution which can read and interpreter single-letter Morse Code.
Provided code is in: `/src` folder.

Toolset:
- Windows 10
- Visual Studio 2022 Preview
- C++11 ( Windows target only )

> As stated, the solution is only aimed for Windows systems, due to using `GetAsyncKeyState` and key states from the `<Windows.h>` library. Feel free to customize to suit to either another operating system target, or cross-platform solution.

![morse code output from terminal](https://github.com/TheE7Player/MorseCodeTree/blob/main/resources/c++_output.jpg?raw=true)

## Aims
[1] Learn C++ by applying a solution to a problem
[2] Solve the problem without using a simple solution (apply a more difficult data structure)
[3] Make it efficient enough, but not too extreme in optimisation (favour readability, if possible)
[4] Prove that anyone without hard-knowledge of `C++` can program in it (from a `C#` programmer aspect)

## Additional Knowledge
The first draft was completed within 4 hours. Further work has been made to split it into multiple files and utilise a more better solution for an input thread loop. ChatGPT was only used in areas of confusion, or where a C++ optimisation warning suggested another way of writing the code (around 5- 15% usage). Total days to complete it efficiently was 4 days.

> This guide will be more descriptive based than tutorial, as it will help you understand my faults and areas of how you can improve solutions if you plan and experiment with multiple ideas.

# 1.0 Use-case of a Binary Node Tree
I was always interested into how you can translate dits and dashes (`.`; `-`) to parse it into characters. The first step I took was to look for a good existing solution to see how they tackled the problem, as well as looking to see how it was done traditionally.

I came across [MarcScott](https://github.com/raspberrypilearning/morse-code-virtual-radio "Morse Code Virtual Radio")'s RaspberryPi learning project, which allows you to use an actual Morse Key to input the pulses. From my experience in College and University from Data Structures & Algorithms, it was made clear that a Binary Tree Structure can be used to parse the pulses into their occurring characters.

The first thought was to use binary to interpret the characters, which a dit has a value of `0`, and dash has a value of `1`. Originally, it was only meant to process numbers and letters (`[A-Z0-9]`). Meaning I'll need to have a byte array of length 5 to accommodate all possible patterns (or an integer range of 0 to 31).

So the blueprint for the tree initially looked as:
```c++
char character;
std::bitset<5> number; // Bit array of N length: 5

MorseTreeCode* left;
MorseTreeCode* right;
```
But with further tests, this may lead to unwanted size. The `character` made since, but the `number` felt unnecessary. Another problem meant that reoccurring pulses did result in the same value number!
Take the following set for example: 
```c++
CHARACTER (PULSE)	  16 8 4 2 1
E         (.    )  =   0 0 0 0 0 = 0
I         (..   )  =   0 0 0 0 0 = 0
S         (...  )  =   0 0 0 0 0 = 0
H         (.... )  =   0 0 0 0 0 = 0
5         (.....)  =   0 0 0 0 0 = 0
```
It doesn't matter what direction you adjust, it all results in the same number despite being different. But this is not the same for the dashes, as these holds 1 bit for each dash character.
```c++
CHARACTER (PULSE)	  16 8 4 2 1
T         (-    )  =   0 0 0 0 1 = 1
M         (--   )  =   0 0 0 1 1 = 3
O         (---  )  =   0 0 1 1 1 = 7
INVALID   (---- )  =   0 1 1 1 1 = 15
0         (-----)  =   1 1 1 1 1 = 31
```
Therefore, we cannot justify using bits unless we utilise an integer which holds up to 8 bytes (128) - but that means we hold more data for it not to be used. We can justify an array buffer, but the solution to track our current stage is already present within the data structure we have utilised!

So we can finalise the struct of our Morse Code Tree being:
```c++
// ./src/MorseCodeTree.h
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
```
In later versions of C++, smart pointers (`std::unique_ptr<T>`) helps to manage objects in the heap. Meaning we don't really need to understand the garbage collector handling as such; but never use this without learning how do it manually! There is no overhead of using these over standard pointers.

The `const` keyboard at the end of most functions stated within the structure tells the compiler and these typically won't change the object that is being called for (VS IntelliSense suggested).

The methods and functions outside of the structure relate to the data type.

If you're not use to `C++` code, logic is usually split between headers (`.h`) and code files (`.cpp`). In simple terms, the header just tells the linker what you required to be linked between classes and libraries, for example. The best way is to treat headers as if it were contract, or an `Interface` in `C#` - a requirement of variables and functions one code needs to implement.
> Please take time with self-learning of header files, as the project goes more linker issues will appear if not taking carefully. Learn the best practice of header files in the early stages to prevent bad habits later.

## 1.1 Generation of Nodes
We've got our structure settled, but what about the data? Well, there could have been a more efficient way of populating the tree - but I did it manually. Yeah... all 43 nodes with a depth of 5 layers... manually tracked and tested. Have fun looking at it in `/src/MorseTreeCode.cpp [19-120]`. Here is an example of its structure:
```c++
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
```
We identify an invalid pulse pattern by using the null character `\0`, which is the roots default value and any other invalid pattern - but is required by some to allow the travel to the next depth, which would be a number or symbol.

Notice I changed the branches from (`left`;`right`) to (`dit`;`dash`) to make readability easier, but it also adds an interesting look into the pulse pattern to get to that character.
For example:
```
    root->dit->dit->dash->AddDit('F');
           .    .    -      .          =   F
    root->dash->dash->dash->dash->AddDit('9');
           -     -     -     -      .           = 9
```

I tracked each node and its adjacent children to ensure all nodes possibilities where written (I stopped highlighting near the end):
![Binary Tree Morse Decoding](https://github.com/TheE7Player/MorseCodeTree/blob/main/resources/mt_progress.jpg?raw=true)

## 1.2 Look-Flow for the Tree (Find the character for the pulses)
Let's investigate the `ProcessMoreBuffer` function from `/src/MorseTreeCode.cpp` from line 122.
```c++
char ProcessMorseBuffer()
{
    auto tree = BuildTree();  // 1.0

    auto move = tree.get();   // 1.1

    for (size_t i = 0; i < morsePointer; i++)  // 1.2
    {
        move = !morseBuffer[i] ? move->MoveToDit() : move->MoveToDash(); // 1.3
    }

    if (move == nullptr) return '\0'; // 1.4

    return move->IsCodeValid() ? move->GetCharacter() : '\0'; // 1.5
}
```
`1.0`: We first generate our tree, this will be a pointer to the tree (`std::unique_ptr<MorseTreeCode>`).
`1.1`: We take a new copy of this tree, this will allow us to iterate though each pulse possible
`1.2`: We iterate though all the pulse characters possible (minimum: 1, maximum: 5) 
`1.3`: If the `morseBuffer` at index `[i]` is a dit (false), we move into the `dit` branch, `dash` branch if not
`1.4`: If the current node we landed on is non-existent or empty ( `C++` has this as `nullptr`, equivalent to `null` in `C#` )
`1.5`: If the current node is valid (has a character that is not `\0`) then return that character, if not return a null character (`\0`)

All it took was 7 lines of code for us to walk or flow around the tree to get the characters we need from the pulses. We can likely reduce it further to around 5, but if it works - it stays.

## 1.3 Morse Buffer
Near the top of `/src/MorseTreeCode.cpp`, we have two variables that are important to denote the character we want to parse:
```c++
std::unique_ptr<bool[]> morseBuffer(new bool[5]);
int morsePointer = 0;
```
We have a:
- Array buffer of size 5, which is the maximum units allowed for each morse code.
- Array pointer, which allows us to track how many pulses (units) are present to parse through

`ProcessInput` uses this to track each unit, with an enum which can denote when we're using a pulse, or a process command (when < 5 is reached, but the user hasn't inserted a character in a while):
```c++
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
```
The `MorseType::PROCESS` only gets called on key press up from the space bar, this allows the program to allow inputs that are less than 5 to be accepted as a response, while ignoring empty requests. If the enum type is a dit or dash, we use the `Beep` method to create a sound of a morse code and display the symbol to the console screen. If we hit 5 from an on-going request, we reset and parse the parser right away.

Finally, the `Reset_And_Parse_MorseBuffer()` method resets the `morseBuffer` and `morsePointer` back to its default position, and attempts to tree-walk to get the character from the user input:
```c++
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
```
The approach of manually setting each index in this method is a style choice more than readability. We can save only a few instructions to reset each index manually - although most computers will see no performance details, and this process could be done by the compiler in `RELEASE` mode if it believes its more performant.

## 2.0 Threading (For Keyboard Input Loop)
It only makes sense for a keyboard thread loop to detect and calculate differences for the parser. If we had it in a single thread, the Beep sounds may interrupt the console updates, or key press timings. This means even if we press the keyboard with the Beep sound taking priority, our times are likely unaffected and will continue to differentiate between dit and dash timings.

Let's first looking into the previous solution which had two threads instead of one:
```c++
// FIRST REVISION OF KEYBOARD INPUT THREAD(S)
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
```
To enable the threads to run a wait, we use the following:
```c++
int main()
{
    std::thread pressUp(Process_OnKeyUp);
    std::thread pressDown(Process_OnKeyDown);

    pressUp.join(); pressDown.join();
    
    return 0;
}
```
The `join()` method simply tells the caller to wait until the thread is released, i.e finished.
It is likely unnecessary to have two threads waiting, as both do not have breakout clauses (infinite loop).

This approach raises many issues:
- Usage of multiple block guards (`KeyDownLock`, `KEYUP`, `LoopLock`) to handle the logic
- Not likely thread-safe ( race conditions, due to two threads )
- Likely timing issues between threads in most cases (not severe, but may lead to unwanted behaviours)

Here is a diagram of the old, and new approach:

![Old threading keyloop](![InputLoop_Original.png](https://github.com/TheE7Player/MorseCodeTree/blob/main/resources/InputLoop_Original.png?raw=true)

![New threading keyloop](![InputLoop_New.png](https://github.com/TheE7Player/MorseCodeTree/blob/main/resources/InputLoop_New.png?raw=true)

Noticeable changes compared to the first revision approach:
- All in one thread approach, more control, and less resource heavy
- Usage of an `enum` (pre-defined constants) to distinct between types (`::DIT`, `::DASH`,  `::PROCESS`). The approach solves two problems of readability, and key-press delay issues.
- Using only two guards, one coupled to the structure (`std::atomic<bool> keyPressDown`, thread safe boolean!), and one local to the thread scope (`KEYUP`) for key press detection.
- Moved duration into the thread scope, as well adding a new time duration variable to handle during of last key press input (to invoke `::PROCESS` after 1 second of no inputs, instead of an multiplication delay)

## 2.1 Spacebar Receiver Logic

Let's take a look into our structure in `./src/SpacebarReceiver.h`:
```c++
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
```
and how we call it from `./src/MorseCodeTree.cpp`:
```c++
#include "MorseCodeTree.h"

const int DOT_DURATION = 300;

int main()
{
    auto MorseCodeReceiver = new SpacebarReceiver(ProcessInput, DOT_DURATION);

    MorseCodeReceiver->WaitReceiver();

    return 0;
}
```

We use a function pointer `void (*)(MorseType)` from an named alias (`PressDurationCallback`; used for variable: `inputCallback`). This allows the structure to be flexible and not be constraint to a fixed method.
> The IDE suggests this approach is not recommended, and to use either a template or std::function.
> This is a valid argument, but these approaches might not be available to most compilers available, so feel free to adjust accordingly.

Now let's go over our new thread logic in `./src/SpacebarReceiver.cpp`:
```c++
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
```
Although it looks like more has been added (that's true), the logic is more thought out and robust compared to what it was originally written. Let's breakdown each section:

### 2.1.0 Thread Scope Variables
To save any troubles and spending too long create variables per cycle, it is a good practice to assign your variables before the loop if you read and write from them frequently.
```c++
 bool KEYUP = false;

 std::unique_ptr<Clock> timeLastPress = nullptr;
 std::unique_ptr<Clock> pressEndTime = nullptr;
 std::unique_ptr<long long> durationTime = nullptr;
```
The unique pointers are assigned null (`nullptr`) initially. This solution allows the algorithm to identify when an input was given initially. A regular problem was without pressing any buttons would active a singular dit. This meant it was not possible to invoke either the E character, as each pulse increments the pointer. We use the `nullptr` as a guard to let the algorithm know if input was acknowledged or interacted with the user.

Therefore, 4 clock variables are used for process and unit detection for the Morse code:
`SpacebarReceiver::pressStartTime`: Time where the first press of the space bar was detected (time when first keydown press)
`timeLastPress`: Time duration since last keyboard press (time since keyup press of the spacebar is received)
`pressEndTime`: Time of where last cycle was given
`durationTime`: Variable which holds the milliseconds or seconds of two-time intervals

### 2.1.1 Key Press Detection and Cycle Duration

```c++
// 1.0
KEYUP =  !(GetAsyncKeyState(SPACE_KEY) & SignificateBit_KEYDOWN);  

// 1.1
if  (!KEYUP &&  !keyPressDown)  
{ 
	keyPressDown.store(true); 
	pressStartTime = std::make_unique<Clock>(std::chrono::steady_clock::now());
} 

// 1.2
pressEndTime = std::make_unique<Clock>(std::chrono::steady_clock::now());
```
`1.0`:
We start by detecting if the spacebar at the occurring cycle is pressed down. We use the Bitwise AND operator (&) and compare it, to see if the Signification Bit is active (meaning keypress is down, not up). We then wrap this around a boolean flip with brackets, as we want to validate if the key is currently in an up state.
 > Logic is based on key state up, instead of the traditional way of detecting of the key state is down. Either method works, just different approaches to the solution. 
 
`1.1`:
We then compare if both:
A) If the spacebar is currently pressed down :: `!KEYUP`
B) If the atomic boolean is not set to true :: `!keyPressDown`

If both A and B evaluate to true, we then set the atomic boolean to true, and track the current time to `pressStartTime`.

The second conditional (B) will help prevent double fires within the starting time. A `mutex` could be introduced instead of this approach.

`1.2`:
We then track the pressEndTime, this approach will prevent this variable from being used more than once per cycle (due to two end-goals). This is used to A) calculate time interval to find if it should process as a dit or a dash and B) if the last user input duration is 1 second or longer.

### 2.1.2 Dit and Dash detection (with Auto-Dash input)
> Auto-Dash will force the pulse to be a dash (-) if the keypress is longer than the dash duration value, to prevent the user from just holding the spacebar and get no visual feedback from the console.
```c++
// 1.0
if (pressStartTime != nullptr)
{
    // 1.1
	durationTime = std::make_unique<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(*pressEndTime - *pressStartTime).count());
    
    // 1.2.1                                   1.2.2
    if (keyPressDown && (*durationTime > SpacebarReceiver::DashDuration || KEYUP))
    {
        // 1.3
        keyPressDown.store(false);
        
        // 1.4
        MorseType type = *durationTime < SpacebarReceiver::DashDuration ? MorseType::DIT : MorseType::DASH;
        
        // 1.5
        pressStartTime.release();
        
        // 1.6
        inputCallback(type);
        
        // 1.7
        timeLastPress = std::make_unique<Clock>(std::chrono::steady_clock::now());
    }
    // ... ... goto 2.1.3
}
```

`1.0`:
We first check to see if an input has been recognised, we know this by evaluating if the pointer is not empty (`nullptr`).

`1.1`:
We then calculate the duration while the spacebar is currently pressed down. The pointer will always retain the time since first press if we are still holding it down. We want the response in milliseconds, and the result to be a `long long` variable for precision. The `.count()` will return a number, of the duration. 

`1.2`:
To acknowledge an input to be verified, we need to have two conditions satisfied:
A) Ensure the atomic boolean has been set to true since last cycle
B) Either:
   1. The duration time during the current cycle was longer than the dash limit duration
   2. Or the key state was no longer pressed down (to acknowledge a dit pulse)

`1.3`:
We now set the atomic boolean to false, meaning we are satisfied with the result, and can either resolve a pulse of a dit or a dash.

`1.4`:
Using a ternary operator (`?`), we compare against the duration limit duration of a dash to evaluate if the pulse is a dit or a dash.

`1.5`:
We release the unique pointer, which dereferences the ownership of the pointer, and automatically sets it back to null (`nullptr`).

`1.6`:
We then call the function pointer which allows the console to accept the pulse and show the symbol and sound back to the user, from the console

`1.7`:
We then store the current time for `timeLastPress`, as this acknowledgement now means we've no longer have the spacebar pressed (to allow morse pulses less than equal to 5).

### 2.1.3 Auto-Process and Cycle Reset Logic
```c++
// 1.0
else if (timeLastPress != nullptr)
{
    // 1.1
    durationTime = std::make_unique<long long>(std::chrono::duration_cast<std::chrono::seconds>(*pressEndTime - *timeLastPress).count());

	// 1.2
    if (*durationTime > 1)
    {
        inputCallback(MorseType::PROCESS);
        timeLastPress.release();
    }
}

// 1.3
pressEndTime.release();
durationTime.release();

// 1.4
std::this_thread::sleep_for(std::chrono::milliseconds(20));
```
`1.0`:
If keypress is not pressed down, and `timeLastPress` has been assigned (to prevent non-keypress or accidental fires when first loading), start to buffer process as we can assume that the buffer has at least 1 or more units to be processed

`1.1`:
We calculate the duration since the last keypress time, in seconds and not milliseconds.

`1.2`:
If the calculation results in a number higher than 1, then we can call the functional pointer to invoke the buffer to start processing the 1 to 4 units stored.

`1.3`:
We reset the ownership of our smart pointers, so we cannot re-fire after the cycle. Therefore, we ensure no actions are taking place until the user interacts with the space bar. This may solve the double-firing cycle during the re-write of this logic.

`1.4`:
Finally, we tell the thread running the algorithm to sleep for 20 milliseconds to prevent timing issues or thread unresponsiveness.

# Conclusion
The re-write resulted in a 26.23% higher code base (amount of lines), but with a more robust and precise approach from the first attempt. It is important to take time outside of programming to solve and come up with solutions. With more thoughtful thinking and planning, this project wouldn't have need to take 4 days to be completed; optimistically, could be done in a single day (or 9 hours cap).

Learning a programming language from a problem is an fantastic way of learning how a language operates in terms of abstractions, keywords and benefits.

From this I know learned:
- Smart Pointers with C++ (`std::unqiue_ptr<T>`, `std::make_unique<T>`)
- Simplifying type aliases with `using` keyword
- Threading, usage of mutexes (first attempt), and atomic variables
- Separating logic into headers and code files

It is a very crucial ability to learn the main concepts first before learning any language. Such as data structures, algorithms, threading, etc. It helps to speed up the process and helps you to appreciate how each language achieves a problem.
