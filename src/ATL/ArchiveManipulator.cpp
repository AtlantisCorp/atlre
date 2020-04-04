//
//  ArchiveManipulator.cpp
//  atlre
//
//  Created by jacques tronconi on 16/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "ArchiveManipulator.h"
#include "Error.h"
#include <fstream>

namespace Atl
{
    void ArchiveManipulator::extract(const std::string& arname, const std::string& src, const std::string& dest, Archive& ar) const
    {
        std::ofstream stream(dest, std::ios::out);
        extract(arname, src, stream, ar);
    }
    
    void ArchiveManipulator::extract(const std::string& arname, const std::string& src, std::ostream& stream, Archive& ar) const
    {
        std::unique_ptr < std::streambuf > buff(this->stream(arname, src));
        
        if (!buff)
            throw NullError("ArchiveManipulator", "extract", "null streambuf for file %s:%s.", arname.data(), src.data());
        
        std::istream in(buff.get());
        
        std::copy(std::istreambuf_iterator<char>(in),
                  std::istreambuf_iterator<char>(),
                  std::ostream_iterator<char>(stream));
    }
    
    void ArchiveManipulator::save(const std::string& arname, const std::string& src, const std::string& dest, Archive& ar) const
    {
        std::ifstream stream(src, std::ios::in);
        save(arname, stream, dest, ar);
    }
    
    void ArchiveManipulator::save(const std::string& arname, std::istream& stream, const std::string& dest, Archive& ar) const
    {
        if (!exists(arname, dest))
            create(arname, dest);
        
        std::unique_ptr < std::streambuf > buff(this->stream(arname, dest));
        
        if (!buff)
            throw NullError("ArchiveManipulator", "extract", "null streambuf for file %s:%s.", arname.data(), dest.data());
        
        std::ostream out(buff.get());
        
        std::copy(std::istreambuf_iterator<char>(stream),
                  std::istreambuf_iterator<char>(),
                  std::ostream_iterator<char>(out));
    }
}
