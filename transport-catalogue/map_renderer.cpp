#include "map_renderer.h"
#include "request_handler.h"
#include "geo.h"

namespace renderer
{
    //Initializing stuff
    
    
	MapRenderer::MapRenderer(json::Dict& raw_query, catalogue::manager::TransportCatalogue& db)
		:db_(db){
		SetData(raw_query);
	}

	void MapRenderer::SetData(json::Dict& raw_query)
	{
		if (raw_query.contains("width")) {width_ = raw_query.at("width").AsDouble();}

		if (raw_query.contains("height")) { height_ = raw_query.at("height").AsDouble(); }

		if (raw_query.contains("padding")) { padding_ = raw_query.at("padding").AsDouble(); }

		if (raw_query.contains("line_width")) { line_width_ = raw_query.at("line_width").AsDouble(); }

		if (raw_query.contains("stop_radius")) { stop_radius_ = raw_query.at("stop_radius").AsDouble(); }

		if (raw_query.contains("bus_label_font_size")) { bus_label_font_size_ = raw_query.at("bus_label_font_size").AsInt(); }

		if (raw_query.contains("bus_label_offset")) 
		{ 
			json::Array arr(std::move(raw_query.at("bus_label_offset").AsArray()));
			bus_label_offset_ = {arr[0].AsDouble(), arr[1].AsDouble()};
		}

		if (raw_query.contains("stop_label_font_size")) { stop_label_font_size_ = raw_query.at("stop_label_font_size").AsInt(); }

		if (raw_query.contains("stop_label_offset"))
		{
			json::Array arr(std::move(raw_query.at("stop_label_offset").AsArray()));
			stop_label_offset_ = { arr[0].AsDouble(), arr[1].AsDouble() };
		}

		if (raw_query.contains("underlayer_color")) 
		{
			underlayer_color_ = handler::utils::NodeToColor(raw_query.at("underlayer_color"));
		}

		if (raw_query.contains("underlayer_width")) { underlayer_width_ = raw_query.at("underlayer_width").AsDouble(); }

		if (raw_query.contains("color_palette"))
		{
			json::Array arr(raw_query.at("color_palette").AsArray());

			for (const json::Node& node : arr)
			{
				color_palette_.push_back(handler::utils::NodeToColor(node));
			}
		}
	}
	svg::Document& MapRenderer::GetDocument()
	{
		return document_;
	}

	svg::Color MapRenderer::GetColor()
	{
		svg::Color color;

		if (current_color < color_palette_.size())
		{
			color = color_palette_[current_color];
			++current_color;
			return color;
		}
		else
		{
			current_color = 0;
			color = color_palette_[current_color];
			++current_color;
			return color;
		}
	}

	void MapRenderer::AddRoute(std::vector<geo::Coordinates> points_raw, const geo::SphereProjector& proj, const svg::Color& color)
	{
		svg::Polyline track;

		for (const auto& geo_coord : points_raw) {
			const svg::Point screen_coord = proj(geo_coord);
			track.AddPoint(screen_coord);
		}
		routes_.push_back(std::move(track.SetStrokeWidth(line_width_)
                                    .SetFillColor("none")
                                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                                    .SetStrokeColor(color)));
	}

	void MapRenderer::AddBusLabel(geo::Coordinates position, const geo::SphereProjector& proj, const svg::Color& color, const std::string& name)
	{
		svg::Text label;
		svg::Text outline;
		const svg::Point screen_coord = proj(position);
        
		label.SetPosition(screen_coord)
            .SetOffset(bus_label_offset_)
            .SetFontSize(bus_label_font_size_)
            .SetFontFamily("Verdana")
            .SetFontWeight("bold")
            .SetData(std::move(name));
        
		outline.SetPosition(screen_coord)
            .SetOffset(bus_label_offset_)
            .SetFontSize(bus_label_font_size_)
            .SetFontFamily("Verdana")
            .SetFontWeight("bold")
            .SetData(std::move(name));

		bus_labels_.push_back(std::move(outline.SetStrokeColor(underlayer_color_)
                                        .SetFillColor(underlayer_color_)
                                        .SetStrokeWidth(underlayer_width_)
                                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)));
        
