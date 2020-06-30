
#ifndef WZ_DISABLE_EXTENSION_LOGS


    #ifndef WZ_EXTENSIONS_USE_CORE_LOGGER

        #define WZ_EXT_TRACE    WZ_TRACE
        #define WZ_EXT_DEBUG    WZ_DEBUG
        #define WZ_EXT_INFO     WZ_INFO
        #define WZ_EXT_WARN     WZ_WARN
        #define WZ_EXT_ERROR    WZ_ERROR
        #define WZ_EXT_CRTICIAL WZ_CRITICAL

    #else
        #define WZ_EXT_TRACE    WZ_CORE_TRACE
        #define WZ_EXT_DEBUG    WZ_CORE_DEBUG
        #define WZ_EXT_INFO     WZ_CORE_INFO
        #define WZ_EXT_WARN     WZ_CORE_WARN
        #define WZ_EXT_ERROR    WZ_CORE_ERROR
        #define WZ_EXT_CRTICIAL WZ_CORE_CRITICAL
    #endif

#else

    #define WZ_EXT_TRACE
    #define WZ_EXT_DEBUG
    #define WZ_EXT_INFO
    #define WZ_EXT_WARN
    #define WZ_EXT_ERROR
    #define WZ_EXT_CRTICIAL

#endif

#define __COMPONENT_DECL(t) struct t : public Component<t>
#define __SYSTEM_DECL(t, ...) class t : public System<t, __VA_ARGS__>