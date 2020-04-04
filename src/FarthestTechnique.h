//
//  FarthestTechnique.h
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#ifndef ATL_FARTHESTTECHNIQUE_H
#define ATL_FARTHESTTECHNIQUE_H

#include "RenderTechnique.h"

namespace Atl
{
    //! @brief Sorts nodes from the farthest to the nearest.
    class FarthestTechnique : public RenderTechnique
    {
    public:
        
        virtual ~FarthestTechnique() = default;
        
    protected:
        ATL_SHAREABLE(FarthestTechnique)
        
        virtual std::size_t sort(const RenderNode& node, const Camera& camera, const Frustum& frustum, NodesMap& nodes) const;
    };
}

#endif /* ATL_FARTHESTTECHNIQUE_H */
