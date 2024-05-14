#include "transport_router.h"
// Вставьте сюда решение из предыдущего спринта
namespace router
{
    void RouteManager::AddVertex()
    {
        for (catalogue::manager::Stop* stop : db_.GetAllStops())
        {
            stop_to_vertex[stop] = { total_vert, total_vert + 1 };
            total_vert = total_vert + 2;
        }
        //std::cout << "TOTAL VERTEX: " << total_vert << std::endl;
    }

    void RouteManager::AddEdge(size_t from, size_t to, double weight, const std::string& label, int span)
    {
        Edge new_edge{ {from, to, weight }, label, false, span };

        info_.edges.push_back(std::move(new_edge));
        info_.total_time += wait_time_;

        //Pointers
        std::shared_ptr<Edge> edge_ptr = std::make_shared<Edge>(info_.edges.back());

        edge_info_.edges.push_back(edge_ptr);
        edge_info_.existing_vertex[from] = to;
        bus_edge_info_.edges.push_back(edge_ptr);
    }

    void RouteManager::AddWaitEdges()
    {
        for (const auto& stop : stop_to_vertex)
        {
            std::pair<size_t, size_t> vertex = stop.second;

            Edge new_edge{ {vertex.first, vertex.second, wait_time_}, stop.first->name, true, 1 };

            info_.edges.push_back(std::move(new_edge));
            info_.total_time += wait_time_;

            //Pointers
            std::shared_ptr<Edge> edge_ptr = std::make_shared<Edge>(info_.edges.back());

            edge_info_.edges.push_back(edge_ptr);
            wait_edge_info_.edges.push_back(edge_ptr);
            wait_edge_info_.stop_to_edge[stop.first] = edge_ptr;
        }
        //======DEBUG======
        //std::cout << "TOTAL EDGES: " << edge_info_.edges.size() << std::endl;
        //std::cout << "TOTAL WAIT EDGES: " << wait_edge_info_.edges.size() << std::endl;
    }

    void RouteManager::AddBusEdges()
    {
        for (const catalogue::manager::Bus* bus : db_.GetBusPointerList())
        {

            if (bus->is_roundtrip)
            {
                for (auto stop = bus->stops.begin(); stop != (bus->stops).end() - 1; ++stop)
                {
                    double current_distance = 0;
                    int span = 0;
                    catalogue::manager::Stop* prev;

                    size_t from = stop_to_vertex[*stop].second;

                    size_t offset = std::distance(bus->stops.begin(), stop) + 1;
                    for (auto destination = bus->stops.begin() + offset; destination != (bus->stops).end(); ++destination)
                    {
                        size_t to = stop_to_vertex[*destination].first;

                        if (destination != bus->stops.begin() + offset)
                        {
                            double distance = db_.GetDistance({ prev, *destination });

                            if (distance < 0)
                            {
                                prev = *destination;
                                continue;
                            }

                            current_distance += distance;
                        }
                        else
                        {
                            double distance = db_.GetDistance({ *stop, *destination });

                            if (distance < 0)
                            {
                                prev = *destination;
                                continue;
                            }

                            current_distance += distance;
                        }
                        prev = *destination;
                        ++span;

                        //======DEBUG======
                        //std::cout << "New Edge: " << (*stop)->name << " -> " << (*destination)->name << " Weight: " << current_distance / velocity_ << std::endl;
                        //std::cout << from << " -> " << to << std::endl;

                        AddEdge(from, to, current_distance / velocity_, bus->name, span);
                    }
                }
            }
            else
            {
                for (auto stop = bus->stops.begin(); stop != bus->stops.end(); ++stop)
                {
                    double current_distance = 0;
                    int span = 0;
                    catalogue::manager::Stop* prev = *stop;

                    size_t from = stop_to_vertex[*stop].second;
                    size_t offset = std::distance(bus->stops.begin(), stop) + 1;
                    for (auto destination = bus->stops.begin() + offset; destination != bus->stops.end(); ++destination)
                    {

                        if (*stop != *destination)
                        {
                            size_t to = stop_to_vertex[*destination].first;

                            double distance = db_.GetDistance({ prev, *destination });
                            if (distance >= 0)
                            {
                                current_distance += distance;
                                ++span;

                                //======DEBUG======
                                //std::cout << "New Edge: " << (*stop)->name << " -> " << (*destination)->name << " Weight: " << current_distance / velocity_ << std::endl;
                                //std::cout << from << " -> " << to << std::endl;

                                AddEdge(from, to, current_distance / velocity_, bus->name, span);
                            }
                            else
                            {
                                current_distance += geo::ComputeDistance((*stop)->location, (*destination)->location);
                            }
                            prev = *destination;
                        }
                    }
                }

                for (auto stop = bus->stops.rbegin(); stop != bus->stops.rend(); ++stop)
                {
                    double current_distance = 0;
                    int span = 0;
                    catalogue::manager::Stop* prev = *stop;

                    size_t from = stop_to_vertex[*stop].second;
                    size_t offset = std::distance(bus->stops.rbegin(), stop) + 1;
                    for (auto destination = bus->stops.rbegin() + offset; destination != bus->stops.rend(); ++destination)
                    {
                        if (*stop != *destination)
                        {
                            size_t to = stop_to_vertex[*destination].first;

                            double distance = db_.GetDistance({ prev, *destination });
                            if (distance >= 0)
                            {
                                current_distance += distance;
                                ++span;

                                //======DEBUG======
                                //std::cout << "New Edge: " << (*stop)->name << " -> " << (*destination)->name << " Weight: " << current_distance / velocity_ << std::endl;
                                //std::cout << from << " -> " << to << std::endl;

                                AddEdge(from, to, current_distance / velocity_, bus->name, span);
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
            //======DEBUG======
            //std::cout << '\n';
        }
        //======DEBUG======
        //std::cout << "TOTAL EDGES: " << edge_info_.edges.size() << std::endl;
        //std::cout << "TOTAL WAIT EDGES: " << wait_edge_info_.edges.size() << std::endl;
    }

    std::shared_ptr<Edge> RouteManager::GetEdge(size_t id) const
    {
        return edge_info_.edges[id];
    }

    void RouteManager::Build()
    {
        AddVertex();
        AddWaitEdges();
        AddBusEdges();

        for (Edge& edge : info_.edges)
        {
            graph_.AddEdge(edge.value);
        }
    }

    RouteManager::RouteManager(json::Dict& raw_query, catalogue::manager::TransportCatalogue& db)
        :db_(db),
        graph_(db_.GetAllStops().size() * 2)
    {
        velocity_ = raw_query.at("bus_velocity").AsDouble() * 1000 / 60;
        wait_time_ = raw_query.at("bus_wait_time").AsDouble();
        Build();
        router_ = std::make_shared<graph::Router<double>>(graph::Router<double>(graph_));
    }

    std::optional<graph::Router<double>::RouteInfo> RouteManager::GetRouteInfo(const std::string& from, const std::string& to) const
    {
        size_t from_ = wait_edge_info_.stop_to_edge.at(db_.FindStop(from))->value.from;
        size_t to_ = wait_edge_info_.stop_to_edge.at(db_.FindStop(to))->value.from;
        //std::cout << "| " << db_.FindStop(from)->name << ":" << from_ << " | " << db_.FindStop(to)->name << ":" << to_ << " |\n";
        return router_.get()->BuildRoute(from_, to_);
    }

}