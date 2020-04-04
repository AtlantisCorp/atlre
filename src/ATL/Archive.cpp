//
//  Archive.cpp
//  atlre
//
//  Created by jacques tronconi on 17/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "Archive.h"

namespace Atl
{
    Archive::Archive(Archive::Manager& manager, const std::string& name)
    : TResource(manager, name)
    {
        
    }
    
    void Archive::setFilename(const std::string &filename)
    {
        std::lock_guard l(mMutex);
        mFilename = filename;
    }
    
    std::string Archive::filename() const
    {
        std::lock_guard l(mMutex);
        return mFilename;
    }
    
    void Archive::setManipulator(const ArchiveManipulatorPtr& manip)
    {
        std::atomic_store(&mManipulator, manip);
    }
    
    ArchiveManipulatorPtr Archive::manipulator() const
    {
        return std::atomic_load(&mManipulator);
    }
    
    std::future < void > Archive::open(const std::string& filename, std::iostream& stream)
    {
        return std::async(std::launch::async, [this, &filename, &stream](){
            auto manip = manipulator();
            
            if (!manip)
                throw NullError("Archive", "open", "Null ArchiveManipulator.");
            
            manip->extract(mFilename, filename, stream, *this);
        });
    }
    
    std::future < std::streambuf* > Archive::stream(const std::string& filename)
    {
        return std::async(std::launch::async, [this, &filename](){
            auto manip = manipulator();
            
            if (!manip)
                throw NullError("Archive", "stream", "Null ArchiveManipulator.");
            
            return manip->stream(mFilename, filename);
        });
    }
    
    std::future < void > Archive::forEachFile(std::function < void(const ArchiveManipulator&, const std::string&) > callback)
    {
        return std::async(std::launch::async, [this, &callback](){
            auto manip = manipulator();
            
            if (!manip)
                throw NullError("Archive", "forEachFile", "Null ArchiveManipulator.");
            
            return manip->forEachFile(mFilename, callback);
        });
    }
}
