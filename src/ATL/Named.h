//
//  Named.h
//  atlre
//
//  Created by jacques tronconi on 07/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ATL_NAMED_H
#define ATL_NAMED_H

#include "Platform.h"

namespace Atl
{
    //! @brief Interface for a non resource class that needs a name.
    class Named 
    {
        //! @brief The fabulous name.
        std::string mName;

        //! @brief The mutex.
        mutable std::mutex mMutex;

    public:

        //! @brief Constructs this interface with a name (or not).
        Named(const std::string& name = std::string()): mName(name) {}

        //! @brief Returns \ref mName.
        inline std::string name() const { 
            std::lock_guard l(mMutex);
            return mName; 
        }

        //! @brief Sets \ref mName.
        inline void setName(const std::string& rhs) { 
            std::lock_guard l(mMutex);
            mName = rhs;
        }
    };
}

#endif // ATL_NAMED_H