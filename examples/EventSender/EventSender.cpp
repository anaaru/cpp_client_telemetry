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
    config[CFG_STR_CACHE_FILE_PATH] = "storage.db"; // Offline storage path
    config[CFG_INT_MAX_TEARDOWN_TIME] = 1;          // Time to spend on HTTP upload during SDK teardown

    // LogManager initialization
    ILogger *logger = LogManager::Initialize(TOKEN);

    // Log simple event without any properties
    logger->LogEvent("simple_event");

    // Log detailed event with various properties
    EventProperties detailed_event("detailed_event",
        {
            // Log compiler version
            { "_MSC_VER", _MSC_VER },
            // Pii-typed fields
            { "piiKind.None",               EventProperty("field_value",  PiiKind_None) },
            { "piiKind.DistinguishedName",  EventProperty("/CN=Jack Frost,OU=ARIA,DC=REDMOND,DC=COM",  PiiKind_DistinguishedName) },
            { "piiKind.GenericData",        EventProperty("generic_data",  PiiKind_GenericData) },
            { "piiKind.IPv4Address",        EventProperty("127.0.0.1", PiiKind_IPv4Address) },
            { "piiKind.IPv6Address",        EventProperty("2001:0db8:85a3:0000:0000:8a2e:0370:7334", PiiKind_IPv6Address) },
            { "piiKind.MailSubject",        EventProperty("RE: test",  PiiKind_MailSubject) },
            { "piiKind.PhoneNumber",        EventProperty("+1-425-829-5875", PiiKind_PhoneNumber) },
            { "piiKind.QueryString",        EventProperty("a=1&b=2&c=3", PiiKind_QueryString) },
            { "piiKind.SipAddress",         EventProperty("sip:info@microsoft.com", PiiKind_SipAddress) },
            { "piiKind.SmtpAddress",        EventProperty("Jack Frost <jackfrost@fabrikam.com>", PiiKind_SmtpAddress) },
            { "piiKind.Identity",           EventProperty("Jack Frost", PiiKind_Identity) },
            { "piiKind.Uri",                EventProperty("http://www.microsoft.com", PiiKind_Uri) },
            { "piiKind.Fqdn",               EventProperty("www.microsoft.com", PiiKind_Fqdn) },
            // Various typed key-values
            { "strKey1",  "hello1" },
            { "strKey2",  "hello2" },
            { "int64Key", 1L },
            { "dblKey",   3.14 },
            { "boolKey",  false },
            { "guidKey0", GUID_t("00000000-0000-0000-0000-000000000000") },
            { "guidKey1", GUID_t("00010203-0405-0607-0809-0A0B0C0D0E0F") },
            { "guidKey2", GUID_t("00010203-0405-0607-0809-0A0B0C0D0E0F") },
            { "timeKey1",  time_ticks_t((uint64_t)0) },     // time in .NET ticks
        });
    logger->LogEvent(detailed_event);

    // Shutdown
    LogManager::FlushAndTeardown();

    printf("[ DONE ]\n");
    return 0;
}
