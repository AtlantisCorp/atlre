//
//  Platform.h
//  atlre
//
//  Created by jacques tronconi on 17/03/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef Platform_h
#define Platform_h

#include "Configure.h"

#include <variant>
#include <future>
#include <functional>

// ------------------------------------------------------------------------------------
// Platform Macros

#define ATL_PLATFORM_DARWIN
#define ATL_PLATFORM_WINDOWS
#define ATL_PLATFORM_UNIX
#define ATL_PLATFORM_PS3
#define ATL_PLATFORM_PS4
#define ATL_PLATFORM_ANDROID
#define ATL_PLATFORM_IOS
#define ATL_PLATFORM_XBOX360
#define ATL_PLATFORM_XBOX_ONE

// ------------------------------------------------------------------------------------
// Architecture Macros

#define ATL_ARCH_X86
#define ATL_ARCH_X64
#define ATL_ARCH_ARM
#define ATL_ARCH_ARM64

// ------------------------------------------------------------------------------------
// Compiler Macros

#define ATL_COMPILER_MSVC 1
#define ATL_COMPILER_GCC 2
#define ATL_COMPILER_CLANG 3

#if defined(__GNUC__)
#   define ATL_COMPILER ATL_COMPILER_GCC
#   define ATL_COMPILER_IS_GCC

#elif defined(_MSC_VER)
#   define ATL_COMPILER ATL_COMPILER_MSVC
#   define ATL_COMPILER_IS_MSVC

#elif defined(__clang__)
#   define ATL_COMPILER ATL_COMPILER_CLANG
#   define ATL_COMPILER_IS_CLANG

#endif

// ------------------------------------------------------------------------------------
// Per Architecture Macros

#if defined(ATL_COMPILER_IS_MSVC)
#   if defined(_M_IX86)
#       define ATL_ARCH ATL_ARCH_X86
#       define ATL_ARCH_IS_X86
#
#   elif defined(_M_X64) || defined(_M_AMD_64)
#       define ATL_ARCH ATL_ARCH_X64
#       define ATL_ARCH_IS_X64
#
#   elif defined(_M_ARM)
#       define ATL_ARCH ATL_ARCH_ARM
#       define ATL_ARCH_IS_ARM
#
#   elif defined(_M_ARM64)
#       define ATL_ARCH ATL_ARCH_ARM64
#       define ATL_ARCH_IS_ARM64
#
#   endif
#
#elif defined(ATL_COMPILER_IS_GCC) || defined(ATL_COMPILER_IS_CLANG)
#   if defined(i386) || defined(__i386) || defined(__i386__)
#       define ATL_ARCH ATL_ARCH_X86
#       define ATL_ARCH_IS_X86
#
#   elif defined(__amd64) || defined(__amd64__) || defined(__x86_64__) || defined(__x86_64)
#       define ATL_ARCH ATL_ARCH_X64
#       define ATL_ARCH_IS_X64
#
#   elif defined(__arm__) || defined(__thumb__)
#       define ATL_ARCH ATL_ARCH_ARM
#       define ATL_ARCH_IS_ARM
#
#   elif defined(__aarch64__)
#       define ATL_ARCH ATL_ARCH_ARM64
#       define ATL_ARCH_IS_ARM64
#
#   endif
#endif

// ------------------------------------------------------------------------------------
// Per Platforms Macros

#if defined(__DARWIN__) || defined(__APPLE__)

#   define ATL_PLATFORM ATL_PLATFORM_DARWIN
#   define ATL_PLATFORM_IS_DARWIN

#   include <dlfcn.h>
#   define ATL_DYNLIB_EXTENSION ".dylib"
#   define ATL_DYNLIB_OPEN(a) dlopen(a, RTLD_LAZY)
#   define ATL_DYNLIB_DLSYM(a, b) dlsym(a, b)
#   define ATL_DYNLIB_CLOSE(a) dlclose(a)
#   define ATL_DYNLIB_HANDLE void*

#endif

// ------------------------------------------------------------------------------------
// Export symbols

// Define EXPORTED for any platform
#if defined _WIN32 || defined __CYGWIN__
#   ifdef WIN_EXPORT
        // Exporting...
#       ifdef __GNUC__
#           define EXPORTED __attribute__ ((dllexport))
#       else
#           define EXPORTED __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#       endif
#   else
#       ifdef __GNUC__
#           define EXPORTED __attribute__ ((dllimport))
#       else
#           define EXPORTED __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#       endif
#   endif
#   define NOT_EXPORTED
#else
#   if __GNUC__ >= 4
#       define EXPORTED __attribute__ ((visibility ("default")))
#       define NOT_EXPORTED  __attribute__ ((visibility ("hidden")))
#   else
#       define EXPORTED
#       define NOT_EXPORTED
#   endif
#endif

// ------------------------------------------------------------------------------------
// Real definition

// #define ATL_REAL_AS_DOUBLE

#ifndef ATL_REAL_AS_DOUBLE
#   define Real float
#else
#   define Real double
#endif

// ------------------------------------------------------------------------------------
// GLM Headers and typedefs (for Real).

// NOTES: We cannot include system headers because this would require a system available
// GLM, which may not be the case on, for example, Windows platforms (or even macOS). So 
// we directly includes the library from our libs dir (from here, '../../libs/glm/' as prefix).

#include "../../libs/glm/glm/glm.hpp"
#include "../../libs/glm/glm/gtc/quaternion.hpp"
#include "../../libs/glm/glm/gtx/quaternion.hpp"
#include "../../libs/glm/glm/gtx/transform.hpp"
#include "../../libs/glm/glm/gtx/matrix_decompose.hpp"

namespace Atl
{
    typedef glm::tvec1 < Real > rvec1;
    typedef glm::tvec2 < Real > rvec2;
    typedef glm::tvec3 < Real > rvec3;
    typedef glm::tvec4 < Real > rvec4;
    typedef glm::tmat2x2 < Real > rmat2x2;
    typedef glm::tmat2x3 < Real > rmat2x3;
    typedef glm::tmat2x4 < Real > rmat2x4;
    typedef glm::tmat3x2 < Real > rmat3x2;
    typedef glm::tmat3x3 < Real > rmat3x3;
    typedef glm::tmat3x4 < Real > rmat3x4;
    typedef glm::tmat4x2 < Real > rmat4x2;
    typedef glm::tmat4x3 < Real > rmat4x3;
    typedef glm::tmat4x4 < Real > rmat4x4;
    typedef glm::tquat < Real > rquat;
}

// ------------------------------------------------------------------------------------
// std::filesystem includes.

#include ATL_FILESYSTEM_INCLUDE

namespace Atl
{
    namespace fs = ATL_FILESYSTEM_NAMESPACE;
}

// ------------------------------------------------------------------------------------
// Misc functions

//! @brief Defines a generic New() function for a class to allocate a std::shared_ptr
//! of this class.
#define ATL_SHAREABLE(T) template < typename... Args > \
static std::shared_ptr < T > New(Args&&... args) { \
    return std::make_shared < T >(std::forward < Args >(args)...); \
}

//! @brief Defines a Generic Handle.
//! This handle can be used for any platform specific handles.
typedef void* GenHandle;

#endif /* Platform_h */
