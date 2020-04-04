//
//  Module.h
//  atlre
//
//  Created by jacques tronconi on 17/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef Module_h
#define Module_h

#include "Platform.h"
#include "Resource.h"
#include "ResourceListener.h"
#include "Emitter.h"

#include <functional>

namespace Atl
{
    class Module;
    class ModuleListener;
    class ModuleManager;
    class ModuleManagerListener;
    
    //! @brief Defines some structure to be returned by your 'GetInfos' function.
    //! The structure should be allocated on the heap in your main module, and a long
    //! pointer to the data should be returned.
    struct EXPORTED ModuleInfos
    {
        //! @brief The name of the module.
        std::string name;
        //! @brief The name of the main author, or all of the authors. (optional)
        std::string author;
        //! @brief A string qualifying a version number for this module. (optional)
        std::string version;
        //! @brief An UUID identifying this module. (optional)
        std::string uuid;
    };
    
#   define PFNATLMODULESTART    void(*)()
#   define PFNATLMODULESTOP     void(*)()
#   define PFNATLMODULEGETINFOS ModuleInfos*(*)()
    typedef void(*ModuleStartFn)();
    typedef void(*ModuleStopFn)();
    typedef ModuleInfos*(*ModuleGetInfosFn)();
    
    //! @brief Defines the name of the 'Start' function to load in your modules.
#   define PFNATLMODULESTART_NAME "AtlModuleStart"
    //! @brief Defines the name of the 'Stop' function to load in your modules.
#   define PFNATLMODULESTOP_NAME "AtlModuleStop"
    //! @brief Defines the name of the 'GetInfos' function to load in your modules.
#   define PFNATLMODULEGETINFOS_NAME "AtlModuleGetInfos"
    
    struct EXPORTED ModuleNotFound : public Error
    { using Error::Error; };
    
    struct EXPORTED ModuleInvalid : public Error
    { using Error::Error; };
    
    //! @brief A Listener for a Module.
    class EXPORTED ModuleListener : public ResourceListener
    {
    public:
        virtual ~ModuleListener() = default;
        //! @brief Called before the Module's Start function is called.
        virtual void onModuleWillStart(Module&) {}
        //! @brief Called after the Module's Start function has been called.
        virtual void onModuleDidStart(Module&) {}
        //! @brief Called before the Module's Stop function is called.
        virtual void onModuleWillStop(Module&) {}
        //! @brief Called after the Module's Stop function has been called.
        virtual void onModuleDidStop(Module&) {}
    };
    
    //! @brief The Module Manager Listener.
    struct EXPORTED ModuleManagerListener :
    virtual public ManagerListener <
    ModuleManager,
    Module,
    ModuleManagerListener >{};
    
    //! @brief The Module Manager.
    struct EXPORTED ModuleManager :
    public Manager <
    ModuleManager,
    Module,
    ModuleManagerListener >{};
    
    //! @brief Defines a base class for a Module.
    //! A Module is a dynamic loaded library, or a shared object (thx linux). A Module must
    //! have a 'start' and a 'stop' function, however the 'GetInfos' function is optional.
    //!
    //! \see setStrictMode() to define the behaviour on 'GetInfos'.
    //!
    //! @note When loading your Module, please specify the name without the extension, or
    //! uses ATL_DYNLIB_EXTENSION macro to add the extension to the name manually.
    class EXPORTED Module : public TResource < Module, ModuleManager >
    {
        //! @brief Handle to the shared library.
        ATL_DYNLIB_HANDLE mHandle;
        //! @brief Pointer to the start function.
        ModuleStartFn mFnStart;
        //! @brief Pointer to the stop function.
        ModuleStopFn mFnStop;
        //! @brief Pointer to the GetInfos function.
        ModuleGetInfosFn mFnGetInfos;
        
        mutable std::mutex mMutex;
        
    public:
        typedef ModuleListener Listener;
        
        //! @brief Default constructor.
        Module(Manager& manager, const std::string& name);
        
        //! @brief Default destructor.
        ~Module();
        
        //! @brief Loads the shared library.
        //! @param filename A file path to the shared library. This path may or may not
        //! have the required extension. If you set the extension, use the macro
        //! ATL_DYNLIB_EXTENSION.
        //! @return A std::future that catches exceptions when std::future::get() is
        //! called.
        std::future < void > load(const std::string& filename);
        
        //! @brief Unloads the shared library.
        //! @return A std::future that catches exceptions when std::future::get() is
        //! called.
        std::future < void > unload();
        
        //! @brief Starts the shared library.
        //! @return A std::future that catches exceptions when std::future::get() is
        //! called.
        std::future < void > start();
        
        //! @brief Stops the shared library.
        //! @return A std::future that catches exceptions when std::future::get() is
        //! called.
        std::future < void > stop();
        
        //! @brief Returns a pointer to the ModuleInfos* structure.
        const ModuleInfos* infos() const;
        
        //! @brief Tries to load a function in this Module.
        template < typename Fn > auto symbol(const char* name) {
            return reinterpret_cast < Fn >(symbolC(name));
        }
        
        //! @brief Tries to load a function in this Module.
        void* symbolC(const char* name);
        
        //! @brief Always return Zero.
        inline std::size_t usedSize() const { return 0; }
    };
    
    template < >
    class EXPORTED LoaderTrait < Module > : public BaseLoader
    {
    public:
        void load(Module& module, const std::string& filename, const Params& params) {
            module.load(filename).get();
        }
    };
    
    ATL_BASE_RESOURCE(Module)
    ATL_LOADER_RESOURCE(Module)
}

#endif /* Module_h */
