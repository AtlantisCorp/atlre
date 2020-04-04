//
//  Renderer.cpp
//  atl
//
//  Created by jacques tronconi on 22/03/2020.
//

#include "Renderer.h"
#include "Module.h"

namespace Atl
{
    Renderer::Renderer(Manager& manager, const std::string& name)
    : TResource(manager, name), mSurfaces(*this), mBuffManager(*this)
    {
        RendererLoaderDb::Get().addLoader("", std::make_shared < ModuleRendererLoader >());
        setCommandConstructor<RenderCommand, RenderCommand>();
    }
    
    bool Renderer::areAllSurfacesClosed() const
    {
        std::atomic_flag isClosed;
        
        mSurfaces.forEach([&isClosed](const RenderSurfacePtr& rhs)
        {
            if (rhs && rhs->isClosed())
                isClosed.test_and_set();
        });
        
        return isClosed.test_and_set();
    }
    
    std::future < RenderWindowPtr > Renderer::newWindow(const std::string &name, const Params &params)
    {
        return std::async(std::launch::async, [this, &name, &params]()
        {
            RenderSurfacePtr surface = mSurfaces.make("RenderWindow", *this, name, params);
            
            if (surface)
            {
                mSurfaces.add(surface);
                return std::dynamic_pointer_cast < RenderWindow >(surface);
            }
            
            return RenderWindowPtr();
        });
    }
    
    std::future < void > Renderer::render(RenderTarget& target, RenderCommand& command)
    {
        return std::async(std::launch::async, [&target, &command]()
        {
            target.bind();
            command.prepare();
            command.render();
            command.finish();
        });
    }
    
    RenderHdwBufferPtr Renderer::newHdwBuffer(const std::type_index& type, std::size_t sz)
    {
        if (!mBuffManager.isSizeAvailable(sz))
            throw NotEnoughMemory("Renderer", "newHdwBuffer", "Memory limit exceeded for %i bytes.",
                                  sz);
        
        RenderHdwBufferPtr buffer = mBuffManager.make(type);
        buffer->allocate(sz);
        
        mBuffManager.add(buffer);
        
        return buffer;
    }
    
    RenderHdwBufferPtr Renderer::newHdwBuffer(const std::type_index& type, std::size_t sz, const void* mem)
    {
        if (!mem)
            throw NullError("Renderer", "newHdwBuffer", "Null memory passed.");

        if (!mBuffManager.isSizeAvailable(sz))
            throw NotEnoughMemory("Renderer", "newHdwBuffer", "Memory limit exceeded for %i bytes.",
                                  sz);

        RenderHdwBufferPtr buffer = mBuffManager.make(type);
        buffer->allocate(sz, mem);
        
        mBuffManager.add(buffer);
        
        return buffer;
    }

    RenderHdwBufferManager& Renderer::hdwBufferManager() 
    {
        return mBuffManager;
    }

    const RenderHdwBufferManager& Renderer::hdwBufferManager() const 
    {
        return mBuffManager;
    }
    
    void ModuleRendererLoader::load(Renderer &rhs, const std::string &filename, const Params &params)
    {
        ModulePtr module = ModuleManager::Get().loadOrGet(filename, filename).get();
        
        if (!module)
            throw NullError("ModuleRendererLoader", "load", "File %s invalid.",
                            filename.data());
        
        std::string loadRendererFnName = "FillRenderer";
        auto it = params.find("loadRendererFnName");
        
        if (it != params.end())
            loadRendererFnName = std::any_cast < std::string >(it->second);
        
        typedef void(*LoadRendererFn)(Renderer&, const Params&);
        auto fn = module->symbol<LoadRendererFn>(loadRendererFnName.data());
        
        if (!fn)
            throw NullError("ModuleRendererLoader", "load", "Module %s has no function named %s.",
                            name().data(), loadRendererFnName.data());
        
        fn(rhs, params);
    }
    
    std::string ModuleRendererLoader::name() const
    {
        return "ModuleRendererLoader";
    }
    
    std::size_t ModuleRendererLoader::neededSize(const std::string &, const Params &) const
    {
        return 0;
    }
}
