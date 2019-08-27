// Copyright (c) Microsoft. All rights reserved.
#include "common/Common.hpp"
#include "../modules/dataviewer/DefaultDataViewer.hpp"
#include "public/IHttpClient.hpp"

using namespace testing;
using namespace MAT;

class MockHttpClient : public MAT::IHttpClient
{
public:
    MockHttpClient() {}

    IHttpRequest* CreateRequest() override
    {
        return new MAT::SimpleHttpRequest("MockSimpleHttpRequest");
    }

    std::function<void(MAT::IHttpRequest*, MAT::IHttpResponseCallback*)> funcSendRequestAsync;
    void SendRequestAsync(MAT::IHttpRequest* request, MAT::IHttpResponseCallback* callback) override
    {
        if (funcSendRequestAsync)
        {
            funcSendRequestAsync(request, callback);
        }
        else
        {
            m_request = std::shared_ptr<MAT::IHttpRequest>(request);
            m_responseCallback = std::shared_ptr<MAT::IHttpResponseCallback>(callback);
        }
    }

    void CancelRequestAsync(std::string const&) override
    {
    }

    void CancelAllRequests() override {}

    std::shared_ptr<MAT::IHttpRequest>& GetReceivedRequest() noexcept
    {
        return m_request;
    }

    std::shared_ptr<MAT::IHttpResponseCallback>& GetResponseCallback() noexcept
    {
        return m_responseCallback;
    }

private:
    std::shared_ptr<MAT::IHttpRequest> m_request;
    std::shared_ptr<MAT::IHttpResponseCallback> m_responseCallback;
};

class MockDefaultDataViewer : public MAT::DefaultDataViewer
{
public:
    MockDefaultDataViewer(std::shared_ptr<IHttpClient> httpClient, const char* machineFriendlyIdentifier)
        : DefaultDataViewer(httpClient, machineFriendlyIdentifier) {}

    using MAT::DefaultDataViewer::GetHttpClient;
    using MAT::DefaultDataViewer::GetMachineFriendlyIdentifier;
    using MAT::DefaultDataViewer::IsTransmissionEnabled;
    using MAT::DefaultDataViewer::SetTransmissionEnabled;
};

TEST(DefaultDataViewerTests, Constructor_HttpClientNotPassed_HttpClientSetsOrThrowsBasedOnConfig)
{
#ifdef HAVE_MAT_DEFAULT_HTTP_CLIENT
    MockDefaultDataViewer viewer(nullptr, "Test");
    ASSERT_TRUE(viewer.GetHttpClient());
#else
    ASSERT_THROW(MockDefaultDataViewer(nullptr, "Test"), std::invalid_argument);
#endif
}

TEST(DefaultDataViewerTests, Constructor_MachineIdentifierValid_MachineIdentifierSetCorrectly)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    ASSERT_EQ(viewer.GetMachineFriendlyIdentifier(), "Test");
}

TEST(DefaultDataViewerTests, Constructor_MachineIdentifierInvalid_ThrowsInvalidArgument)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    ASSERT_THROW(MockDefaultDataViewer(mockHttpClient, NULL), std::invalid_argument);
    ASSERT_THROW(MockDefaultDataViewer(mockHttpClient, ""), std::invalid_argument);
    ASSERT_THROW(MockDefaultDataViewer(mockHttpClient, "   "), std::invalid_argument);
}

TEST(DefaultDataViewerTests, EnableRemoteViewer_ValidEndpoint_TransmissionEnabled)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());

    mockHttpClient->funcSendRequestAsync = [](MAT::IHttpRequest*, MAT::IHttpResponseCallback* callback)
    {
        auto response = std::unique_ptr<MAT::SimpleHttpResponse>(new SimpleHttpResponse("1"));
        response->m_statusCode = 200;
        callback->OnHttpResponse(response.get());
    };
    
    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    viewer.EnableRemoteViewer("TestEndpoint");
    
    ASSERT_TRUE(viewer.IsTransmissionEnabled());
}

TEST(DefaultDataViewerTests, EnableRemoteViewer_InvalidEndpoint_TransmissionNotEnabled)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());

    mockHttpClient->funcSendRequestAsync = [](MAT::IHttpRequest*, MAT::IHttpResponseCallback* callback)
    {
        auto response = std::unique_ptr<MAT::SimpleHttpResponse>(new SimpleHttpResponse("1"));
        response->m_statusCode = 404;
        response->m_result = HttpResult::HttpResult_NetworkFailure;
        callback->OnHttpResponse(response.get());
    };
    
    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    viewer.EnableRemoteViewer("TestEndpoint");
    
    ASSERT_FALSE(viewer.IsTransmissionEnabled());
}

