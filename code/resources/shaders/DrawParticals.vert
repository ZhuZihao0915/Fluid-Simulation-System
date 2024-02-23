
#version 330

layout(location = 0) in vec2 pointPosition;
layout(location = 1) in float density;

out float densNorm;

void main() {
    gl_PointSize = 4;
    gl_Position = vec4(pointPosition.xy, 0.0, 1.0);

    densNorm = density / 5000.0f;   // normalization
}
