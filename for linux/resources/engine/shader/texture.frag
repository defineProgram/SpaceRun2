#version 410
		
layout(location = 0) in vec4 Color;
layout(location = 1) in vec2 Tex;
		
uniform sampler2D Tex0;
		
layout(location = 0) out vec4 FragColor;

layout(std140) uniform pscbSprite
{
	vec4 g_colorAdd;
	vec4 g_sdfParam;
};

vec4 OutputColor(const vec4 color)
{
	return color + g_colorAdd;
}
		
void main()
{
	FragColor = OutputColor(texture(Tex0, Tex) * Color);
}
