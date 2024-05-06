#include "timer.h"
#include <cmath>

        
Timer::Timer(std::string* greets, int amount, float duration, const int& faceIndex) // colors, position()
{

    this->amount=amount;
    this->duration = duration;
    this->greets = greets;
    this->faceIndex = faceIndex;

    // Calculate
    int totalLetters = 0;
    for (int i = 0; i < amount; i++)
    {
        totalLetters += greets[i].length();
    }
    // 5 letters, 10 s duration, 2 s per letter
    // How many letters per second  5/10 = 0.5
    this->textSpeed = (float)totalLetters/duration;

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
        return greets[amount-1];
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