#include "transport_catalogue.h"

namespace catalogue
{
    namespace manager
    {
  
Stop::Stop(const std::string& name_, const geo::Coordinates& location_)
:name(name_),
location(location_){}

void TransportCatalogue::AddBus(const Bus& bus) 
{
    routes_.push_back(std::move(bus));
    Bus& tmp = routes_.back();
    
    busname_to_bus_.insert({tmp.name, &tmp});
}
    
void TransportCatalogue::AddStop(const Stop& stop)
{
    stops_.push_back(std::move(stop));
    Stop& tmp = stops_.back();
    
    name_to_stops_.insert({tmp.name,  &tmp});
}

Bus* TransportCatalogue::FindBus(const std::string_view key) const
{
    if (busname_to_bus_.contains(key))
    return busname_to_bus_.at(key);
    else
    return nullptr;
}

Stop* TransportCatalogue::FindStop(const std::string_view key) const
{
    if (name_to_stops_.contains(key))
    return name_to_stops_.at(key);
    else
    return nullptr;
}
    
std::tuple<int, int, double> TransportCatalogue::GetRouteInfo(std::string_view route) const
{

    Bus* ptr = FindBus(route);
    
    if(ptr == nullptr)
    {
        return {-1, 0, 0};
    }else
    {
        double total = 0;
        
        Stop* prev = nullptr;
        for(Stop* stop : ptr->stops)
        {
            
            if(prev!=NULL)
            {
                total += ComputeDistance(prev->location, stop->location);
                
                prev = stop;
            }else
            {
                prev = stop;
            }
        }
        
        return {ptr->stops.size(), std::set<Stop*>(ptr->stops.begin(), ptr->stops.end()).size(), total};
    }
}

std::pair<bool, std::set<std::string>> TransportCatalogue::GetStopInfo(std::string_view stop) const
{
    Stop* ptr = FindStop(stop);
    
    if(ptr == nullptr)
    {
        return {false, {}};
        
    }else
    {
        std::set<std::string> names;
        
        for(const Bus& bus : routes_)
        {
            auto iter = std::find(bus.stops.begin(), bus.stops.end(), ptr);
            if(iter != bus.stops.end())
            {
                names.insert(bus.name);
            }
        }
            
        return {true, names};
    }
}
    }
}