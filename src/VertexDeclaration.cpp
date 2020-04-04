//
//  VertexDeclaration.cpp
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#include "VertexDeclaration.h"
#include "Error.h"

namespace Atl
{
    const VertexElement& VertexDeclaration::addElement(unsigned short source, std::size_t offset, VertexElementType type, const std::string &meaning)
    {
        mElements.push_back(VertexElement(source, offset, type, meaning));
        return *mElements.rbegin();
    }
    
    const VertexElement& VertexDeclaration::insertElement(unsigned index, unsigned short source, std::size_t offset, VertexElementType type, const std::string &meaning)
    {
        if (index > mElements.size())
            throw OutOfRange("VertexDeclaration", "insertElement", "index %i is not a valid position.", index);
        
        auto it = mElements.begin();
        std::advance(it, index);
        
        mElements.insert(it, VertexElement(source, offset, type, meaning));
        
        it = mElements.begin();
        std::advance(it, index);
        return *it;
    }
    
    const VertexElement& VertexDeclaration::setElement(unsigned index, unsigned short source, std::size_t offset, VertexElementType type, const std::string &meaning)
    {
        if (index >= mElements.size())
            throw OutOfRange("VertexDeclaration", "setElement", "index %i is not a valid position.", index);
        
        auto it = mElements.begin();
        std::advance(it, index);
        
        VertexElement& element = *it;
        element = VertexElement(source, offset, type, meaning);
        
        return element;
    }
    
    void VertexDeclaration::removeElement(unsigned index)
    {
        if (index >= mElements.size())
            throw OutOfRange("VertexDeclaration", "removeElement", "index %i is not a valid position.", index);
        
        auto it = mElements.begin();
        std::advance(it, index);
        
        mElements.erase(it);
    }
    
    void VertexDeclaration::removeElement(const std::string &meaning)
    {
        auto it = std::find_if(mElements.begin(), mElements.end(), [&meaning](auto const& el){
            return el.meaning() == meaning;
        });
        
        if (it != mElements.end())
            mElements.erase(it);
    }
    
    void VertexDeclaration::removeAllElements()
    {
        mElements.clear();
    }
    
    const VertexElement& VertexDeclaration::findElement(unsigned index) const
    {
        if (index >= mElements.size())
            throw OutOfRange("VertexDeclaration", "findElement", "index %i is not a valid position.", index);
        
        auto it = mElements.begin();
        std::advance(it, index);
        
        return *it;
    }
    
    const VertexElement& VertexDeclaration::findElement(const std::string &meaning) const
    {
        auto it = std::find_if(mElements.begin(), mElements.end(), [&meaning](auto const& el){
            return el.meaning() == meaning;
        });
        
        if (it == mElements.end())
            throw OutOfRange("VertexDeclaration", "findElement", "meaning %s not found in declaration.", meaning.data());
        
        return *it;
    }
    
    VertexElementList VertexDeclaration::findElementsForSource(unsigned short source) const
    {
        VertexElementList els;
        
        std::for_each(mElements.begin(), mElements.end(), [&els, &source](auto const& el){
            if (el.source() == source)
                els.push_back(el);
        });
        
        return els;
    }
    
    std::size_t VertexDeclaration::vertexSize() const
    {
        std::size_t size = 0;
        
        std::for_each(mElements.begin(), mElements.end(), [&size](VertexElement const& el){
            size += el.size();
        });
        
        return size;
    }
    
    std::size_t VertexDeclaration::vertexSizeForSource(unsigned short source) const
    {
        std::size_t size = 0;
        
        std::for_each(mElements.begin(), mElements.end(), [&size, &source](VertexElement const& el){
            if (el.source() == source)
                size += el.size();
        });
        
        return size;
    }
    
    std::size_t VertexDeclaration::elementsCount() const
    {
        return mElements.size();
    }
}
