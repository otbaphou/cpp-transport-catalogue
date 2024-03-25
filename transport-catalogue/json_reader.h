#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
namespace reader_debugging
{
	std::string Print(const json::Node& node);
}

namespace reader{
	struct Query
	{
		Query();
		Query(const json::Array&, const json::Array&, const json::Dict&);

		json::Array base_requests;
		json::Array stat_requests;
		json::Dict render_settings;
	};

	Query ParseRaw(json::Document& doc);

	void ApplyCommands(catalogue::manager::TransportCatalogue& catalogue, json::Array);
	void PrintStat(const catalogue::manager::TransportCatalogue& transport_catalogue, renderer::MapRenderer& map_renderer, json::Array, std::ostream& output);
}