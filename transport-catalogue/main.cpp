#include <fstream>
#include <iostream>
#include "json.h"
#include "json_reader.h"
#include "transport_catalogue.h"
#include "transport_router.h"

int main()
{
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */

    json::Document data = json::Load(std::cin);

    //Initializing Transport Catalogue
    catalogue::manager::TransportCatalogue catalogue;

    reader::Query query = reader::ParseRaw(data);

    //Initializeing Renderer
    renderer::MapRenderer renderer(query.render_settings, catalogue);
    handler::RequestHandler request_handler(catalogue, renderer);

    reader::ApplyCommands(catalogue, query.base_requests);
    router::RouteManager router(query.routing_settings, catalogue);

    reader::PrintStat(catalogue, renderer, router, query.stat_requests, std::cout);

}