TEST(DefaultDataViewerTests, DisableViewer_TransmissionEnabled_TransmissionDisabled)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());

    mockHttpClient->funcSendRequestAsync = [](MAT::IHttpRequest*, MAT::IHttpResponseCallback* callback)
    {
        auto response = std::unique_ptr<MAT::SimpleHttpResponse>(new SimpleHttpResponse("1"));
        response->m_statusCode = 200;
        callback->OnHttpResponse(response.get());
    };

    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    viewer.SetTransmissionEnabled(true);

    viewer.DisableViewer();
    ASSERT_FALSE(viewer.IsTransmissionEnabled());
}

TEST(DefaultDataViewerTests, DiableViewer_TransmissionDisabled_TransmissionDisabled)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    viewer.SetTransmissionEnabled(false);

    ASSERT_FALSE(viewer.IsTransmissionEnabled());
    viewer.DisableViewer();
    ASSERT_FALSE(viewer.IsTransmissionEnabled());
}

TEST(DefaultDataViewerTests, DiableViewer_CallOnDisableNotification_NotificationCalled)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    
    bool onDisableNotificationCalled { false };
    viewer.RegisterOnDisableNotification([&onDisableNotificationCalled]() noexcept
        {
            onDisableNotificationCalled = true;
        });

    viewer.DisableViewer();
    ASSERT_TRUE(onDisableNotificationCalled);
}

TEST(DefaultDataViewerTests, DiableViewer_CallMultipleOnDisableNotifications_NotificationsCalled)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    
    int onDisableNotificationCalled { 0 };
    std::function<void()> notificationA = [&onDisableNotificationCalled]() noexcept
    {
        onDisableNotificationCalled++;
    };
    std::function<void()> notificationB = [&onDisableNotificationCalled]() noexcept
    {
        onDisableNotificationCalled++;
    };
    std::function<void()> notificationC = [&onDisableNotificationCalled]() noexcept
    {
        onDisableNotificationCalled++;
    };

    viewer.RegisterOnDisableNotification(notificationA);
    viewer.RegisterOnDisableNotification(notificationB);
    viewer.RegisterOnDisableNotification(notificationC);

    viewer.DisableViewer();
    ASSERT_EQ(onDisableNotificationCalled, 3);
}

TEST(DefaultDataViewerTests, EnableLocalViewer_ThrowsLogicError)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    
    MockDefaultDataViewer viewer(mockHttpClient, "Test");

    ASSERT_THROW(viewer.EnableLocalViewer(), std::logic_error);
    ASSERT_THROW(viewer.EnableLocalViewer("AppId", "AppPackage"), std::logic_error);
}

TEST(DefaultDataViewerTests, ReceiveData_TransmissionNotEnabled_DoesntSendsDataToHttpClient)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    bool wasSendRequestAsyncCalled { false };
    mockHttpClient->funcSendRequestAsync = [&wasSendRequestAsyncCalled](MAT::IHttpRequest*, MAT::IHttpResponseCallback*)
    {
        wasSendRequestAsyncCalled = true;
    };

    MockDefaultDataViewer viewer(mockHttpClient, "Test");

    viewer.ReceiveData(std::vector<std::uint8_t>{});
    ASSERT_FALSE(wasSendRequestAsyncCalled);
}

TEST(DefaultDataViewerTests, ReceiveData_TransmissionEnabled_SendsCorrectBodyToHttpClient)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    int sendRequestAsyncCalledCount { 0 };
    auto sp_request = std::shared_ptr<MAT::SimpleHttpRequest>(new SimpleHttpRequest("1"));
    mockHttpClient->funcSendRequestAsync = [&sendRequestAsyncCalledCount, &sp_request](MAT::IHttpRequest* request, MAT::IHttpResponseCallback* callback)
    {
        sendRequestAsyncCalledCount++;
        auto response = std::unique_ptr<MAT::SimpleHttpResponse>(new SimpleHttpResponse("1"));
        response->m_statusCode = 200;
        callback->OnHttpResponse(response.get());

        sp_request->m_body = request->GetBody();
        sp_request->m_headers = request->GetHeaders();
    };

    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    viewer.SetTransmissionEnabled(true);
    auto packet = std::vector<std::uint8_t> { 1, 2, 3 };
    viewer.ReceiveData(packet);
    ASSERT_EQ(sendRequestAsyncCalledCount, 1);

    ASSERT_EQ(sp_request->GetBody(), packet);
}

