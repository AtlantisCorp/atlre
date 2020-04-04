//
//  Module.cpp
//  atlre
//
//  Created by jacques tronconi on 17/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "Module.h"

namespace Atl
{
    Module::Module(Manager& manager, const std::string& name)
    : TResource(manager, name)
    {
        
    }
    
    Module::~Module()
    {
        if (mHandle)
            unload();
    }
    
    std::future < void > Module::load(const std::string &filename)
    {
        return std::async(std::launch::async, [this, &filename]()
        {
            // We do not have to call preload(), because 1) there is no loader for a Module,
            // and 2) we do not have to check for size available to load a module.
            send(&Listener::onResourceWillLoad, (Resource&)*this);
            
            const std::string extension = File::Extension(filename);
            std::string dynlibName = filename;
            
            if (extension != ATL_DYNLIB_EXTENSION)
                dynlibName = filename + ATL_DYNLIB_EXTENSION;
            
            ATL_DYNLIB_HANDLE handle = ATL_DYNLIB_OPEN(dynlibName.data());
            
            if (!handle)
                throw ModuleNotFound("Module", "load", "Module %s not found.",
                                     dynlibName.data());
            
            ModuleStartFn fnStart = (ModuleStartFn) ATL_DYNLIB_DLSYM(handle, PFNATLMODULESTART_NAME);
            
            if (!fnStart)
                throw ModuleInvalid("Module", "load", "Module %s start function not found.",
                                    dynlibName.data());
            
            ModuleStopFn fnStop = (ModuleStopFn) ATL_DYNLIB_DLSYM(handle, PFNATLMODULESTOP_NAME);
            
            if (!fnStop)
                throw ModuleInvalid("Module", "load", "Module %s stop function not found.",
                                    dynlibName.data());
            
            
            ModuleGetInfosFn fnGetInfos = (ModuleGetInfosFn) ATL_DYNLIB_DLSYM(handle, PFNATLMODULEGETINFOS_NAME);
            // 'GetInfos' is optional: we do not need to have a valid function pointer. However, calling
            // infos() will require it so be sure to call hasInfos() before.
            
            {
                std::lock_guard l(mMutex);
                mHandle = handle;
                mFnStart = fnStart;
                mFnStop = fnStop;
                mFnGetInfos = fnGetInfos;
            }
            
            postload(filename);
        });
    }
    
    std::future < void > Module::unload()
    {
        return std::async(std::launch::async, [this]()
        {
            if (!mHandle)
                throw ModuleInvalid("Module", "unload", "Module %s has no handle.",
                                    name().data());
            
            send(&Listener::onResourceWillUnload, (Resource&)*this);
            
            {
                std::lock_guard l(mMutex);
                ATL_DYNLIB_CLOSE(mHandle);
                mHandle = nullptr;
            }
            
            setState(0);
            send(&Listener::onResourceDidUnload, (Resource&)*this);
        });
    }
    
    std::future < void > Module::start()
    {
        return std::async(std::launch::async, [this]()
        {
            if (!mFnStart)
                throw ModuleInvalid("Module", "start", "Module %s has no handle.",
                                    name().data());
            
            {
                std::lock_guard l(mMutex);
                mFnStart();
            }
            
            send(&Listener::onModuleDidStart, *this);
        });
    }
    
    std::future < void > Module::stop()
    {
        return std::async(std::launch::async, [this]()
        {
            if (!mFnStop)
                throw ModuleInvalid("Module", "start", "Module %s has no handle.",
                                    name().data());
            
            send(&Listener::onModuleWillStop, *this);
            
            {
                std::lock_guard l(mMutex);
                mFnStop();
            }
        });
    }
    
    const ModuleInfos* Module::infos() const
    {
        if (!mFnGetInfos)
            throw ModuleInvalid("Module", "infos", "Module %s has no GetInfos function.",
                                name().data());
        
        return mFnGetInfos();
    }
    
    void* Module::symbolC(const char *name)
    {
        if (!mHandle)
            throw ModuleInvalid("Module", "symbol", "Module %s has no handle.",
                                this->name().data());
        
        return ATL_DYNLIB_DLSYM(mHandle, name);
    }
}
