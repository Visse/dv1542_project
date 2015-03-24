#pragma once

#include <string>

typedef unsigned int RenderQueueId;


enum RenderQueue : RenderQueueId
{
    RQ_DeferredFirst = 0,
    RQ_DeferredDefault = 5,
    RQ_DeferredLast  = 10,
    
    RQ_LightFirst = 11,
    RQ_Light = 13,
    RQ_LightLast = 15,
    
    RQ_OverlayFirst = 16,
    RQ_Overlay = 18,
    RQ_OverlayLast = 20,
    
    RQ_Count
};

enum class BlendMode {
    Replace,
    AlphaBlend,
    AddjectiveBlend
};

enum class DepthCheck {
    None,
    Less,
    Equal,
    Greater,
    
    LessEqual,
    GreaterEqual
};

enum class DrawMode {
    Points,
    Triangles
};

enum class LogSeverity {
    Debug = 0,
    Information,
    Warning,
    Error,
    Critical,
    
    
    COUNT
    
};

RenderQueueId renderQueueFromString( const std::string &str );
BlendMode blendModeFromString( const std::string &str );
DepthCheck depthCheckFromString( const std::string &str );

LogSeverity logSeverityFromString( const std::string &str );
std::string logSeverityToString( LogSeverity severity );