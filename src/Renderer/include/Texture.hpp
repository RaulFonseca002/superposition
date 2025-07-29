#pragma once
#include <glad/glad.h>
#include <iostream>

class Texture {

    public:

        unsigned int ID;
        Texture(const unsigned char* data, int width, int height, int nrComponents);
        virtual ~Texture();

        //pure method, does not change state in any form
        void bind() const;
};

