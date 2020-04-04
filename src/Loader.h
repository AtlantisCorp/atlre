//
//  Loader.h
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef Loader_h
#define Loader_h

#include "Platform.h"
#include "Params.h"

#include <cstdint>
#include <string>

namespace Atl
{
    //! @brief Defines a common base for all loaders.
    class EXPORTED BaseLoader
    {
    public:
        //! @brief Default destructor.
        virtual ~BaseLoader() = default;
        
        //! @brief Returns the loader's name.
        virtual std::string name() const = 0;
        
        //! @brief Returns the needed size to load specified filename.
        virtual std::size_t neededSize(const std::string& filename, const Params& params) const = 0;
        
        //! Ideally your load function will look like this :
        //! virtual void load(T& resource, ...) const = 0;
        //! All your derived loaders will follow this signature to load the resource.
    };
    
    //! @brief Specialize this class to have a loader for your resource.
    template < class T >
    class LoaderTrait : public BaseLoader
    { };
}

#endif /* Loader_h */
