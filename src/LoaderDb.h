//
//  LoaderDb.h
//  atlre
//
//  Created by jacques tronconi on 09/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef LoaderDb_h
#define LoaderDb_h

#include "Platform.h"
#include "Loader.h"
#include "Error.h"
#include "Singleton.h"

#include <memory>
#include <string>
#include <map>
#include <mutex>

namespace Atl
{
    struct EXPORTED LoaderNotFound : public Error
    { using Error::Error; };
    
    template < typename T >
    class LoaderDb : public Singleton < LoaderDb < T > >
    {
    public:
        typedef T loader_type;
        typedef std::shared_ptr < loader_type > shared_loader_type;
        typedef std::map < std::string, shared_loader_type > shared_loader_map_type;
        
    private:
        shared_loader_map_type mLoaders;
        mutable std::mutex mMutex;
        
    public:
        
        //! @brief Adds a loader for this db.
        void addLoader(const std::string& extension, const shared_loader_type& loader)
        {
            std::lock_guard l(mMutex);
            mLoaders.insert(std::make_pair(extension, loader));
        }
        
        //! @brief Returns a loader available for specified extension.
        shared_loader_type find(const std::string& extension)
        {
            std::lock_guard l(mMutex);
            
            // Tries to find the loader in the map.
            auto it = mLoaders.find(extension);
            
            return it == mLoaders.end() ? nullptr : it->second;
        }
        
        //! @brief Finds a loader with specified name.
        shared_loader_type findByName(const std::string& name)
        {
            std::lock_guard l(mMutex);
            
            auto it = std::find_if(mLoaders.begin(), mLoaders.end(), [name](auto& pair){
                return pair.second->name() == name;
            });
            
            return it == mLoaders.end() ? nullptr : it->second;
        }

        //! @brief Returns true if a Loader handles the extension.
        bool isLoadable(const std::string& ext) const
        {
            std::lock_guard l(mMutex);
            return mLoaders.find(ext) != mLoaders.end();
        }
    };
}

#endif /* LoaderDb_h */
