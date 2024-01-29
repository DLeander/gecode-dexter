/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2006
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

namespace Gecode { namespace Search {

   /*
    * Creation functions for stop objects
    *
    */
  Stop*
  Stop::node(unsigned long long int l) {
    return new NodeStop(l);
  }
  Stop*
  Stop::fail(unsigned long long int l) {
    return new FailStop(l);
  }
  Stop*
  Stop::time(double l) {
    return new TimeStop(l);
  }
  Stop*
  Stop::restart(unsigned long long int  l) {
    return new RestartStop(l);
  }


  /*
   * Stopping for node limit
   *
   */
  bool
  NodeStop::stop(const Statistics& s, const Options&) {
    return s.node > l;
  }


  /*
   * Stopping for failure limit
   *
   */
  bool
  FailStop::stop(const Statistics& s, const Options&) {
    return s.fail > l;
  }


  /*
   * Stopping for time limit
   *
   */
  bool
  TimeStop::stop(const Statistics&, const Options&) {
    return t.stop() > l;
  }

  /*
   * Stopping for restart limit
   *
   */
  bool
  RestartStop::stop(const Statistics& s, const Options&) {
    return s.restart > l;
  }

}}

// STATISTICS: search-other
