//
//  DrawIndexedArraysCommand.h
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#ifndef ATL_DRAWINDEXEDARRAYSCOMMAND_H
#define ATL_DRAWINDEXEDARRAYSCOMMAND_H

#include "RenderCommand.h"
#include "VertexInfos.h"
#include "IndexBufferData.h"

namespace Atl
{
    //! @brief Defines a \ref RenderCommand to draw a data set of Indexes.
    struct EXPORTED DrawIndexedArraysCommand : public RenderCommandBase
    {
        using RenderCommandBase::RenderCommandBase;

        //! @brief Destructor.
        virtual ~DrawIndexedArraysCommand() = default;

        //! @brief Constructs the command.
        virtual void construct(const VertexInfosPtr& infos, const IndexBufferDataPtr& indexes) = 0;
    };

    //! @brief Defines a Pointer to the \ref DrawVertexArraysCommand.
    typedef std::shared_ptr < DrawIndexedArraysCommand > DrawIndexedArraysCommandPtr;

    //! @brief Defines a list of commands.
    typedef std::vector < DrawIndexedArraysCommandPtr > DrawIndexedArraysCommandList;
}

#endif // ATL_DRAWINDEXEDARRAYSCOMMAND_H