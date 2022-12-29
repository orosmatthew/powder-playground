#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

uniform vec3 u_resolution;

void main() {
    float pi = 3.14159265358979323846;

    float directions = 16.0;
    float quality = 3.0;
    float size = 6.0;

    vec2 res = vec2(1200, 900);

    vec2 radius = size / res;

    vec2 uv = vec2(fragTexCoord.x, 1 - fragTexCoord.y);

    vec4 color = texture(texture0, uv);

    for (float d = 0.0; d < (pi * 2); d += ((pi * 2) / directions)) {
        for (float i = (1.0 / quality); i <= 1.0; i += (1.0 / quality)) {
            color += texture(texture0, uv + vec2(cos(d), sin(d)) * radius * i);
        }
    }

    color /= quality * directions - 15.0;
    finalColor = color;
}