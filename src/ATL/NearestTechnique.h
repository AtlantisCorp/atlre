//
//  NearestTechnique.h
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#ifndef ATL_NEARESTTECHNIQUE_H
#define ATL_NEARESTTECHNIQUE_H

#include "RenderTechnique.h"
#include "Resource.h"

namespace Atl
{
    //! @brief Sorts nodes from the nearest to the farthest.
    class NearestTechnique : public RenderTechnique
    {
    public:
        ATL_SHAREABLE(NearestTechnique)
        
        //! @brief Destructor.
        virtual ~NearestTechnique() = default;
        
    protected:
        //! @brief Sorts nodes nearest first.
        //! Nodes are added in the map upon their distance to the camera.
        virtual std::size_t sort(const RenderNode& node, const Camera& camera, const Frustum& frustum, NodesMap& nodes) const;
    };
}

#endif /* ATL_NEARESTTECHNIQUE_H */
