#include "transport_catalogue.h"

namespace catalogue
{
    namespace packets
    {
        StopInfo::StopInfo(bool found_, std::set<std::string> names_)
            :found(found_), names(names_) {}

        BusInfo::BusInfo(int stops_, int unique_stops_, double total_distance_, double true_total_distance_)
            :stops(stops_), unique_stops(unique_stops_), total_distance(total_distance_),
            true_total_distance(true_total_distance_) {}
    }
    namespace manager
    {

        Stop::Stop(const std::string& name_, const geo::Coordinates& location_)
            :name(name_),
            location(location_) {}

        void TransportCatalogue::AddBus(const Bus& bus)
        {
            buses_.push_back(std::move(bus));
            Bus& tmp = buses_.back();

            busname_to_bus_.insert({ tmp.name, &tmp });
        }

        void TransportCatalogue::AddStop(const Stop& stop)
        {
            stops_.push_back(std::move(stop));
            Stop& tmp = stops_.back();

            name_to_stops_.insert({ tmp.name,  &tmp });
        }

        void TransportCatalogue::AddDistance(const std::pair<Stop*, Stop*>& stops, const int distance)
        {
            stop_pair_to_distance_.insert({ stops, distance });
        }

        int TransportCatalogue::GetDistance(std::pair<Stop*, Stop*> stops) const
        {
            if (stop_pair_to_distance_.contains(stops))
            {
                return stop_pair_to_distance_.at(stops);
            }
            else
            {
                if (stop_pair_to_distance_.contains({ stops.second, stops.first }))
                {
                    return stop_pair_to_distance_.at({ stops.second, stops.first });
                }
                else
                {
                    return -1;
                }
            }
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

        packets::BusInfo TransportCatalogue::GetBusInfo(std::string_view route) const
        {

            Bus* ptr = FindBus(route);

            if (ptr == nullptr)
            {
                return { -1, 0, 0, 0 };
            }
            else
            {
                double total = 0;
                double total_true = 0;

                Stop* prev = nullptr;
                for (Stop* stop : ptr->stops)
                {

                    if (prev != NULL)
                    {
                        if (stop_pair_to_distance_.contains({ prev, stop }))
                        {
                            total += stop_pair_to_distance_.at({ prev, stop });

                        }
                        else
                        {
                            total += stop_pair_to_distance_.at({ stop, prev });
                        }

                        total_true += ComputeDistance(prev->location, stop->location);

                        prev = stop;

                    }
                    else
                    {
                        prev = stop;
                    }
                }
                
                if(!ptr->is_roundtrip)
                {
                    int offset = 0;
                    for (auto stop_iter = ptr->stops.rbegin(); stop_iter != ptr->stops.rend(); ++stop_iter)
                    {
                        if (prev != NULL && !(prev == *stop_iter))
                        {
                            if (stop_pair_to_distance_.contains({ prev, *stop_iter }))
                            {
                                total += stop_pair_to_distance_.at({ prev, *stop_iter });
                                ++offset;
                            }
                            else
                            {
                                if (stop_pair_to_distance_.contains({ *stop_iter, prev }))
                                {
                                    total += stop_pair_to_distance_.at({ *stop_iter, prev });
                                    ++offset;
                                }
                            }

                            total_true += ComputeDistance(prev->location, (*stop_iter)->location);

                            prev = *stop_iter;

                        }
                        else
                        {
                            prev = *stop_iter;
                        }
                    }
                    return { (int)ptr->stops.size() + offset, (int)std::set<Stop*>(ptr->stops.begin(), ptr->stops.end()).size(), total, total_true };
                }

                return { (int)ptr->stops.size(), (int)std::set<Stop*>(ptr->stops.begin(), ptr->stops.end()).size(), total, total_true };
            }
        }

        std::vector<geo::Coordinates> TransportCatalogue::GetBusLocations(const Bus* route) const
        {
            std::vector<geo::Coordinates> points_raw;

            for (const catalogue::manager::Stop* stop : route->stops)
            {
                points_raw.push_back(stop->location);
            }

            if (route->stops.front() != route->EndPoint.back())
            {
                for (auto iter = route->stops.rbegin() + 1; iter != route->stops.rend(); ++iter)
                {
                    points_raw.push_back((*iter)->location);
                }
            }

            return points_raw;
        }

        std::vector<Bus*> TransportCatalogue::GetBusPointerList() const
        {
            std::vector<Bus*> ptr_list;

            for (const Bus& bus : buses_)
            {
                ptr_list.push_back(busname_to_bus_.at(bus.name));
            }

            return ptr_list;
        }

        packets::StopInfo TransportCatalogue::GetStopInfo(std::string_view stop) const
        {
            Stop* ptr = FindStop(stop);

            if (ptr == nullptr)
            {
                return { false, {} };

            }
            else
            {
                std::set<std::string> names;

                for (const Bus& bus : buses_)
                {
                    auto iter = std::find(bus.stops.begin(), bus.stops.end(), ptr);
                    if (iter != bus.stops.end())
                    {
                        names.insert(bus.name);
                    }
                }

                return { true, names };
            }
        }

        std::vector<geo::Coordinates> TransportCatalogue::GetAllStopsLocations() const
        {
            std::vector<geo::Coordinates> points_raw;

            for(const Bus& bus : buses_)
            {
                for (catalogue::manager::Stop* stop : bus.stops)
                {
                    points_raw.push_back(stop->location);
                }
            }

            return points_raw;
        }

        std::vector<Stop*> TransportCatalogue::GetAllStops() const
        {
            std::vector<Stop*> stops;

            for (const Stop& stop : stops_)
            {
                stops.push_back(FindStop(stop.name));
            }

            return stops;
        }
    }
}