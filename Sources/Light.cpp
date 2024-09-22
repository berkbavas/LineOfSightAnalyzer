#include "Light.h"

LineOfSightAnalyzer::Light::Light()
    : mDirection(0, -1, 0)
    , mColor(1.0f, 1.0f, 1.0f, 1.0f)
    , mAmbient(0.25f)
    , mDiffuse(0.75f)
    , mSpecular(1.0f)
{
    mDirection.normalize();
}