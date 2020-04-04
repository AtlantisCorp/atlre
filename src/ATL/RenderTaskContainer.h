//
//  RenderTaskContainer.h
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#ifndef RenderTaskContainer_h
#define RenderTaskContainer_h

#include "Renderable.h"
#include <functional>
#include <vector>

namespace Atl
{
    //! @brief Defines a rendering task.
    typedef std::function < void(RenderCommand&) > RenderTaskFunction;
    //! @brief Defines multiple rendering tasks.
    typedef std::vector < RenderTaskFunction > RenderTaskFunctionList;

    //! @brief A Renderable container.
    //! This container holds Render functions (\see RenderTaskFunction) and calls them 
    //! on \ref render(), with the appropriate RenderCommand. This enables grouping of
    //! multiple Render functions. 
    //! Render Functions are separated into *ordered* and *unordered* tasks. The *ordered*
    //! tasks are called one by one, waiting for the previous one to finish. The *unordered*
    //! ones are called all at the same time wrapped into std::async(), and are waited 
    //! at the end of this container's \ref render() function.
    class EXPORTED RenderTaskContainer : public Renderable
    {
        //! @brief Holds ordered rendering task.
        RenderTaskFunctionList mOrderedTasks;

        //! @brief Holds unordered tasks.
        RenderTaskFunctionList mUnorderedTasks;

        //! @brief The mutex.
        mutable std::mutex mMutex;

    public:

        //! @brief Defines a Renderable under a std::weak_ptr.
        typedef std::weak_ptr < Renderable > RenderableWeak;

        //! @brief Adds a renderable to this container.
        //! @param rhs A non null Renderable pointer. The Renderable is added by constructing
        //! a simple RenderTaskFunction which calls \ref Renderable::render() and then returns
        //! after waiting for the function (\ref std::future::get()).
        //! @note The function is added to the list of *ordered* tasks.
        void add(const RenderablePtr& rhs);

        //! @brief Adds a render function to this container.
        //! @param function A function of type \ref RenderTaskFunction. The function is added
        //! to the list of *ordered* tasks.
        void add(RenderTaskFunction function);

        //! @brief Adds a renderable to the unordered tasks of this container.
        //! @param rhs A non null Renderable pointer. The Renderable is added by constructing
        //! a simple RenderTaskFunction which calls \ref Renderable::render() and then returns
        //! after waiting for the function (\ref std::future::get()).
        //! @note The function is added to the list of *unordered* tasks.
        void addUnordered(const RenderablePtr& rhs);

        //! @brief Adds a render function to this container.
        //! @param function A function of type \ref RenderTaskFunction. The function is added
        //! to the list of *unordered* tasks.
        void addUnordered(RenderTaskFunction function);

        //! @brief Does nothing.
        //! This function should build only cache data related to this object. We shouldn't call
        //! renderable's build functions here if we have some renderables in the Render Functions,
        //! so we don't have to store them.
        std::future < void > build(Renderer&);

        //! @brief Renders all Render Functions.
        std::future < void > render(RenderCommand& command) const;

        //! @brief Always return zero, as this object has no cache.
        std::size_t size(Renderer&) const;

        //! @brief Clears all ordered tasks.
        void clearOrderedTasks();

        //! @brief Clears all unordered tasks.
        void clearUnorderedTasks();

        //! @brief Clears all tasks.
        void clear();
    };

    //! @brief Pointer to RenderTaskContainer.
    typedef std::shared_ptr < RenderTaskContainer > RenderTaskContainerPtr;
}

#endif // RenderTaskContainer_h