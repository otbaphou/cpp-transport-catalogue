#pragma once

#include <deque>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <set>
#include <iomanip>
#include <algorithm>
#include <set>

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
            RouteInfo(int stops_, int unique_stops_, double total_distance_, double true_total_distance);

            int stops;
            int unique_stops;
            double total_distance;
            double true_total_distance;
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
            Bus(const std::string& name_, bool roundtrip)
                :name(name_), is_roundtrip(roundtrip) {}

            std::string name;
            std::vector<Stop*> stops;
            std::vector<Stop*> EndPoint;
            bool is_roundtrip;
        };

        class StopPairHasher {
        public:

            size_t operator()(const std::pair<Stop*, Stop*>& pair_) const
            {
                return std::hash<Stop*>{}(pair_.first) ^ std::hash<Stop*>{}(pair_.second);
            }

        };

        class TransportCatalogue
        {
            // Реализуйте класс самостоятельно
        public:

            void AddBus(const Bus& bus);
            void AddStop(const Stop& stop);

            void AddDistance(const std::pair<Stop*, Stop*>& stops, const int distance);

            Bus* FindBus(const std::string_view key) const;
            Stop* FindStop(const std::string_view key) const;


            std::vector<Bus*> GetBusPointerList() const;

            std::vector<geo::Coordinates> GetRouteLocations(const Bus* route) const;
            std::vector<geo::Coordinates> GetAllStopsLocations() const;
            std::vector<Stop*> GetAllStops() const;

            packets::RouteInfo GetRouteInfo(std::string_view route) const;
            packets::StopInfo GetStopInfo(std::string_view stop) const;

        private:

            std::deque<Stop> stops_;
            std::unordered_map<std::string_view, Stop*> name_to_stops_;

            std::unordered_map<std::string_view, Bus*> busname_to_bus_;
            std::deque<Bus> routes_;

            std::unordered_map<std::pair<Stop*, Stop*>, int, StopPairHasher> stop_pair_to_distance_;
        };
    }
}