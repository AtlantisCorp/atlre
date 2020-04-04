//
//  File.cpp
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "File.h"

namespace Atl
{
    std::string File::Extension( const std::string& filename )
    {
        return filename.substr( filename.find_last_of( '.' ), std::string::npos );
    }
}
