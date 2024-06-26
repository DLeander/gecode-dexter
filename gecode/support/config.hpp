/* gecode/support/config.hpp.  Generated from config.hpp.in by configure.  */
/*
 *  Main authors:
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Guido Tack, 2008
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/* Whether to build with default memory allocator */
#define GECODE_ALLOCATOR /**/

/* Whether to include audit code */
/* #undef GECODE_AUDIT */

/* User-defined prefix of dll names */
#define GECODE_DLL_USERPREFIX ""

/* User-defined suffix of dll names */
#define GECODE_DLL_USERSUFFIX ""

/* Supported version of FlatZinc */
#define GECODE_FLATZINC_VERSION "1.6"

/* max freelist size on 32 bit platforms */
/* #undef GECODE_FREELIST_SIZE_MAX32 */

/* max freelist size on 64 bit platforms */
/* #undef GECODE_FREELIST_SIZE_MAX64 */

/* Whether gcc understands visibility attributes */
#define GECODE_GCC_HAS_CLASS_VISIBILITY /**/

/* whether __builtin_ffsll is available */
/* #undef GECODE_HAS_BUILTIN_FFSLL */

/* whether __builtin_popcountll is available */
/* #undef GECODE_HAS_BUILTIN_POPCOUNTLL */

/* Whether counting-based search support available */
/* #undef GECODE_HAS_CBS */

/* Whether CPProfiler support available */
/* #undef GECODE_HAS_CPPROFILER */

/* Whether to build FLOAT variables */
#define GECODE_HAS_FLOAT_VARS /**/

/* Whether Gist is available */
#define GECODE_HAS_GIST /**/

/* Whether to build INT variables */
#define GECODE_HAS_INT_VARS /**/

/* Whether MPFR is available */
#define GECODE_HAS_MPFR /**/

/* Whether we have mtrace for memory leak debugging */
/* #undef GECODE_HAS_MTRACE */

/* Whether Qt is available */
#define GECODE_HAS_QT /**/

/* Whether to build SET variables */
#define GECODE_HAS_SET_VARS /**/

/* Gecode version */
#define GECODE_LIBRARY_VERSION "6-3-0"

/* Heap memory alignment */
/* #undef GECODE_MEMORY_ALIGNMENT */

/* How to check allocation size */
/* #undef GECODE_MSIZE */

/* Whether to track peak heap size */
/* #undef GECODE_PEAKHEAP */

/* Whether we need malloc.h */
/* #undef GECODE_PEAKHEAP_MALLOC_H */

/* Whether we need malloc/malloc.h */
/* #undef GECODE_PEAKHEAP_MALLOC_MALLOC_H */

/* Whether we are compiling static libraries */
/* #undef GECODE_STATIC_LIBS */

/* Whether we are compiling with thread support */
#define GECODE_HAS_THREADS 1

/* Whether to use unfair mutexes on macOS */
/* #undef GECODE_USE_OSX_UNFAIR_MUTEX */

/* Gecode version */
#define GECODE_VERSION "6.3.0"

/* Gecode version */
#define GECODE_VERSION_NUMBER 600300

/* How to tell the compiler to really, really inline */
#define forceinline inline __attribute__ ((__always_inline__))

// STATISTICS: support-any
