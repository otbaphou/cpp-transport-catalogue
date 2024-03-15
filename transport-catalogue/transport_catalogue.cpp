#include "transport_catalogue.h"

namespace catalogue
{
    namespace packets
    {
        StopInfo::StopInfo(bool found_, std::set<std::string> names_)
        :found(found_), names(names_){}
        
        RouteInfo::RouteInfo(int stops_, int unique_stops_, double total_distance_)
        :stops(stops_), unique_stops(unique_stops_), total_distance(total_distance_){}
    }
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
    
packets::RouteInfo TransportCatalogue::GetRouteInfo(std::string_view route) const
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
        
        return {(int)ptr->stops.size(), (int)std::set<Stop*>(ptr->stops.begin(), ptr->stops.end()).size(), total};
    }
}

packets::StopInfo TransportCatalogue::GetStopInfo(std::string_view stop) const
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