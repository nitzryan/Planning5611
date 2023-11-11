#pragma once

#include "ColorRGBA.h"

struct Material {
	ColorRGBA color;
	float ka, kd, ks, specPower;
	int textureID;

	Material() : color(0,0,0,1){
		ka = 0;
		kd = 0;
		ks = 0;
		specPower = 10;
		textureID = -1;
	}
};