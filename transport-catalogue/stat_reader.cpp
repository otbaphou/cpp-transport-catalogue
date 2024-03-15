#include "stat_reader.h"

namespace catalogue
{
    namespace out
    {
  
void ParseAndPrintStat(const manager::TransportCatalogue& transport_catalogue, 
std::string_view request, std::ostream& output) 
{
    if(request[0]=='B')
    transport_catalogue.GetRouteInfo(request.substr(4, request.size()-4), output);
    if(request[0]=='S')
    transport_catalogue.GetStopInfo(request.substr(5, request.size()-5), output);
}
    }
}