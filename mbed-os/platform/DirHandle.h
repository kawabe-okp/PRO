/* mbed Microcontroller Library
 * Copyright (c) 2006-2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_DIRHANDLE_H
#define MBED_DIRHANDLE_H

#include <stdint.h>
#include "platform/mbed_toolchain.h"
#include "platform/mbed_retarget.h"
#include "platform/NonCopyable.h"

namespace mbed {
/** \addtogroup platform-public-api */
/** @{*/
/**
 * \defgroup platform_DirHandle DirHandle functions
 * @{
 */

class DirHandle : private NonCopyable<DirHandle> {
public:
    virtual ~DirHandle() {}

    /** New API - delegates to old API by default for Mbed 2 compat */
    virtual ssize_t read(struct dirent *ent)
    {
        struct dirent *result = readdir();
        if (result) { *ent = *result; return 1; }
        return 0;
    }

    virtual int close()
    {
        return closedir();
    }

    virtual void seek(off_t offset)
    {
        seekdir(offset);
    }

    virtual off_t tell()
    {
        return telldir();
    }

    virtual void rewind()
    {
        rewinddir();
    }

    virtual size_t size()
    {
        off_t off = tell();
        size_t size = 0;
        struct dirent *ent = new struct dirent;

        rewind();
        while (read(ent) > 0) {
            size += 1;
        }
        seek(off);

        delete ent;
        return size;
    }

    /** Old API - standalone defaults for Mbed 2 compat (no circular delegation) */
    virtual int closedir()
    {
        return -1;
    }

    virtual struct dirent *readdir()
    {
        return NULL;
    }

    virtual void rewinddir()
    {
    }

    virtual off_t telldir()
    {
        return -1;
    }

    virtual void seekdir(off_t location)
    {
        (void)location;
    }
};

/**@}*/

/**@}*/
} // namespace mbed

#endif /* MBED_DIRHANDLE_H */
