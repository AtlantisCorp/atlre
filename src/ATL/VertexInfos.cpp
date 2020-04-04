//
//  VertexInfos.cpp
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#include "VertexInfos.h"
#include "Error.h"

namespace Atl
{
    VertexInfos::VertexInfos()
    {
        mDeclaration = std::make_shared < VertexDeclaration >();
        mBinding = std::make_shared < VertexBufferBinding >();
        mBaseVertex = 0;
        mCountVertexes = 0;
    }
    
    VertexInfos::VertexInfos(const VertexDeclarationPtr& declaration, const VertexBufferBindingPtr& binding, std::size_t base, std::size_t count)
    : mDeclaration(declaration), mBinding(binding), mBaseVertex(base), mCountVertexes(count)
    {
        
    }
    
    VertexDeclarationPtr VertexInfos::declaration() const
    {
        return std::atomic_load(&mDeclaration);
    }
    
    VertexBufferBindingPtr VertexInfos::binding() const
    {
        return std::atomic_load(&mBinding);
    }
    
    std::size_t VertexInfos::baseVertex() const
    {
        return mBaseVertex.load();
    }
    
    void VertexInfos::setBaseVertex(std::size_t base)
    {
        mBaseVertex.store(base);
    }
    
    std::size_t VertexInfos::vertexesCount() const
    {
        return mCountVertexes.load();
    }
    
    void VertexInfos::setVertexesCount(std::size_t count)
    {
        mCountVertexes.store(count);
    }
    
    std::size_t VertexInfos::baseOffset(unsigned short source) const
    {
        VertexDeclarationPtr decla = declaration();
        if (!decla) throw NullError("VertexInfos", "baseOffset", "No declaration in vertex data set.");
        
        return baseVertex() * decla->vertexSizeForSource(source);
    }
    
    const VertexElement& VertexInfos::addElement(unsigned short source, std::size_t offset, VertexElementType type, const std::string& meaning)
    {
        VertexDeclarationPtr decla = declaration();
        if (!decla) throw NullError("VertexInfos", "addElement", "No declaration in vertex data set.");
        return decla->addElement(source, offset, type, meaning);
    }

    void VertexInfos::setDeclaration(const VertexDeclarationPtr& declaration)
    {
        std::atomic_store(&mDeclaration, declaration);
    }
}
