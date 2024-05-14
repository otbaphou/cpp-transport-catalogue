#pragma once

#include <memory>
#include "transport_catalogue.h"
#include "router.h"
#include "geo.h"
#include "json.h"

namespace router
{
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
        
            void AddVertex();
            void AddEdge(size_t, size_t, double, const std::string& label, int);

            void AddWaitEdges();
            void AddBusEdges();

            std::shared_ptr<Edge> GetEdge(size_t id) const;

            void Build();
            RouteManager(json::Dict& raw_query, catalogue::manager::TransportCatalogue&);

            std::optional<graph::Router<double>::RouteInfo> GetRouteInfo(const std::string& from, const std::string& to) const;
        
        private:
        
            catalogue::manager::TransportCatalogue& db_;

            graph::DirectedWeightedGraph<double> graph_;
            std::shared_ptr<graph::Router<double>> router_;

            RouteInfo info_;

            EdgeInfo edge_info_;
            BusEdgeInfo bus_edge_info_;
            WaitEdgeInfo wait_edge_info_;
                
            double velocity_ = 0;
            double wait_time_ = 0;
            
            size_t total_vert = 0;

            std::unordered_map<catalogue::manager::Stop*, std::pair<size_t, size_t>> stop_to_vertex;
    };
}