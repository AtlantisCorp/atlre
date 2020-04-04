//
//  ResourceListener.h
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ResourceListener_h
#define ResourceListener_h

#include "Platform.h"
#include "Emitter.h"

namespace Atl
{
    class Resource;
    //! @brief Interface for a basic Resource's listener.
    class EXPORTED ResourceListener : virtual public ListenerBase
    {
    public:
        virtual ~ResourceListener() = default;
        virtual void onResourceWillLoad(Resource&) {}
        virtual void onResourceWillCache(Resource&) {}
        virtual void onResourceWillUncache(Resource&) {}
        virtual void onResourceWillUnload(Resource&) {}
        virtual void onResourceDidLoad(Resource&) {}
        virtual void onResourceDidUnload(Resource&) {}
        virtual void onResourceDidCache(Resource&) {}
        virtual void onResourceDidUncache(Resource&) {}
    };
}

#endif /* ResourceListener_h */
