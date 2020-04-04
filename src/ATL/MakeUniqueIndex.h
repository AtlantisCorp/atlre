//
//  MakeUniqueIndex.h
//  atl
//
//  Created by jacques tronconi on 30/03/2020.
//

#ifndef ATL_MAKEUNIQUEINDEX_H
#define ATL_MAKEUNIQUEINDEX_H

#include <cstdint>
#include <atomic>

namespace Atl
{
    //! @brief A static index counter.
    template < typename T >
    class MakeUniqueIndex
    {
        //! @brief The index.
        std::atomic < std::uint64_t > mIndex;

    public:

        //! @brief Creates a new Index.
        MakeUniqueIndex() {
            static std::atomic < std::uint64_t > nextIndex(1);
            mIndex.store(nextIndex.fetch_add(1));
        }

        //! @brief Returns the index.
        std::uint64_t index() const {
            return mIndex.load();
        }

        //! @brief A Helper to retrieve the index type (always std::uint64_t).
        typedef std::uint64_t Index;
    };
}

#endif // ATL_MAKEUNIQUEINDEX_H