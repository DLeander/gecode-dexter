/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2009
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

#include <gecode/search.hh>
#include <gecode/search/support.hh>

#include <gecode/search/seq/dfs.hh>
#ifdef GECODE_HAS_THREADS
#include <gecode/search/par/dfs.hh>
#endif

namespace Gecode { namespace Search {

  Engine*
  dfsengine(Space* s, const Options& o) {
    Options to = o.expand();
#ifdef GECODE_HAS_THREADS
    if (to.threads == 1.0) {
      if (to.tracer)
        return new WorkerToEngine<Seq::DFS<TraceRecorder>>(s,to);
      else
        return new WorkerToEngine<Seq::DFS<NoTraceRecorder>>(s,to);
    } else {
      if (to.tracer)
        return new Par::DFS<EdgeTraceRecorder>(s,to);
      else
        return new Par::DFS<NoTraceRecorder>(s,to);
    }
#else
    if (to.tracer)
      return new WorkerToEngine<Seq::DFS<TraceRecorder>>(s,to);
    else
      return new WorkerToEngine<Seq::DFS<NoTraceRecorder>>(s,to);
#endif
  }

}}

// STATISTICS: search-other
