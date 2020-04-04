//
//  TransparentTechnique.h
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#ifndef ATL_TRANSPARENTTECHNIQUE_H
#define ATL_TRANSPARENTTECHNIQUE_H

#include "FarthestTechnique.h"

namespace Atl
{
    class TransparentTechnique : public FarthestTechnique
    {
    public:
        
        virtual ~TransparentTechnique() = default;
        
    protected:
        ATL_SHAREABLE(TransparentTechnique)
        
        virtual std::size_t sort(const RenderNode& node, const Camera& camera, const Frustum& frustum, NodesMap& nodes) const;
    };
}

#endif /* ATL_TRANSPARENTTECHNIQUE_H */
