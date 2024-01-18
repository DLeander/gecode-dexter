/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Guido Tack, 2014
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

#include "test/flatzinc.hh"

namespace Test { namespace FlatZinc {

  namespace {
    /// Helper class to create and register tests
    class Create {
    public:

      /// Perform creation and registration
      Create(void) {
        (void) new FlatZincTest("bugfix_r7854","\
array[1 .. 9] of var 0 .. 8: pos__253 :: output_array([1..9]);\
array[1 .. 17] of var 0 .. 1: y__967::var_is_introduced;\
array[1 .. 17] of var bool: ful;\
array[1 .. 34] of int: prefs__250 = \
  [ 0, 2, 0, 4, 0, 7, 1, 4, 1, 8, 2, 3, 2, 4, 3, 0, 3, 4, 4, 5, 4, 0, 5, 0, \
    5, 8, 6, 2, 6, 7, 7, 8, 7, 6 ];\
var 0 .. 8: sat :: output_var;\
var bool: b__268::var_is_introduced;\
var bool: b__275::var_is_introduced;\
var bool: b__292::var_is_introduced;\
var bool: b__299::var_is_introduced;\
var bool: b__316::var_is_introduced;\
var bool: b__323::var_is_introduced;\
var bool: b__340::var_is_introduced;\
var bool: b__347::var_is_introduced;\
var bool: b__364::var_is_introduced;\
var bool: b__371::var_is_introduced;\
var bool: b__388::var_is_introduced;\
var bool: b__395::var_is_introduced;\
var bool: b__412::var_is_introduced;\
var bool: b__419::var_is_introduced;\
var bool: b__436::var_is_introduced;\
var bool: b__443::var_is_introduced;\
var bool: b__460::var_is_introduced;\
var bool: b__467::var_is_introduced;\
var bool: b__484::var_is_introduced;\
var bool: b__491::var_is_introduced;\
var bool: b__508::var_is_introduced;\
var bool: b__515::var_is_introduced;\
var bool: b__532::var_is_introduced;\
var bool: b__539::var_is_introduced;\
var bool: b__556::var_is_introduced;\
var bool: b__563::var_is_introduced;\
var bool: b__580::var_is_introduced;\
var bool: b__587::var_is_introduced;\
var bool: b__604::var_is_introduced;\
var bool: b__611::var_is_introduced;\
var bool: b__628::var_is_introduced;\
var bool: b__635::var_is_introduced;\
var bool: b__652::var_is_introduced;\
var bool: b__659::var_is_introduced;\
constraint bool_xor(b__652, b__659, ful[17]);\
constraint bool_xor(b__628, b__635, ful[16]);\
constraint bool_xor(b__604, b__611, ful[15]);\
constraint bool_xor(b__580, b__587, ful[14]);\
constraint bool_xor(b__556, b__563, ful[13]);\
constraint bool_xor(b__532, b__539, ful[12]);\
constraint bool_xor(b__508, b__515, ful[11]);\
constraint bool_xor(b__484, b__491, ful[10]);\
constraint bool_xor(b__460, b__467, ful[9]);\
constraint bool_xor(b__436, b__443, ful[8]);\
constraint bool_xor(b__412, b__419, ful[7]);\
constraint bool_xor(b__388, b__395, ful[6]);\
constraint bool_xor(b__364, b__371, ful[5]);\
constraint bool_xor(b__340, b__347, ful[4]);\
constraint bool_xor(b__316, b__323, ful[3]);\
constraint bool_xor(b__292, b__299, ful[2]);\
constraint bool_xor(b__268, b__275, ful[1]);\
constraint \
  int_lin_eq(\
    [ -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ], \
    [ sat, y__967[17], y__967[16], y__967[15], y__967[14], y__967[13], \
      y__967[12], y__967[11], y__967[10], y__967[9], y__967[8], y__967[7], \
      y__967[6], y__967[5], y__967[4], y__967[3], y__967[2], y__967[1] ], 0);\
constraint bool2int(ful[17], y__967[17]);\
constraint bool2int(ful[16], y__967[16]);\
constraint bool2int(ful[15], y__967[15]);\
constraint bool2int(ful[14], y__967[14]);\
constraint bool2int(ful[13], y__967[13]);\
constraint bool2int(ful[12], y__967[12]);\
constraint bool2int(ful[11], y__967[11]);\
constraint bool2int(ful[10], y__967[10]);\
constraint bool2int(ful[9], y__967[9]);\
constraint bool2int(ful[8], y__967[8]);\
constraint bool2int(ful[7], y__967[7]);\
constraint bool2int(ful[6], y__967[6]);\
constraint bool2int(ful[5], y__967[5]);\
constraint bool2int(ful[4], y__967[4]);\
constraint bool2int(ful[3], y__967[3]);\
constraint bool2int(ful[2], y__967[2]);\
constraint bool2int(ful[1], y__967[1]);\
constraint int_lt(pos__253[1], pos__253[2]);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[9], pos__253[8] ], -1, b__635);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[9], pos__253[6] ], -1, b__563);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[9], pos__253[2] ], -1, b__371);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[8], pos__253[9] ], -1, b__628);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[8], pos__253[7] ], -1, b__652);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[8], pos__253[7] ], -1, b__611);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[8], pos__253[1] ], -1, b__323);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[7], pos__253[8] ], -1, b__659);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[7], pos__253[8] ], -1, b__604);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[7], pos__253[3] ], -1, b__580);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[6], pos__253[9] ], -1, b__556);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[6], pos__253[5] ], -1, b__491);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[6], pos__253[1] ], -1, b__532);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[5], pos__253[6] ], -1, b__484);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[5], pos__253[4] ], -1, b__467);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[5], pos__253[3] ], -1, b__419);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[5], pos__253[2] ], -1, b__347);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[5], pos__253[1] ], -1, b__508);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[5], pos__253[1] ], -1, b__299);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[4], pos__253[5] ], -1, b__460);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[4], pos__253[3] ], -1, b__395);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[4], pos__253[1] ], -1, b__436);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[3], pos__253[7] ], -1, b__587);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[3], pos__253[5] ], -1, b__412);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[3], pos__253[4] ], -1, b__388);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[3], pos__253[1] ], -1, b__275);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[2], pos__253[9] ], -1, b__364);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[2], pos__253[5] ], -1, b__340);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[1], pos__253[8] ], -1, b__316);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[1], pos__253[6] ], -1, b__539);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[1], pos__253[5] ], -1, b__515);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[1], pos__253[5] ], -1, b__292);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[1], pos__253[4] ], -1, b__443);\
constraint \
  int_lin_eq_reif([ 1, -1 ], [ pos__253[1], pos__253[3] ], -1, b__268);\
constraint int_ne(pos__253[1], pos__253[2]);\
constraint int_ne(pos__253[1], pos__253[3]);\
constraint int_ne(pos__253[1], pos__253[4]);\
constraint int_ne(pos__253[1], pos__253[5]);\
constraint int_ne(pos__253[1], pos__253[6]);\
constraint int_ne(pos__253[1], pos__253[7]);\
constraint int_ne(pos__253[1], pos__253[8]);\
constraint int_ne(pos__253[1], pos__253[9]);\
constraint int_ne(pos__253[2], pos__253[3]);\
constraint int_ne(pos__253[2], pos__253[4]);\
constraint int_ne(pos__253[2], pos__253[5]);\
constraint int_ne(pos__253[2], pos__253[6]);\
constraint int_ne(pos__253[2], pos__253[7]);\
constraint int_ne(pos__253[2], pos__253[8]);\
constraint int_ne(pos__253[2], pos__253[9]);\
constraint int_ne(pos__253[3], pos__253[4]);\
constraint int_ne(pos__253[3], pos__253[5]);\
constraint int_ne(pos__253[3], pos__253[6]);\
constraint int_ne(pos__253[3], pos__253[7]);\
constraint int_ne(pos__253[3], pos__253[8]);\
constraint int_ne(pos__253[3], pos__253[9]);\
constraint int_ne(pos__253[4], pos__253[5]);\
constraint int_ne(pos__253[4], pos__253[6]);\
constraint int_ne(pos__253[4], pos__253[7]);\
constraint int_ne(pos__253[4], pos__253[8]);\
constraint int_ne(pos__253[4], pos__253[9]);\
constraint int_ne(pos__253[5], pos__253[6]);\
constraint int_ne(pos__253[5], pos__253[7]);\
constraint int_ne(pos__253[5], pos__253[8]);\
constraint int_ne(pos__253[5], pos__253[9]);\
constraint int_ne(pos__253[6], pos__253[7]);\
constraint int_ne(pos__253[6], pos__253[8]);\
constraint int_ne(pos__253[6], pos__253[9]);\
constraint int_ne(pos__253[7], pos__253[8]);\
constraint int_ne(pos__253[7], pos__253[9]);\
constraint int_ne(pos__253[8], pos__253[9]);\
solve \
  ::int_search(pos__253, first_fail, indomain, complete) maximize sat;\
", "\
pos__253 = array1d(1..9, [0, 1, 4, 3, 2, 8, 5, 6, 7]);\n\
sat = 8;\n\
----------\n\
==========\n\
");
      }
    };

    Create c;
  }

}}

// STATISTICS: test-flatzinc
