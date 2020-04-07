//
//  Error.h
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef Error_h
#define Error_h

#include "Platform.h"

#include <string>
#include <exception>

namespace Atl
{
    //! @brief Defines an Error for all errors thrown by the Atl Engine.
    class EXPORTED Error : public std::exception
    {
        std::string mMessage;
        std::string mClassName;
        std::string mFnName;
        std::string mWhat;
        
    public:

        //! @brief Constructs an empty Error.
        Error() = default;
        
        Error(const char* className, const char* fnName, const char* format, ...);
        const char* what() const noexcept;
        const char* className() const noexcept;
        const char* fnName() const noexcept;
    };
    
    //! @brief An Error for a Memory Allocation Problem.
    struct EXPORTED NotEnoughMemory : public Error
    { using Error::Error; };
    
    struct EXPORTED NullError : public Error
    { using Error::Error; };
    
    struct EXPORTED OutOfRange : public Error
    { using Error::Error; };
}

#endif /* Error_h */
