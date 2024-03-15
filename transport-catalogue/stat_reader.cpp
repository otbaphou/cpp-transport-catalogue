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
            
           std::tuple<int, int, double> data = transport_catalogue.GetRouteInfo(route);
            
           if(get<0>(data) == -1)
           {
               output << "not found\n"; 
           }else
           {
               output << get<0>(data) << " stops on route, " << get<1>(data)
               << " unique stops, " << get<2>(data) << " route length\n";
           }
           
        }
    
        if(request[0]=='S')
        {  
            const std::string_view stop = request.substr(offset, request.size()-offset);
            output << "Stop " << stop << ": ";
             
            std::pair<bool, std::set<std::string>> data = transport_catalogue.GetStopInfo(stop);
            
            if(!data.first)
            {
                output << "not found\n"; 
               
            }else
            {
                if(data.second.size() == 0)
                {
                    output << "no buses\n"; 
                    
                }else
                {
                    output << "buses "; 
                    
                for(auto iter = data.second.begin(); iter != data.second.end(); iter++) 
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