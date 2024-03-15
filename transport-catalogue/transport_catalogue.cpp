#include "transport_catalogue.h"

namespace catalogue
{
    namespace manager
    {
  
Stop::Stop(const std::string& name_, const geo::Coordinates& location_)
:name(name_),
location(location_){}

void TransportCatalogue::AddBus(Bus& bus) 
{
    routes_.push_back(std::move(bus));
    Bus& tmp = routes_.back();
    
    busname_to_bus_.insert({tmp.name, &tmp});
}
    
void TransportCatalogue::AddStop(Stop& stop)
{
    stops_.push_back(std::move(stop));
    Stop& tmp = stops_.back();
    
    name_to_stops_.insert({tmp.name,  &tmp});
}
    
Bus* TransportCatalogue::FindBus(const std::string_view& key)
{
    if (busname_to_bus_.contains(key))
    return busname_to_bus_[key];
    else
    return nullptr;
}

Bus* TransportCatalogue::FindBus(const std::string_view& key) const
{
    if (busname_to_bus_.contains(key))
    return busname_to_bus_.at(key);
    else
    return nullptr;
}
    
Stop* TransportCatalogue::FindStop(const std::string_view& key)
{
    if (name_to_stops_.contains(key))
    return name_to_stops_[key];
    else
    return nullptr;
}

Stop* TransportCatalogue::FindStop(const std::string_view& key) const
{
    if (name_to_stops_.contains(key))
    return name_to_stops_.at(key);
    else
    return nullptr;
}
    
void TransportCatalogue::GetRouteInfo(std::string_view route, std::ostream& output) const
{
    output << "Bus " << route << ": ";

    Bus* ptr = FindBus(route);
    
    if(ptr == nullptr)
    {
        output << "not found\n"; 
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
        
        output << ptr->stops.size() << " stops on route, " 
        << std::set<Stop*>(ptr->stops.begin(), ptr->stops.end()).size() << " unique stops, "
        << total << " route length\n";
    }
}

void TransportCatalogue::GetStopInfo(std::string_view stop, std::ostream& output) const
{
    output << "Stop " << stop << ": ";

    Stop* ptr = FindStop(stop);
    
    if(ptr == nullptr)
    {
        output << "not found\n"; 
    }else
    {
        bool found = false;
        std::set<std::string> names;
        for(const Bus& bus : routes_)
        {
            
        auto iter = std::find(bus.stops.begin(), bus.stops.end(), ptr);
        if(iter != bus.stops.end())
        {
            if(!found)
            {
            output << "buses ";
            found = true;
            }
            
            names.insert(bus.name);
        }
            
        }
        
        if(!found)
        {
        output << "no buses\n";
        return;
        }
        
        for(auto iter = names.begin(); iter != names.end(); iter++)
        {
            output << *iter << ' ';
        }
            
        output << std::endl;
        
    }
}
    }
}