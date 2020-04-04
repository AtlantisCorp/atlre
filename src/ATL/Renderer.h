//
//  Renderer.h
//  atl
//
//  Created by jacques tronconi on 22/03/2020.
//

#ifndef Renderer_h
#define Renderer_h

#include "RenderCommand.h"
#include "RenderCacheFactory.h"
#include "Resource.h"
#include "Emitter.h"
#include "RenderWindow.h"

#include "RenderHdwBufferManager.h"
#include "RenderHdwBuffer.h"

namespace Atl
{
    class Renderer;
    class RendererListener;
    class RendererManager;
    class RendererManagerListener;
    
    //! @brief Defines a structure for common initialization parameters among multiple
    //! renderers.
    struct RendererInfos
    {
        //! @brief A structure for a Color format.
        struct 
        {
            //! @brief The red channel's size, in bits.
            unsigned mRed = 8;
            //! @brief The green channel's size, in bits.
            unsigned mGreen = 8;
            //! @brief The blue channel's size, in bits.
            unsigned mBlue = 8;
            //! @brief The alpha channel's size, in bits.
            unsigned mAlpha = 0;
        } mColorSize;

        //! @brief The depth buffer size, in bits. No depth-buffer if zero.
        unsigned mDepthSize = 24;

        //! @brief The stencil buffer size, in bits. No stencil buffer if zero.
        unsigned mStencilSize = 0;

        //! @brief The number of buffers in the swap chain. Typically this is two,
        //! but some renderers support an arbitrary number of swap buffers (DX12, Vulkan).
        unsigned mBuffers = 2;

        //! @brief A structure for a Multisample format.
        struct 
        {
            //! @brief Number of samples per buffer.
            unsigned mSamples = 1;
            //! @brief Number of buffers.
            unsigned mBuffers = 1;
        } mMultisample;
    };
    
    //! @brief Defines a Listener for this resource.
    struct RendererListener :
    virtual public ResourceListener
    {
        //! @brief Destructor.
        virtual ~RendererListener() = default;
    };

    //! @brief Defines an object that renders other objects.
    class Renderer :
        virtual public TResource < Renderer, RendererManager >
    {
        mutable std::mutex mMutex;
        
        //! @brief The RenderCache factory.
        RenderCacheFactory mCacheFactory;
        
        //! @brief The RenderCommand factory.
        RenderCommandFactory mCommandFactory;
        
        //! @brief The surface's manager.
        RenderSurfaceManager mSurfaces;
        
        //! @brief The RenderHdwBuffer manager.
        RenderHdwBufferManager mBuffManager;
        
    public:
        
        //! @brief Constructs a new Renderer.
        //! @param manager The manager that created this renderer.
        //! @param name The name for this renderer.
        Renderer(Manager& manager, const std::string& name);
        
        //! @brief Creates a new Cache of type RenderCache < T >.
        template < typename T >
        auto newCache(T& rhs)
        {
            return mCacheFactory.construct(*this, rhs);
        }
        
        //! @brief Creates a new command of type T. This type must be registered
        //! previously by the renderer's module.
        template < typename T >
        auto newCommand()
        {
            auto cmd = mCommandFactory.construct(typeid(T), *this);
            return std::dynamic_pointer_cast < T >(cmd);
        }
        
        //! @brief Returns the Cache Factory.
        RenderCacheFactory& cacheFactory();
        
        //! @brief Returns the Command Factory.
        RenderCommandFactory& commandFactory();
        
        //! @brief Makes a Constructor for a derived class of RenderCommandBase.
        //! This function is a helper that creates a lambda for a derived class of
        //! the RenderCommandBase. This enables a Renderer's Module to register its
        //! custom render commands.
        template < typename T, typename Derived >
        void setCommandConstructor()
        {
            mCommandFactory.setConstructor(typeid(T), [](Renderer& rhs){
                return std::make_shared < Derived >(rhs);
            });
        }
        
        //! @brief Returns true if all surfaces are closed.
        bool areAllSurfacesClosed() const;
        
        //! @brief Creates a new Window.
        //! @param name The Window's name.
        //! @param params The Parameters to pass to the Window constructor.
        //! @return A std::future to a RenderWindowPtr. As the window's creation may be a time
        //! consuming process, the function is launched asynchroneously. You can still
        //! wait for the result with std::future::get(), which also rethrows exceptions from
        //! the child thread.
        std::future < RenderWindowPtr > newWindow(const std::string& name, const Params& params);
        
        //! @brief Renders a command into a target.
        std::future < void > render(RenderTarget& target, RenderCommand& command);
        
        //! @brief Returns always Zero.
        inline std::size_t usedSize() const { return 0; }
        
        //! @brief Returns a new RenderHdwBuffer of given type and size.
        template < typename T > 
        std::shared_ptr < T > newHdwBuffer(std::size_t sz, const void* ptr = nullptr)
        {
            if (ptr)
                return std::dynamic_pointer_cast < T >(newHdwBuffer(typeid(T), sz));
            else
                return std::dynamic_pointer_cast < T >(newHdwBuffer(typeid(T), sz, ptr));
        }
        
        //! @brief Constructs a new RenderHdwBuffer of given type and size.
        RenderHdwBufferPtr newHdwBuffer(const std::type_index& type, std::size_t sz);
        
        //! @brief Constructs a new RenderHdwBuffer of given type and size.
        RenderHdwBufferPtr newHdwBuffer(const std::type_index& type, std::size_t sz, const void* mem);

        //! @brief Returns the \ref RenderHdwBufferManager for this Renderer.
        RenderHdwBufferManager& hdwBufferManager();

        //! @brief Returns the \ref RenderHdwBufferManager for this Renderer.
        const RenderHdwBufferManager& hdwBufferManager() const;
    };
    
    template < >
    class LoaderTrait < Renderer > : public BaseLoader
    {
    public:
        virtual void load(Renderer& renderer, const std::string& filename, const Params& params) = 0;
    };
    
    class ModuleRendererLoader : public LoaderTrait < Renderer >
    {
    public:
        void load(Renderer& rhs, const std::string& filename, const Params& params);
        
        //! @brief Returns the loader's name.
        std::string name() const;
        
        //! @brief Always return Zero.
        std::size_t neededSize(const std::string&, const Params&) const;
    };
    
    ATL_BASE_RESOURCE(Renderer)
    ATL_LOADER_RESOURCE(Renderer)
    
    struct EXPORTED RendererManagerListener : public ManagerListener <
    RendererManager,
    Renderer,
    RendererManagerListener >{};
    
    struct EXPORTED RendererManager : public Manager <
    RendererManager,
    Renderer,
    RendererManagerListener >{};
}

#endif /* Renderer_h */
