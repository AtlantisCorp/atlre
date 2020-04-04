//
//  ArchiveManipulator.h
//  atlre
//
//  Created by jacques tronconi on 16/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ArchiveManipulator_h
#define ArchiveManipulator_h

#include <string>
#include <iostream>
#include <functional>

#include "Platform.h"

namespace Atl
{
    class Archive;
    
    //! @brief Class to manipulate an Archive.
    //! When implementing a new Archive extension, derive from this class and overwrite the save(), etxract()
    //! and forEachFile() methods. Your loader should then set the ArchiveManipulator of the specified Archive
    //! when loading it, asserting the Archive file already exists.
    //!
    //! @warning This class should be thread-safe ! The Archive class actually uses std::async to extract/save
    //! files in the Archive file, and so the ArchiveManipulator functions may be used from multiple threads.
    class EXPORTED ArchiveManipulator
    {
    public:
        
        //! @brief Default destructor.
        virtual ~ArchiveManipulator() = default;
        
        //! @brief Extracts a file from an archive.
        //! @param arname The name of the archive.
        //! @param src The file to extract, as a relative path from the root
        //! of the specified archive.
        //! @param dest The file to write to, as a relative or an absolute path on disk.
        //! @param ar The Archive that owns this manipulator.
        virtual void extract(const std::string& arname,
                             const std::string& src,
                             const std::string& dest,
                             Archive& ar) const;
        
        //! @brief Extracts a file from an archive.
        //! @param arname The name of the archive.
        //! @param src The file to extract, as a relative path from the root
        //! of the specified archive.
        //! @param stream A stream where to write the content of the file.
        //! @param ar The Archive that owns this manipulator.
        virtual void extract(const std::string& arname,
                             const std::string& src,
                             std::ostream& stream,
                             Archive& ar) const;
        
        virtual void save(const std::string& arname,
                          const std::string& src,
                          const std::string& dest,
                          Archive& ar) const;
        
        virtual void save(const std::string& arname,
                          std::istream& stream,
                          const std::string& dest,
                          Archive& ar) const;
        
        virtual void forEachFile(const std::string& arname, std::function < void(const ArchiveManipulator&, const std::string&) > callback) const = 0;
        
        virtual std::streambuf* stream(const std::string& arname, const std::string& filename) const = 0;
        
        virtual bool exists(const std::string& arname, const std::string& filename) const = 0;
        
        virtual void create(const std::string& arname, const std::string& filename) const = 0;
    };
    
    typedef std::shared_ptr < ArchiveManipulator > ArchiveManipulatorPtr;
}

#endif /* ArchiveManipulator_h */
