#version 330 core
out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D mTexture;


void apply(){
    vec3 centerPixel = texture(mTexture, texCoord).rgb;

	mat3 kernel = mat3(
        -0.1, -0.1, -0.1,
        -0.1,  1.3, -0.1,
        -0.1, -0.1, -0.1
    );
    
    // 锐化处理
    vec3 sharpColor = centerPixel + 
        kernel[0][0] * texture(mTexture, texCoord + vec2(-1.0, -1.0)).rgb +
        kernel[0][1] * texture(mTexture, texCoord + vec2(0.0, -1.0)).rgb +
        kernel[0][2] * texture(mTexture, texCoord + vec2(1.0, -1.0)).rgb +
        kernel[1][0] * texture(mTexture, texCoord + vec2(-1.0, 0.0)).rgb +
        kernel[1][1] * centerPixel +
        kernel[1][2] * texture(mTexture, texCoord + vec2(1.0, 0.0)).rgb +
        kernel[2][0] * texture(mTexture, texCoord + vec2(-1.0, 1.0)).rgb +
        kernel[2][1] * texture(mTexture, texCoord + vec2(0.0, 1.0)).rgb +
        kernel[2][2] * texture(mTexture, texCoord + vec2(1.0, 1.0)).rgb;
    
    sharpColor = min(sharpColor, vec3(1.0));

    // 输出锐化后的颜色
    FragColor = vec4(sharpColor, 1.0);
}

void main()
{
	vec3 centerPixel = texture(mTexture, texCoord).rgb;

	mat3 kernel = mat3(
        -0.1, -0.1, -0.1,
        -0.1,  1.3, -0.1,
        -0.1, -0.1, -0.1
    );
    
    // 锐化处理
    vec3 sharpColor = centerPixel + 
        kernel[0][0] * texture(mTexture, texCoord + vec2(-1.0, -1.0)).rgb +
        kernel[0][1] * texture(mTexture, texCoord + vec2(0.0, -1.0)).rgb +
        kernel[0][2] * texture(mTexture, texCoord + vec2(1.0, -1.0)).rgb +
        kernel[1][0] * texture(mTexture, texCoord + vec2(-1.0, 0.0)).rgb +
        kernel[1][1] * centerPixel +
        kernel[1][2] * texture(mTexture, texCoord + vec2(1.0, 0.0)).rgb +
        kernel[2][0] * texture(mTexture, texCoord + vec2(-1.0, 1.0)).rgb +
        kernel[2][1] * texture(mTexture, texCoord + vec2(0.0, 1.0)).rgb +
        kernel[2][2] * texture(mTexture, texCoord + vec2(1.0, 1.0)).rgb;
    
    sharpColor = min(sharpColor, vec3(1.0));

    // 输出锐化后的颜色
    FragColor = vec4(sharpColor, 1.0);
}