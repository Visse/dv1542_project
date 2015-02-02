#pragma once

#define DEBUG_LEVEL_NONE 0
#define DEBUG_LEVEL_NORMAL 1
#define DEBUG_LEVEL_HIGH 2


#ifndef DEBUG_LEVEL
#   define DEBUG_LEVEL DEBUG_LEVEL_NORMAL
#endif

#define DEBUG_CHECK( level, expression )

#if DEBUG_LEVEL >= DEBUG_LEVEL_NORMAL
#   define USE_DEBUG_NORMAL
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_HIGH
#   define USE_DEBUG_HIGH
#endif
