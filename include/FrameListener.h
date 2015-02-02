#pragma once

class Camera;


class FrameListener {
public:
    virtual ~FrameListener() = default;
    
    virtual void onFrameBegun() {}
    virtual void onFrameEnded() {}
    
    
    virtual void onPreCameraDraw( Camera *camera ) {}
    virtual void onPostCameraDraw( Camera *camera ) {}
};