		bus_labels_.push_back(std::move(label.SetFillColor(color)));

	}

	void MapRenderer::AddStopLabel(geo::Coordinates position, const geo::SphereProjector& proj, const std::string& name)
	{
		svg::Text label;
		svg::Text outline;
		const svg::Point screen_coord = proj(position);
        
		label.SetPosition(screen_coord)
            .SetOffset(stop_label_offset_)
            .SetFontSize(stop_label_font_size_)
            .SetFontFamily("Verdana")
            .SetData(std::move(name));
        
		outline.SetPosition(screen_coord)
            .SetOffset(stop_label_offset_)
            .SetFontSize(stop_label_font_size_)
            .SetFontFamily("Verdana").
            SetData(std::move(name));

		stop_labels_.push_back(std::move(outline.SetStrokeColor(underlayer_color_)
                                         .SetFillColor(underlayer_color_)
                                         .SetStrokeWidth(underlayer_width_)
                                         .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                                         .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)));
        
		stop_labels_.push_back(std::move(label.SetFillColor("black")));

	}

	void MapRenderer::AddCircle(geo::Coordinates position, const geo::SphereProjector& proj)
	{
		svg::Circle circle;
		const svg::Point screen_coord = proj(position);
		
		circles_.push_back(std::move(circle.SetCenter(screen_coord).SetRadius(stop_radius_).SetFillColor("white")));

	}

	void MapRenderer::AddGraphics()
	{
		for (const svg::Polyline& route : routes_)
		{
			document_.Add(route);
		}

		for (const svg::Text& label : bus_labels_)
		{
			document_.Add(label);
		}

		for (const svg::Circle& circle : circles_)
		{
			document_.Add(circle);
		}

		for (const svg::Text& label : stop_labels_)
		{
			document_.Add(label);
		}
	}
    
    void MapRenderer::DrawRoute(const geo::SphereProjector& proj, const catalogue::manager::Bus* route_ptr)
    {
        	const svg::Color c = GetColor();
            
			AddBusLabel(route_ptr->stops[0]->location, proj, c, route_ptr->name);

			if (route_ptr->stops[0] != route_ptr->stops.back() && !route_ptr->is_roundtrip)
			{
				AddBusLabel(route_ptr->stops.back()->location, proj, c, route_ptr->name);
			}

			AddRoute(db_.GetRouteLocations(route_ptr), proj, c);
    }
    
    void MapRenderer::DrawStops(const geo::SphereProjector& proj, 
    const std::vector<catalogue::manager::Stop*>& stop_arr, const std::set<catalogue::manager::Stop*>& unique_stops)
    {
        for (const auto& stop : stop_arr)
		{	
			if(unique_stops.contains(stop))
				AddCircle(stop->location, proj);
		}
		for (const auto& stop : stop_arr)
		{
			if (unique_stops.contains(stop))
				AddStopLabel(stop->location, proj, stop->name);
		}
    }
    
	void MapRenderer::RenderMap(std::vector<geo::Coordinates> cords, std::ostringstream& output)
	{

		const geo::SphereProjector proj{ cords.begin(), cords.end(), width_, height_, padding_ };
		auto bus_arr = db_.GetBusPointerList();

		std::set<catalogue::manager::Stop*> unique_stops;


		std::sort(bus_arr.begin(), bus_arr.end(), [](const catalogue::manager::Bus* lhs, const  catalogue::manager::Bus* rhs) 
        {
            return lhs->name < rhs->name; 
        });
        
		for (const auto& route : bus_arr)
		{

			for (auto& stop : route->stops)
			{
				unique_stops.insert(stop);
			}
           
            DrawRoute(proj, route);
		}
        
		auto stop_arr = db_.GetAllStops();
        
		std::sort(stop_arr.begin(), stop_arr.end(), [](const catalogue::manager::Stop* lhs, const  catalogue::manager::Stop* rhs) 
        {
            return lhs->name < rhs->name; 
        });
        
        DrawStops(proj, stop_arr,  unique_stops);

		AddGraphics();
		document_.Render(output);
	}

}