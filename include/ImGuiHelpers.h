#include <imgui.h>
#include <limits>
#include <cmath>

#include "ValueHistory.h"
#include "FixedSizeTypes.h"

namespace ImGui 
{
    void PlotLines(const char* label, const ValueHistory<float> &values, float maxGranity=0.1f, float minGranity=0.1f, ImVec2 graph_size = ImVec2(0,0) )
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
        // visual studio didn't support snprintf :(, but since I'm using fixed with arguments, sprintf should be safe :)
        std::sprintf( buffer, "Max: %-10.3g\nMin: %-10.3g\nAvg: %-10.3g", values.getMax(), values.getMin(), values.getAvarage() );
        
        ImGui::PlotLines( label, data, size, index, buffer, min, max, graph_size );
    }

    
    void PlotLines(const char* label, const ValueHistory<int> &values, int maxGranity=10, int minGranity=10, ImVec2 graph_size = ImVec2(0,0) )
    {
        ValueHistory<float> tmp = values.convertTo<float>();
        
        int max = std::numeric_limits<int>::min(),
            min = std::numeric_limits<int>::max();
              
        size_t size = values.getSize(),
               index = values.getIndex();
        const int *data = values.getPtr();
        
        for( size_t i=0; i < size; ++i ) {
            if( data[i] > max ) max = data[i];
            if( data[i] < min ) min = data[i];
        }
        
        if( maxGranity > 0 ) {
            max = ((max+maxGranity-1)/maxGranity) * maxGranity;
        }
        else {
            max = -maxGranity;
        }
        if( minGranity > 0 ) {
            min = (min/minGranity) * minGranity;
        }
        else {
            min = -minGranity;
        }
        
        char buffer[128];
        // visual studio didn't support snprintf :(, but since I'm using fixed with arguments, sprintf should be safe :)
        std::sprintf( buffer, "Max: %-10i\nMin: %-10i\nAvg: %-10i", values.getMax(), values.getMin(), values.getAvarage() );
        
        auto valueGetter = []( void* data, int idx ) -> float {
            return ((int*)data)[idx];
        };
        
        ImGui::PlotLines( label, valueGetter, const_cast<int*>(data), size, index, buffer, min, max, graph_size );
    }

    template< typename Type, typename Getter >
    void PlotLines( const char* label, Getter &&values, size_t size, Type maxGranity, Type minGranity, ImVec2 graph_size )
    {
        Type max = std::numeric_limits<Type>::min(),
             min = std::numeric_limits<Type>::max(),
             avarage = 0;

        for( size_t i=0; i < size; ++i ) {
            Type value = values(i);
            if( value > max ) max = value;
            if( value < min ) min = value;
            avarage += value;
        }
        
        if( maxGranity > 0 ) {
            max = ((max+maxGranity-1)/maxGranity) * maxGranity;
        }
        else {
            max = -maxGranity;
        }
        if( minGranity > 0 ) {
            min = (min/minGranity) * minGranity;
        }
        else {
            min = -minGranity;
        }
        
        char buffer[128];
        // visual studio didn't support snprintf :(, but since I'm using fixed with arguments, sprintf should be safe :)
        std::sprintf( buffer, "Max: %-10f\nMin: %-10f\nAvg: %-10f", (float)max, (float)min, (float)avarage/(float)size );
        
        float (*valueGetter)(void*,int) = []( void* data, int idx ) -> float {
            return (float)(*reinterpret_cast<Getter*>(data))(idx);
        };
        
        ImGui::PlotLines( label, valueGetter, reinterpret_cast<void*>(&values), size, 0, buffer, min, max, graph_size );
    }

    
    
}