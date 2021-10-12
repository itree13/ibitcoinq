#include "logger.h"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/exceptions.hpp>
#include <boost/core/null_deleter.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

void init_logger() {
    logging::add_file_log
    (
        keywords::file_name = "ibitcoin2waygrid_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = "[%TimeStamp%]: %Message%",
        keywords::auto_flush = true
    )->set_filter(
        logging::trivial::severity >= logging::trivial::debug
    );

    logging::add_console_log
    (
        std::cout,
        boost::log::keywords::format = "%Message%",
        keywords::auto_flush = true
    )->set_filter(
        logging::trivial::severity >= logging::trivial::info
    );

    logging::add_common_attributes();

    logging::core::get()->set_exception_handler(
        logging::make_exception_suppressor());
}
