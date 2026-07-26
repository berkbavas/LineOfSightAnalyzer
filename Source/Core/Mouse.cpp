#include "Mouse.h"

#include <cmath>

void LineOfSightAnalyzer::Mouse::SetButtonPressed(Qt::MouseButton Button, bool Pressed)
{
    mButtonPressedStates[Button] = Pressed;
}

bool LineOfSightAnalyzer::Mouse::IsButtonPressed(Qt::MouseButton Button) const
{
    return mButtonPressedStates.value(Button, false);
}

void LineOfSightAnalyzer::Mouse::SetLastPressPosition(Qt::MouseButton Button, const QPointF &Position)
{
    mButtonLastPressPositions[Button] = Position;
}

QPointF LineOfSightAnalyzer::Mouse::GetLastPressPosition(Qt::MouseButton Button) const
{
    return mButtonLastPressPositions.value(Button, QPointF());
}

void LineOfSightAnalyzer::Mouse::AddCumulativeMovement(Qt::MouseButton Button, const QPointF &Movement)
{
    mButtonCumulativeMovements[Button] += Movement;
}

void LineOfSightAnalyzer::Mouse::ConsumeCumulativeMovement(Qt::MouseButton Button, const QPointF &Movement)
{
    mButtonCumulativeMovements[Button] -= Movement;
}

QPointF LineOfSightAnalyzer::Mouse::GetCumulativeMovement(Qt::MouseButton Button) const
{
    return mButtonCumulativeMovements.value(Button, QPointF());
}

void LineOfSightAnalyzer::Mouse::ResetCumulativeMovementIfAlmostZero(Qt::MouseButton Button)
{
    const auto Movement = mButtonCumulativeMovements.value(Button, QPointF());

    if (std::abs(Movement.x()) < 0.1f)
    {
        mButtonCumulativeMovements[Button] = QPointF(0, Movement.y());
    }

    if (std::abs(Movement.y()) < 0.1f)
    {
        mButtonCumulativeMovements[Button] = QPointF(Movement.x(), 0);
    }
}

void LineOfSightAnalyzer::Mouse::ResetCumulativeMovement(Qt::MouseButton Button)
{
    mButtonCumulativeMovements[Button] = QPointF();
}

void LineOfSightAnalyzer::Mouse::Reset()
{
    mButtonPressedStates.clear();
    mButtonLastPressPositions.clear();
    mButtonCumulativeMovements.clear();
}
