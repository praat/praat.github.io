/* ggml-memory-pool.cpp
*
 * Copyright (C) 2026 Anastasia Shchupak
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ggml-memory-pool.h"
#include "ggml.h"
#include "melder.h"

GgmlMemoryPool *theGgmlMemoryPool = nullptr;

void GgmlMemoryPool :: add(void *ptr, size_t size, bool aligned) {
    TRACE
    if (! ptr) {
        trace (U"Trying to add nullptr to allocations)");
        return;
    }
    if (allocations.count (ptr))
        trace (U"Something is very wrong: pointer already in allocations (possible double allocation or missing remove)");

    Allocation allocation = {ptr, size, aligned};
    trace (U"Added to allocations: ", allocation.size, U" bytes at ", Melder_pointer (allocation.ptr), allocation.aligned ? U", aligned" : U"");
    allocations [ptr] = allocation;
}

void GgmlMemoryPool :: remove(void *ptr) {
    TRACE
    if (! ptr) {
        trace (U"Trying to remove nullptr from allocations)");
        return;
    }
    Allocation allocation = allocations [ptr];
    trace (U"Removed from allocations: ", allocation.size, U" bytes at ", Melder_pointer (allocation.ptr), allocation.aligned ? U", aligned" : U"");
    allocations .erase (ptr);
}

void GgmlMemoryPool :: remove(void *ptr, size_t size) {
    TRACE
    if (! ptr) {
        trace (U"Trying to remove nullptr from allocations)");
        return;
    }
    Allocation allocation = allocations [ptr];
    Melder_assert (size == allocation.size);
    trace (U"Removed from allocations: ", allocation.size, U" bytes at ", Melder_pointer (allocation.ptr), allocation.aligned ? U", aligned" : U"");
    allocations .erase (ptr);
}

void GgmlMemoryPool :: clear() {
    theGgmlMemoryPool = nullptr;   // to prevent ggml_aligned_free() and ggml_raw_free() from intervening

    TRACE
    trace (U"Clearing allocations...");
    if (allocations.empty()) {
        trace (U"Allocations were empty");
        return;
    }

    for (auto & allocation_pair : allocations) {
        if (allocation_pair.first == nullptr)
            continue;
        trace (U"Emergency freeing: ", allocation_pair.second.size, U" bytes at ", Melder_pointer (allocation_pair.first),
                allocation_pair.second.aligned ? U", aligned" : U"");
        if (allocation_pair.second.aligned)
            ggml_aligned_free (allocation_pair.first, allocation_pair.second.size);
        else
            ggml_raw_free (allocation_pair.first);
    }

    trace (U"Allocations cleared");
    allocations.clear();
}
