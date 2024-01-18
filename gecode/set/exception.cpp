/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2004, 2005
 *     Guido Tack, 2005
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

namespace Gecode { namespace Set {

  OutOfLimits::OutOfLimits(const char* l)
    : Exception(l,"Number out of limits") {}

  VariableEmptyDomain::VariableEmptyDomain(const char* l)
    : Exception(l,"Attempt to create variable with empty domain") {}

  TooFewArguments::TooFewArguments(const char* l)
    : Exception(l,"Passed argument array has too few elements") {}

  ArgumentSizeMismatch::ArgumentSizeMismatch(const char* l)
    : Exception(l,"Sizes of argument arrays mismatch") {}

  IllegalOperation::IllegalOperation(const char* l)
    : Exception(l,"Illegal operation type") {}

  UnknownBranching::UnknownBranching(const char* l)
    : Exception(l,"Unknown branching type") {}

  UnknownRelation::UnknownRelation(const char* l)
    : Exception(l,"Unknown relation type") {}

  UnknownOperation::UnknownOperation(const char* l)
    : Exception(l,"Unknown operation type") {}

}}

// STATISTICS: set-other

