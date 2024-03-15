#pragma once

#include <deque>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <iomanip>
#include <algorithm>

#include <iostream>

#include "geo.h"

namespace catalogue
{
    namespace packets
    {
        struct StopInfo
        {
            StopInfo(bool found_, std::set<std::string> names_);
            
            bool found;
            std::set<std::string> names;
        };
        
        struct RouteInfo
        {
            RouteInfo(int stops_, int unique_stops_, double total_distance_);
            
            int stops, unique_stops;
            double total_distance;
        };
    }
    
    namespace manager
    {
  
//Initialization
struct Stop;
struct Bus;

struct Stop
{
    Stop(const std::string& name_, const geo::Coordinates& location_);
    
    std::string name;
    geo::Coordinates location;
};

struct Bus
{
    Bus(const std::string& name_)
    :name(name_){}
    
    std::string name;
    std::vector<Stop*> stops;
};

class TransportCatalogue 
{
	// Реализуйте класс самостоятельно
    public:
    
    void AddBus(const Bus& bus);
    void AddStop(const Stop& stop);
    
    Bus* FindBus(const std::string_view key) const;
    Stop* FindStop(const std::string_view key) const;
    
    packets::RouteInfo GetRouteInfo(std::string_view route) const;
    packets::StopInfo GetStopInfo(std::string_view stop) const;
    
    private:
    
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Stop*> name_to_stops_;

    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::deque<Bus> routes_;
};
    }
}