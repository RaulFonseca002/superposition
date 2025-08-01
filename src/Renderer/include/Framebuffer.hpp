#pragma once

class Framebuffer {
public:

    unsigned int FBO;
    unsigned int textureColorBuffer;

    Framebuffer(int width, int height);
    ~Framebuffer();

    void bind();
    void unbind();

private:
    unsigned int RBO;
};