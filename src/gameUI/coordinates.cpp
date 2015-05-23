#include "main.h"

rect2f CoordinateFrame::toContainer(const rect2f &rect) const
{
	return rect2f(toContainer(rect.minB), toContainer(rect.maxB));
}
vec2f CoordinateFrame::toContainer(const vec2f &vec) const
{
	return vec2f(start.x + vec.x * frameScale.x, start.y + vec.y * frameScale.y);
}

rect2f CoordinateFrame::fromContainer(const rect2f &rect) const
{
	return rect2f(fromContainer(rect.minB), fromContainer(rect.maxB));
}
vec2f CoordinateFrame::fromContainer(const vec2f &vec) const
{
	return vec2f((vec.x - start.x) / frameScale.x, (vec.y - start.y) / frameScale.y);
}