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
    // --------------------------------------------------------------------------------------------
    // Renderer

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
            LockableGuard l(target);
            target.bind();

            command.prepare();
            command.render();
            command.finish();
        });
    }

    std::future < void > Renderer::render(RenderTarget& target, RenderPass& pass)
    {
        return std::async(std::launch::async, [&target, &pass]()
        {
            LockableGuard l(target);
            target.bind();
            
            RenderPipelinePtr pipeline = pass.pipeline();
            RenderCommandPtr command = pass.command();

            pipeline->bind();
            command->prepare();
            command->render();
            command->finish();
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

    std::future < ShaderPtr > Renderer::newShader(const std::string& name, const std::string& filename, ShaderType type, const Params& params)
    {
        return std::async(std::launch::async, [this, name, filename, type, params]
        {
            LockableGuard l(*this);

            ShaderPtr shader = mShaderManager.findName(name);

            if (shader && !(shader->filename() == filename))
                throw AlreadyLoaded("Renderer", "newShader", "Shader %s already loaded with file %s.", 
                    name.data(), shader->filename().data());

            if (!shader)
            {
                shader = _createShader(*this, name);
                if (!shader)
                    throw NullError("Renderer", "newShader", "Cannot create shader %s.", 
                        name.data());
                shader->load(type, filename, params).get();
                mShaderManager.add(shader);
            }

            return shader;
        });
    }

    std::future < RenderPipelinePtr > Renderer::newPipeline(const std::string& name, bool checkUnique)
    {
        return std::async(std::launch::async, [this, name, checkUnique]
        {
            LockableGuard l(*this);

            RenderPipelinePtr pipeline = mPipelineManager.findName(name);

            if (pipeline) 
            {
                if (checkUnique)
                    throw AlreadyLoaded("Renderer", "newPipeline", "Pipeline %s is not a unique name.",
                        name.data());
                return pipeline;
            }

            pipeline = _createPipeline(*this, name);
            mPipelineManager.add(pipeline);

            return pipeline;
        });
    }

    RenderPassPtr Renderer::newPass(const std::string& name, const RenderPipelinePtr& pipeline, const RenderCommandPtr& command, bool checkUnique)
    {
        LockableGuard l(*this);

        RenderPassPtr pass = mPassManager.findName(name);

        if (pass)
        {
            if (checkUnique)
                throw AlreadyLoaded("Renderer", "newPass", "Pass %s is not a unique name.",
                    name.data());
            
            pass->setPipeline(pipeline);
            pass->setCommand(command);
            return pass;
        }

        pass = RenderPass::New(*this, name, pipeline, command);
        mPassManager.add(pass);

        return pass;
    }

    // --------------------------------------------------------------------------------------------
    // ModuleRendererLoader
    
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
