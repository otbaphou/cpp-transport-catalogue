#pragma once

#include <memory>
#include "transport_catalogue.h"
#include "router.h"
#include "geo.h"
#include "json.h"

namespace router
{
    struct BusProps
    {
        double velocity_ = 0;
        double wait_time_ = 0;  
    };
    
    struct Edge
    {
        graph::Edge<double> value;
        std::string label;
        bool is_wait;
        int span;
    };

    struct RouteInfo
    {
        double total_time;
        std::vector<Edge> edges;
    };
    
    struct EdgeInfo
    {
        std::vector<std::shared_ptr<Edge>> edges;
        std::unordered_map<size_t, size_t> existing_vertex;
    };
    
    struct BusEdgeInfo
    {
        std::vector<std::shared_ptr<Edge>> edges;
    };
    
    struct WaitEdgeInfo
    {
        std::vector<std::shared_ptr<Edge>> edges;
        std::unordered_map<catalogue::manager::Stop*, std::shared_ptr<Edge>> stop_to_edge;
    };
    
    class RouteManager
    {
        public:
        
            RouteManager(double, double, catalogue::manager::TransportCatalogue&);
        
            const double METERS_PER_MINUTE = 1000.0 / 60;
        
            std::shared_ptr<Edge> GetEdge(size_t id) const;
        
            std::optional<graph::Router<double>::RouteInfo> GetRouteInfo(const std::string& from, const std::string& to) const;
        
        private:
        
            void AddVertex();
            void AddEdge(size_t, size_t, double, const std::string& label, int);
        
            template <typename Iter>
            void AddSingleLaneBusEdges(Iter begin, Iter end, const catalogue::manager::Bus* bus) //What a stupid name
            {
                for (auto stop = begin; stop != end; ++stop)
                {
                    double current_distance = 0;
                    int span = 0;
                    catalogue::manager::Stop* prev = *stop;

                    size_t from = stop_to_vertex[*stop].second;
                    size_t offset = std::distance(begin, stop) + 1;
                    for (auto destination = begin + offset; destination != end; ++destination)
                    {

                        if (*stop != *destination)
                        {
                            size_t to = stop_to_vertex[*destination].first;

                            double distance = db_.GetDistance({ prev, *destination });
                            if (distance >= 0)
                            {
                                current_distance += distance;
                                ++span;

                                AddEdge(from, to, current_distance / properties.velocity_, bus->name, span);
                            }
                            else
                            {
                                current_distance += geo::ComputeDistance((*stop)->location, (*destination)->location);
                            }
                            prev = *destination;
                        }
                    }
                }
            }
        
            void AddWaitEdges();
            void AddBusEdges();
        
            void Build();
        
            catalogue::manager::TransportCatalogue& db_;

            graph::DirectedWeightedGraph<double> graph_;
            std::shared_ptr<graph::Router<double>> router_;

            RouteInfo info_;

            EdgeInfo edge_info_;
            BusEdgeInfo bus_edge_info_;
            WaitEdgeInfo wait_edge_info_;
                
            BusProps properties;
            
            size_t total_vert = 0;

            std::unordered_map<catalogue::manager::Stop*, std::pair<size_t, size_t>> stop_to_vertex;
    };
}