//
//  MaterialCache.h
//  atl
//
//  Created by jacques tronconi on 01/04/2020.
//

#ifndef ATL_MATERIALCACHE_H
#define ATL_MATERIALCACHE_H

#include "RenderCache.h"
#include "ShaderVariableCommand.h"

namespace Atl
{
    class Material; 

    //! @brief A RenderCache for Material.
    //! This cache creates a list of ShaderVariableCommands. The list has a reserved size of the maximum 
    //! number of elements the Material can hold. However, only the elements used in the Material are used
    //! and are filled with a non null ShaderVariableCommand.
    class MaterialCache : public RenderCache < Material >
    {
        //! @brief List of RenderCommands.
        ShaderVariableCommandList mCommands;

        //! @brief The cache mutex.
        mutable std::mutex mMutex;

    public:
        ATL_SHAREABLE(MaterialCache)

        //! @brief The listener class.
        typedef RenderableListener Listener;

        //! @brief Constructs a new cache.
        //! @param rhs The renderer related to this cache.
        //! @param material The material related to this cache.
        MaterialCache(Renderer& rhs, Material& material);

        //! @brief Builds the cache.
        std::future < void > build(Renderer& rhs);

        //! @brief Renders the cache.
        std::future < void > render(RenderCommand& cmd) const;

        //! @brief Returns the sum of all values.
        std::size_t size(Renderer&) const;
    };
}

#endif // ATL_MATERIALCACHE_H