#version 330 core
out vec4 FragColor;

in float ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D mTexture;

void main()
{
	FragColor = texture(mTexture, TexCoord) * vec4(ourColor*3);
}