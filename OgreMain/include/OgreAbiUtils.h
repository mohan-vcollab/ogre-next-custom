/*
-----------------------------------------------------------------------------
This source file is part of OGRE-Next
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-present Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef Ogre_AbiUtils_H_
#define Ogre_AbiUtils_H_

#include "OgrePrerequisites.h"

#include "Hash/MurmurHash3.h"
#include "OgreFastArray.h"
#include "OgreIdString.h"

#if OGRE_ARCH_TYPE == OGRE_ARCHITECTURE_32
#    define OGRE_HASH128_FUNC MurmurHash3_x86_128
#else
#    define OGRE_HASH128_FUNC MurmurHash3_x64_128
#endif

namespace Ogre
{
    struct AbiCookie
    {
        uint64 val[2];
    };

    /**
    @brief generateAbiCookie
        Generates a cookie containing all (or most) relevant settings that
        determine ABI compatibility between a library and an application
    @return
        AbiCookie.
        See testAbiCookie
    */
    FORCEINLINE AbiCookie generateAbiCookie()
    {
        AbiCookie abiCookie;

        FastArray<uint32> data;
        data.push_back( OGRE_DEBUG_MODE );
        uint32 setting = 0;
#ifdef OGRE_STATIC_LIB
        data.push_back( setting );
#endif
        ++setting;
#ifdef OGRE_GCC_VISIBILITY
        data.push_back( setting );
#endif
        ++setting;
        data.push_back( sizeof( IdString ) );
        data.push_back( OGRE_FLEXIBILITY_LEVEL );
        data.push_back( OGRE_SIMD_ALIGNMENT );
        data.push_back( OGRE_USE_SIMD );
        data.push_back( OGRE_RESTRICT_ALIASING );
        data.push_back( OGRE_ASSERT_MODE );
        data.push_back( OGRE_DOUBLE_PRECISION );
        data.push_back( OGRE_MEMORY_ALLOCATOR );
        data.push_back( OGRE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR );
        data.push_back( OGRE_STRING_USE_CUSTOM_MEMORY_ALLOCATOR );
        data.push_back( OGRE_THREAD_SUPPORT );
        data.push_back( OGRE_THREAD_PROVIDER );
        data.push_back( OGRE_HASH_BITS );
        data.push_back( OGRE_VERSION );

        OGRE_HASH128_FUNC( data.begin(), static_cast<int>( data.size() * sizeof( uint32 ) ),
                           IdString::Seed, &abiCookie );

        return abiCookie;
    }

    /**
    @brief testAbiCookie
        Tests the given clientCookie (generated by generateAbiCookie)
        against a cookie generated in the library.

        They must match. If they don't, then:

            - The libraries are outdated or too new
            - Wrong Ogre version (e.g. 2.3 vs 2.4)
            - The app was compiled against the wrong headers
            - The app was compiled against the wrong OgreBuildSettings.h
            - Some stale compiler cache. Rebuild everything from scratch.
            - Compiler settings mismatch

    @param appCookie
        Cookie generated by generateAbiCookie
    @param bAbortOnError
        When true, we won't return false on error. We will call abort()
    @return
        True on success.
        False if there is an ABI mismatch and we should not continue.
    */
    bool _OgreExport testAbiCookie( const AbiCookie appCookie, bool bAbortOnError = true );
}  // namespace Ogre

#undef OGRE_HASH128_FUNC

#endif
