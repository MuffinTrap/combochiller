#include "timer.h"
#include <cmath>
#include <mgdl-wii.h>

        
Timer::Timer(std::string* greets, int amount, float duration, const int& faceIndex, float rotationDuration, float rotationSpeed) // colors, position()
{

    this->amount=amount;
    this->duration = duration;
    this->greets = greets;
    this->faceIndex = faceIndex;
    this->rotationDuration = rotationDuration;
    this->rotationSpeed = rotationSpeed;

    // Calculate
    int totalLetters = 0;
    for (int i = 0; i < amount; i++)
    {
        totalLetters += greets[i].length();
    }
    // 5 letters, 10 s duration, 2 s per letter
    // How many letters per second  5/10 = 0.5
    this->textSpeed = (float)totalLetters/duration;

    // TODO
    // Minimum text speed?

    greetsIndex = 0;
    letterIndex = 0.0f;
    this->elapsed = 0.0f;
}

void Timer::Update(float deltaTime)
{
    elapsed += deltaTime;
    letterIndex += textSpeed * deltaTime;
    if (letterIndex >= greets[greetsIndex].length())
    {
        greetsIndex += 1;
        letterIndex = 0.0f;
    }
}

std::string Timer::GetLine()
{
    if (greetsIndex<amount)
    {
        return greets[greetsIndex];
    }
    else
    {
        gdl_assert(false, "No more lines left");
        return "";
    }
}

float Timer::GetProgress()
{
    return elapsed / duration;
}

float Timer::GetLetterProgress()
{
    float prog = letterIndex - std::floor(letterIndex);
    return prog;
}

float Timer::GetFaceRotation()
{
    if (elapsed <= rotationDuration)
    {
        // Goes towards zero so that
        // rotation always ends on 0.0

                // Full rotation done            Rotation so far
        return rotationDuration*rotationSpeed - elapsed* rotationSpeed;
    }
    else
    {
        return 0.0f;
    }
}