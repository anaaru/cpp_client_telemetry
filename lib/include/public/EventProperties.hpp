// Copyright (c) Microsoft. All rights reserved.

#ifndef EVENTPROPERTIES_H
#define EVENTPROPERTIES_H

#include "EventProperty.hpp"
#include "Version.hpp"
#include "ctmacros.hpp"
#include "Enums.hpp"
#include <stdint.h>
#include <string>
#include <map>

// *INDENT-OFF*
namespace Microsoft { namespace Applications { namespace Telemetry {
// *INDENT-ON*


/// <summary>
/// EventProperties struct that holds properties of an event
/// </summary>
	class ARIASDK_LIBABI EventProperties
	{
	public:
		/// <summary>
		/// Construct an EventProperties object. 
		/// A non-empty name is required whenever any custom properties are provided for the event via SetProperty.
		/// </summary>
		EventProperties(const std::string& name);
		EventProperties();
		EventProperties(EventProperties const& copy);		
		EventProperties& operator=(EventProperties const& copy);				
		/// <summary>
		/// Construct an EventProperties object from a map of string to EventProperty<br>
		/// A non-empty name is required whenever any custom properties are provided for the event via SetProperty.
		/// </summary>
		EventProperties(const std::string& name, const std::map<std::string, EventProperty> &properties);

		/// <summary>
		/// Add a map of <string, EventProperty> to EventProperties
		/// </summary>
		EventProperties& operator+=(const std::map<std::string, EventProperty> &properties);

		/// <summary>
		/// Assign a map of <string, EventProperty> to EventProperties
		/// </summary>
		EventProperties& operator=(const std::map<std::string, EventProperty> &properties);

		/// <summary>
		/// EventProperties constructor using C++11 initializer list
		/// </summary>
		EventProperties(const std::string& name, std::initializer_list<std::pair<std::string const, EventProperty> > properties);

		/// <summary>
		/// EventProperties assignment operator using C++11 initializer list
		/// </summary>
		EventProperties& operator=(std::initializer_list<std::pair<std::string const, EventProperty> > properties);

		/// <summary>
		/// Specify the name of an event.
		/// A non-empty name is required whenever any custom properties are provided for the event via SetProperty.
		/// </summary>
		/// <param name="name">Name of the event</param>
		bool SetName(const std::string& name);

		/// <summary>
		/// Get the name of an event.
		/// An empty string will be returned if it was not specified explicitly.
		/// </summary>
		/// <returns>Name of the event</returns>
		const std::string& GetName() const;

		/// <summary>
		/// Specify the Base Type of an event. This field is populated in Records.Type
		/// </summary>
		/// <param name="name">Base Type of event record</param>
		bool SetType(const std::string& recordType);

		/// <summary>
		/// Get the Base Type of an event.
		/// </summary>
		/// <returns>Type of the event</returns>
		const std::string& GetType() const;

		/// <summary>
		/// [optional] Specify the timestamp in milliseconds of an event.
		/// This overrides the default timestamp generated by the telemetry system for the event.
		/// </summary>
		/// <param name="timestampInEpochMillis">Unix timestamp in milliseconds since 00:00:00 
		/// Coordinated Universal Time (UTC), 1 January 1970 not counting leap seconds</param>
		void SetTimestamp(const int64_t timestampInEpochMillis);

		/// <summary>
		/// Get the timestamp in milliseconds of an event.
		/// 0 will be returned if this was not specified explicitly by calling SetTimestamp.
		/// </summary>
		/// <returns>timestamp in milliseconds of the event</returns>
		int64_t GetTimestamp() const;

		/// <summary>
		/// [optional] Specify transmit priority of an event.
		/// Default transmit priority will be used for transmitting the event if none was specified. 
		/// </summary>
		/// <param name="priority">Transmit priority of the event</param>
		void SetPriority(EventPriority priority);

		/// <summary>
		/// Get the transmit priority of the event.
		/// </summary>
		/// <returns>Transmit priority of the event<returns>
		EventPriority GetPriority() const;

		/// <summary>
		/// [optional] Specify Policy Bit flags for UTC usage of an event.
		/// Default values will be used for transmitting the event if none was specified. 
		/// </summary>
		/// <param name="priority">Transmit priority of the event</param>
		void SetPolicyBitFlags(uint64_t policyBitFlags);

		/// <summary>
		/// Get the Policy bit flags for UTC usage of the event.
		/// </summary>
		/// <returns>Transmit priority of the event<returns>
		uint64_t GetPolicyBitFlags() const;

		/// <summary>
		/// Specify a property for an event.
		/// It either creates a new property if none exists or overwrites the existing one.
		/// </summary>
		/// <param name='name'>Name of the property</param>
		/// <param name='value'>Value of the property</param>
		/// <param name='piiKind'>PIIKind of the property</param>
		void SetProperty(const std::string& name, EventProperty value);

		/// <summary>
		/// Specify a property for an event.
		/// It either creates a new property if none exists or overwrites the existing one.
		/// </summary>
		void SetProperty(const std::string& name, char const*  value, PiiKind piiKind = PiiKind_None);

		/// <summary>
		/// Specify a property for an event.
		/// It either creates a new property if none exists or overwrites the existing one.
		/// </summary>
		void SetProperty(const std::string& name, std::string  value, PiiKind piiKind = PiiKind_None);

