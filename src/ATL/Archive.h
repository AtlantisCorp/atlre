//
//  Archive.h
//  atlre
//
//  Created by jacques tronconi on 16/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef Archive_h
#define Archive_h

#include "Resource.h"
#include "Emitter.h"
#include "ArchiveManipulator.h"

namespace Atl
{
    class Archive;
    class ArchiveManager;
    class ArchiveListener;
    class ArchiveManagerListener;
    ATL_BASE_RESOURCE(Archive)
    
    template < >
    struct LoaderTrait < Archive > : public BaseLoader
    {
        virtual ~LoaderTrait() = default;
        virtual void load(Archive&, const std::string& filename, const Params& params) = 0;
    };
    
    class EXPORTED ArchiveLoaderDb : public LoaderDb < Archive >{};
    
    class EXPORTED ArchiveListener : public ResourceListener
    {
    public:
        virtual ~ArchiveListener() = default;
        
        virtual void onArchiveDidOpenFile(Archive&, const std::string& filename){}
    };
    
    struct EXPORTED ArchiveManagerListener : public ManagerListener <
    ArchiveManager,
    Archive,
    ArchiveManagerListener >{};
    
    struct EXPORTED ArchiveManager : public Manager <
    ArchiveManager,
    Archive,
    ArchiveManagerListener >{};
    
    class EXPORTED Archive : public Atl::TResource < Archive, ArchiveManager >
    {
        //! @brief The Archive's filename.
        std::string mFilename;
        
        //! @brief The Archive's manipulator.
        ArchiveManipulatorPtr mManipulator;
        
        mutable std::mutex mMutex;
        
    public:
        typedef ArchiveListener Listener;
        
        Archive(Manager& manager, const std::string& name);
        
        void setFilename(const std::string& filename);
        
        std::string filename() const;
        
        void setManipulator(const ArchiveManipulatorPtr& manip);
        
        ArchiveManipulatorPtr manipulator() const;
        
        std::future < void > open(const std::string& filename, std::iostream& stream);
        
        std::future < std::streambuf* > stream(const std::string& filename);
        
        std::future < void > forEachFile(std::function < void(const ArchiveManipulator&, const std::string&) > callback);
        
        //! @brief Returns the size used for this resource.
        inline std::size_t usedSize() const { return 0; }
    };
}

#endif /* Archive_h */
