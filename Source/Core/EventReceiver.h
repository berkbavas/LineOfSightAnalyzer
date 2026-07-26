#pragma once

#include <QEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

namespace LineOfSightAnalyzer
{
    class EventReceiver
    {
      public:
        EventReceiver() = default;
        virtual ~EventReceiver() = default;

        virtual bool OnKeyPressed(QKeyEvent* pEvent);
        virtual bool OnKeyReleased(QKeyEvent* pEvent);
        virtual bool OnMousePressed(QMouseEvent* pEvent);
        virtual bool OnMouseReleased(QMouseEvent* pEvent);
        virtual bool OnMouseMoved(QMouseEvent* pEvent);
        virtual bool OnWheelMoved(QWheelEvent* pEvent);
        virtual bool OnLeaveEvent(QEvent* pEvent);
        virtual bool OnFocusOutEvent(QFocusEvent* pEvent);
    };
}