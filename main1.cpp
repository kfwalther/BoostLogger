#include <iostream>
#include <string>
#include <fstream>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/filesystem.hpp>

// Includes from the example: http://www.boost.org/doc/libs/1_62_0/libs/log/example/doc/extension_stat_collector.cpp
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/phoenix.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes/value_visitation.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

// Includes from example: https://www.ociweb.com/resources/publications/sett/may-2016-boostlog-library/
#include <boost/log/sources/global_logger_storage.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

BOOST_LOG_GLOBAL_LOGGER(Channel1Logger, src::severity_channel_logger<logging::trivial::severity_level>);
BOOST_LOG_GLOBAL_LOGGER(Channel2Logger, src::severity_channel_logger<logging::trivial::severity_level>);
BOOST_LOG_GLOBAL_LOGGER(Channel3Logger, src::severity_channel_logger<logging::trivial::severity_level>);

BOOST_LOG_GLOBAL_LOGGER_CTOR_ARGS(Channel1Logger, src::severity_channel_logger<logging::trivial::severity_level>, (keywords::channel = "Category1"));
BOOST_LOG_GLOBAL_LOGGER_CTOR_ARGS(Channel2Logger, src::severity_channel_logger<logging::trivial::severity_level>, (keywords::channel = "Category2"));
BOOST_LOG_GLOBAL_LOGGER_CTOR_ARGS(Channel3Logger, src::severity_channel_logger<logging::trivial::severity_level>, (keywords::channel = "Category3"));

#define LOG_CATEGORY1(LEVEL) BOOST_LOG_SEV(Channel1Logger::get(), logging::trivial::LEVEL)
#define LOG_CATEGORY2(LEVEL) BOOST_LOG_SEV(Channel2Logger::get(), logging::trivial::LEVEL)
#define LOG_CATEGORY3(LEVEL) BOOST_LOG_SEV(Channel3Logger::get(), logging::trivial::LEVEL)

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)


// The backend collects records being forwarded from the front-end loggers.
class MyCustomBackend : public sinks::basic_sink_backend< 
        sinks::combine_requirements<
            sinks::synchronized_feeding,            
            sinks::flushing                         
        >::type>
{
private:
    // The file to write the collected information to.
    std::ofstream logFile;

public:
    // The constructor initializes the internal data
    explicit MyCustomBackend(const char * file_name);

    // The function consumes the log records that come from the frontend
    void consume(logging::record_view const& rec);
    // The function flushes the file
    void flush();

};

// The constructor initializes the internal data
MyCustomBackend::MyCustomBackend(const char * file_name) :
        logFile(file_name) {
    if (!logFile.is_open()) {
        throw std::runtime_error("Could not open the specified file!");
    }
}

// This function consumes the log records that come from the frontend.
void MyCustomBackend::consume(logging::record_view const & rec) {
    // If the NoNewline attribute is present, skip the newline.
    if (rec.attribute_values().count("NoNewline")) {
        logFile << *rec[boost::log::expressions::smessage];
    } else {
        logFile << *rec[boost::log::expressions::smessage] << std::endl;
    }
    // This is the equivalent of setting auto_flush.
    this->flush();
}

/** The function flushes the file. */
void MyCustomBackend::flush() {
    logFile.flush();
}

/** Initialize the Boost.Log logging. */
void init() {
    // Alias the custom sink types.
    typedef boost::log::sinks::synchronous_sink<MyCustomBackend> CustomBackendType;
    // Grab the Boost Log core.
    auto coreHandle = boost::log::core::get();
    // Define the path where the log files should reside.
    boost::filesystem::path destinationDir("C:\\test");

    // Create a minimal severity table filter
    typedef expr::channel_severity_filter_actor< std::string, logging::trivial::severity_level > min_severity_filter;
    min_severity_filter minSeverity = expr::channel_severity_filter(channel, severity);

    // Set up the minimum severity levels for different channels.
    minSeverity["Category1"] = logging::trivial::info;
    minSeverity["Category2"] = logging::trivial::info;
    minSeverity["Category3"] = logging::trivial::info;

    // Define log file 1.
    boost::filesystem::path logFile1(destinationDir / "category1.log");
    // Create a custom backend.
    boost::shared_ptr<MyCustomBackend> customBackend(new MyCustomBackend(logFile1.string().c_str()));
    // Create a sink with the custom backend.
    boost::shared_ptr<CustomBackendType> customSink(new CustomBackendType(customBackend));
    // Add a filter to the sink.
    customSink->set_filter((channel == "Category1") && minSeverity && (severity >= logging::trivial::info));
    // Add the sink to the Boost.Log core.
    coreHandle->add_sink(customSink);

    // Define log file 2.
    boost::filesystem::path logFile2(destinationDir / "category2.log");
    // Create a custom backend.
    customBackend = boost::make_shared<MyCustomBackend>(logFile2.string().c_str());
    // Create a sink with the custom backend.
    customSink = boost::make_shared<CustomBackendType>(customBackend);
    // Add a filter to the sink.
    customSink->set_filter((channel == "Category2") && minSeverity && (severity >= logging::trivial::info));
    // Add the sink to the Boost.Log core.
    coreHandle->add_sink(customSink);

    // Define log file 3.
    boost::filesystem::path logFile3(destinationDir / "category3.log");
    // Create a custom backend.
    customBackend = boost::make_shared<MyCustomBackend>(logFile3.string().c_str());
    // Create a sink with the custom backend.
    customSink = boost::make_shared<CustomBackendType>(customBackend);
    // Add a filter to the sink.
    customSink->set_filter((channel == "Category3") && minSeverity && (severity >= logging::trivial::info));
    // Add the sink to the Boost.Log core.
    coreHandle->add_sink(customSink);
}

int main (int numArgs, char const * const argList) {
    double number1 = 42;

    // Initialize the Boost.Log logging.
    init();

    LOG_CATEGORY1(info) << "New Category1 log.";
    // Won't print to file b/c doesn't meet severity requirements.
    LOG_CATEGORY2(trace) << "New Category2 log.";

    // Remove newline character ('\n') from specific logs. 
    LOG_CATEGORY3(info) << logging::add_value("NoNewline", true) << "[Put this on line 1]";
    LOG_CATEGORY3(info) << "[Put this on line 1 also]";
    LOG_CATEGORY3(info) << "[Put this on line 2]";

    return EXIT_SUCCESS;
}