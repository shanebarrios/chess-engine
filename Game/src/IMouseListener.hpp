#pragma once

class IMouseListener {
public:
    virtual ~IMouseListener() = default;
    virtual void onMouseMove(int x, int y) = 0;
    virtual void onMouseRelease(int x, int y) = 0;
    virtual void onMousePress(int x, int y) = 0;
};