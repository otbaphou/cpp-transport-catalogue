#include "json_reader.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
namespace reader_debugging
{
    std::string Print(const json::Node& node)
    {
        std::ostringstream out;
        json::Print(json::Document{ node }, out);
        return out.str();
    }
}

namespace reader
{
    Query::Query() = default;
    Query::Query(const json::Array& br, const json::Array& sr, const json::Dict& rs)
        :base_requests(std::move(br)), stat_requests(std::move(sr)), render_settings(std::move(rs))
    {
    }

    Query ParseRaw(json::Document& doc)
    {
        const json::Dict& mother_dict = doc.GetRoot().AsDict();

        Query query(mother_dict.at("base_requests").AsArray(), mother_dict.at("stat_requests").AsArray(), mother_dict.at("render_settings").AsDict());

        return query;
    }

    namespace parsing
    {
        namespace input
        {
            void ParseStops(catalogue::manager::TransportCatalogue& catalogue, json::Array& commands)
            {
                for (const auto& prompt : commands)
                {
                    const json::Dict& object = prompt.AsDict();
                    if (object.at("type").AsString() == "Stop")
                    {
                        catalogue::manager::Stop tmp(object.at("name").AsString(), { object.at("latitude").AsDouble(), object.at("longitude").AsDouble() });
                        catalogue.AddStop(tmp);
                    }
                }
            }

            void ParseDistance(catalogue::manager::TransportCatalogue& catalogue, json::Array& commands)
            {
                for (const auto& prompt : commands)
                {
                    const json::Dict& object = prompt.AsDict();
                    if (object.at("type").AsString() == "Stop")
                    {
                        for (const auto& stop : object.at("road_distances").AsDict())
                        {
                            const int distance = stop.second.AsInt(); // Reading The Value From road_distances Map

                            catalogue::manager::Stop* destination = catalogue.FindStop(stop.first);

                            catalogue.AddDistance({ catalogue.FindStop(object.at("name").AsString()), destination }, distance);
                        }
                    }
                }
            }

            void ParseRoutes(catalogue::manager::TransportCatalogue& catalogue, json::Array& commands)
            {
                for (const auto& prompt : commands)
                {
                    const json::Dict& object = prompt.AsDict();
                    if (object.at("type").AsString() == "Bus")
                    {
                        bool is_roundtrip = object.at("is_roundtrip").AsBool();
                        std::vector<std::string_view> stop_array(std::move(handler::utils::ParseRoute(object.at("stops").AsArray())));
                        catalogue::manager::Bus bus(object.at("name").AsString(), is_roundtrip);

                        for (std::string_view& stop : stop_array)
                        {
                            bus.stops.push_back(catalogue.FindStop(stop));
                            bus.EndPoint.push_back(catalogue.FindStop(stop));
                        }

                        if (!is_roundtrip && bus.EndPoint.front() == bus.EndPoint.back())
                            bus.EndPoint.pop_back();

                        catalogue.AddBus(bus);
                    }
                }
            }
        }

        namespace output
        {
            void InsertStopInfo(const catalogue::manager::TransportCatalogue& transport_catalogue, const json::Dict& query, json::Builder& out)
            {
                out.StartDict();
                catalogue::packets::StopInfo data = transport_catalogue.GetStopInfo(query.at("name").AsString());

                if (!data.found)
                {
                    out.Key("request_id").Value(query.at("id").AsInt()).Key("error_message").Value("not found");
                }
                else
                {
                    if (data.names.size() == 0)
                    {
                        out.Key("buses").Value(json::Array{});
                    }
                    else
                    {
                        out.Key("buses").StartArray();
                        for (auto iter = data.names.begin(); iter != data.names.end(); iter++)
                        {
                            out.Value(*iter);
                        }
                        out.EndArray();
                    }
                    out.Key("request_id").Value(query.at("id").AsInt());
                }
                out.EndDict();
            }

            void InsertBusInfo(const catalogue::manager::TransportCatalogue& transport_catalogue, const json::Dict& query, json::Builder& out)
            {
                out.StartDict();
                catalogue::packets::RouteInfo data = transport_catalogue.GetRouteInfo(query.at("name").AsString());

                if (data.stops == -1)
                {
                    out.Key("request_id").Value(query.at("id").AsInt()).Key("error_message").Value("not found");
                }
                else
                {
                    out.Key("curvature").Value(data.total_distance / data.true_total_distance)
                        .Key("request_id").Value(query.at("id").AsInt())
                        .Key("route_length").Value(data.total_distance).Key("stop_count").Value(data.stops)
                        .Key("unique_stop_count").Value(data.unique_stops);
                }
                out.EndDict();
            }

            void InsertMapData(const catalogue::manager::TransportCatalogue& transport_catalogue,
                renderer::MapRenderer& map_renderer, const json::Dict& query, json::Builder& out)
            {
                out.StartDict();

                std::string xml;
                std::ostringstream stream;
                map_renderer.RenderMap(transport_catalogue.GetAllStopsLocations(), stream);
                xml = stream.str();
                out.Key("map").Value(xml);
                out.Key("request_id").Value(query.at("id").AsInt());

                out.EndDict();
            }
        }
    }

    void ApplyCommands(catalogue::manager::TransportCatalogue& catalogue, json::Array& commands)
    {
        parsing::input::ParseStops(catalogue, commands);
        parsing::input::ParseDistance(catalogue, commands);
        parsing::input::ParseRoutes(catalogue, commands);
    }

    void PrintStat(const catalogue::manager::TransportCatalogue& transport_catalogue, renderer::MapRenderer& map_renderer, json::Array arr, std::ostream& output)
    {
        json::Builder builder{};
        builder.StartArray();

        for (const auto& element : arr)
        {
            const json::Dict& query = element.AsDict();
            

            if (query.at("type").AsString() == "Stop")
            {
                parsing::output::InsertStopInfo(transport_catalogue, query, builder);
            }

            if (query.at("type").AsString() == "Bus")
            {
                parsing::output::InsertBusInfo(transport_catalogue, query, builder);
            }

            if (query.at("type").AsString() == "Map")
            {
                parsing::output::InsertMapData(transport_catalogue, map_renderer, query, builder);
            }
        }
        builder.EndArray();
        json::Print(json::Document{ builder.Build() }, output);
    }
}