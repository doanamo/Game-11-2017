#include "Precompiled.hpp"
#include "Sprite.hpp"
using namespace Graphics;

Sprite::Info::Info() :
    texture(nullptr),
    transparent(false),
    filter(true)
{
}

bool Sprite::Info::operator==(const Info& other) const
{
    return texture == other.texture && transparent == other.transparent && filter == other.filter;
}

bool Sprite::Info::operator!=(const Info& other) const
{
    return !this->operator==(other);
}

Sprite::Data::Data() :
    transform(1.0f),
    rectangle(0.0f, 0.0f, 1.0f, 1.0f),
    color(1.0f, 1.0f, 1.0f, 1.0f)
{
}