		/// <summary>
		/// Specify a property for an event.
		/// It either creates a new property if none exists or overwrites the existing one.
		/// </summary>
		void SetProperty(const std::string& name, double       value, PiiKind piiKind = PiiKind_None);

		/// <summary>
		/// Specify a property for an event.
		/// It either creates a new property if none exists or overwrites the existing one.
		/// </summary>
		void SetProperty(const std::string& name, int64_t      value, PiiKind piiKind = PiiKind_None);

		/// <summary>
		/// Specify a property for an event.
		/// It either creates a new property if none exists or overwrites the existing one.
		/// </summary>
		void SetProperty(const std::string& name, bool         value, PiiKind piiKind = PiiKind_None);

		/// <summary>
		/// Specify a property for an event.
		/// It either creates a new property if none exists or overwrites the existing one.
		/// </summary>
		void SetProperty(const std::string& name, time_ticks_t value, PiiKind piiKind = PiiKind_None);

		/// <summary>
		/// Specify a property for an event.
		/// It either creates a new property if none exists or overwrites the existing one.
		/// </summary>
		void SetProperty(const std::string& name, GUID_t       value, PiiKind piiKind = PiiKind_None);

		/// <summary>
		/// Specify a property for an event. It either creates a new property if none exists or overwrites the existing one.<br>
		/// All integer types are currently being converted to int64_t.
		/// </summary>
		void SetProperty(const std::string& name, int8_t       value, PiiKind piiKind = PiiKind_None) { SetProperty(name, (int64_t)value, piiKind); }

		/// <summary>
		/// Specify a property for an event. It either creates a new property if none exists or overwrites the existing one.<br>
		/// All integer types are currently being converted to int64_t.
		void SetProperty(const std::string& name, int16_t      value, PiiKind piiKind = PiiKind_None) { SetProperty(name, (int64_t)value, piiKind); }

		/// <summary>
		/// Specify a property for an event. It either creates a new property if none exists or overwrites the existing one.<br>
		/// All integer types are currently being converted to int64_t.
		void SetProperty(const std::string& name, int32_t      value, PiiKind piiKind = PiiKind_None) { SetProperty(name, (int64_t)value, piiKind); }

		/// <summary>
		/// Specify a property for an event. It either creates a new property if none exists or overwrites the existing one.<br>
		/// All integer types are currently being converted to int64_t.
		void SetProperty(const std::string& name, uint8_t      value, PiiKind piiKind = PiiKind_None) { SetProperty(name, (int64_t)value, piiKind); }

		/// <summary>
		/// Specify a property for an event. It either creates a new property if none exists or overwrites the existing one.<br>
		/// All integer types are currently being converted to int64_t.
		void SetProperty(const std::string& name, uint16_t     value, PiiKind piiKind = PiiKind_None) { SetProperty(name, (int64_t)value, piiKind); }

		/// <summary>
		/// Specify a property for an event. It either creates a new property if none exists or overwrites the existing one.<br>
		/// All integer types are currently being converted to int64_t.
		void SetProperty(const std::string& name, uint32_t     value, PiiKind piiKind = PiiKind_None) { SetProperty(name, (int64_t)value, piiKind); }

		/// <summary>
		/// Specify a property for an event. It either creates a new property if none exists or overwrites the existing one.<br>
		/// All integer types are currently being converted to int64_t.
		void SetProperty(const std::string& name, uint64_t     value, PiiKind piiKind = PiiKind_None) { SetProperty(name, (int64_t)value, piiKind); }

        /// <summary>
        /// Specify a property for an event. It either creates a new property if none exists or overwrites the existing one.<br>
        void SetProperty(const std::string& name, char const*  value, CustomerContentKind ccKind) { SetProperty(name, EventProperty(value, ccKind)); }
        
        /// <summary>
        /// Specify a property for an event. It either creates a new property if none exists or overwrites the existing one.<br>
        void SetProperty(const std::string& name, std::string  value, CustomerContentKind ccKind) { SetProperty(name, EventProperty(value, ccKind)); }

		/// <summary>
		/// Get the properties bag of an event.
		/// </summary>
		/// <returns>Properties bag of the event</returns>
		const std::map<std::string, EventProperty>& GetProperties() const;

		/// <summary>
		/// Get the Pii properties bag of an event.
		/// </summary>
		/// <returns>Pii Properties bag of the event</returns>
		const std::map<std::string, std::pair<std::string, PiiKind> > GetPiiProperties() const;

        /// <summary>
        /// Get the Costomer content properties bag of an event.
        /// </summary>
        /// <returns>Customer content Properties bag of the event</returns>
        const std::map<std::string, std::pair<std::string, CustomerContentKind> > GetCustomerContentProperties() const;

		virtual ~EventProperties();

	private:
		std::string*     m_eventNameP;
		std::string*     m_eventTypeP;
		EventPriority    m_eventPriority;
		uint64_t         m_eventPolicyBitflags;
		int64_t          m_timestampInMillis;

		std::map<std::string, EventProperty>* m_propertiesP;
	};

}}} // namespace Microsoft::Applications::Telemetry

#endif //MYAPPLICATION_EVENTPROPERTIES_H