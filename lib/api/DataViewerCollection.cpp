#include "DataViewerCollection.hpp"
#include <algorithm>
#include <mutex>

namespace ARIASDK_NS_BEGIN {

    MATSDK_LOG_INST_COMPONENT_CLASS(DataViewerCollection, "EventsSDK.DataViewerCollection", "Microsoft Telemetry Client - DataViewerCollection class");

    void DataViewerCollection::DispatchDataViewerEvent(const std::vector<uint8_t>& packetData) const noexcept
    {
        if (IsViewerEnabled() == false)
            return;

        std::lock_guard<std::mutex> lock(m_dataViewerMapLock);
        for(const auto& viewer : m_dataViewerCollection)
        {
            // Task 3568800: Integrate ThreadPool to IDataViewerCollection
            viewer->ReceiveData(packetData);
        }
    };

    void DataViewerCollection::RegisterViewer(const std::shared_ptr<IDataViewer>& dataViewer)
    {
        if (dataViewer == nullptr)
        {
            throw std::invalid_argument("nullptr passed for data viewer");
        }

        std::lock_guard<std::mutex> lock(m_dataViewerMapLock);

        if (IsViewerEnabledHelper(dataViewer->GetName()))
        {
            throw std::invalid_argument(std::string { "Viewer: '%s' is already registered", dataViewer->GetName() });
        }
        
        m_dataViewerCollection.push_back(dataViewer);
    }

    void DataViewerCollection::UnregisterViewer(const char* viewerName)
    {
        if (viewerName == nullptr)
        {
            throw std::invalid_argument("nullptr passed for viewer name");
        }

        std::lock_guard<std::mutex> lock(m_dataViewerMapLock);
        auto toErase = std::find_if(m_dataViewerCollection.begin(), m_dataViewerCollection.end(), [&viewerName](std::shared_ptr<IDataViewer> viewer)
            {
                return viewer->GetName() == viewerName;
            });
        
        if (toErase == m_dataViewerCollection.end())
        {
            throw std::invalid_argument(std::string { "Viewer: '%s' is not currently registered", viewerName });
        }

        m_dataViewerCollection.erase(toErase);
    }

    void DataViewerCollection::UnregisterAllViewers()
    {
        std::lock_guard<std::mutex> lock(m_dataViewerMapLock);
        m_dataViewerCollection.clear();
    }

    bool DataViewerCollection::IsViewerEnabled(const char* viewerName) const
    {
        if (viewerName == nullptr)
        {
            throw std::invalid_argument("nullptr passed for viewer name");
        }

        std::lock_guard<std::mutex> lock(m_dataViewerMapLock);

		  return IsViewerEnabledHelper(viewerName);
    }
	 
	 bool DataViewerCollection::IsViewerEnabledHelper(const char* viewerName) const
    {
        if (viewerName == nullptr)
        {
            throw std::invalid_argument("nullptr passed for viewer name");
        }

        auto lookup = std::find_if(m_dataViewerCollection.begin(), m_dataViewerCollection.end(), [&viewerName](std::shared_ptr<IDataViewer> viewer)
			  {
				  return strcmp(viewer->GetName(), viewerName) == 0;
			  });

		  auto result = lookup != m_dataViewerCollection.end();

		  return result;
    }

    bool DataViewerCollection::IsViewerEnabled() const noexcept
    {
        std::lock_guard<std::mutex> lock(m_dataViewerMapLock);
        return m_dataViewerCollection.empty() == false;
    }
} ARIASDK_NS_END