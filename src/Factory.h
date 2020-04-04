//
//  Factory.h
//  atl
//
//  Created by jacques tronconi on 22/03/2020.
//

#ifndef Factory_h
#define Factory_h

#include "Error.h"

#include <functional>
#include <map>
#include <mutex>

namespace Atl
{
    struct FactoryNullConstructor : public Error
    { using Error::Error; };
    
    //! @brief A Generic factory template pattern.
    //! Uses this class if you want to have a factory for your type.
    //! @tparam Key The Key to store the Constructors. It must be usable by a std::map (like
    //! std::string, int, or even std::type_index).
    //! @tparam Return The Return type you want for your Constructors.
    //! @tparam Args The multiple Args needed to call the Constructor.
    template <
    typename Key,
    typename Return,
    typename... Args >
    class Factory
    {
        mutable std::mutex mMutex;
        
        typedef std::function < Return(Args...) > Constructor;
        //! @brief The map to store each constructor for each key.
        std::map < Key, Constructor > mConstructors;
        
    public:
        
        typedef Key key_t;
        typedef Return return_t;
        typedef Constructor constructor_t;
        
        //! @brief Constructs a new object for given key.
        //! @param key The key to look for the constructor.
        //! @param args The args to pass to the constructor.
        Return construct(const Key& key, Args&&... args) {
            std::lock_guard l(mMutex);
            
            auto it = mConstructors.find(key);
            
            if (it == mConstructors.end())
                throw FactoryNullConstructor("Factory", "construct", "Null Constructor.");
            
            return it->second(std::forward < Args >(args)...);
        }
        
        //! @brief Sets a constructor for the given key.
        //! @param key The key for which the constructor will be stored.
        //! @param value The Constructor. This constructor may be a lambda and will be stored
        //! in a std::function.
        void setConstructor(const Key& key, Constructor value) {
            std::lock_guard l(mMutex);
            mConstructors[key] = value;
        }
        
        //! @brief Removes the constructor for given key.
        void removeConstructor(const Key& key) {
            std::lock_guard l(mMutex);
            mConstructors.erase(mConstructors.find(key));
        }
        
        //! @brief Removes all constructors.
        void removeAllConstructors() {
            std::lock_guard l(mMutex);
            mConstructors.clear();
        }
    };
}

#endif /* Factory_h */
