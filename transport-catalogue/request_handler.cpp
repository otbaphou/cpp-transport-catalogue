#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
namespace handler
{
    RequestHandler::RequestHandler(const catalogue::manager::TransportCatalogue& db, const renderer::MapRenderer& renderer)
        :db_(db), renderer_(renderer) {}

    //Utilities

    namespace utils
    {
        svg::Color NodeToColor(const json::Node& node)
        {
            if (node.IsString())
                return svg::Color{ node.AsString() };
            else
            {
                if (node.IsArray())
                {
                    json::Array arr(std::move(node.AsArray()));

                    switch (arr.size())
                    {
                        case 3:
                            return svg::Color{ svg::Rgb{(uint8_t)arr[0].AsInt(), (uint8_t)arr[1].AsInt(), (uint8_t)arr[2].AsInt()} };
                            break;
                        case 4:
                            return svg::Color{ svg::Rgba{(uint8_t)arr[0].AsInt(), (uint8_t)arr[1].AsInt(), (uint8_t)arr[2].AsInt(), arr[3].AsDouble()} };
                            break;
                    default:
                        throw std::out_of_range("Invalid Amount Of Arguments! (Color)");
                        break;
                    }
                }
            }
            return svg::Color{};
        }

        std::vector<std::string_view> NodeArrayToStringView(const json::Array& arr)
        {
            std::vector<std::string_view> result;

            for (const auto& elem : arr)
            {
                result.push_back(std::move(elem.AsString()));
            }

            return result;
        }

        std::vector<std::string_view> ParseRoute(const json::Array& route)
        {
            std::vector<std::string_view> result = std::move(NodeArrayToStringView(route));

            return result;
        }
    }
}