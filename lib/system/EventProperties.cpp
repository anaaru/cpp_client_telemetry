#include "pal/PAL.hpp"

#include "bond/generated/AriaProtocol_types.hpp"
#include "EventProperty.hpp"
#include "EventProperties.hpp"
#include "EventPropertiesStorage.hpp"
#include "DebugEvents.hpp"
#include "ILogManager.hpp"
#include "utils/Utils.hpp"
#include <string>
#include <algorithm>
#include <cctype>
#include <map>

#include "utils/annex_k.hpp"

using namespace std;
using namespace MAT;

namespace ARIASDK_NS_BEGIN {

    const char* const DefaultEventName = "undefined";

    EventProperties::EventProperties(const std::string& name, const std::map<std::string, EventProperty> &properties) :
        EventProperties(name)
    {
        (*this) += properties;
    }

    EventProperties& EventProperties::operator+=(const std::map<std::string, EventProperty> &properties)
    {
        for (auto &kv : properties)
        {
            auto key = kv.first;
            auto val = kv.second;
            m_storage->properties[key] = val;
        }
        return (*this);
    }

    EventProperties& EventProperties::operator=(const std::map<std::string, EventProperty> &properties)
    {
        m_storage->properties.clear();
        (*this) += properties;
        return (*this);
    }


    EventProperties::EventProperties()
        : EventProperties(DefaultEventName)
    {
    }

    /**
    * \brief EventProperties constructor
    * \param name Event name - must not be empty!
    */
    EventProperties::EventProperties(const string& name)
        : m_storage(new EventPropertiesStorage())
    {
        if (!name.empty())
        {
            SetName(name);
        }
        else {
            SetName(DefaultEventName);
        }
    }

    EventProperties::EventProperties(EventProperties const& copy)
    {
        m_storage = new EventPropertiesStorage(*copy.m_storage);
    }

    EventProperties& EventProperties::operator=(EventProperties const& copy)
    {
        *m_storage = *copy.m_storage;

        return *this;
    }

    EventProperties::~EventProperties()
    {
        delete m_storage;
    }

    /// <summary>
    /// EventProperties constructor using C++11 initializer list
    /// </summary>
    EventProperties::EventProperties(const std::string& name, std::initializer_list<std::pair<std::string const, EventProperty> > properties)
        : EventProperties(name)
    {
        (*this) = properties;
    }

    /// <summary>
    /// EventProperties assignment operator using C++11 initializer list
    /// </summary>
    EventProperties& EventProperties::operator=(std::initializer_list<std::pair<std::string const, EventProperty> > properties)
    {
        m_storage->properties.clear();
        m_storage->propertiesPartB.clear();

        for (auto &kv : properties)
        {
            auto key = kv.first;
            auto val = kv.second;

            m_storage->properties[key] = val;
        }

        return (*this);
    };

    /// <summary>
    /// Set the Epoch unix timestamp in milliseconds of the event. 
    /// This will override default timestamp generated by telemetry system.
    /// <param name="timestampInEpochMillis">Unix timestamp in milliseconds since 00:00:00 
    /// Coordinated Universal Time (UTC), 1 January 1970 not counting leap seconds</param>
    /// </summary>
    void EventProperties::SetTimestamp(const int64_t timestampInEpochMillis)
    {
        m_storage->timestampInMillis = timestampInEpochMillis;
    }

    /// <summary>
    /// Returns the timestamp of the event.
    /// If this was not set explicitly by calling SetTimestamp, it will return 0 by default.
    /// </summary>
    int64_t EventProperties::GetTimestamp() const
    {
        return m_storage->timestampInMillis;
    }

    /// <summary>
    /// Set transmit priority of this event
    /// Default transmit priority will be used if none specified 
    /// </summary>
    void EventProperties::SetPriority(EventPriority priority)
    {
        m_storage->eventLatency = (EventLatency)priority;
        if (priority >= EventPriority_High)
        {
            m_storage->eventLatency = EventLatency_RealTime;
            m_storage->eventPersistence = EventPersistence_Critical;
        }
        else
        if (priority >= EventPriority_Low)
        {
            // TODO: 1438270 - [v3][1DS] Direct upload to respect low priority
            m_storage->eventLatency = EventLatency_Normal;
            m_storage->eventPersistence = EventPersistence_Normal;
        }
    }

    /// <summary>
    /// Get transmit priority of this event
    /// Default transmit priority will be used if none specified 
    /// </summary>
    EventPriority EventProperties::GetPriority() const
    {
        return static_cast<EventPriority>(m_storage->eventLatency);
    }

    /// <summary>
    /// Set transmit Latency of this event
    /// Default transmit Latency will be used if none specified 
    /// </summary>
    void EventProperties::SetLatency(EventLatency latency)
    {
        m_storage->eventLatency = latency;
    }

    /// <summary>
    /// Get transmit priority of this event
    /// Default transmit priority will be used if none specified 
    /// </summary>
    EventLatency EventProperties::GetLatency() const
    {
        return m_storage->eventLatency;
    }

