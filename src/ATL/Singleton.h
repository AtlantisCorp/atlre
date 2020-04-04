//
//  Singleton.h
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef Singleton_h
#define Singleton_h

#include <memory>

namespace Atl
{
    //! @brief Defines a Singleton where the global instance is stored into a
    //! shared_ptr, allowing us to use shared_from_this() as the Singleton may
    //! be a listener.
    template < typename T >
    class Singleton : public std::enable_shared_from_this < T >
    {
    public:
        
        //! @brief Returns the instance for the Singleton.
        static T& Get()
        {
            static std::shared_ptr < T > mInstance = std::make_shared < T >();
            return *std::atomic_load(&mInstance);
        }
    };
}

#endif /* Singleton_h */
