//
//  Version.h
//  atl
//
//  Created by jacques tronconi on 07/04/2020.
//

#ifndef ATL_VERSION_H
#define ATL_VERSION_H

namespace Atl
{
    //! @brief Very basic version number structure.
    struct Version 
    {
        short major = 0;
        short minor = 0;
        short patch = 0;
    };

    //! @brief Returns true if two versions are equals.
    inline bool operator == (const Version& lhs, const Version& rhs) {
        return lhs.major == rhs.major &&
            lhs.minor == rhs.minor &&
            lhs.patch == rhs.patch;
    }

    //! @brief Returns true if two versions are different.
    inline bool operator != (const Version& lhs, const Version& rhs) {
        return !(lhs == rhs);
    }

    //! @brief Returns true if lhs is inferior to rhs.
    inline bool operator < (const Version& lhs, const Version& rhs) {
        if (lhs.major < rhs.major) 
            return true;

        if (lhs.major == rhs.major) {
            if (lhs.minor < rhs.minor) 
                return true;
            if (lhs.minor == rhs.minor) {
                if (lhs.patch < rhs.patch) 
                    return true;
            }
        }

        return false;
    }

    //! @brief Returns true if lhs is inferior or equal to rhs.
    inline bool operator <= (const Version& lhs, const Version& rhs) {
        if (lhs == rhs) 
            return true;
        return lhs < rhs;
    }

    //! @brief Returns true if lhs is superior to rhs.
    inline bool operator > (const Version& lhs, const Version& rhs) {
        return !(lhs <= rhs);
    }

    //! @brief Returns true if lhs is superior or equal to rhs.
    inline bool operator >= (const Version& lhs, const Version& rhs) {
        return !(lhs < rhs);
    }
}

#endif // ATL_VERSION_H