#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 texCoord;

out vec3 ourColor;
out vec3 TexCoord;
vec4 v1 = vec4( 1.0, 0.0f, 0.0f, 0.0f);
vec4 v2 = vec4( 0.0, cos(45), -sin(45), 0.0);
vec4 v3 = vec4( 0.0, sin(45), cos(45), 0.0);
vec4 v4 = vec4( 0.0, 0.0, 1.0, 1.0);

mat4 rotate = mat4(v1, v2, v3, v4);

mat4 r2 = mat4(
cos(45), 0.0, sin(45), 0.0,
0.0, 1.0, 0.0, 0.0,
-sin(45), 0.0, cos(45), 0.0,
0.0, 0.0 ,0.0, 1.0
);

mat4 r3 = mat4(
1.0, 0.0, 0.0, 0.0,
0.0, 1.0, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.0, 0.0 ,0.0, 1.0
);

uniform mat4 tc_rotate;
uniform mat4 otho_mat;
uniform float z_position;
void main()
{
	gl_Position = tc_rotate * vec4(position, z_position, 1.0f);
//	vec4 rr = r2 * r;
//	gl_Position = r3 * rr;
//gl_Position = vec4(position, 0.5f, 1.0f);
	ourColor = color;
//	ourColor.a = 1.0f;
	TexCoord = vec3(texCoord.x,1- texCoord.y, z_position + 0.5f);
	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	// TexCoord = texCoord;
//	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
