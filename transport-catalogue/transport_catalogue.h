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
    
    void AddBus(Bus& bus);
    
    void AddStop(Stop& stop);
    
    Bus* FindBus(const std::string_view& key);
    Bus* FindBus(const std::string_view& key) const;
    
    Stop* FindStop(const std::string_view& key);
    Stop* FindStop(const std::string_view& key) const;
    
    void GetRouteInfo(std::string_view route, std::ostream& output) const;
    void GetStopInfo(std::string_view stop, std::ostream& output) const;
    
    private:
    
    std::deque<Stop> stops_;
    std::unordered_map<std::string_view, Stop*> name_to_stops_;

    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::deque<Bus> routes_;
};
    }
}