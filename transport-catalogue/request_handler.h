#pragma once
#include "transport_catalogue.h"
#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

 // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
 // с другими подсистемами приложения.
 // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
namespace handler
{
    class RequestHandler
    {
    public:

        RequestHandler(const catalogue::manager::TransportCatalogue&, const renderer::MapRenderer&);

    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
        const catalogue::manager::TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
    };

    namespace utils
    {
        svg::Color NodeToColor(const json::Node& node);

        std::vector<std::string_view> NodeArrayToStringView(const json::Array& arr);
        std::vector<std::string_view> ParseRoute(const json::Array& route);
    }
}