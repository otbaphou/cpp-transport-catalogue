#pragma once
#include "transport_catalogue.h"
#include "json.h"
#include <algorithm>

namespace renderer
{
	class MapRenderer
	{
	public:

		MapRenderer(json::Dict& raw_query, catalogue::manager::TransportCatalogue&);

		void SetData(json::Dict& raw_query);
		svg::Document& GetDocument();

		void AddRoute(std::vector<geo::Coordinates> points_raw, const geo::SphereProjector& proj, const svg::Color& color);
		void AddBusLabel(geo::Coordinates position, const geo::SphereProjector& proj, const svg::Color& color, const std::string& name);
		void AddCircle(geo::Coordinates position, const geo::SphereProjector& proj);
		void AddStopLabel(geo::Coordinates position, const geo::SphereProjector& proj, const std::string& name);

		void AddGraphics();

		void DrawRoute(const geo::SphereProjector& proj, const catalogue::manager::Bus* route_ptr);
		void DrawStops(const geo::SphereProjector& proj, const std::vector<catalogue::manager::Stop*>& stop_arr,
			const std::set<catalogue::manager::Stop*>& unique_stops);


		void RenderMap(std::vector<geo::Coordinates>, std::ostringstream&);

		svg::Color GetColor();

	private:

		//{points_raw.begin(), points_raw.end(), width_, height_, padding_}
		double width_ = 0;
		double height_ = 0;

		double padding_ = 0;

		double line_width_ = 0;
		double stop_radius_ = 0;

		int bus_label_font_size_ = 0;
		svg::Point bus_label_offset_;

		int stop_label_font_size_ = 0;
		svg::Point stop_label_offset_;

		svg::Color underlayer_color_;
		double underlayer_width_ = 0;


		size_t current_color = 0;
		std::vector<svg::Color> color_palette_;

		std::vector<svg::Polyline> routes_;
		std::vector<svg::Circle> circles_;
		std::vector<svg::Text> bus_labels_;
		std::vector<svg::Text> stop_labels_;
		svg::Document document_;

		const catalogue::manager::TransportCatalogue& db_;
	};


}