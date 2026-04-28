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

#include "melder.h"   // include early, so that size_t is defined
#include "ggml-memory-pool.h"
#include "ggml-impl.h"
#include "SpeechRecognizer.h"
#include "whisper.h"

GgmlMemoryPool theGgmlMemoryPool;

void GgmlMemoryPool :: add(void *ptr, size_t size, bool aligned) {
	//TRACE
	if (! ptr) {
		trace (U"Trying to add nullptr to allocations)");
		return;
	}
	if (allocations.count (ptr))
		trace (U"Something is very wrong: pointer already in allocations (possible double allocation or missing remove)");

	allocations [ptr] = { size, aligned };
}

bool GgmlMemoryPool :: remove(void *ptr) {
	//TRACE
	if (! ptr) {
		trace (U"Trying to remove nullptr from allocations)");
		return false;
	}
	auto it = allocations.find (ptr);
	if (it == allocations.end ())
		return false;
	allocations .erase (it);
	return true;
}

bool GgmlMemoryPool :: remove(void *ptr, size_t size) {
	//TRACE
	if (! ptr) {
		trace (U"Trying to remove nullptr from allocations)");
		return false;
	}

	auto it = allocations.find (ptr);
	if (it == allocations.end ())
		return false;
	Melder_assert (size == it->second.size);   // otherwise something weird: size does not match
	allocations .erase (it);
	return true;
}

void GgmlMemoryPool :: clear() {
	//TRACE
	trace (U"--- WE ARE IN THE EMERGENCY CLEAN UP ---");
	trace (U"Number of allocations in the memory pool is  ", n_allocations());
	trace (U"Total memory in bytes is  ", sizeInBytes());
	trace (U"Destroying all living SpeechRecognizers...");
	for (auto & speechRecognizer : theLivingSpeechRecognizers) {
		if (speechRecognizer -> whisperContext.ptr) {
			whisper_free (speechRecognizer -> whisperContext.ptr);
			speechRecognizer -> whisperContext.ptr = nullptr;
		}
		trace (U"speechRecognizer destroyed");
	}

	trace (U"Number of allocations in the memory pool is  ", n_allocations());
	trace (U"Total memory in bytes is  ", sizeInBytes());
	trace (U"Clearing allocations...");
	for (auto & allocation_pair : allocations) {
		if (! allocation_pair.first)
			continue;
		if (allocation_pair.second.aligned)
			ggml_aligned_free (allocation_pair.first, allocation_pair.second.size, false);
		else
			ggml_raw_free (allocation_pair.first, false);
	}
	allocations.clear();
	trace (U"Allocations cleared");
	trace (U"Number of allocations in the memory pool is  ", n_allocations());
	trace (U"Total memory in bytes is  ", sizeInBytes());
}

size_t GgmlMemoryPool :: n_allocations() const {
	return allocations.size();
}

size_t GgmlMemoryPool :: sizeInBytes() const {
	size_t size = 0;
	for (auto & allocation_pair : allocations) {
		if (allocation_pair.first)
			size += allocation_pair.second.size;
	}
	return size;
}

/* End of file ggml-memory-pool.cpp */
