#include "LogManagerProvider.hpp"

#include "LogManagerFactory.hpp"

namespace ARIASDK_NS_BEGIN {

    ILogManager * LogManagerProvider::Get(
        ILogConfiguration & config,
        status_t &status,
		  IHttpClient* httpClient
#ifdef ANDROID
        , JNIEnv * env
        , jclass contextClass
        , jobject contextObject
#endif
    )
    {
        return LogManagerFactory::Get(config, status, httpClient);
    }

    // TODO: consider utilizing a default reference
    ILogManager* LogManagerProvider::Get(
        const char * moduleName,
        status_t& status
#ifdef ANDROID
        JNIEnv *env,
        jclass contextClass,
        jobject  contextObject,
#endif
    )
    {
        return LogManagerFactory::Get(moduleName, status);
    }

    /// <summary> 
    /// Releases the LogManager identified by moduleName
    /// <param name="moduleName">Module name</param> 
    /// </summary> 
    status_t LogManagerProvider::Release(const char * moduleName)
    {
        return LogManagerFactory::Release(moduleName);
    }

    /// <summary>
    /// Releases the specified LogManager identified by its log configuration
    /// </summary>
    /// <param name="logConfiguration">The log configuration.</param>
    /// <returns></returns>
    status_t LogManagerProvider::Release(ILogConfiguration & config)
    {
        return LogManagerFactory::Release(config);
    }

} ARIASDK_NS_END
