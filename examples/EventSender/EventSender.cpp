#include <cstdio>
#include <cstdlib>

#include "LogManager.hpp"

using namespace Microsoft::Applications::Events;

// Define it once per .exe or .dll in any compilation module
LOGMANAGER_INSTANCE

// Replace line below by your API key
#define TOKEN "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx-xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx-xxxx"

int main(int argc, char *argv[])
{
    printf("Sending event to Microsoft Event Analytics... ");

    // LogManager configuration
    auto& config = LogManager::GetLogConfiguration();
    config["name"] = "EventSender";                 // Module name
    config["version"] = "1.0.0";                    // Module semver
    config["config"]["host"] = "EventSender";       // Module owner for Host / Guest mode (shared LogManager)
    config[CFG_STR_CACHE_FILE_PATH] = "storage.db"; // Offline storage path
    config[CFG_INT_MAX_TEARDOWN_TIME] = 1;          // Time to spend on HTTP upload during SDK teardown

    // LogManager initialization
    ILogger *logger = LogManager::Initialize(TOKEN);

    // Log one event
    logger->LogEvent("simple_event");

    // Shutdown
    LogManager::FlushAndTeardown();

    printf("[ DONE ]\n");
    return 0;
}
