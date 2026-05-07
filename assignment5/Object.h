#pragma once
#include "Vector2D.h"

class Object
{
public:
    virtual ~Object() = default;
    virtual Vector2D getPosition() const = 0;
};