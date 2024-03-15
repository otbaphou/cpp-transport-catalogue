#pragma once

#include <iosfwd>
#include <string_view>
#include <string>

#include "transport_catalogue.h"

//Debugging 
#include <iostream>

namespace catalogue
{
    namespace out
    {
  
void ParseAndPrintStat(const manager::TransportCatalogue& tansport_catalogue, 
std::string_view request, std::ostream& output);
            
    }
}