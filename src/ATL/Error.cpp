//
//  Error.cpp
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "Error.h"
#include <cstdarg>

namespace Atl
{
    Error::Error(const char* className, const char* fnName, const char* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        char buffer[2048] = { 0 };
        vsnprintf(buffer, 2048, format, ap);
        va_end(ap);
        mMessage = std::string(buffer);
        mClassName = std::string(className);
        mFnName = std::string(fnName);
        mWhat = std::string("[") + mClassName + "](" + mFnName + ") " + mMessage;
    }
    
    const char* Error::what() const noexcept
    {
        return mWhat.data();
    }
    
    const char* Error::className() const noexcept
    {
        return mClassName.data();
    }
    
    const char* Error::fnName() const noexcept
    {
        return mFnName.data();
    }
}
