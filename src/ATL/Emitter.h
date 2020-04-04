//
//  Emitter.h
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef Emitter_h
#define Emitter_h

#include "Error.h"

#include <mutex>
#include <memory>
#include <vector>
#include <cstdint>
#include <future>
#include <functional>

namespace Atl
{
    struct ListenerBase 
    { virtual ~ListenerBase() = default; };

    typedef std::shared_ptr < ListenerBase > ListenerBasePtr;
    typedef std::weak_ptr < ListenerBase > ListenerBaseWeak;
    typedef std::vector < ListenerBasePtr > ListenerBaseList;
    typedef std::vector < ListenerBaseWeak > ListenerBaseWList;

    struct ListenerBadCast : public Error
    { using Error::Error; };

    //! @brief Defines a generic event emitter.
    //! An Emitter can send any type of event to its listener. However, the listener
    //! must be castable to the correct event's listener class. In other words, if
    //! your listener class is Foo, your listener must be castable to Foo with
    //! dynamic_cast < Foo* >(). 
    //!
    //! @section Declaring a new Listener Class
    //! The new listener should always derive from ListenerBase. If your listener 
    //! will be used by another class, that already defines its own listener, derive
    //! from this class. For example, if you create a new RenderNode, you can create
    //! a custom listener deriving from RenderNode::Listener. Your custom emitter
    //! should define the typedef Listener with the correct class to listen to your
    //! class. 
    //!
    //! @section Sending events
    //! The emitter sends event through two versions of \ref send(). One version sends
    //! an event in the form 'void Listener::onXXX(args...)', and the second version
    //! sends in the form 'type Listener::onXXX(args...)'. 
    //! If your event doesn't require any answer, the listener should define it with 
    //! a void return type. If your event does require an answer, you can return anything
    //! you want from the listener's function, but the type must be storable in a std::vector.
    //! What the emitter will do with this function depends on the implementation.
    class Emitter
    {
        //! @brief The list of listeners for this emitter.
        ListenerBaseWList mListeners;

        //! @brief The mutex.
        mutable std::mutex mMutex;

    public:

        //! @brief Constructs a new Emitter.
        Emitter() = default;

        //! @brief Destructs the Emitter.
        virtual ~Emitter() = default;

        //! @brief Sends an event through registered listeners.
        //! @param callback The function inside the Listener we want to call, as 
        //! an event trigger. The function is of the form '&Listener::onXXX'. 
        //! @param args The arguments passed to the function.
        //! @return A std::vector of returns from each function called.
        template < typename Return, typename T, typename... Args >
        std::future < std::vector < Return > > 
        send(Return(T::*callback)(Args...), Args&&... args) const
        {
            return std::async(std::launch::async, [this, &callback, &args...]()
            {
                std::lock_guard l(mMutex);
                std::vector < Return > results;

                for (const ListenerBaseWeak& listener : mListeners)
                {
                    ListenerBasePtr listenerPtr = listener.lock();

                    if (listenerPtr)
                    {
                        T* derivedPtr = dynamic_cast < T* >(listenerPtr.get());

                        if (!derivedPtr)
                            throw ListenerBadCast("Emitter", "send", "Listener is not a %s.", typeid(T).name());
                        
                        Return r = (derivedPtr->*callback)(std::forward < Args >(args)...);
                        results.push_back(r);
                    }
                }

                return results;
            });
        }

        //! @brief Sends an event through registered listeners.
        //! @param callback The function inside the Listener we want to call, as 
        //! an event trigger. The function is of the form '&Listener::onXXX'. 
        //! @param args The arguments passed to the function.
        template < typename T, typename... Args >
        std::future < void > 
        send(void(T::*callback)(Args...), Args&&... args) const
        {
            return std::async(std::launch::async, [this, &callback, &args...]()
            {
                std::lock_guard l(mMutex);

                for (const ListenerBaseWeak& listener : mListeners)
                {
                    ListenerBasePtr listenerPtr = listener.lock();

                    if (listenerPtr)
                    {
                        T* derivedPtr = dynamic_cast < T* >(listenerPtr.get());

                        if (!derivedPtr)
                            throw ListenerBadCast("Emitter", "send", "Listener is not a %s.", typeid(T).name());
                        
                        (derivedPtr->*callback)(std::forward < Args >(args)...);
                    }
                }
            });
        }

        //! @brief Adds a listener derived from ListenerBase.
        template < typename T >
        void addListener(const std::shared_ptr < T >& rhs)
        {
            addListener(std::static_pointer_cast < ListenerBase >(rhs));
        }

        //! @brief Adds a new listener for this emitter.
        void addListener(const ListenerBasePtr& rhs)
        {
            std::lock_guard l(mMutex);
            mListeners.push_back(rhs);
        }

        //! @brief Removes a listener derived from ListenerBase.
        template < typename T >
        void removeListener(const std::shared_ptr < T >& rhs)
        {
            removeListener(std::static_pointer_cast < ListenerBase >(rhs));
        }

        //! @brief Removes a listener from this emitter.
        void removeListener(const ListenerBasePtr& rhs)
        {
            std::lock_guard l(mMutex);
            mListeners.erase(std::find_if(mListeners.begin(), 
                mListeners.end(),
                [&rhs](const ListenerBaseWeak& weak){
                    return weak.lock() == rhs;
            }));
        }

        //! @brief Removes all listeners.
        void removeAllListeners()
        {
            std::lock_guard l(mMutex);
            mListeners.clear();
        }
    };
}

#endif /* Emitter_h */
