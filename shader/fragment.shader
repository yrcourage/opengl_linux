#version 330 core
in vec3 ourColor;
in vec3 TexCoord;

out vec4  color;
//out vec4 color;

// Texture samplers
uniform sampler3D tex;

void main()
{
	// Linearly interpolate between both textures (second texture is only slightly combined)
//	color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);
	//color = vec4(0.0, 0.0, 0.1, 1.0);
//	color = vec4(0.6, 0.0,0.0,1.0);
//    color = texture(tex, TexCoord).rgba;
    vec4 c = texture(tex, TexCoord);
    if(!(c.r<120.0f/256))
    {
        color = vec4(0.0,0.0,0.0, 0.2f);
    }

     else
    color = vec4(c.r, c.g, c.b, 1.0f);
//     color = vec4(gl_FragColor.rgb, 0.0f);
//     color.a = ourColor.a;
//    gl_FragColor = color;
}
