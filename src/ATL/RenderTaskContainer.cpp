//
//  RenderTaskContainer.cpp
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#include "RenderTaskContainer.h"
#include "Error.h"

namespace Atl
{
    void RenderTaskContainer::add(const RenderablePtr& rhs)
    {
        if (!rhs)
            throw NullError("RenderTaskContainer", "add", "Null renderable passed.");

        std::lock_guard l(mMutex);
        std::weak_ptr < Renderable > weak(rhs);

        mOrderedTasks.push_back([weak](RenderCommand& command){ 
            if (!weak.expired())
                return weak.lock()->render(command).get(); 
        });
    }

    void RenderTaskContainer::add(RenderTaskFunction function)
    {
        std::lock_guard l(mMutex);
        mOrderedTasks.push_back(function);
    }

    void RenderTaskContainer::addUnordered(const RenderablePtr& rhs)
    {
        if (!rhs)
            throw NullError("RenderTaskContainer", "add", "Null renderable passed.");

        std::lock_guard l(mMutex);
        std::weak_ptr < Renderable > weak(rhs);

        mUnorderedTasks.push_back([weak](RenderCommand& command){ 
            if (!weak.expired())
                return weak.lock()->render(command).get(); 
        });
    }

    void RenderTaskContainer::addUnordered(RenderTaskFunction function)
    {
        std::lock_guard l(mMutex);
        mUnorderedTasks.push_back(function);
    }

    std::future < void > RenderTaskContainer::build(Renderer&)
    {
        return {};
    }

    std::future < void > RenderTaskContainer::render(RenderCommand& command) const
    {
        return std::async(std::launch::async, [this, &command]() 
        {
            typedef std::future < void > TaskResult;
            typedef std::vector < TaskResult > TaskResultList;
            TaskResultList tasksResults;
            std::lock_guard l(mMutex);

            // Launches the unordered tasks first.

            for (const RenderTaskFunction& fun : mUnorderedTasks)
            {
                TaskResult result = std::async(std::launch::async, [&fun, &command](){ fun(command); });
                tasksResults.push_back(std::move(result));
            }

            // Launches every ordered tasks.

            for (const RenderTaskFunction& fun : mOrderedTasks)
                fun(command);

            // Now wait for all unordered tasks.

            for (TaskResult& result : tasksResults)
                result.get();
        });
    }

    std::size_t RenderTaskContainer::size(Renderer&) const
    {
        return 0;
    }

    void RenderTaskContainer::clearOrderedTasks()
    {
        std::lock_guard l(mMutex);
        mOrderedTasks.clear();
    }

    void RenderTaskContainer::clearUnorderedTasks()
    {
        std::lock_guard l(mMutex);
        mUnorderedTasks.clear();
    }

    void RenderTaskContainer::clear()
    {
        std::lock_guard l(mMutex);
        mOrderedTasks.clear();
        mUnorderedTasks.clear();
    }
}