#include <imgui.h>
#include <limits>
#include <cmath>

#include "ValueHistory.h"

namespace ImGui 
{
    void PlotLines(const char* label, const ValueHistory<float> &values, int values_offset = 0, float maxGranity=0.1f, float minGranity=0.1f, ImVec2 graph_size = ImVec2(0,0) )
    {
        float max = std::numeric_limits<float>::min(),
              min = std::numeric_limits<float>::max();
              
        size_t size = values.getSize(),
               index = values.getIndex();
        const float *data = values.getPtr();
        
        for( size_t i=0; i < size; ++i ) {
            if( data[i] > max ) max = data[i];
            if( data[i] < min ) min = data[i];
        }
        
        if( maxGranity > 0 ) {
            max = std::ceil( max/maxGranity ) * maxGranity;
        }
        else {
            max = -maxGranity;
        }
        if( minGranity > 0 ) {
            min = std::floor( min/minGranity ) * minGranity;
        }
        else {
            min = -minGranity;
        }
        
        char buffer[128];
        std::snprintf( buffer, 128, "Max: %-10.3g\nMin: %-10.3g\nAvg: %-10.3g", values.getMax(), values.getMin(), values.getAvarage() );
        
        ImGui::PlotLines( label, data, size, index, buffer, min, max, graph_size );
    }
}