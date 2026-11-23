#include "day6_api.h"
#include <boost/property_tree/json_parser.hpp>
#include <emscripten/emscripten.h>
#include <sstream>
#include <string>

extern "C" {
EMSCRIPTEN_KEEPALIVE
const char* day6_compute(const char* input) {
    static std::string result;
    try {
        if (!input) throw std::invalid_argument("Empty request");
        std::istringstream stream(input);
        boost::property_tree::ptree request;
        boost::property_tree::read_json(stream, request);
        if (request.get<std::string>("day", "") != "6")
            throw std::invalid_argument("Only Day 6 is supported");
        boost::property_tree::ptree response;
        response.put("ok", true);
        response.put("result", day6::execute(request));
        response.put("engine", "C++ WebAssembly");
        std::ostringstream out;
        boost::property_tree::write_json(out, response, false);
        result = out.str();
    } catch (const std::exception& error) {
        boost::property_tree::ptree response;
        response.put("ok", false);
        response.put("error", error.what());
        std::ostringstream out;
        boost::property_tree::write_json(out, response, false);
        result = out.str();
    }
    return result.c_str();
}
}
