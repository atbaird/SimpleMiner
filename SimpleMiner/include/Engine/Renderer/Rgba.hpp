#pragma once

#ifndef RGBA_HPP
#define RGBA_HPP

class Rgba
{
public:
	//constructors
	Rgba();
	Rgba(const float& red, const float& green, const float& blue, const float& alpha = 1.f);

	//setters
	void SetRGBA(const float& in_r, const float& in_g, const float& in_b, const float& in_a);
	void SetRGB(const float& in_r, const float& in_g, const float& in_b);
	void setAlpha(const float& in_a);

	//getters
	void GetRGBA(float& out_r, float& out_g, float& out_b, float& out_a) const;

	//variables
	float Red;
	float Green;
	float Blue;
	float Alpha;
};
#endif // !RGBA_HPP