    /// <summary>
    /// [optional] Specify Persistence priority of an event.
    /// Default Persistence priority will be used for persisting the event if none was specified. 
    /// </summary>
    void EventProperties::SetPersistence(EventPersistence persistence)
    {
        m_storage->eventPersistence = persistence;
    }

    /// <summary>
    /// Get Persistence of this event
    /// Default Persistence will be used if none specified 
    /// </summary>
    EventPersistence EventProperties::GetPersistence() const
    {
        return m_storage->eventPersistence;
    }

    /// <summary>
    /// [optional] Specify popSample of an event.
    /// </summary>
    /// <param name="priority">popSample of the event</param>
    void EventProperties::SetPopsample(double popSample)
    {
        m_storage->eventPopSample = popSample;
    }

    /// <summary>
    /// Get the popSample of the event.
    /// </summary>
    /// <returns>popSample of the event<returns>
    double EventProperties::GetPopSample() const
    {
        return m_storage->eventPopSample;
    }

    /// <summary>
    ///  Specify Policy Bit flags for UTC usage of an event.
    /// </summary>
    void EventProperties::SetPolicyBitFlags(uint64_t policyBitFlags)
    {
        m_storage->eventPolicyBitflags = policyBitFlags;
    }

    /// <summary>
    /// Get the Policy bit flags for UTC usage of the event.
    /// </summary>
    uint64_t EventProperties::GetPolicyBitFlags() const
    {
        return m_storage->eventPolicyBitflags;
    }

    /// <summary>
    /// Set name of this event
    /// Default name will be used if none specified (e.g. for LogPageView, name = "PageView")
    /// </summary>
    bool EventProperties::SetName(const string& name)
    {
        std::string sanitizedEventName = sanitizeIdentifier(name);

        EventRejectedReason isValidEventName = validateEventName(sanitizedEventName);
        if (isValidEventName != REJECTED_REASON_OK) {
            LOG_ERROR("Invalid event name!");
            DebugEvent evt;
            evt.type = DebugEventType::EVT_REJECTED;
            evt.param1 = isValidEventName;
            ILogManager::DispatchEventBroadcast(evt);
            return false;
        }
        m_storage->eventName.assign(sanitizedEventName);
        return true;
    }

    /// <summary>
    /// Returns the name for this event. 
    /// If this was not set explicitly by calling SetName, it will return an empty string.
    /// </summary>
    const string& EventProperties::GetName() const
    {
        return m_storage->eventName;
    }

    /// <summary>
    /// Specify the Base Type of an event. This field is populated in Records.Type
    /// </summary>
    bool EventProperties::SetType(const string& recordType)
    {
        std::string eventType = toLower(recordType);
        eventType = sanitizeIdentifier(eventType);
        EventRejectedReason isValidEventName = validateEventName(eventType);
        if (isValidEventName != REJECTED_REASON_OK) {
            LOG_ERROR("Invalid event type!");
            DebugEvent evt;
            evt.type = DebugEventType::EVT_REJECTED;
            evt.param1 = isValidEventName;
            ILogManager::DispatchEventBroadcast(evt);
            return false;
        }
        m_storage->eventType.assign(eventType);
        return true;
    }

    /// <summary>
    /// Returns the Base Type for this event. 
    /// If this was not set explicitly by calling SetType, it will return an empty string.
    /// </summary>
    const string& EventProperties::GetType() const
    {
        return m_storage->eventType;
    }

    /// <summary>
    /// Specify a property of an event
    /// It creates a new property if none exists or overwrites an existing one
    /// <param name='name'>Name of the property</param>
    /// <param name='value'>String value of the property</param>
    /// <param name='piiKind'>PIIKind of the property</param>
    /// </summary>
    void EventProperties::SetProperty(const string& name, EventProperty prop)
    {
        EventRejectedReason isValidPropertyName = validatePropertyName(name);
        if (isValidPropertyName != REJECTED_REASON_OK)
        {
            LOG_ERROR("Context name is invalid: %s", name.c_str());
            DebugEvent evt;
            evt.type = DebugEventType::EVT_REJECTED;
            evt.param1 = isValidPropertyName;
            ILogManager::DispatchEventBroadcast(evt);
            return;
        }

        m_storage->properties[name] = prop;
    }

    //
    void EventProperties::SetProperty(const std::string& name, char const*  value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }
    void EventProperties::SetProperty(const std::string& name, const std::string&  value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }
    void EventProperties::SetProperty(const std::string& name, double       value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }
    void EventProperties::SetProperty(const std::string& name, int64_t      value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }
    void EventProperties::SetProperty(const std::string& name, bool         value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }
    void EventProperties::SetProperty(const std::string& name, time_ticks_t value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }
    void EventProperties::SetProperty(const std::string& name, GUID_t       value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }

    void EventProperties::SetProperty(const std::string& name, std::vector<int64_t>&     value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }
    void EventProperties::SetProperty(const std::string& name, std::vector<double>&      value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }
    void EventProperties::SetProperty(const std::string& name, std::vector<GUID_t>&      value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }
    void EventProperties::SetProperty(const std::string& name, std::vector<std::string>& value, PiiKind piiKind, DataCategory category) { SetProperty(name, EventProperty(value, piiKind, category)); }

