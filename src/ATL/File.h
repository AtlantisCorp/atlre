//
//  File.h
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef File_h
#define File_h

#include "Platform.h"
#include <string>

namespace Atl
{
    //! @brief Defines some utilities functions for a File.
    class EXPORTED File
    {
    public:
        
        //! @brief Returns the extension of the given filename.
        static std::string Extension( const std::string& filename );
    };
}

#endif /* File_h */
