#version 330 core

in vec2 textureCoordsOut;

out vec4 colour;

uniform sampler2D texture0;

float getLuminance(vec3 colour)
{
	return (colour.r+colour.g+colour.b)/3.0f;
}

void main()
{
	vec4 textureColour=texture(texture0,textureCoordsOut);
	float luminance=getLuminance(textureColour.rgb);
	colour=vec4(luminance,luminance,luminance,1.0f);
}