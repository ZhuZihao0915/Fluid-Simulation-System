#version 330 core
out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D mTexture;

void main()
{
    // ��ȡ��������ֵ
    vec2 pixelSize = vec2(1.0 / textureSize(mTexture, 0));
    vec2 uv = texCoord - mod(texCoord, pixelSize);
    vec3 pixelColor = texture(mTexture, uv).rgb;

    // ������ػ������ɫ
    FragColor = vec4(pixelColor, 1.0);
}