#pragma once
#include <string>
#include "fx.h"

class Timer
{
    public:
        Timer(std::string* greets, int amount, float duration, const int& faceIndex,
        float rotationDuration = 0.0f, float rotationSpeed = 0.0f); // colors, position


        void Update(float deltaTime);
        std::string GetLine();
        float GetProgress();
        float GetLetterProgress();
        float GetFaceRotation();


        FX effect;
        float elapsed;
        float textSpeed;
        float duration;
        float letterIndex;
        float rotationDuration;
        float rotationSpeed;
        int amount;
        int faceIndex;
        int greetsIndex = 0;
        std::string* greets;
};