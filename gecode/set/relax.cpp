/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2016
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

#include <gecode/set.hh>
#include <gecode/search/relax.hh>

namespace Gecode { namespace Set {

  /// Class for posting equalities for non-relaxed variables
  class Relax {
  public:
    /// Post the actual equality for non-relaxed variables
    ExecStatus operator ()(Home home, SetVar x, SetVar sx);
  };

  forceinline ExecStatus
  Relax::operator ()(Home home, SetVar x, SetVar sx) {
    SetView xv(x);
    SetView sxv(sx);
    SetVarGlbRanges l(sxv);
    GECODE_ME_CHECK(xv.includeI(home, l));
    SetVarLubRanges u(sxv);
    GECODE_ME_CHECK(xv.intersectI(home, u));
    return ES_OK;
  }

}}

namespace Gecode {

  void
  relax(Home home, const SetVarArgs& x, const SetVarArgs& sx,
        Rnd r, double p) {
    if (x.size() != sx.size())
      throw Set::ArgumentSizeMismatch("Set::relax");
    if ((p < 0.0) || (p > 1.0))
      throw Set::OutOfLimits("Set::relax");
    Set::Relax ir;
    Search::relax<SetVarArgs,Set::Relax>(home,x,sx,r,p,ir);
  }

}

// STATISTICS: set-other

