#include "timer.h"
#include <cmath>
#include <mgdl-wii.h>

LineEffect::LineEffect(int i, LineFX fx, u_int c)
{
    lineIndex = i;
    effect = fx;
    color = c;
}

        
Timer::Timer(std::string* greets, int amount, int showAmount, float duration, const int& faceIndex, float rotationDuration, float rotationSpeed) // colors, position()
{
    this->amount=amount;
    this->showAmount = showAmount;
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

    // TODO add little delay to end
    this->textSpeed = (float)totalLetters/(duration-0.3f);

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

void Timer::AddLineEffect(LineEffect fx)
{
    lineEffects.push_back(fx);
}

LineFX Timer::GetLineFXAt(int relativeIndex)
{
    int index = greetsIndex + relativeIndex;
    for(u_int i = 0; i < lineEffects.size(); i++)
    {
        if (lineEffects[i].lineIndex == index)
        {
            return lineEffects[i].effect;
        }
    }
    return LineFX_NONE;
}

const LineEffect& Timer::GetLineEffectAt(int relativeIndex)
{
    int index = greetsIndex + relativeIndex;
    return GetLineEffectEx(index);
}

const LineEffect& Timer::GetLineEffectEx(int index)
{
    for(u_int i = 0; i < lineEffects.size(); i++)
    {
        if (lineEffects[i].lineIndex == index)
        {
            return lineEffects[i];
        }
    }
    gdl_assert(false, "No lineEffect for line %d", index);
    static LineEffect dummy = LineEffect(0, LineFX_NONE, 0);
    return dummy;
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

std::string Timer::GetLineAt(int relativeIndex)
{
    int i = greetsIndex + relativeIndex;
    if (i >=0 && i < amount)
    {
        return greets[i];
    }
    else
    {
        return "";
    }
}

std::string Timer::GetLineEx(int exactIndex)
{
    int i = exactIndex;
    if (i >=0 && i < amount)
    {
        return greets[i];
    }
    else
    {
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