    const map<string, EventProperty>& EventProperties::GetProperties(DataCategory category) const
    {
        if (category == DataCategory_PartC)
        {
            return m_storage->properties;
        }
        else
        {
            return m_storage->propertiesPartB;
        }
    }

    /// <summary>
    /// Get Pii properties map
    /// </summary>
    /// <returns></returns>
    const map<string, pair<string, PiiKind> > EventProperties::GetPiiProperties(DataCategory category) const
    {
        std::map<string, pair<string, PiiKind> > pIIExtensions;
        auto &props = (category == DataCategory_PartC) ? m_storage->properties : m_storage->propertiesPartB;
        for (const auto &kv : props)
        {
            auto k = kv.first;
            auto v = kv.second;
            if (v.piiKind != PiiKind_None)
            {
                pIIExtensions[k] = std::pair<string, PiiKind>(v.to_string(), v.piiKind);
            }
        }
        return pIIExtensions;
    }

#ifdef ARIA_C_API
    static inline void cppprop_to_cprop(EventProperty &rhs, evt_prop &lhs)
    {
        switch (rhs.type)
        {
        case TYPE_STRING:
            lhs.value.as_string = rhs.as_string;
            break;
        case TYPE_INT64:
            lhs.value.as_int64  = rhs.as_int64;
            break;
        case TYPE_DOUBLE:
            lhs.value.as_double = rhs.as_double;
            break;
        case TYPE_TIME:
            lhs.value.as_time = rhs.as_time_ticks.ticks;
            break;
        case TYPE_BOOLEAN:
            lhs.value.as_bool = rhs.as_bool;
            break;
        case TYPE_GUID:
            lhs.value.as_guid = new evt_guid_t();
            // TODO: copy from GUID_t to aria_guid_t
            break;
#if 0
        case TYPE_STRING_ARRAY:
            // TODO: Not implemented
            break;
        case TYPE_INT64_ARRAY:
            // TODO: Not implemented
            break;
        case TYPE_DOUBLE_ARRAY:
            // TODO: Not implemented
            break;
        case TYPE_GUID_ARRAY:
            // TODO: Not implemented
            break;
#endif
        default:
            break;
        }
    }

    evt_prop* EventProperties::pack()
    {
        size_t size = m_storage->properties.size() + m_storage->propertiesPartB.size() + 1;
        evt_prop * result = static_cast<evt_prop *>(calloc(sizeof(evt_prop), size));
        size_t i = 0;
        for(auto &props : { m_storage->properties, m_storage->propertiesPartB })
            for (auto &kv : props)
            {
                auto k = kv.first;
                auto v = kv.second;
                result[i].name = (char *)k.c_str();
                result[i].type = static_cast<evt_prop_t>(v.type);
                result[i].piiKind = v.piiKind;
                cppprop_to_cprop(v, result[i]);
            };
        result[size-1].type = TYPE_NULL;
        return result;
    }

    bool EventProperties::unpack(evt_prop *packed)
    {
        // List of attributes going into envelope section
        static const std::string keyName = "name";
        static const std::string keyTime = "time";
        static const std::string keyPopSample = "popSample";

        evt_prop *curr = packed;
        for (size_t i = 0; curr->type != TYPE_NULL; i++, curr++)
        {
            // Event name
            if (keyName == curr->name)
            {
                SetName(curr->value.as_string);
                continue;
            }

            // Event time
            if (keyTime == curr->name)
            {
                // TODO: set custom event time
                // NOT IMPLEMENTED!
                continue;
            }

            // Pop sample
            // TODO:
            // * popSample
            // * flags for UTC
            // * cV

            switch (curr->type)
            {
            case TYPE_STRING:
                SetProperty(curr->name, curr->value.as_string, (PiiKind)curr->piiKind);
                break;
            case TYPE_INT64:
                SetProperty(curr->name, curr->value.as_int64,  (PiiKind)curr->piiKind);
                break;
            case TYPE_DOUBLE:
                SetProperty(curr->name, curr->value.as_double, (PiiKind)curr->piiKind);
                break;
            case TYPE_TIME:
                SetProperty(curr->name, time_ticks_t(curr->value.as_time), (PiiKind)curr->piiKind);
                break;
            case TYPE_BOOLEAN:
                SetProperty(curr->name, curr->value.as_bool, (PiiKind)curr->piiKind);
                break;
            case TYPE_GUID:
                SetProperty(curr->name, curr->value.as_guid, (PiiKind)curr->piiKind);
                break;

            /* TODO: add support for arrays passing across C API
                // Arrays of basic types
                TYPE_STRING_ARRAY
                TYPE_INT64_ARRAY
                TYPE_DOUBLE_ARRAY
                TYPE_TIME_ARRAY
                TYPE_BOOL_ARRAY
                TYPE_GUID_ARRAY
            */
            default:
                break;
            }
        }
        return true;
    }
#endif /* end of ARIA_C_API */

} ARIASDK_NS_END

