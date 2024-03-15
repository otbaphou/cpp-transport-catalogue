#include "stat_reader.h"

namespace catalogue
{
    namespace out
    {
  
void ParseAndPrintStat(const manager::TransportCatalogue& transport_catalogue, 
std::string_view request, std::ostream& output) 
{
    size_t offset = request.find(' ') + 1;
    if(offset != std::string::npos)
    {
        if(request[0]=='B')
        {    
           const std::string_view route = request.substr(offset, request.size()-offset);
           output << "Bus " << route << ": ";
            
           packets::RouteInfo data = transport_catalogue.GetRouteInfo(route);
            
           if(data.stops == -1)
           {
               output << "not found\n"; 
               
           }else
           {
               output << data.stops << " stops on route, " << data.unique_stops
               << " unique stops, " << data.total_distance << " route length\n";
           }
           
        }
    
        if(request[0]=='S')
        {  
            const std::string_view stop = request.substr(offset, request.size()-offset);
            output << "Stop " << stop << ": ";
             
            packets::StopInfo data = transport_catalogue.GetStopInfo(stop);
            
            if(!data.found)
            {
                output << "not found\n"; 
               
            }else
            {
                if(data.names.size() == 0)
                {
                    output << "no buses\n"; 
                    
                }else
                {
                    output << "buses "; 
                    
                for(auto iter = data.names.begin(); iter != data.names.end(); iter++) 
                { 
                    output << *iter << ' '; 
                }
                
                output << std::endl; 
                }
            }
        }
    }
}
    }
}