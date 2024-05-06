#pragma once
#include <string>

class Timer
{
    public:
        Timer(std::string* greets, int amount, float duration, const int& faceIndex); // colors, position


        void Update(float deltaTime);
        std::string GetLine();
        float GetProgress();
        float GetLetterProgress();


        float elapsed;
        float textSpeed;
        float duration;
        float letterIndex;
        int amount;
        int faceIndex;
        int greetsIndex = 0;
        std::string* greets;
};