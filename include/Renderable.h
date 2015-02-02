#pragma once

class GpuBuffer;
class Material;
class VertexArrayObject;

class Renderable {
public:
    virtual ~Renderable() = default;
    
    virtual void render() = 0;
};