//
//  DrawVertexArraysCommand.h
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#ifndef ATL_DRAWVERTEXARRAYSCOMMAND_H
#define ATL_DRAWVERTEXARRAYSCOMMAND_H

#include "RenderCommand.h"
#include "VertexInfos.h"

namespace Atl
{
    //! @brief Defines a \ref RenderCommand to draw a data set of Vertexes.
    struct EXPORTED DrawVertexArraysCommand : public RenderCommandBase
    {
        using RenderCommandBase::RenderCommandBase;

        //! @brief Destructor.
        virtual ~DrawVertexArraysCommand() = default;

        //! @brief Constructs the command.
        virtual void construct(const VertexInfosPtr& infos) = 0;
    };

    //! @brief Defines a Pointer to the \ref DrawVertexArraysCommand.
    typedef std::shared_ptr < DrawVertexArraysCommand > DrawVertexArraysCommandPtr;

    //! @brief Defines a list of commands.
    typedef std::vector < DrawVertexArraysCommandPtr > DrawVertexArraysCommandList;
}

#endif // ATL_DRAWVERTEXARRAYSCOMMAND_H