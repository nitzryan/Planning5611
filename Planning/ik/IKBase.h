#pragma once
#include "../rendering/SphereRenderable.h"

class IKBase :
    public SphereRenderable
{
public:
    IKBase(const Pos3F& center, float radius, const Material& material);

private:

};

