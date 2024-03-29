#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_builder.h"
#include <sstream>
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
namespace reader_debugging
{
    std::string Print(const json::Node& node);
}

namespace reader {
    struct Query
    {
        Query();
        Query(const json::Array&, const json::Array&, const json::Dict&);

        json::Array base_requests;
        json::Array stat_requests;
        json::Dict render_settings;
    };

    Query ParseRaw(json::Document& doc);

    namespace parsing
    {
        namespace input
        {
            void ParseStops(catalogue::manager::TransportCatalogue& catalogue, json::Array& commands);
            void ParseDistance(catalogue::manager::TransportCatalogue& catalogue, json::Array& commands);
            void ParseRoutes(catalogue::manager::TransportCatalogue& catalogue, json::Array& commands);
        }

        namespace output
        {
            void InsertStopInfo(const catalogue::manager::TransportCatalogue&, const json::Dict&, json::Dict&);
            void InsertBusInfo(const catalogue::manager::TransportCatalogue&, const json::Dict&, json::Dict&);
            void InsertMapData(const catalogue::manager::TransportCatalogue&, renderer::MapRenderer&, const json::Dict&, json::Dict&);
        }
    }

    void ApplyCommands(catalogue::manager::TransportCatalogue& catalogue, json::Array&);
    void PrintStat(const catalogue::manager::TransportCatalogue& transport_catalogue, renderer::MapRenderer& map_renderer, json::Array, std::ostream& output);
}