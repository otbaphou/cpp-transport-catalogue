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
		const json::Dict& mother_dict = doc.GetRoot().AsMap();

		Query query(mother_dict.at("base_requests").AsArray(), mother_dict.at("stat_requests").AsArray(), mother_dict.at("render_settings").AsMap());

		return query;
	}

    void ApplyCommands(catalogue::manager::TransportCatalogue& catalogue, json::Array commands)
    {

        for (const auto& prompt : commands)
        {
            const json::Dict& object = prompt.AsMap();
            if (object.at("type").AsString() == "Stop")
            {
                catalogue::manager::Stop tmp(object.at("name").AsString(), {object.at("latitude").AsDouble(), object.at("longitude").AsDouble()});
                catalogue.AddStop(tmp);
            }
        }
        
        for (const auto& prompt : commands)
        {
            const json::Dict& object = prompt.AsMap();
            if (object.at("type").AsString() == "Stop")
            {
                for(const auto& stop : object.at("road_distances").AsMap())
                {
                    const int distance = stop.second.AsInt(); // Reading The Value From road_distances Map

                    catalogue::manager::Stop* destination = catalogue.FindStop(stop.first);

                    catalogue.AddDistance({ catalogue.FindStop(object.at("name").AsString()), destination }, distance);
                }
            }
        }

        for (const auto& prompt : commands)
        {
            const json::Dict& object = prompt.AsMap();
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

    void PrintStat(const catalogue::manager::TransportCatalogue& transport_catalogue, renderer::MapRenderer& map_renderer, json::Array arr, std::ostream& output)
    {
        json::Array out_array;

        for (const auto& element : arr)
        {
            const json::Dict& query = element.AsMap();
            json::Dict out_map;
            if (query.at("type").AsString() == "Stop")
            {
                catalogue::packets::StopInfo data = transport_catalogue.GetStopInfo(query.at("name").AsString());

                if (!data.found)
                {
                    out_map.insert({"request_id", query.at("id").AsInt()});
         
                    std::string s = "not found";
                    out_map.insert({ "error_message", std::move(s)});
                }
                else
                {
                        if (data.names.size() == 0)
                        {
                            out_map.insert({ "buses", json::Array{} });
                        }
                        else 
                        {

                            json::Array route_container;
                            for (auto iter = data.names.begin(); iter != data.names.end(); iter++)
                            {
                                route_container.push_back(*iter);
                            }

                            out_map.insert({ "buses", route_container });
                        }
                        out_map.insert({ "request_id",  query.at("id").AsInt()});
                }
            }

            if (query.at("type").AsString() == "Bus")
            {
                catalogue::packets::RouteInfo data = transport_catalogue.GetRouteInfo(query.at("name").AsString());

                if (data.stops == -1)
                {
                    out_map.insert({ "request_id", query.at("id").AsInt() });

                    std::string s = "not found";
                    out_map.insert({ "error_message", std::move(s) });
                }
                else
                {
                    out_map.insert({ "curvature", data.total_distance / data.true_total_distance });
                    out_map.insert({ "request_id", query.at("id").AsInt() });
                    out_map.insert({ "route_length", data.total_distance });
                    out_map.insert({ "stop_count", data.stops });
                    out_map.insert({ "unique_stop_count", data.unique_stops });
                }
            }

            if (query.at("type").AsString() == "Map")
            {
                std::string xml;
                std::ostringstream stream;
                map_renderer.RenderMap(transport_catalogue.GetAllStopsLocations(), stream);
                xml = stream.str();
                out_map.insert({"map", xml});
                out_map.insert({ "request_id", query.at("id").AsInt() });
            }

            out_array.push_back(out_map);
        }
        json::Print(json::Document{ out_array }, output);
    }
}
