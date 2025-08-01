#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float exposure;

void main()
{ 
    const float gamma = 2.2;
    // 1. Sample the HDR color from our framebuffer texture
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
    
    // 2. Apply exposure control
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    
    // 3. Apply gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));
    
    FragColor = vec4(mapped, 1.0);
}
