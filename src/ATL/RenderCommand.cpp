//
//  RenderCommand.cpp
//  atl
//
//  Created by jacques tronconi on 21/03/2020.
//

#include "RenderCommand.h"

namespace Atl
{
    RenderCommandBase::RenderCommandBase(Renderer& rhs)
    : RenderObject(rhs)
    {
        
    }
    
    RenderCommand::RenderCommand(Renderer& rhs)
    : RenderCommandBase(rhs)
    {
        
    }
    
    void RenderCommand::addSubCommand(const RenderCommandBasePtr &subCommand)
    {
        std::lock_guard l(mMutex);
        mSubCommands.push_back(subCommand);
    }
    
    void RenderCommand::addSubCommands(const RenderCommandBaseList &subCommands, bool skipNulls)
    {
        std::lock_guard l(mMutex);

        if (!skipNulls)
            mSubCommands.insert(mSubCommands.end(), subCommands.begin(), subCommands.end());

        else
        {
            for (auto const& command : subCommands)
            {
                if (!command)
                    continue;

                mSubCommands.push_back(command);
            }
        }
    }
    
    void RenderCommand::removeSubCommand(const RenderCommandBasePtr &subCommand)
    {
        std::lock_guard l(mMutex);
        
        RenderCommandBaseList::iterator it = std::find(mSubCommands.begin(),
                                                       mSubCommands.end(),
                                                       subCommand);
        
        if (it != mSubCommands.end())
            mSubCommands.erase(it);
    }
    
    void RenderCommand::removeAllSubCommands()
    {
        std::lock_guard l(mMutex);
        mSubCommands.clear();
    }
    
    void RenderCommand::prepare()
    {
        
    }
    
    void RenderCommand::finish()
    {
        
    }
    
    void RenderCommand::render()
    {
        std::lock_guard l(mMutex);
        
        for (RenderCommandBasePtr& subCommand : mSubCommands)
        {
            if (subCommand)
            {
                subCommand->prepare();
                subCommand->render();
                subCommand->finish();
            }
        }
    }

    void RenderCommand::lock() const
    {
        mMutex.lock();
    }

    void RenderCommand::unlock() const
    {
        mMutex.unlock();
    }
}
