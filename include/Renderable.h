#pragma once

class Renderer;


class Renderable 
{
public:
    virtual ~Renderable() = default;
    virtual void render( Renderer &renderer ) = 0;
};