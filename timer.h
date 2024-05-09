#pragma once
#include <string>
#include <vector>
#include "fx.h"

enum LineFX
{
    LineFX_NONE = 0,
    LineFX_NULL = 1,
    LineFX_FRUITS = 2
};

struct LineEffect
{
    int lineIndex;
    LineFX effect;
    u_int color;

    LineEffect(int i, LineFX fx, u_int c);
};

class Timer
{
    public:
        Timer(std::string* greets, int amount, int showAmount, float duration, const int& faceIndex,
        float rotationDuration, float rotationSpeed, float endDelay = 1.0f);
        void SetFX(FX effect);
        void AddLineEffect(LineEffect effect);
        void ResetTimers();


        void Update(float deltaTime);
        std::string GetLine();
        std::string GetLineAt(int relativeIndex);
        std::string GetLineEx(int exactIndex);
        float GetProgress();
        float GetLetterProgress();
        float GetFaceRotation();

        LineFX GetLineFXAt(int index);
        const LineEffect& GetLineEffectAt(int index);
        const LineEffect& GetLineEffectEx(int index);

        std::vector<LineEffect> lineEffects;
        Vector2 facePositionTarget;
        Vector2 textCenter;

        FX effect = FXnone;
        float elapsed;
        float textSpeed;
        float duration;
        float letterIndex;
        float rotationDuration;
        float rotationSpeed;
        float endDelay;
        int amount;
        int showAmount; // How many lines to show at once
        int faceIndex;
        int greetsIndex = 0;
        std::string* greets;
};