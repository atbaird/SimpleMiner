#include "Engine/Renderer/Rgba.hpp"

//constructors
Rgba::Rgba()
{
	//constructor
	Red = 0.f;
	Green = 0.f;
	Blue = 0.f;
	Alpha = 0.f;
}
Rgba::Rgba(const float& red, const float& green, const float& blue, const float& alpha)
{
	//constructor 2
	Red = red;
	Green = green;
	Blue = blue;
	Alpha = alpha;
}

//setters
void Rgba::SetRGBA(const float& in_r, const float& in_g, const float& in_b, const float& in_a)
{
	//sets Red, Green, Blue and Alpha values
	Red = in_r;
	Green = in_g;
	Blue = in_b;
	Alpha = in_a;
}

void Rgba::SetRGB(const float& in_r, const float& in_g, const float& in_b)
{
	//sets just the Red, Green and Blue colors
	Red = in_r;
	Green = in_g;
	Blue = in_b;
}
void Rgba::setAlpha(const float& in_a)
{
	//sets the alpha
	Alpha = in_a;
}

//getters
void Rgba::GetRGBA(float& out_r, float& out_g, float& out_b, float& out_a) const
{
	out_r = Red;
	out_g = Green;
	out_b = Blue;
	out_a = Alpha;
}