TEST(DefaultDataViewerTests, ReceiveData_TransmissionEnabled_SendsCorrectHeadersToHttpClient)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    int sendRequestAsyncCalledCount { 0 };
    auto sp_request = std::shared_ptr<MAT::SimpleHttpRequest>(new SimpleHttpRequest("1"));
    mockHttpClient->funcSendRequestAsync = [&sendRequestAsyncCalledCount, &sp_request](MAT::IHttpRequest* request, MAT::IHttpResponseCallback* callback)
    {
        sendRequestAsyncCalledCount++;
        auto response = std::unique_ptr<MAT::SimpleHttpResponse>(new SimpleHttpResponse("1"));
        response->m_statusCode = 200;
        callback->OnHttpResponse(response.get());

        sp_request->m_body = request->GetBody();
        sp_request->m_headers = request->GetHeaders();
    };

    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    viewer.SetTransmissionEnabled(true);
    viewer.ReceiveData(std::vector<std::uint8_t> { });

    ASSERT_EQ(sendRequestAsyncCalledCount, 1);
    ASSERT_EQ(sp_request->GetHeaders().get("Machine-Identifier"), "Test");
    ASSERT_EQ(sp_request->GetHeaders().get("Content-Type"), "Application/bond-compact-binary");
    ASSERT_FALSE(sp_request->GetHeaders().get("App-Name").empty());
    ASSERT_FALSE(sp_request->GetHeaders().get("App-Platform").empty());
}

TEST(DefaultDataViewerTests, ReceiveData_PacketGoesOutOfScope_SendsCorrectPacketToClient)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());
    int sendRequestAsyncCalledCount { 0 };
    auto l_request = std::shared_ptr<MAT::SimpleHttpRequest>(new SimpleHttpRequest("1"));
    mockHttpClient->funcSendRequestAsync = [&sendRequestAsyncCalledCount, &l_request](MAT::IHttpRequest* request, MAT::IHttpResponseCallback* callback)
    {
        sendRequestAsyncCalledCount++;
        auto response = std::unique_ptr<MAT::SimpleHttpResponse>(new SimpleHttpResponse("1"));
        response->m_statusCode = 200;
        callback->OnHttpResponse(response.get());

        l_request->m_body = request->GetBody();
        l_request->m_headers = request->GetHeaders();
    };

    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    viewer.SetTransmissionEnabled(true);
    
    {
        auto packet = std::vector<std::uint8_t> { 1, 2, 3 };
        viewer.ReceiveData(packet);
    }

    ASSERT_EQ(sendRequestAsyncCalledCount, 1);

    ASSERT_EQ(l_request->GetBody(), (std::vector<std::uint8_t> { 1, 2, 3 }));
    ASSERT_EQ(l_request->GetHeaders().get("Machine-Identifier"), "Test");
    ASSERT_EQ(l_request->GetHeaders().get("Content-Type"), "Application/bond-compact-binary");
    ASSERT_FALSE(l_request->GetHeaders().get("App-Name").empty());
    ASSERT_FALSE(l_request->GetHeaders().get("App-Platform").empty());
}

TEST(DefaultDataViewerTests, ReceiveData_FailToSend_TransmissionDisabled)
{
    auto mockHttpClient = std::shared_ptr<MockHttpClient>(new MockHttpClient());

    mockHttpClient->funcSendRequestAsync = [](MAT::IHttpRequest*, MAT::IHttpResponseCallback* callback)
    {
        auto response = std::unique_ptr<MAT::SimpleHttpResponse>(new SimpleHttpResponse("Failure_Response"));
        response->m_statusCode = 404;
        callback->OnHttpResponse(response.get());
    };

    MockDefaultDataViewer viewer(mockHttpClient, "Test");
    viewer.SetTransmissionEnabled(true);
    auto packet = std::vector<std::uint8_t> { 1, 2, 3 };
    ASSERT_TRUE(viewer.IsTransmissionEnabled());
    viewer.ReceiveData(packet);
    ASSERT_FALSE(viewer.IsTransmissionEnabled());
}