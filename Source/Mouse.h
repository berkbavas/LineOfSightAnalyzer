#pragma once

namespace LineOfSightAnalyzer
{
    class Mouse
    {
    public:
        Mouse();

    public:
        bool mPressed;
        float mX;
        float mY;
        float mDx;
        float mDy;
    };
}
