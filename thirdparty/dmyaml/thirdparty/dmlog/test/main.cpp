#include "dmlog.h"

int main(int argc, char *argv[])
{
    LOG_CRITICAL("hello world {}.", 2019);
    LOG_INFO("This is a basic logging message.");

    LOG_INFO("This message includes {} substitutions, similar to how {} work.", 2,
        "printf(), fprintf(), etc.");

    LOG_CRITICAL("This is a fatal message with magenta coloring.");
    LOG_ERROR("This is an error message with red coloring.");
    LOG_WARN("This is a warning message with orange coloring.");
    LOG_INFO("This is an information message with green coloring.");

    LOG_DEBUG("This is a debugging message (that never gets printed).");

    LOG_DEBUG("This is a debugging message with blue coloring.");
    LOG_TRACE("This is a tracing message with cyan coloring.");

    LOG_INFO("This will be printed to the console, as well as \"demo.log\" as its first entry.");

    for (int i = 0; i < 10000; i++) {
        LOG_INFO("This message is 116 characters long including the info that comes before it. {}", i);
    }

    LOG_INFO("This message will cause the file to exceed the rollover value, which triggers it to "
        "be closed and renamed with an incremental number appended to it. A new file with the "
        "original filename is then opened and used in its place.");
    LOG_INFO("This message will be the first message in the new log file.");

    LOG_INFO("This concludes the feature demo for dmlog!");
    return 0;
}