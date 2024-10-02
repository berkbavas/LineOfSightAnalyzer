#pragma once

#include "Node/Renderable.h"

namespace LineOfSightAnalyzer
{
    class Quad : public Renderable
    {
      public:
        Quad() = default;

        void Construct() override;
        void Render() override;
        void Destroy() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };
}
