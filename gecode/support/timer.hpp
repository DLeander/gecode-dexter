/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Mikael Lagerkvist <lagerkvist@gecode.org>
 *
 *  Copyright:
 *     Mikael Lagerkvist, 2009
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

#include <chrono>

namespace Gecode { namespace Support {

  /** \brief %Timer
   *
   * This class represents a best-effort at measuring wall-clock time
   * in milliseconds.
   *
   * \ingroup FuncSupport
   */
  class GECODE_SUPPORT_EXPORT Timer {
  private:
    using time_point = std::chrono::time_point<std::chrono::steady_clock>;
    time_point t0; ///< Start time

  public:
    /// Start timer
    void start(void);
    /// Get time since start of timer
    double stop(void);
  };

  inline void
  Timer::start(void) {
    t0 = std::chrono::steady_clock::now();
  }

  inline double
  Timer::stop(void) {
    std::chrono::duration<double, std::milli> duration = std::chrono::steady_clock::now() - t0;
    return duration.count();
  }

}}

// STATISTICS: support-any
