#pragma once

#include <QByteArray>
#include <QString>

namespace LineOfSightAnalyzer
{
    class Util
    {
      public:
        Util() = delete;

        static QByteArray GetBytes(const QString& path);
    };
}