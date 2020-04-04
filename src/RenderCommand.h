//
//  RenderCommand.h
//  atl
//
//  Created by jacques tronconi on 21/03/2020.
//

#ifndef RenderCommand_h
#define RenderCommand_h

#include "Platform.h"
#include "Factory.h"
#include "RenderObject.h"

#include <memory>
#include <vector>
#include <mutex>
#include <typeindex>

namespace Atl
{
    class Renderer;
    
    //! @brief Interface for a basic rendering command.
    //! A rendering command can be any rendering operation, from rendering multiple child
    //! commands, or rendering vertex data. Those commands are created by the Renderer, and
    //! are valid only for this Renderer.
    //!
    //! @section Deriving from RenderCommandBase
    //! In order to create new RenderCommands, you should derive from RenderCommandBase and
    //! uses the default constructor (with Renderer& ref). Then, you should follow the
    //! RenderCommands Guidelines: make a construct() function for your command. Then, the
    //! command user will have to call \ref Renderer::newCommand < YourCommand >(), and
    //! YourCommand::construct() in order to use it correctly. Then you should provide
    //! the basic prepare(), render() and finish() methods.
    //!
    //! @note
    //! If your custom command doesn't need a construct() method, don't add it as it may confuse
    //! possible users if they should construct it or not (the construct() method may be called
    //! way after for performance optimization).
    class EXPORTED RenderCommandBase : public RenderObject
    {
    public:
        
        //! @brief Default constructor.
        RenderCommandBase(Renderer& rhs);
        
        //! @brief Default destructor.
        virtual ~RenderCommandBase() = default;
        
        //! @brief Prepares the render command for rendering.
        virtual void prepare() = 0;
        
        //! @brief Renders the render command.
        virtual void render() = 0;
        
        //! @brief Finishes all operations from this command.
        virtual void finish() = 0;
    };
    
    typedef std::shared_ptr < RenderCommandBase > RenderCommandBasePtr;
    typedef std::vector < RenderCommandBasePtr > RenderCommandBaseList;
    
    //! @brief A basic RenderCommand that enables sub commands to be rendered.
    class EXPORTED RenderCommand : public RenderCommandBase
    {
        //! @brief The sub commands regitered for this command.
        RenderCommandBaseList mSubCommands;
        
        //! @brief Mutex for data.
        mutable std::mutex mMutex;
        
    public:
        
        //! @brief Default constructor.
        RenderCommand(Renderer& rhs);
        
        //! @brief Default destructor.
        virtual ~RenderCommand() = default;
        
        //! @brief Adds a sub command.
        virtual void addSubCommand(const RenderCommandBasePtr& subCommand);
        
        //! @brief Adds multiple sub commands.
        virtual void addSubCommands(const RenderCommandBaseList& subCommands, bool skipNulls = false);

        //! @brief Adds multiple sub commands, which are from a derived type.
        //! This is slower because we have to cast each pointer inside a new list, but
        //! this is a helper method.
        template < typename T > 
        void addSubCommands(const std::vector < std::shared_ptr < T > >& dCommands, bool skipNulls = false){
            RenderCommandBaseList subCommands;
            subCommands.reserve(dCommands.size());

            for (auto const& dCommand : dCommands)
                subCommands.push_back(std::static_pointer_cast < RenderCommandBase >(dCommand));

            addSubCommands(subCommands, skipNulls);
        }
        
        //! @brief Removes a sub command.
        virtual void removeSubCommand(const RenderCommandBasePtr& subCommand);
        
        //! @brief Removes all sub commands.
        virtual void removeAllSubCommands();
        
        //! @brief Does nothing. \see render().
        virtual void prepare();
        
        //! @brief Does nothing. \see render().
        virtual void finish();
        
        //! @brief Calls, for each sub command, the prepare/render/finish pipeline
        //! in order to render all sub commands in the correct order.
        virtual void render();
    };
    
    typedef std::shared_ptr < RenderCommand > RenderCommandPtr;
    typedef std::vector < RenderCommandPtr > RenderCommandList;
    
    using RenderCommandFactory = Factory <
    std::type_index,
    RenderCommandBasePtr,
    Renderer& >;
}

#endif /* RenderCommand_h */
