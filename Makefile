#  -*-Makefile-*-
#
#  Main authors:
#     Christian Schulte <schulte@gecode.org>
#     Guido Tack <tack@gecode.org>
#
#  Contributing authors:
#     Mikael Lagerkvist <lagerkvist@gecode.org>
#
#  Copyright:
#     Christian Schulte, 2005
#     Guido Tack, 2005
#     Mikael Lagerkvist, 2020
#
#  This file is part of Gecode, the generic constraint
#  development environment:
#     http://www.gecode.org
#
#  Permission is hereby granted, free of charge, to any person obtaining
#  a copy of this software and associated documentation files (the
#  "Software"), to deal in the Software without restriction, including
#  without limitation the rights to use, copy, modify, merge, publish,
#  distribute, sublicense, and/or sell copies of the Software, and to
#  permit persons to whom the Software is furnished to do so, subject to
#  the following conditions:
#
#  The above copyright notice and this permission notice shall be
#  included in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
#  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
#  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

#
# General Settings
#

export prefix = /usr/local
export exec_prefix = ${prefix}
export bindir = ${exec_prefix}/bin
export libdir = ${exec_prefix}/lib
export sharedlibdir = ${exec_prefix}/lib
export includedir = ${prefix}/include
export srcdir = .
export datarootdir = ${prefix}/share
export datadir = ${datarootdir}
export docdir= ${datarootdir}/doc/gecode
export top_srcdir = .
export top_builddir = .

# Install to different root directory
DESTDIR=

export subdirs =  

export VERSION	= 6.3.0

export CXX = g++
export CC = gcc
export MOC = moc
ifeq "$(top_srcdir)" "$(top_builddir)"
CPPFLAGS=-I$(top_srcdir)
else
CPPFLAGS=-I$(top_builddir) -I$(top_srcdir)
endif
export CPPFLAGS
export CXXFLAGS = $(CPPFLAGS) -fcx-limited-range -fno-signaling-nans -fno-rounding-math -ffinite-math-only -fno-math-errno -fno-strict-aliasing -O3 -fvisibility=hidden -ggdb -std=c++17 -pipe -Wno-unknown-pragmas -Wall -Wextra -fPIC -DNDEBUG $(CXXUSR)
export CFLAGS = $(CPPFLAGS) -fcx-limited-range -fno-signaling-nans -fno-rounding-math -ffinite-math-only -fno-math-errno -fno-strict-aliasing -O3 -fvisibility=hidden -ggdb -pipe -Wno-unknown-pragmas -Wall -Wextra -fPIC -DNDEBUG $(CUSR)
export EXAMPLES_EXTRA_CXXFLAGS = 

export QT_CPPFLAGS =  -DQT_NO_DEBUG -DQT_PRINTSUPPORT_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB  -I. -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtPrintSupport -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I. -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++
export LINKQT =  $(SUBLIBS) /usr/lib/x86_64-linux-gnu/libQt5PrintSupport.so /usr/lib/x86_64-linux-gnu/libQt5Widgets.so /usr/lib/x86_64-linux-gnu/libQt5Gui.so /usr/lib/x86_64-linux-gnu/libQt5Core.so -lGL -lpthread   
export MPFR_CPPFLAGS =  
ifeq "yes" "yes"
export LINKMPFR =   -lmpfr  -lgmp
else
export LINKMPFR =
endif

ifeq "yes" "yes"
export LINKSTATICQT
else
export LINKSTATICQT = $(LINKQT)
endif

export GLDFLAGS = 
export DLLFLAGS = -shared
export DLLPATH = -L.

export RANLIB = ranlib
export TAR = tar

export RUNENVIRONMENT = LD_LIBRARY_PATH=.

#
# Use suffixes to get consistent treatment of dots
#
export OBJSUFFIX	= .o
export SBJSUFFIX	= .s
export DLLSUFFIX	= .so.51.0
export MANIFESTSUFFIX	= .so.51.0.manifest
export RCSUFFIX		= .so.51.0.rc
export RESSUFFIX	= .so.51.0.res
export LIBSUFFIX	= .so.51.0
export PDBSUFFIX	= .pdb
export EXPSUFFIX	= .exp
export STATICLIBSUFFIX	= .a
export EXESUFFIX	= 

export LIBPREFIX 	= libgecode
export LINKPREFIX	= -lgecode
export LINKSUFFIX	= 

export SOLINKSUFFIX     = .so
export SOSUFFIX         = .so.51

#
# Programs
#
export RMF		= rm -rf
export MV		= mv
export MANIFEST		= @true -nologo
export FIXMANIFEST	= perl $(top_srcdir)/misc/fixmanifest.perl $(DLLSUFFIX)
export RESCOMP		= @true -n -i$(top_srcdir)
export RCGEN		= perl $(top_srcdir)/misc/genrc.perl $(DLLSUFFIX) $(top_srcdir)

ifeq "yes" "yes"
export CREATELINK       = ln -fs
else
export CREATELINK       = @true
endif

ifeq "no" "yes"
%.res: %.rc
	$(RESCOMP) -fo $@ $<
.PRECIOUS: %.res %.rc
endif

#
# SUPPORT COMPONENTS
#
SUPPORTSRC0 = \
	exception allocator heap \
	thread/thread \
	hw-rnd
SUPPORTHDR0 = \
	block-allocator cast hash dynamic-array \
	dynamic-stack exception allocator heap \
	macros random sort static-stack \
	marked-pointer int-type auto-link \
	thread thread/thread timer \
	dynamic-queue bitset-base bitset bitset-offset \
	hw-rnd run-jobs ref-count

SUPPORTSRC1	=  $(SUPPORTSRC0:%=gecode/support/%.cpp)
SUPPORTHDR 	=  gecode/support.hh \
	$(SUPPORTHDR0:%=gecode/support/%.hpp)
SUPPORTOBJ	=  $(SUPPORTSRC1:%.cpp=%$(OBJSUFFIX))
SUPPORTSRC	=  $(SUPPORTSRC1)

export SUPPORTDLL	= $(LIBPREFIX)support$(DLLSUFFIX)
export SUPPORTLIB	= $(LIBPREFIX)support$(LIBSUFFIX)
ifeq "yes" "yes"
export SUPPORTSONAME     = \
	-Wl,-soname=$(LIBPREFIX)support$(SOSUFFIX)
else
export SUPPORTSONAME     =
endif
export SUPPORTSTATICLIB	= $(LIBPREFIX)support$(STATICLIBSUFFIX)
export LINKSUPPORT      = \
	$(LINKPREFIX)support$(LINKSUFFIX)
ifeq "no" "yes"
export SUPPORTRC	= $(LIBPREFIX)support$(RCSUFFIX)
export SUPPORTRES	= $(LIBPREFIX)support$(RESSUFFIX)
else
export SUPPORTRES	=
export SUPPORTRC	=
endif

SUPPORTBUILDDIRS = support support/thread


#
# KERNEL COMPONENTS
#

VARIMPHDR = \
	gecode/kernel/var-type.hpp \
	gecode/kernel/var-imp.hpp

VARIMP = $(VARIMPHDR)

KERNELSRC0 = \
	archive core exception gpi \
	data/rnd \
	branch/action branch/afc branch/chb branch/function \
	memory/manager memory/region \
	trace/recorder trace/filter trace/tracer trace/general \
	data/array

KERNELHDR0 = \
	archive core exception macros modevent gpi \
	shared-object shared-space-data range-list \
	view var \
	memory/config memory/manager memory/region memory/allocators \
	data/array data/rnd data/shared-array data/shared-data \
	propagator/pattern propagator/advisor propagator/subscribed \
	propagator/wait \
	branch/var branch/val branch/tiebreak \
	branch/traits branch/afc branch/action branch/chb \
	branch/view-sel branch/merit \
	branch/val-sel branch/val-commit branch/view branch/view-val \
	branch/val-sel-commit branch/print branch/filter \
	trace/traits trace/filter trace/tracer trace/recorder \
	trace/general trace/print


KERNELSRC 	= $(KERNELSRC0:%=gecode/kernel/%.cpp)
KERNELHDR 	= \
	gecode/kernel.hh \
	$(KERNELHDR0:%=gecode/kernel/%.hpp)
KERNELOBJ	= $(KERNELSRC:%.cpp=%$(OBJSUFFIX))

export KERNELDLL	= $(LIBPREFIX)kernel$(DLLSUFFIX)
export KERNELLIB	= $(LIBPREFIX)kernel$(LIBSUFFIX)
ifeq "yes" "yes"
export KERNELSONAME     = \
	-Wl,-soname=$(LIBPREFIX)kernel$(SOSUFFIX)
else
export KERNELSONAME     =
endif
export KERNELSTATICLIB	= $(LIBPREFIX)kernel$(STATICLIBSUFFIX)
export LINKKERNEL      	= $(LINKPREFIX)kernel$(LINKSUFFIX)
ifeq "no" "yes"
export KERNELRC		= $(LIBPREFIX)kernel$(RCSUFFIX)
export KERNELRES	= $(LIBPREFIX)kernel$(RESSUFFIX)
else
export KERNELRES	=
export KERNELRC		=
endif

KERNELBUILDDIRS = kernel \
	kernel/data kernel/propagator kernel/branch kernel/memory kernel/trace

ifeq "yes" "yes"
EXTRA_HEADERS   = gecode/support/config.hpp gecode/flatzinc/parser.tab.hpp
else
EXTRA_HEADERS   = gecode/support/config.hpp
endif

#
# SEARCH COMPONENTS
#

SEARCHSRC0 = \
	stop options cutoff engine \
	dfs bab lds \
	seq/rbs seq/dead seq/pbs par/pbs \
	rbs pbs nogoods exception tracer \
	cpprofiler/tracer
SEARCHHDR0 = \
	statistics.hpp stop.hpp options.hpp cutoff.hpp \
	support.hh worker.hh exception.hpp engine.hpp base.hpp \
	nogoods.hh nogoods.hpp build.hpp traits.hpp sebs.hpp \
	seq/path.hh seq/path.hpp seq/dfs.hh seq/dfs.hpp \
	seq/bab.hh seq/bab.hpp seq/lds.hh seq/lds.hpp \
	seq/rbs.hh seq/rbs.hpp seq/dead.hh \
	seq/pbs.hh seq/pbs.hpp \
	par/path.hh par/path.hpp par/engine.hh par/engine.hpp \
	par/dfs.hh par/dfs.hpp par/bab.hh par/bab.hpp \
	par/pbs.hh par/pbs.hpp \
	dfs.hpp bab.hpp lds.hpp rbs.hpp pbs.hpp \
	relax.hh tracer.hpp trace-recorder.hpp \
	cpprofiler/message.hpp cpprofiler/connector.hpp

SEARCHSRC	= $(SEARCHSRC0:%=gecode/search/%.cpp)
SEARCHHDR	= gecode/search.hh $(SEARCHHDR0:%=gecode/search/%)
SEARCHOBJ	= $(SEARCHSRC:%.cpp=%$(OBJSUFFIX))

ifeq "yes" "yes"
export SEARCHDLL	= $(LIBPREFIX)search$(DLLSUFFIX)
export SEARCHSTATICLIB	= $(LIBPREFIX)search$(STATICLIBSUFFIX)
export SEARCHLIB	= $(LIBPREFIX)search$(LIBSUFFIX)
export LINKSEARCH       = $(LINKPREFIX)search$(LINKSUFFIX)
ifeq "yes" "yes"
export SEARCHSONAME     = \
	-Wl,-soname=$(LIBPREFIX)search$(SOSUFFIX)
else
export SEARCHSONAME     =
ifeq "no" "yes"
export SEARCHRC		= $(LIBPREFIX)search$(RCSUFFIX)
export SEARCHRES	= $(LIBPREFIX)search$(RESSUFFIX)
else
export SEARCHRES	=
export SEARCHRC		=
endif
endif
else
export SEARCHDLL	=
export SEARCHSTATICLIB	=
export SEARCHLIB	=
export LINKSEARCH       =
export SEARCHRES	=
export SEARCHRC		=
endif
SEARCHBUILDDIRS = search search/seq search/par search/cpprofiler


#
# INTEGER COMPONENTS
#

INTSRC0 = \
	int-set.cpp var-imp/int.cpp var-imp/bool.cpp var/int.cpp \
	var/bool.cpp array.cpp bool.cpp bool/eqv.cpp \
	extensional/dfa.cpp extensional/tuple-set.cpp \
	extensional-regular.cpp extensional-tuple-set.cpp \
	dom.cpp rel.cpp precede.cpp element.cpp count.cpp \
	arithmetic.cpp exec.cpp \
	exec/when.cpp element/pair.cpp \
	linear/int-post.cpp linear-int.cpp \
	linear/bool-post.cpp linear-bool.cpp \
	branch.cpp distinct/eqite.cpp distinct/cbs.cpp \
	distinct.cpp sorted.cpp gcc.cpp \
	channel.cpp channel/link-single.cpp channel/link-multi.cpp \
	unshare.cpp sequence.cpp \
	bin-packing.cpp bin-packing/propagate.cpp \
	bin-packing/conflict-graph.cpp \
	order.cpp order/propagate.cpp \
	unary.cpp cumulative.cpp cumulatives.cpp \
	circuit.cpp no-overlap.cpp nvalues.cpp \
	member.cpp branch/action.cpp branch/chb.cpp \
	arithmetic/mult.cpp  \
	branch/view-sel.cpp branch/val-sel-commit.cpp \
	branch/view-values.cpp \
	relax.cpp \
	ldsb.cpp ldsb/sym-imp.cpp ldsb/sym-obj.cpp \
	trace.cpp trace/tracer.cpp \
	exception.cpp

INTHDR0 = \
	limits.hpp support-values.hh support-values.hpp \
	idx-view.hh idx-view.hpp div.hh div.hpp \
	exec.hh exec/when.hpp \
	arithmetic/abs.hpp arithmetic/max.hpp arithmetic/argmax.hpp \
	arithmetic/mult.hpp arithmetic/divmod.hpp \
	arithmetic/pow-ops.hpp arithmetic/pow.hpp arithmetic/nroot.hpp \
	bool/or.hpp bool/eq.hpp bool/lq.hpp bool/eqv.hpp bool/base.hpp \
	bool/clause.hpp bool/ite.hpp \
	precede.hh precede/single.hpp \
	branch/traits.hpp branch/var.hpp branch/val.hpp branch/assign.hpp \
	branch/view-values.hpp branch/merit.hpp \
	branch/val-sel.hpp branch/val-commit.hpp branch/ngl.hpp \
	branch/cbs.hpp branch.hpp \
	count.hh count/rel.hpp \
	count/int-base.hpp count/int-eq.hpp \
	count/int-gq.hpp count/int-lq.hpp \
	count/view-base.hpp count/view-eq.hpp \
	count/view-gq.hpp count/view-lq.hpp \
	view-val-graph.hh view-val-graph/graph.hpp \
	view-val-graph/bi-link.hpp view-val-graph/comb-ptr-flag.hpp \
	view-val-graph/edge.hpp view-val-graph/node.hpp \
	view-val-graph/iter-prune-val.hpp \
	distinct/graph.hpp distinct/dom-ctrl.hpp \
	distinct/bnd.hpp distinct/dom.hpp \
	distinct/val.hpp distinct/ter-dom.hpp \
	distinct/cbs.hpp \
	distinct/eqite.hpp \
	dom/range.hpp dom/set.hpp \
	element/int.hpp element/view.hpp element/pair.hpp \
	gcc/bnd.hpp gcc/dom.hpp gcc/bnd-sup.hpp gcc/dom-sup.hpp \
	gcc/val.hpp gcc/view.hpp gcc/post.hpp \
	linear/post.hpp \
	linear/int-noview.hpp linear/int-bin.hpp linear/int-ter.hpp \
	linear/int-nary.hpp linear/int-dom.hpp \
	linear/bool-int.hpp linear/bool-view.hpp linear/bool-scale.hpp \
	extensional/dfa.hpp extensional/layered-graph.hpp \
	extensional/tuple-set.hpp extensional/compact.hpp \
	extensional/tiny-bit-set.hpp extensional/bit-set.hpp \
	extensional.hpp \
	rel/eq.hpp rel/lex.hpp rel/lq-le.hpp rel/nq.hpp \
	sorted/matching.hpp sorted/narrowing.hpp \
	sorted/order.hpp sorted/propagate.hpp sorted/sortsup.hpp \
	int-set-1.hpp int-set-2.hpp var-imp/delta.hpp var/print.hpp var/bool.hpp \
	var-imp/int.hpp var-imp/bool.hpp var/int.hpp \
	view/bool.hpp view/cached.hpp view/constint.hpp view/zero.hpp view/int.hpp \
	view/minus.hpp view/print.hpp view/neg-bool.hpp view/bool-test.hpp \
	view/offset.hpp view/rel-test.hpp view/scale.hpp view/iter.hpp \
	arithmetic.hh array.hpp array-traits.hpp bool.hh branch.hh \
	distinct.hh dom.hh \
	element.hh exception.hpp  gcc.hh linear.hh \
	reify.hpp propagator.hpp extensional.hh rel.hh \
	sorted.hh var-imp.hpp view.hpp ipl.hpp irt.hpp \
	channel.hh channel.hpp channel/dom.hpp channel/val.hpp \
	channel/base.hpp channel/link-single.hpp channel/link-multi.hpp \
	sequence.hh sequence/int.hpp sequence/view.hpp \
	sequence/set-op.hpp sequence/violations.hpp \
	bin-packing.hh bin-packing/propagate.hpp \
	bin-packing/conflict-graph.hpp \
	task.hh task/fwd-to-bwd.hpp task/array.hpp task/sort.hpp \
	task/iter.hpp task/tree.hpp task/purge.hpp task/prop.hpp \
	task/man-to-opt.hpp task/event.hpp \
	order.hh order/propagate.hpp \
	unary.hh unary/task.hpp unary/task-view.hpp \
	unary/tree.hpp unary/overload.hpp unary/detectable.hpp \
	unary/time-tabling.hpp unary/not-first-not-last.hpp \
	unary/edge-finding.hpp unary/subsumption.hpp \
	unary/man-prop.hpp unary/opt-prop.hpp unary/post.hpp \
	cumulative.hh cumulative/man-prop.hpp cumulative/opt-prop.hpp \
	cumulative/task-view.hpp cumulative/overload.hpp \
	cumulative/time-tabling.hpp cumulative/task.hpp \
	cumulative/edge-finding.hpp cumulative/post.hpp \
	cumulative/tree.hpp cumulative/limits.hpp \
	cumulative/subsumption.hpp \
	cumulatives.hh cumulatives/val.hpp \
	circuit.hh circuit/base.hpp circuit/val.hpp circuit/dom.hpp \
	no-overlap.hh no-overlap/dim.hpp no-overlap/box.hpp \
	no-overlap/base.hpp no-overlap/man.hpp no-overlap/opt.hpp \
	nvalues.hh nvalues/range-event.hpp \
	nvalues/sym-bit-matrix.hpp nvalues/graph.hpp \
	nvalues/bool-base.hpp nvalues/bool-eq.hpp \
	nvalues/bool-lq.hpp nvalues/bool-gq.hpp \
	nvalues/int-base.hpp nvalues/int-eq.hpp \
	nvalues/int-lq.hpp nvalues/int-gq.hpp \
	val-set.hh val-set.hpp \
	member.hh member/prop.hpp member/re-prop.hpp \
	branch/afc.hpp branch/action.hpp branch/chb.hpp \
	ldsb.hh ldsb/brancher.hpp ldsb/sym-imp.hpp \
	trace.hpp \
	trace/bool-trace-view.hpp trace/int-trace-view.hpp \
	trace/bool-delta.hpp trace/int-delta.hpp trace/traits.hpp

INTSRC		= $(INTSRC0:%=gecode/int/%)
INTHDR		= gecode/int.hh $(INTHDR0:%=gecode/int/%)
INTOBJ		= $(INTSRC:%.cpp=%$(OBJSUFFIX))

ifeq "yes" "yes"
export INTDLL		= $(LIBPREFIX)int$(DLLSUFFIX)
export INTSTATICLIB	= $(LIBPREFIX)int$(STATICLIBSUFFIX)
export INTLIB		= $(LIBPREFIX)int$(LIBSUFFIX)
export LINKINT      	= $(LINKPREFIX)int$(LINKSUFFIX)
ifeq "yes" "yes"
export INTSONAME     = \
	-Wl,-soname=$(LIBPREFIX)int$(SOSUFFIX)
else
export INTSONAME     =
endif
ifeq "no" "yes"
export INTRC		= $(LIBPREFIX)int$(RCSUFFIX)
export INTRES		= $(LIBPREFIX)int$(RESSUFFIX)
else
export INTRES		=
export INTRC		=
endif
else
export INTDLL		=
export INTSTATICLIB	=
export INTLIB		=
export LINKINT      	=
export INTRES		=
export INTRC		=
endif
INTBUILDDIRS	= \
	int int/var int/var-imp int/view int/extensional \
	int/channel int/arithmetic int/linear int/bool int/branch int/exec \
	int/element int/sequence int/bin-packing \
	int/unary int/cumulative int/cumulatives int/task \
	int/ldsb int/distinct int/trace int/order

#
# FLOAT COMPONENTS
#

FLOATSRC0 = \
	var/float.cpp var-imp/float.cpp \
	arithmetic.cpp array.cpp branch.cpp rel.cpp linear.cpp \
	linear/post.cpp bool.cpp channel.cpp \
	transcendental.cpp trigonometric.cpp \
	branch/action.cpp branch/chb.cpp rounding.cpp exec.cpp \
	branch/val-sel-commit.cpp branch/view-sel.cpp dom.cpp \
	relax.cpp \
	trace.cpp trace/tracer.cpp \
	exception.cpp

FLOATHDR0 = \
	var-imp.hpp array.hpp array-traits.hpp limits.hpp exception.hpp \
	view.hpp view/float.hpp view/minus.hpp view/offset.hpp \
	view/scale.hpp view/print.hpp view/rel-test.hpp \
	var-imp/float.hpp var-imp/delta.hpp \
	var/float.hpp var/print.hpp linear.hh linear/nary.hpp \
	arithmetic.hh arithmetic/sqr-sqrt.hpp arithmetic/pow-nroot.hpp \
	arithmetic/mult.hpp arithmetic/div.hpp arithmetic/min-max.hpp \
	arithmetic/abs.hpp \
	channel.hh channel.hpp channel/channel.hpp \
	transcendental.hh transcendental/exp-log.hpp trigonometric.hh \
	trigonometric/sincos.hpp trigonometric/asinacos.hpp \
	trigonometric/tanatan.hpp \
	rel.hh rel/eq.hpp rel/lq-le.hpp rel/nq.hpp \
	bool.hh bool/ite.hpp \
	branch.hh branch/afc.hpp branch/action.hpp branch/chb.hpp \
	branch/traits.hpp branch.hpp \
	branch/var.hpp branch/val.hpp branch/assign.hpp \
	branch/val-commit.hpp branch/val-sel.hpp branch/merit.hpp \
	num.hpp val.hpp rounding.hpp nextafter.hpp \
	trace.hpp trace/trace-view.hpp trace/delta.hpp trace/traits.hpp

FLOATSRC	= $(FLOATSRC0:%=gecode/float/%)
FLOATHDR	= gecode/float.hh $(FLOATHDR0:%=gecode/float/%)
FLOATOBJ	= $(FLOATSRC:%.cpp=%$(OBJSUFFIX))

ifeq "yes" "yes"
export FLOATDLL		= $(LIBPREFIX)float$(DLLSUFFIX)
export FLOATSTATICLIB	= $(LIBPREFIX)float$(STATICLIBSUFFIX)
export FLOATLIB		= $(LIBPREFIX)float$(LIBSUFFIX)
export LINKFLOAT	= $(LINKPREFIX)float$(LINKSUFFIX)
ifeq "yes" "yes"
export FLOATSONAME     = \
	-Wl,-soname=$(LIBPREFIX)float$(SOSUFFIX)
else
export FLOATSONAME     =
endif
ifeq "no" "yes"
export FLOATRC		= $(LIBPREFIX)float$(RCSUFFIX)
export FLOATRES		= $(LIBPREFIX)float$(RESSUFFIX)
else
export FLOATRES		=
export FLOATRC		=
endif
else
export FLOATDLL	    	=
export FLOATSTATICLIB	=
export FLOATLIB		=
export LINKFLOAT	=
export FLOATRES		=
export FLOATRC		=
endif
FLOATBUILDDIRS	= \
	float float/var float/var-imp float/view \
	float/linear float/branch float/rel float/branch \
	float/arithmetic float/transcendental float/trigonometric \
	float/trace float/channel

#
# SET COMPONENTS
#

SETSRC0 =								 \
	branch.cpp cardinality.cpp convex.cpp convex/conv.cpp convex/hull.cpp \
	dom.cpp rel.cpp \
	rel-op.cpp rel-op-ternary.cpp rel-op-singleton.cpp \
	rel-op/post-compl.cpp rel-op/post-nocompl.cpp \
	rel-op/post-compl-cvv.cpp rel-op/post-nocompl-cvv.cpp \
	rel-op/post-compl-vvc.cpp rel-op/post-nocompl-vvc.cpp \
	rel-op/post-compl-cvc.cpp rel-op/post-nocompl-cvc.cpp \
	rel-op-const-vvc.cpp rel-op-const-vcv.cpp rel-op-const-vcc.cpp \
	rel-op-const-cvc.cpp rel-op-const-cvv.cpp \
	int.cpp channel.cpp bool.cpp \
	element.cpp sequence.cpp			 \
	distinct.cpp distinct/atmostOne.cpp				 \
	precede.cpp \
	sequence/seq.cpp sequence/seq-u.cpp array.cpp var-imp/set.cpp	 \
	var-imp/integerset.cpp var/set.cpp \
	exec.cpp branch/action.cpp branch/chb.cpp branch/view-sel.cpp \
	branch/val-sel-commit.cpp  branch/ngl.cpp \
	ldsb.cpp ldsb/sym-imp.cpp \
	relax.cpp \
	trace.cpp trace/tracer.cpp \
	exception.cpp
SETHDR0 =								     \
	limits.hpp                                                           \
	view.hpp exception.hpp int.hh int.hpp element.hh var-imp.hpp         \
	int/card.hpp     						     \
	int/minmax.hpp int/weights.hpp                                       \
	channel/sorted.hpp channel/int.hpp channel/bool.hpp channel/set.hpp  \
	element/inter.hpp element/union.hpp                                  \
	element/unionConst.hpp                                               \
	element/disjoint.hpp array.hpp array-traits.hpp		             \
	var-imp/set.hpp var-imp/integerset.hpp	     			     \
	var-imp/delta.hpp var-imp/iter.hpp 				     \
	var/set.hpp var/print.hpp					     \
	view/complement.hpp view/const.hpp view/set.hpp view/singleton.hpp   \
	view/cached.hpp view/print.hpp                                       \
	rel/subset.hpp rel/re-subset.hpp rel/eq.hpp rel/nq.hpp rel/re-eq.hpp \
	rel/lq.hpp rel/re-lq.hpp \
	rel/common.hpp rel/nosubset.hpp sequence.hh	                     \
	rel-op/union.hpp rel-op/partition.hpp rel-op/subofunion.hpp	     \
	rel-op/inter.hpp rel-op/superofinter.hpp			     \
	rel-op/post.hpp rel-op/common.hpp rel.hh			     \
	distinct.hh distinct/atmostOne.hpp				     \
	sequence/seq.hpp sequence/seq-u.hpp				     \
	sequence/common.hpp convex/conv.hpp convex/hull.hpp convex.hh	     \
	precede.hh precede/single.hpp \
	rel-op.hh					     \
	branch.hh branch/afc.hpp branch/action.hpp branch/chb.hpp \
	branch/traits.hpp branch.hpp \
	branch/var.hpp branch/val.hpp branch/assign.hpp \
	branch/merit.hpp branch/val-commit.hpp branch/val-sel.hpp \
	branch/ngl.hpp \
	ldsb.hh ldsb/brancher.hpp channel.hh \
	trace.hpp trace/trace-view.hpp trace/delta.hpp trace/traits.hpp


SETSRC		= $(SETSRC0:%=gecode/set/%)
SETHDR		= gecode/set.hh $(SETHDR0:%=gecode/set/%)
SETOBJ		= $(SETSRC:%.cpp=%$(OBJSUFFIX))

ifeq "yes" "yes"
export SETDLL		= $(LIBPREFIX)set$(DLLSUFFIX)
export SETSTATICLIB	= $(LIBPREFIX)set$(STATICLIBSUFFIX)
export SETLIB		= $(LIBPREFIX)set$(LIBSUFFIX)
export LINKSET      	= $(LINKPREFIX)set$(LINKSUFFIX)
ifeq "yes" "yes"
export SETSONAME     = \
	-Wl,-soname=$(LIBPREFIX)set$(SOSUFFIX)
else
export SETSONAME     =
endif
ifeq "no" "yes"
export SETRC		= $(LIBPREFIX)set$(RCSUFFIX)
export SETRES		= $(LIBPREFIX)set$(RESSUFFIX)
else
export SETRES		=
export SETRC		=
endif
else
export SETDLL		=
export SETSTATICLIB	=
export SETLIB		=
export LINKSET      	=
export SETRES		=
export SETRC		=
endif
SETBUILDDIRS	=						\
	set set/convex set/distinct set/int set/rel set/rel-op	\
	set/element set/sequence set/var set/var-imp set/view \
	set/branch set/channel set/ldsb set/trace


#
# MINIMODEL COMPONENTS
#

MMSRC0 = \
	int-expr.cpp int-rel.cpp int-arith.cpp bool-expr.cpp \
	set-expr.cpp set-rel.cpp dom.cpp \
	float-expr.cpp float-rel.cpp float-arith.cpp \
	reg.cpp optimize.cpp exception.cpp ipl.cpp
MMHDR0 = \
	int-expr.hpp int-rel.hpp float-expr.hpp float-rel.hpp \
	bool-expr.hpp set-expr.hpp set-rel.hpp \
	exception.hpp matrix.hpp \
	optimize.hpp reg.hpp ldsb.hpp channel.hpp aliases.hpp \
	ipl.hpp

MMSRC 		= $(MMSRC0:%=gecode/minimodel/%)
MMHDR 		= gecode/minimodel.hh $(MMHDR0:%=gecode/minimodel/%)
MMOBJ		= $(MMSRC:%.cpp=%$(OBJSUFFIX))
MMSBJ		= $(MMSRC:%.cpp=%$(SBJSUFFIX))

ifeq "yes" "yes"
export MMDLL		= $(LIBPREFIX)minimodel$(DLLSUFFIX)
export MMSTATICLIB	= $(LIBPREFIX)minimodel$(STATICLIBSUFFIX)
export MMLIB		= $(LIBPREFIX)minimodel$(LIBSUFFIX)
export LINKMM      	= $(LINKPREFIX)minimodel$(LINKSUFFIX)
ifeq "yes" "yes"
export MMSONAME     = \
	-Wl,-soname=$(LIBPREFIX)minimodel$(SOSUFFIX)
else
export MMSONAME     =
endif
ifeq "no" "yes"
export MMRC		= $(LIBPREFIX)minimodel$(RCSUFFIX)
export MMRES		= $(LIBPREFIX)minimodel$(RESSUFFIX)
else
export MMRES		=
export MMRC		=
endif
else
export MMDLL		=
export MMSTATICLIB	=
export MMLIB		=
export LINKMM      	=
export MMRES		=
export MMRC		=
endif
MMBUILDDIRS	= minimodel

#
# DRIVER COMPONENTS
#

DRIVERSRC0 = options.cpp script.cpp
DRIVERHDR0 = options.hpp script.hpp

DRIVERSRC 	= $(DRIVERSRC0:%=gecode/driver/%)
DRIVERHDR 	= gecode/driver.hh $(DRIVERHDR0:%=gecode/driver/%)
DRIVEROBJ	= $(DRIVERSRC:%.cpp=%$(OBJSUFFIX))
DRIVERSBJ	= $(DRIVERSRC:%.cpp=%$(SBJSUFFIX))

ifeq "yes" "yes"
export DRIVERDLL	= $(LIBPREFIX)driver$(DLLSUFFIX)
export DRIVERSTATICLIB	= $(LIBPREFIX)driver$(STATICLIBSUFFIX)
export DRIVERLIB	= $(LIBPREFIX)driver$(LIBSUFFIX)
export LINKDRIVER      	= $(LINKPREFIX)driver$(LINKSUFFIX)
ifeq "yes" "yes"
export DRIVERSONAME     = \
	-Wl,-soname=$(LIBPREFIX)driver$(SOSUFFIX)
else
export DRIVERSONAME     =
endif
ifeq "no" "yes"
export DRIVERRC		= $(LIBPREFIX)driver$(RCSUFFIX)
export DRIVERRES	= $(LIBPREFIX)driver$(RESSUFFIX)
else
export DRIVERRES	=
export DRIVERRC		=
endif
else
export DRIVERDLL	=
export DRIVERSTATICLIB	=
export DRIVERLIB	=
export LINKDRIVER      	=
export DRIVERRES	=
export DRIVERRC		=
endif
DRIVERBUILDDIRS	= driver

#
# ITERATOR HEADERS
#
ITERHDR0 =							\
	ranges-add ranges-append ranges-array ranges-cache	\
	ranges-compl ranges-diff ranges-empty			\
	ranges-inter ranges-minmax ranges-minus			\
	ranges-offset ranges-operations ranges-rangelist	\
	ranges-scale ranges-singleton ranges-singleton-append	\
	ranges-size ranges-union ranges-values			\
	ranges-negative ranges-positive ranges-map		\
	ranges-list                                             \
	values-array values-minus values-offset values-ranges 	\
	values-positive values-negative				\
	values-singleton values-unique values-inter 		\
	values-union values-map	values-bitset values-list       \
	values-bitsetoffset
ITERHDR = gecode/iter.hh \
	$(ITERHDR0:%=gecode/iter/%.hpp)


#
# GIST
#

GISTHDR0 = \
	drawingcursor.hh drawingcursor.hpp \
	gist.hpp gecodelogo.hh node.hh node.hpp \
	nodecursor.hh nodecursor.hpp nodevisitor.hh \
	nodevisitor.hpp layoutcursor.hh layoutcursor.hpp \
	spacenode.hh spacenode.hpp qtgist.hh \
	textoutput.hh treecanvas.hh zoomToFitIcon.hpp \
	visualnode.hh visualnode.hpp nodestats.hh \
	nodewidget.hh mainwindow.hh preferences.hh \
	stopbrancher.hh

GISTSRC0 = \
	drawingcursor.cpp gist.cpp gecodelogo.cpp node.cpp \
	spacenode.cpp \
	textoutput.cpp treecanvas.cpp qtgist.cpp visualnode.cpp nodestats.cpp \
	nodewidget.cpp mainwindow.cpp preferences.cpp stopbrancher.cpp

GISTMOCSRC0 = \
	moc_treecanvas.cpp moc_qtgist.cpp moc_mainwindow.cpp moc_textoutput.cpp\
	moc_preferences.cpp moc_nodestats.cpp

GISTSRC = $(GISTSRC0:%=gecode/gist/%)
GISTMOCSRC = $(GISTMOCSRC0:%=gecode/gist/%)
GISTHDR = $(GISTHDR0:%=gecode/gist/%) gecode/gist.hh

GISTOBJ	= $(GISTSRC:%.cpp=%$(OBJSUFFIX)) $(GISTMOCSRC:%.cpp=%$(OBJSUFFIX))
GISTSBJ	= $(GISTSRC:%.cpp=%$(SBJSUFFIX)) $(GISTMOCSRC:%.cpp=%$(SBJSUFFIX))

GISTBUILDDIRS = gist

ifeq "yes" "yes"
export GISTDLL		= $(LIBPREFIX)gist$(DLLSUFFIX)
export GISTSTATICLIB	= $(LIBPREFIX)gist$(STATICLIBSUFFIX)
export GISTLIB		= $(LIBPREFIX)gist$(LIBSUFFIX)
export LINKGIST      	= $(LINKPREFIX)gist$(LINKSUFFIX)
ifeq "yes" "yes"
export GISTSONAME     = \
	-Wl,-soname=$(LIBPREFIX)gist$(SOSUFFIX)
else
export GISTSONAME     =
endif
ifeq "no" "yes"
export GISTRC		= $(LIBPREFIX)gist$(RCSUFFIX)
export GISTRES		= $(LIBPREFIX)gist$(RESSUFFIX)
else
export GISTRES		=
export GISTRC		=
endif
else
export GISTDLL		=
export GISTSTATICLIB	=
export GISTLIB		=
export LINKGIST      	=
export GISTSONAME	=
export GISTRES		=
export GISTRC		=
endif

#
# FLATZINC
#

FLATZINCSRC0 = flatzinc.cpp registry.cpp branch.cpp fzn-pbs.cpp
FLATZINC_GENSRC0 = parser.tab.cpp lexer.yy.cpp
FLATZINCHDR0 = ast.hh conexpr.hh option.hh parser.hh \
	plugin.hh registry.hh symboltable.hh varspec.hh \
	branch.hh branch.hpp lastval.hh complete.hh fzn-pbs.hh search-engine-base.hh

FLATZINCSRC = $(FLATZINCSRC0:%=gecode/flatzinc/%)
FLATZINC_GENSRC = $(FLATZINC_GENSRC0:%=gecode/flatzinc/%)
FLATZINCHDR = $(FLATZINCHDR0:%=gecode/flatzinc/%) gecode/flatzinc.hh

FLATZINCOBJ = \
	$(FLATZINCSRC:%.cpp=%$(OBJSUFFIX)) \
	$(FLATZINC_GENSRC:%.cpp=%$(OBJSUFFIX))

FLATZINCEXESRC0 = fzn-gecode.cpp
FLATZINCEXESRC  = $(FLATZINCEXESRC0:%=tools/flatzinc/%)
FLATZINCEXEOBJ  = $(FLATZINCEXESRC:%.cpp=%$(OBJSUFFIX))

FLATZINCALLSRC	= $(FLATZINCSRC) $(FLATZINCEXESRC)

FLATZINCBUILDDIRS = gecode/flatzinc tools/flatzinc

ifeq "yes" "yes"
export FLATZINCDLL		= $(LIBPREFIX)flatzinc$(DLLSUFFIX)
export FLATZINCSTATICLIB	= $(LIBPREFIX)flatzinc$(STATICLIBSUFFIX)
export FLATZINCLIB		= $(LIBPREFIX)flatzinc$(LIBSUFFIX)
export LINKFLATZINC		= $(LINKPREFIX)flatzinc$(LINKSUFFIX)
export FLATZINCMZNLIB		= gecode/flatzinc/mznlib
ifeq "yes" "yes"
export FLATZINCCONFIG		= tools/flatzinc/gecode.msc tools/flatzinc/gecode-gist.msc
else
export FLATZINCCONFIG		= tools/flatzinc/gecode.msc
endif
FLATZINCEXE	= tools/flatzinc/fzn-gecode$(EXESUFFIX)
ifeq "yes" "yes"
export FLATZINCSONAME = -Wl,-soname=$(LIBPREFIX)flatzinc$(SOSUFFIX)
else
export FLATZINCSONAME =
endif
ifeq "no" "yes"
export FLATZINCRC		= $(LIBPREFIX)flatzinc$(RCSUFFIX)
export FLATZINCRES		= $(LIBPREFIX)flatzinc$(RESSUFFIX)
else
export FLATZINCRES		=
export FLATZINCRC		=
endif
else
export FLATZINCDLL		=
export FLATZINCSTATICLIB	=
export FLATZINCLIB		=
export LINKFLATZINC		=
export FLATZINCSONAME		=
export FLATZINCMZNLIB		=
export FLATZINCRES		=
export FLATZINCRC		=
FLATZINCEXE	=
endif

#
# EXAMPLES
#


INTEXAMPLEHDR0 =						\
	scowl job-shop-instances
INTEXAMPLESRC0 =						\
	alpha bacp bibd donald efpa eq20 golomb-ruler		\
	graph-color grocery ind-set magic-sequence magic-square	\
	money ortho-latin partition photo queens sudoku sudoku-advanced kakuro  \
	nonogram pentominoes crowded-chess black-hole		\
	minesweeper domino steel-mill sports-league		\
	all-interval langford-number warehouses	radiotherapy    \
	word-square crossword open-shop car-sequencing sat      \
	bin-packing knights tsp perfect-square schurs-lemma     \
	dominating-queens colored-matrix multi-bin-packing	\
	qcp job-shop


INTEXAMPLEHDR  = $(INTEXAMPLEHDR0:%=examples/%.hpp)
INTEXAMPLESRC  = $(INTEXAMPLESRC0:%=examples/%.cpp)
ifeq "yes" "yes"
INTEXAMPLEEXE  = $(INTEXAMPLESRC:%.cpp=%$(EXESUFFIX))
else
INTEXAMPLEEXE  =
endif

MPFRFLOATEXAMPLESRC0 = \
	archimedean-spiral golden-spiral
MPFRFLOATEXAMPLESRC  = $(MPFRFLOATEXAMPLESRC0:%=examples/%.cpp)
ifeq "yes" "yes"
  ifeq "yes" "yes"
  MPFRFLOATEXAMPLEEXE  = $(MPFRFLOATEXAMPLESRC:%.cpp=%$(EXESUFFIX))
  else
  MPFRFLOATEXAMPLEEXE  =
  endif
endif

FLOATEXAMPLESRC0 = \
	cartesian-heart descartes-folium
FLOATEXAMPLESRC  = $(FLOATEXAMPLESRC0:%=examples/%.cpp)
ifeq "yes" "yes"
FLOATEXAMPLEEXE  = $(FLOATEXAMPLESRC:%.cpp=%$(EXESUFFIX))
else
FLOATEXAMPLEEXE  =
endif

SETEXAMPLESRC0 = \
	crew golf hamming steiner queen-armies
SETEXAMPLESRC  = $(SETEXAMPLESRC0:%=examples/%.cpp)
ifeq "yes" "yes"
SETEXAMPLEEXE  = $(SETEXAMPLESRC:%.cpp=%$(EXESUFFIX))
else
SETEXAMPLEEXE  =
endif

EXAMPLEBUILDDIRS = examples

EXAMPLEEXE = \
	$(INTEXAMPLEEXE) $(SETEXAMPLEEXE) $(FLOATEXAMPLEEXE) $(MPFRFLOATEXAMPLEEXE)


#
# THIRD PARTY SOFTWARE
#

BOOSTHDR = \
	config/compiler/borland.hpp config/compiler/clang.hpp \
	config/compiler/codegear.hpp config/compiler/comeau.hpp \
	config/compiler/common_edg.hpp config/compiler/compaq_cxx.hpp \
	config/compiler/digitalmars.hpp config/compiler/gcc.hpp \
	config/compiler/gcc_xml.hpp config/compiler/greenhills.hpp \
	config/compiler/hp_acc.hpp config/compiler/intel.hpp \
	config/compiler/kai.hpp config/compiler/metrowerks.hpp \
	config/compiler/mpw.hpp config/compiler/nvcc.hpp \
	config/compiler/pathscale.hpp config/compiler/pgi.hpp \
	config/compiler/sgi_mipspro.hpp config/compiler/sunpro_cc.hpp \
	config/compiler/vacpp.hpp config/compiler/visualc.hpp \
	config/no_tr1/cmath.hpp config/no_tr1/utility.hpp \
	config/platform/aix.hpp config/platform/amigaos.hpp \
	config/platform/beos.hpp config/platform/bsd.hpp \
	config/platform/cygwin.hpp config/platform/hpux.hpp \
	config/platform/irix.hpp config/platform/linux.hpp \
	config/platform/macos.hpp config/platform/qnxnto.hpp \
	config/platform/solaris.hpp config/platform/symbian.hpp \
	config/platform/vms.hpp config/platform/vxworks.hpp \
	config/platform/win32.hpp config/posix_features.hpp \
	config/select_compiler_config.hpp \
	config/select_platform_config.hpp \
	config/select_stdlib_config.hpp \
	config/stdlib/dinkumware.hpp  config/stdlib/libcomo.hpp \
	config/stdlib/libcpp.hpp config/stdlib/libstdcpp3.hpp \
	config/stdlib/modena.hpp config/stdlib/msl.hpp \
	config/stdlib/roguewave.hpp config/stdlib/sgi.hpp \
	config/stdlib/stlport.hpp config/stdlib/vacpp.hpp \
	config/suffix.hpp config/user.hpp config.hpp \
	detail/endian.hpp detail/limits.hpp \
	numeric/interval/arith.hpp \
	numeric/interval/arith2.hpp \
	numeric/interval/arith3.hpp \
	numeric/interval/checking.hpp \
	numeric/interval/compare/certain.hpp \
	numeric/interval/compare/explicit.hpp \
	numeric/interval/compare/lexicographic.hpp \
	numeric/interval/compare/possible.hpp \
	numeric/interval/compare/set.hpp \
	numeric/interval/compare.hpp \
	numeric/interval/constants.hpp \
	numeric/interval/detail/alpha_rounding_control.hpp \
	numeric/interval/detail/bcc_rounding_control.hpp \
	numeric/interval/detail/bugs.hpp \
	numeric/interval/detail/c99sub_rounding_control.hpp \
	numeric/interval/detail/c99_rounding_control.hpp \
	numeric/interval/detail/division.hpp \
	numeric/interval/detail/ia64_rounding_control.hpp \
	numeric/interval/detail/interval_prototype.hpp \
	numeric/interval/detail/msvc_rounding_control.hpp \
	numeric/interval/detail/ppc_rounding_control.hpp \
	numeric/interval/detail/sparc_rounding_control.hpp \
	numeric/interval/detail/test_input.hpp \
	numeric/interval/detail/x86gcc_rounding_control.hpp \
	numeric/interval/detail/x86_rounding_control.hpp \
	numeric/interval/hw_rounding.hpp \
	numeric/interval/interval.hpp \
	numeric/interval/policies.hpp \
	numeric/interval/rounded_arith.hpp \
	numeric/interval/rounded_transc.hpp \
	numeric/interval/rounding.hpp \
	numeric/interval/transc.hpp \
	numeric/interval/utility.hpp \
	numeric/interval.hpp \
	LICENSE_1_0.txt \
	limits.hpp non_type.hpp type.hpp
THIRDHDR = \
	$(BOOSTHDR:%=gecode/third-party/boost/%)

#
# COLLECTING ALL
#

ALLSRC = \
	$(SUPPORTSRC) $(KERNELSRC) $(SEARCHSRC) \
        $(INTSRC) $(FLOATSRC) $(SETSRC) $(MMSRC) $(DRIVERSRC) \
	$(INTEXAMPLESRC) $(SETEXAMPLESRC) $(FLOATEXAMPLESRC)  $(MPFRFLOATEXAMPLESRC) \
	$(GISTSRC) $(FLATZINCALLSRC)
ALLGECODEHDR = \
	$(SUPPORTHDR) $(KERNELHDR) $(SEARCHHDR) \
        $(INTHDR) $(FLOATHDR) $(SETHDR) $(MMHDR) \
	$(DRIVERHDR) $(ITERHDR) $(GISTHDR) $(FLATZINCHDR) \
	$(INTEXAMPLEHDR)
ALLHDR = \
	$(ALLGECODEHDR) $(THIRDHDR)
ALLOBJ0 = $(ALLSRC:%.cpp=%$(OBJSUFFIX)) \
	$(GISTMOCSRC:%.cpp=%$(OBJSUFFIX)) \
	$(FLATZINC_GENSRC:%.cpp=%$(OBJSUFFIX))
ALLOBJ = $(ALLOBJ0:%.c=%$(OBJSUFFIX))

ALLSBJ0 = $(ALLSRC:%.cpp=%$(SBJSUFFIX))
ALLSBJ  = $(ALLSBJ0:%.c=%$(SBJSUFFIX))

ifeq "yes" "yes"
DLLTARGETS= \
	$(SUPPORTDLL) $(KERNELDLL) $(SEARCHDLL) $(INTDLL) $(FLOATDLL) $(SETDLL) \
	$(MMDLL) $(DRIVERDLL) $(GISTDLL) $(FLATZINCDLL)
export ALLLIB = \
	$(SUPPORTLIB) $(KERNELLIB) $(SEARCHLIB) $(INTLIB) $(FLOATLIB) $(SETLIB) \
	$(MMLIB) $(DRIVERLIB) $(GISTLIB) $(FLATZINCLIB)
else
DLLTARGETS=
ifeq "" "yes"
export ALLLIB = gecode.framework/Versions/51/gecode
else
export ALLLIB = \
	$(SUPPORTSTATICLIB) $(KERNELSTATICLIB) $(SEARCHSTATICLIB) \
	$(INTSTATICLIB) $(FLOATSTATICLIB) $(SETSTATICLIB) $(MMSTATICLIB) \
	$(DRIVERSTATICLIB) $(GISTSTATICLIB) $(FLATZINCSTATICLIB)
endif
endif

ifeq "no" "yes"
STATICTARGETS = \
	$(SUPPORTSTATICLIB) $(KERNELSTATICLIB) $(SEARCHSTATICLIB) \
	$(INTSTATICLIB) $(FLOATSTATICLIB) $(SETSTATICLIB) $(MMSTATICLIB) \
	$(DRIVERSTATICLIB) $(GISTSTATICLIB) $(FLATZINCSTATICLIB)
else
STATICTARGETS=
endif

export LIBTARGETS = $(DLLTARGETS) $(STATICTARGETS)

ifeq "yes" "yes"
export LIBLINKTARGETS = \
	$(DLLTARGETS:%$(DLLSUFFIX)=%$(SOSUFFIX)) \
	$(DLLTARGETS:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
else
export LIBLINKTARGETS =
endif

PDBS = $(ALLLIB:%$(LIBSUFFIX)=%$(PDBSUFFIX))

ifeq "no" "yes"
LIBLIBTARGETS = $(ALLLIB)
PDBTARGETS = $(PDBS)
else
LIBLIBTARGETS =
PDBTARGETS =
endif

EXETARGETS = $(FLATZINCEXE) tools/flatzinc/mzn-gecode

#
# Testing
#

INTTESTSRC00 = \
	channel arithmetic basic bool count dom distinct element extensional \
	rel linear gcc sorted unshare exec sequence \
	mm-arithmetic mm-bool mm-lin mm-count mm-rel \
	bin-packing order unary cumulative cumulatives circuit \
	no-overlap precede nvalues member
INTTESTSRC0 = test/int.cpp $(INTTESTSRC00:%=test/int/%.cpp)
INTTESTOBJ = $(INTTESTSRC0:%.cpp=%$(OBJSUFFIX))

FLOATTESTSRC00 = \
	basic arithmetic transcendental trigonometric linear rel channel \
	mm-lin dom
FLOATTESTSRC0 = test/float.cpp $(FLOATTESTSRC00:%=test/float/%.cpp)

ifeq "yes" "yes"
FLOATTESTOBJ = $(FLOATTESTSRC0:%.cpp=%$(OBJSUFFIX))
else
FLOATTESTOBJ=
endif

SETTESTSRC00 = \
	construct dom rel rel-op rel-op-const convex sequence int element \
	precede distinct exec mm-set channel
SETTESTSRC0 = test/set.cpp $(SETTESTSRC00:%=test/set/%.cpp)

ifeq "yes" "yes"
SETTESTOBJ = $(SETTESTSRC0:%.cpp=%$(OBJSUFFIX))
else
SETTESTOBJ=
endif

FLATZINCTESTSRC0 = \
  test/flatzinc.cpp \
  test/flatzinc/bool_clause.cpp \
  test/flatzinc/bug232.cpp \
  test/flatzinc/bug319.cpp \
  test/flatzinc/bugfix_r6746.cpp \
  test/flatzinc/bugfix_r7854.cpp \
  test/flatzinc/empty_domain_1.cpp \
  test/flatzinc/empty_domain_2.cpp \
  test/flatzinc/int_set_as_type1.cpp \
  test/flatzinc/int_set_as_type2.cpp \
  test/flatzinc/jobshop.cpp \
  test/flatzinc/no_warn_empty_domain.cpp \
  test/flatzinc/output_test.cpp \
  test/flatzinc/queens4.cpp \
  test/flatzinc/sat_arith1.cpp \
  test/flatzinc/sat_array_bool_and.cpp \
  test/flatzinc/sat_array_bool_or.cpp \
  test/flatzinc/sat_cmp_reif.cpp \
  test/flatzinc/sat_eq_reif.cpp \
  test/flatzinc/test_approx_bnb.cpp \
  test/flatzinc/test_array_just_right.cpp \
  test/flatzinc/test_assigned_var_bounds_bad.cpp \
  test/flatzinc/test_flatzinc_output_anns.cpp \
  test/flatzinc/test_fzn_arith.cpp \
  test/flatzinc/test_fzn_arrays.cpp \
  test/flatzinc/test_fzn_coercions.cpp \
  test/flatzinc/test_fzn_comparison.cpp \
  test/flatzinc/test_fzn_logic.cpp \
  test/flatzinc/test_fzn_sets.cpp \
  test/flatzinc/test_int_div.cpp \
  test/flatzinc/test_int_mod.cpp \
  test/flatzinc/test_int_ranges_as_values.cpp \
  test/flatzinc/test_seq_search.cpp \
  test/flatzinc/2dpacking.cpp \
  test/flatzinc/alpha.cpp \
  test/flatzinc/battleships1.cpp \
  test/flatzinc/battleships10.cpp \
  test/flatzinc/battleships2.cpp \
  test/flatzinc/battleships3.cpp \
  test/flatzinc/battleships4.cpp \
  test/flatzinc/battleships5.cpp \
  test/flatzinc/battleships7.cpp \
  test/flatzinc/battleships9.cpp \
  test/flatzinc/blocksworld_instance_1.cpp \
  test/flatzinc/blocksworld_instance_2.cpp \
  test/flatzinc/cumulatives.cpp \
  test/flatzinc/cumulatives_full_1.cpp \
  test/flatzinc/cumulatives_full_2.cpp \
  test/flatzinc/cumulatives_full_3.cpp \
  test/flatzinc/cumulatives_full_4.cpp \
  test/flatzinc/cumulatives_full_5.cpp \
  test/flatzinc/cumulatives_full_6.cpp \
  test/flatzinc/cumulatives_full_7.cpp \
  test/flatzinc/cumulatives_full_8.cpp \
  test/flatzinc/cutstock.cpp \
  test/flatzinc/eq20.cpp \
  test/flatzinc/factory_planning_instance.cpp \
  test/flatzinc/golomb.cpp \
  test/flatzinc/jobshop2x2.cpp \
  test/flatzinc/knights.cpp \
  test/flatzinc/langford2.cpp \
  test/flatzinc/latin_squares_fd.cpp \
  test/flatzinc/multidim_knapsack_simple.cpp \
  test/flatzinc/packing.cpp \
  test/flatzinc/perfsq.cpp \
  test/flatzinc/perfsq2.cpp \
  test/flatzinc/photo.cpp \
  test/flatzinc/product_fd.cpp \
  test/flatzinc/product_lp.cpp \
  test/flatzinc/quasigroup_qg5.cpp \
  test/flatzinc/queen_cp2.cpp \
  test/flatzinc/queen_ip.cpp \
  test/flatzinc/radiation.cpp \
  test/flatzinc/shared_array_element.cpp \
  test/flatzinc/simple_sat.cpp \
  test/flatzinc/singHoist2.cpp \
  test/flatzinc/sudoku.cpp \
  test/flatzinc/template_design.cpp \
  test/flatzinc/tenpenki_1.cpp \
  test/flatzinc/tenpenki_2.cpp \
  test/flatzinc/tenpenki_3.cpp \
  test/flatzinc/tenpenki_4.cpp \
  test/flatzinc/tenpenki_5.cpp \
  test/flatzinc/tenpenki_6.cpp \
  test/flatzinc/warehouses.cpp \
  test/flatzinc/warehouses_small.cpp \
  test/flatzinc/wolf_goat_cabbage.cpp \
  test/flatzinc/zebra.cpp \
  test/flatzinc/magicsq_3.cpp \
  test/flatzinc/magicsq_4.cpp \
  test/flatzinc/magicsq_5.cpp \
  test/flatzinc/oss.cpp \
  test/flatzinc/steiner_triples.cpp \
  test/flatzinc/subtyping.cpp \
  test/flatzinc/timetabling.cpp \
  test/flatzinc/trucking.cpp \
	test/flatzinc/on_restart_complete.cpp \
	test/flatzinc/on_restart_last_val_bool.cpp \
	test/flatzinc/on_restart_last_val_float.cpp \
	test/flatzinc/on_restart_last_val_int.cpp \
	test/flatzinc/on_restart_last_val_set.cpp \
	test/flatzinc/on_restart_sol_bool.cpp \
	test/flatzinc/on_restart_sol_float.cpp \
	test/flatzinc/on_restart_sol_int.cpp \
	test/flatzinc/on_restart_sol_set.cpp

ifeq "yes" "yes"
FLATZINCTESTOBJ = $(FLATZINCTESTSRC0:%.cpp=%$(OBJSUFFIX))
else
FLATZINCTESTOBJ=
endif

BRANCHTESTSRC0 = \
	test/branch.cpp test/branch/int.cpp test/branch/bool.cpp \
	test/branch/set.cpp test/branch/float.cpp \
	test/assign.cpp test/assign/int.cpp test/assign/bool.cpp \
	test/assign/set.cpp test/assign/float.cpp

SEARCHTESTSRC0 = \
	test/search.cpp test/nogoods.cpp

ARRAYTESTSRC0 = \
	test/array.cpp

TESTSRC0 = test/test.cpp test/afc.cpp test/ldsb.cpp test/region.cpp \
	test/groups.cpp

TESTSRC = \
	$(TESTSRC0) $(INTTESTSRC0) $(SETTESTSRC0) $(FLOATTESTSRC0) \
	$(BRANCHTESTSRC0) $(SEARCHTESTSRC0) \
	$(ARRAYTESTSRC0) $(FLATZINCTESTSRC0)

TESTHDR0 = \
	test.hh test.hpp int.hh int.hpp set.hh set.hpp float.hh float.hpp \
	branch.hh assign.hh flatzinc.hh
TESTHDR = $(TESTHDR0:%=test/%)
TESTOBJ	= $(INTTESTOBJ) $(SETTESTOBJ) $(FLOATTESTOBJ) \
	$(FLATZINCTESTOBJ:%.cpp=%$(OBJSUFFIX)) \
	$(BRANCHTESTSRC0:%.cpp=%$(OBJSUFFIX)) \
	$(SEARCHTESTSRC0:%.cpp=%$(OBJSUFFIX)) \
	$(ARRAYTESTSRC0:%.cpp=%$(OBJSUFFIX)) \
	$(TESTSRC0:%.cpp=%$(OBJSUFFIX))
TESTSBJ	= $(TESTOBJ:%$(OBJSUFFIX)=%$(SBJSUFFIX))
TESTEXE	= test/test$(EXESUFFIX)
TESTBUILDDIRS = \
	test test/int test/set test/float \
	test/branch test/assign \
	test/flatzinc

BUILDDIRS = \
	tools \
	$(SUPPORTBUILDDIRS:%=gecode/%) \
	$(KERNELBUILDDIRS:%=gecode/%) $(SEARCHBUILDDIRS:%=gecode/%) \
	$(INTBUILDDIRS:%=gecode/%) $(FLOATBUILDDIRS:%=gecode/%) $(SETBUILDDIRS:%=gecode/%) \
	$(MMBUILDDIRS:%=gecode/%)  \
	$(DRIVERBUILDDIRS:%=gecode/%)  \
	$(GISTBUILDDIRS:%=gecode/%) \
	$(FLATZINCBUILDDIRS) \
	$(EXAMPLEBUILDDIRS) $(TESTBUILDDIRS)

ifeq "yes" "yes"
all: compilelib
	@$(MAKE) compileexamples
	@$(MAKE) makecompletedmessage
else
all: compilelib
	@$(MAKE) makecompletedmessage
endif
compilelib: mkcompiledirs
	@$(MAKE) $(VARIMP) $(LIBTARGETS)
	@$(MAKE) compilesubdirs
	@$(MAKE) framework
	@$(MAKE) flatzinc

compileexamples: $(EXAMPLEEXE)

test: mkcompiledirs
	@$(MAKE) $(VARIMP) $(TESTEXE)

# A basic integrity test
check: test
	$(RUNENVIRONMENT) $(TESTEXE) -iter 2 -threads 0 -test Branch::Int::Dense::3 \
			   -test FlatZinc::magic_square \
			   -test Int::Arithmetic::Abs \
			   -test Int::Arithmetic::ArgMax \
			   -test Int::Arithmetic::Max::Nary \
			   -test Int::Cumulative::Man::Fix::0::4 \
			   -test Int::Distinct::Random \
			   -test Int::Linear::Bool::Int::Lq \
			   -test Int::MiniModel::LinExpr::Bool::352  \
			   -test NoGoods::Queens \
			   -test Search::DFS::Sol::Binary::Nary::Binary::1::1::1 \
			   -test Set::Dom::Dom::Gr \
			   -test Set::RelOp::ConstSSI::Union \
			   -test Set::Sequence::SeqU1 \
			   -test Set::Wait

ifeq "." "."
mkcompiledirs:
else
mkcompiledirs:
	for_builddirs="$(BUILDDIRS)"; for f in $$for_builddirs; do \
	  (test -z "$(top_builddir)"/$$f || mkdir -p "$(top_builddir)"/$$f); \
	done
endif

makecompletedmessage:
	@echo
	@echo Compilation of Gecode finished successfully. To use Gecode, either add
	@echo $(PWD)
	@echo to your search path for libraries, or install Gecode using
	@echo make install
	@echo

# ugly hack by Gr�goire Dooms to call a target on a contribs after evaluating all the variables in this Makefile.
# used as make contribs:cpgraph:doc or make contribs:cpgraph:Makefile.in
.PHONY: contribs\:%
contribs\:%:
	$(MAKE) -C $(shell echo $@ | sed 's/\(contribs:[^:]*\):.*/\1/;s+:+/+') $(shell echo $@ | sed 's/contribs:[^:]*:\(.*\)/\1/;s+:+/+')

# less ugly hack by Guido Tack to call a target
# Just give ICD (in-contrib-dir) and ICT (in-contrib-target) as arguments
# to make incontrib
ICT=
ICD=
.PHONY: incontrib
incontrib:
	$(MAKE) -C contribs/$(ICD) $(ICT)

compilesubdirs:
	@for_subdirs="$(subdirs)"; for i in $$for_subdirs; do \
	  if test -f $$i/Makefile; then \
	    echo "Making in directory $$i"; \
	    $(MAKE) -C $$i; \
	  fi; \
	done

#
# Generated variable implementations
#

VIS = $(top_srcdir)/gecode/int/var-imp/int.vis $(top_srcdir)/gecode/int/var-imp/bool.vis $(top_srcdir)/gecode/set/var-imp/set.vis $(top_srcdir)/gecode/float/var-imp/float.vis
VISDEP = $(VIS) \
	$(top_srcdir)/misc/genvarimp.perl Makefile

gecode/kernel/var-type.hpp: $(VISDEP)
	perl $(top_srcdir)/misc/genvarimp.perl -typehpp $(VIS) > $@
gecode/kernel/var-imp.hpp: $(VISDEP)
	perl $(top_srcdir)/misc/genvarimp.perl -header $(VIS) > $@

#
# Object targets
#

gecode/support/%$(OBJSUFFIX): $(top_srcdir)/gecode/support/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-c -o $@  $<
gecode/support/%$(SBJSUFFIX): $(top_srcdir)/gecode/support/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-S -o $@  $<
gecode/support/%$(OBJSUFFIX): $(top_srcdir)/gecode/support/%.c
	$(CC) $(CFLAGS)  \
	-c -o $@  $<
gecode/support/%$(SBJSUFFIX): $(top_srcdir)/gecode/support/%.c
	$(CC) $(CFLAGS)  \
	-S -o $@  $<

gecode/kernel/%$(OBJSUFFIX): $(top_srcdir)/gecode/kernel/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-c -o $@  $<
gecode/kernel/%$(SBJSUFFIX): $(top_srcdir)/gecode/kernel/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-S -o $@  $<

gecode/search/%$(OBJSUFFIX): $(top_srcdir)/gecode/search/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-c -o $@  $<
gecode/search/%$(SBJSUFFIX): $(top_srcdir)/gecode/search/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-S -o $@  $<

gecode/int/%$(OBJSUFFIX): $(top_srcdir)/gecode/int/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-c -o $@  $<
gecode/int/%$(SBJSUFFIX): $(top_srcdir)/gecode/int/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-S -o $@  $<

gecode/float/%$(OBJSUFFIX): $(top_srcdir)/gecode/float/%.cpp
	$(CXX) $(CXXFLAGS) $(MPFR_CPPFLAGS)  \
	-c -o $@  $<
gecode/float/%$(SBJSUFFIX): $(top_srcdir)/gecode/float/%.cpp
	$(CXX) $(CXXFLAGS) $(MPFR_CPPFLAGS)  \
	-S -o $@  $<

gecode/set/%$(OBJSUFFIX): $(top_srcdir)/gecode/set/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-c -o $@  $<
gecode/set/%$(SBJSUFFIX): $(top_srcdir)/gecode/set/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-S -o $@  $<

gecode/minimodel/%$(OBJSUFFIX): $(top_srcdir)/gecode/minimodel/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-c -o $@  $<
gecode/minimodel/%$(SBJSUFFIX): $(top_srcdir)/gecode/minimodel/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-S -o $@  $<

gecode/driver/%$(OBJSUFFIX): $(top_srcdir)/gecode/driver/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-c -o $@  $<
gecode/driver/%$(SBJSUFFIX): $(top_srcdir)/gecode/driver/%.cpp
	$(CXX) $(CXXFLAGS)  \
	-S -o $@  $<


examples/%$(OBJSUFFIX): $(top_srcdir)/examples/%.cpp
	$(CXX) $(CXXFLAGS) $(EXAMPLES_EXTRA_CXXFLAGS) $(QT_CPPFLAGS) \
	-c -o $@  $<
examples/%$(SBJSUFFIX): $(top_srcdir)/examples/%.cpp
	$(CXX) $(CXXFLAGS) $(EXAMPLES_EXTRA_CXXFLAGS) $(QT_CPPFLAGS) \
	-S -o $@  $<


test/%$(OBJSUFFIX): $(top_srcdir)/test/%.cpp
	$(CXX) $(CXXFLAGS) $(EXAMPLES_EXTRA_CXXFLAGS) \
		-c -o $@  $<
test/%$(SBJSUFFIX): $(top_srcdir)/test/%.cpp
	$(CXX) $(CXXFLAGS) $(EXAMPLES_EXTRA_CXXFLAGS) \
		-S -o $@  $<

tools/%$(OBJSUFFIX): $(top_srcdir)/tools/%.cpp
	$(CXX) $(CXXFLAGS) $(EXAMPLES_EXTRA_CXXFLAGS) \
		-c -o $@  $<
tools/%$(SBJSUFFIX): $(top_srcdir)/tools/%.cpp
	$(CXX) $(CXXFLAGS) $(EXAMPLES_EXTRA_CXXFLAGS) \
		-S -o $@  $<

$(GISTSRC:%.cpp=%$(OBJSUFFIX)): gecode/gist/%$(OBJSUFFIX): $(top_srcdir)/gecode/gist/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CPPFLAGS) \
	 \
	-c -o $@  $<

$(GISTSRC:%.cpp=%$(SBJSUFFIX)): gecode/gist/%$(SBJSUFFIX): $(top_srcdir)/gecode/gist/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CPPFLAGS) \
	 \
	-S -o $@  $<

$(GISTMOCSRC:%.cpp=%$(OBJSUFFIX)): gecode/gist/moc_%$(OBJSUFFIX): gecode/gist/moc_%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CPPFLAGS) \
	 \
	-c -o $@  $<

$(GISTMOCSRC:%.cpp=%$(SBJSUFFIX)): gecode/gist/moc_%$(SBJSUFFIX): gecode/gist/moc_%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CPPFLAGS) \
	 \
	-S -o $@  $<

.PRECIOUS: gecode/gist/moc_%.cpp
gecode/gist/moc_%.cpp: $(top_srcdir)/gecode/gist/%.hh
	$(MOC) $(CPPFLAGS) -pgecode/gist $< -o $@

.PRECIOUS: gecode/flatzinc/%.cpp gecode/flatzinc/%.hh
ifeq "no" "yes"
gecode/flatzinc/lexer.yy.cpp: \
	$(top_srcdir)/gecode/flatzinc/lexer.lxx \
	$(top_srcdir)/gecode/flatzinc/parser.tab.hpp
	flex -ogecode/flatzinc/lexer.yy.cpp \
		$(top_srcdir)/gecode/flatzinc/lexer.lxx

gecode/flatzinc/parser.tab.hpp gecode/flatzinc/parser.tab.cpp: \
	$(top_srcdir)/gecode/flatzinc/parser.yxx
	bison -t -o gecode/flatzinc/parser.tab.cpp -d $<
else
ifneq "$(top_srcdir)" "."
gecode/flatzinc/lexer.yy.cpp: $(top_srcdir)/gecode/flatzinc/lexer.yy.cpp
	cp $(@:%=$(top_srcdir)/%) $@
gecode/flatzinc/parser.tab.hpp: $(top_srcdir)/gecode/flatzinc/parser.tab.hpp
	cp $(@:%=$(top_srcdir)/%) $@
gecode/flatzinc/parser.tab.cpp: $(top_srcdir)/gecode/flatzinc/parser.tab.cpp
	cp $(@:%=$(top_srcdir)/%) $@
endif
endif

$(FLATZINCSRC:%.cpp=%$(OBJSUFFIX)): gecode/flatzinc/%$(OBJSUFFIX): $(top_srcdir)/gecode/flatzinc/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CPPFLAGS) \
	 \
	-c -o $@  $<

$(FLATZINCSRC:%.cpp=%$(SBJSUFFIX)): gecode/flatzinc/%$(SBJSUFFIX): $(top_srcdir)/gecode/flatzinc/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CPPFLAGS) \
	 \
	-S -o $@  $<

$(FLATZINC_GENSRC:%.cpp=%$(OBJSUFFIX)): gecode/flatzinc/%$(OBJSUFFIX): gecode/flatzinc/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CPPFLAGS) \
	 \
	-c -o $@  $<

$(FLATZINC_GENSRC:%.cpp=%$(SBJSUFFIX)): gecode/flatzinc/%$(SBJSUFFIX): gecode/flatzinc/%.cpp
	$(CXX) $(CXXFLAGS) $(QT_CPPFLAGS) \
	 \
	-S -o $@  $<

#
# DLL Targets
#

ifeq "$(DLLSUFFIX)" "$(LIBSUFFIX)"

ifeq "" "yes"
export LINKALL = -F. -framework gecode
else
export LINKALL = \
	$(LINKFLATZINC) $(LINKDRIVER) $(LINKGIST) \
	$(LINKSEARCH) $(LINKMM) $(LINKSET) $(LINKFLOAT) $(LINKMPFR) $(LINKINT) \
	$(LINKKERNEL) $(LINKSUPPORT)
endif

$(SUPPORTDLL): $(SUPPORTOBJ)
	$(CXX) $(DLLFLAGS) $(SUPPORTOBJ) $(SUPPORTSONAME) \
		-o $(SUPPORTDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
$(KERNELDLL): $(KERNELOBJ) $(SUPPORTDLL)
	$(CXX) $(DLLFLAGS) $(KERNELOBJ) $(KERNELSONAME) \
		-L. $(LINKSUPPORT) \
		-o $(KERNELDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
$(SEARCHDLL): $(SEARCHOBJ) $(SUPPORTDLL) $(KERNELDLL)
	$(CXX) $(DLLFLAGS) $(SEARCHOBJ) $(SEARCHSONAME) \
		-L. $(LINKSUPPORT) $(LINKKERNEL) \
		-o $(SEARCHDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
$(INTDLL): $(INTOBJ) $(SUPPORTDLL) $(KERNELDLL)
	$(CXX) $(DLLFLAGS) $(INTOBJ) $(INTSONAME) \
		-L. $(LINKSUPPORT) $(LINKKERNEL) \
		-o $(INTDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
$(FLOATDLL): $(FLOATOBJ) $(SUPPORTDLL) $(KERNELDLL) $(INTDLL)
	$(CXX) $(DLLFLAGS) $(FLOATOBJ) $(FLOATSONAME) \
		-L. $(LINKSUPPORT) $(LINKKERNEL) $(LINKINT) $(LINKMPFR)\
		-o $(FLOATDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
$(SETDLL): $(SETOBJ) $(SUPPORTDLL) $(KERNELDLL) $(INTDLL)
	$(CXX) $(DLLFLAGS) $(SETOBJ) $(SETSONAME) \
		-L. $(LINKSUPPORT) $(LINKKERNEL) $(LINKINT) \
		-o $(SETDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
$(MMDLL): $(MMOBJ) $(SUPPORTDLL) $(KERNELDLL) $(INTDLL) $(SETDLL) $(FLOATDLL)
	$(CXX) $(DLLFLAGS) $(MMOBJ) $(MMSONAME) \
		-L. $(LINKSUPPORT) $(LINKKERNEL) $(LINKINT) $(LINKFLOAT) $(LINKSET) \
		-o $(MMDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
$(DRIVERDLL): $(DRIVEROBJ) $(SUPPORTDLL) $(KERNELDLL) $(SEARCHDLL) $(INTDLL) $(MMDLL) $(GISTDLL)
	$(CXX) $(DLLFLAGS) $(DRIVEROBJ) $(DRIVERSONAME) \
		-L. $(LINKSUPPORT) $(LINKKERNEL) $(LINKSEARCH) $(LINKINT) $(LINKMM) $(LINKGIST) \
		-o $(DRIVERDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
$(GISTDLL): $(GISTOBJ) $(SUPPORTDLL) $(KERNELDLL) $(SEARCHDLL) $(INTDLL)
	$(CXX) $(DLLFLAGS) $(GISTOBJ) $(GISTSONAME) \
		-L. $(LINKSUPPORT) $(LINKKERNEL) $(LINKSEARCH) $(LINKINT) $(LINKQT) \
		-o $(GISTDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
$(FLATZINCDLL): $(FLATZINCOBJ) $(SUPPORTDLL) $(KERNELDLL) $(SEARCHDLL) \
	$(INTDLL) $(SETDLL) $(FLOATDLL) $(GISTDLL) $(MMDLL) $(DRIVERDLL)
	$(CXX) $(DLLFLAGS) $(FLATZINCOBJ) $(FLATZINCSONAME) \
		-L. $(LINKSUPPORT) $(LINKKERNEL) $(LINKSEARCH) $(LINKINT) \
		$(LINKSET) $(LINKFLOAT) $(LINKMM) $(LINKGIST) $(LINKDRIVER) $(LINKQT) \
		-o $(FLATZINCDLL)
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOLINKSUFFIX))
	$(CREATELINK) $@ $(@:%$(DLLSUFFIX)=%$(SOSUFFIX))
else
export LINKALL =

ifeq "no" "yes"
$(SUPPORTRC):
	$(RCGEN) $(SUPPORTDLL) $(SUPPORTHDR) $(SUPPORTSRC) > $@
endif
$(SUPPORTDLL) $(SUPPORTLIB): $(SUPPORTOBJ) $(SUPPORTRES)
	$(CXX) $(DLLFLAGS) $(SUPPORTOBJ) $(SUPPORTRES) \
		-o $(SUPPORTDLL) $(GLDFLAGS)
	$(FIXMANIFEST) $(SUPPORTDLL).manifest
	$(MANIFEST) -manifest $(SUPPORTDLL).manifest \
		    -outputresource:$(SUPPORTDLL)\;2
ifeq "no" "yes"
$(KERNELRC):
	$(RCGEN) $(KERNELDLL) $(KERNELHDR) $(VARIMPHDR) $(KERNELSRC) > $@
endif
$(KERNELDLL) $(KERNELLIB): $(KERNELOBJ) $(KERNELRES) $(SUPPORTDLL)
	$(CXX) $(DLLFLAGS) $(KERNELOBJ) $(KERNELRES) \
		-L. -o $(KERNELDLL) $(GLDFLAGS)
	$(FIXMANIFEST) $(KERNELDLL).manifest
	$(MANIFEST) -manifest $(KERNELDLL).manifest \
		    -outputresource:$(KERNELDLL)\;2
ifeq "no" "yes"
$(SEARCHRC):
	$(RCGEN) $(SEARCHDLL) $(SEARCHHDR) $(SEARCHSRC) > $@
endif
$(SEARCHDLL) $(SEARCHLIB): $(SEARCHOBJ) $(SEARCHRES) $(SUPPORTDLL) $(KERNELDLL)
	$(CXX) $(DLLFLAGS) $(SEARCHOBJ) $(SEARCHRES) \
		-L. -o $(SEARCHDLL) $(GLDFLAGS)
	$(FIXMANIFEST) $(SEARCHDLL).manifest
	$(MANIFEST) -manifest $(SEARCHDLL).manifest \
		    -outputresource:$(SEARCHDLL)\;2
ifeq "no" "yes"
$(INTRC):
	$(RCGEN) $(INTDLL) $(INTHDR) $(INTSRC) > $@
endif
$(INTDLL) $(INTLIB): $(INTOBJ) $(INTRES) $(SUPPORTDLL) $(KERNELDLL)
	$(CXX) $(DLLFLAGS) $(INTOBJ) $(INTRES) \
		-L. -o $(INTDLL) $(GLDFLAGS)
	$(FIXMANIFEST) $(INTDLL).manifest
	$(MANIFEST) -manifest $(INTDLL).manifest \
		    -outputresource:$(INTDLL)\;2
ifeq "no" "yes"
$(FLOATRC):
	$(RCGEN) $(FLOATDLL) $(FLOATHDR) $(FLOATSRC) > $@
endif
$(FLOATDLL) $(FLOATLIB): $(FLOATOBJ) $(FLOATRES) $(SUPPORTDLL) $(KERNELDLL) $(INTDLL)
	$(CXX) $(DLLFLAGS) $(FLOATOBJ) $(FLOATRES) \
		-L. -o $(FLOATDLL) $(GLDFLAGS) $(LINKMPFR)
	$(FIXMANIFEST) $(FLOATDLL).manifest
	$(MANIFEST) -manifest $(FLOATDLL).manifest \
		    -outputresource:$(FLOATDLL)\;2
ifeq "no" "yes"
$(SETRC):
	$(RCGEN) $(SETDLL) $(SETHDR) $(SETSRC) > $@
endif
$(SETDLL) $(SETLIB): $(SETOBJ) $(SETRES) $(SUPPORTDLL) $(KERNELDLL) $(INTDLL)
	$(CXX) $(DLLFLAGS) $(SETOBJ) $(SETRES) \
		-L. -o $(SETDLL) $(GLDFLAGS)
	$(FIXMANIFEST) $(SETDLL).manifest
	$(MANIFEST) -manifest $(SETDLL).manifest \
		    -outputresource:$(SETDLL)\;2
ifeq "no" "yes"
$(MMRC):
	$(RCGEN) $(MMDLL) $(MMHDR) $(MMSRC) > $@
endif
$(MMDLL) $(MMLIB): $(MMOBJ) $(MMRES) \
	$(SUPPORTDLL) $(KERNELDLL) $(INTDLL) $(FLOATDLL) $(SETDLL)
	$(CXX) $(DLLFLAGS) $(MMOBJ) $(MMRES) \
		-L. -o $(MMDLL) $(GLDFLAGS)
	$(FIXMANIFEST) $(MMDLL).manifest
	$(MANIFEST) -manifest $(MMDLL).manifest \
		    -outputresource:$(MMDLL)\;2
ifeq "no" "yes"
$(DRIVERRC):
	$(RCGEN) $(DRIVERDLL) $(DRIVERHDR) $(DRIVERSRC) > $@
endif
$(DRIVERDLL) $(DRIVERLIB): $(DRIVEROBJ) $(DRIVERRES) \
	$(SUPPORTDLL) $(KERNELDLL) \
	$(SEARCHDLL) $(INTDLL) $(MMDLL) $(GISTDLL)
	$(CXX) $(DLLFLAGS) $(DRIVEROBJ) $(DRIVERRES) \
		-L. -o $(DRIVERDLL) $(GLDFLAGS)
	$(FIXMANIFEST) $(DRIVERDLL).manifest
	$(MANIFEST) -manifest $(DRIVERDLL).manifest \
		    -outputresource:$(DRIVERDLL)\;2
ifeq "no" "yes"
$(GISTRC):
	$(RCGEN) $(GISTDLL) $(GISTHDR) $(GISTSRC) > $@
endif
$(GISTDLL) $(GISTLIB): $(GISTOBJ) $(GISTRES) \
	$(SUPPORTDLL) $(KERNELDLL) $(SEARCHDLL) $(INTDLL)
	$(CXX) $(DLLFLAGS) $(GISTOBJ) $(GISTRES) \
		-L. -o $(GISTDLL) $(GLDFLAGS) $(LINKQT)
	$(FIXMANIFEST) $(GISTDLL).manifest
	$(MANIFEST) -manifest $(GISTDLL).manifest \
		    -outputresource:$(GISTDLL)\;2
ifeq "no" "yes"
$(FLATZINCRC):
	$(RCGEN) $(FLATZINCDLL) $(FLATZINCHDR) $(FLATZINCSRC) > $@
endif
$(FLATZINCDLL) $(FLATZINCLIB): $(FLATZINCOBJ) $(FLATZINCRES) \
	$(SUPPORTDLL) $(KERNELDLL) $(SEARCHDLL) $(INTDLL) \
	$(SETDLL) $(FLOATDLL) $(GISTDLL) $(MMDLL) $(DRIVERDLL)
	$(CXX) $(DLLFLAGS) $(FLATZINCOBJ) $(FLATZINCRES) \
		-L. -o $(FLATZINCDLL) $(GLDFLAGS) $(LINKQT)
	$(FIXMANIFEST) $(FLATZINCDLL).manifest
	$(MANIFEST) -manifest $(FLATZINCDLL).manifest \
		    -outputresource:$(FLATZINCDLL)\;2
endif

#
# Static libraries
#

$(SUPPORTSTATICLIB): $(SUPPORTOBJ)
	$(AR) $(ARFLAGS) $@ $(SUPPORTOBJ)
	$(RANLIB) $@
$(KERNELSTATICLIB): $(KERNELOBJ)
	$(AR) $(ARFLAGS) $@ $(KERNELOBJ)
	$(RANLIB) $@
$(SEARCHSTATICLIB): $(SEARCHOBJ)
	$(AR) $(ARFLAGS) $@ $(SEARCHOBJ)
	$(RANLIB) $@
$(INTSTATICLIB): $(INTOBJ)
	$(AR) $(ARFLAGS) $@ $(INTOBJ)
	$(RANLIB) $@
$(FLOATSTATICLIB): $(FLOATOBJ)
	$(AR) $(ARFLAGS) $@ $(FLOATOBJ)
	$(RANLIB) $@
$(SETSTATICLIB): $(SETOBJ)
	$(AR) $(ARFLAGS) $@ $(SETOBJ)
	$(RANLIB) $@
$(MMSTATICLIB): $(MMOBJ)
	$(AR) $(ARFLAGS) $@ $(MMOBJ)
	$(RANLIB) $@
$(DRIVERSTATICLIB): $(DRIVEROBJ)
	$(AR) $(ARFLAGS) $@ $(DRIVEROBJ)
	$(RANLIB) $@
$(GISTSTATICLIB): $(GISTOBJ)
	$(AR) $(ARFLAGS) $@ $(GISTOBJ)
	$(RANLIB) $@
$(FLATZINCSTATICLIB): $(FLATZINCOBJ)
	$(AR) $(ARFLAGS) $@ $(FLATZINCOBJ)
	$(RANLIB) $@

#
# Mac OS X Framework
#

.PHONY: framework
ifeq "" "yes"
framework: gecode.framework/Versions/51/gecode

gecode.framework/Versions/51/gecode: $(STATICTARGETS)
	$(RMF) gecode.framework
	mkdir -p gecode.framework/Versions/51
	$(CXX) -all_load -compatibility_version 51.0 \
	  -current_version 51.0 \
	  $(DLLFLAGS) $(LINKQT) \
	  -o gecode.framework/Versions/51/gecode $^
	$(MAKE) doinstallheaders \
	  prefix=gecode.framework/Versions/51 DESTDIR=
	mv gecode.framework/Versions/51/include/gecode/* \
	  gecode.framework/Versions/51/include/
	rmdir gecode.framework/Versions/51/include/gecode
	cd gecode.framework/Versions && ln -s 51 Current
	ln -s Versions/Current/include \
	  gecode.framework/Headers
	ln -s Versions/Current/gecode \
	  gecode.framework/gecode
else
framework:
endif

#
# EXE Targets
#

.PRECIOUS: examples/%$(OBJSUFFIX)
ifeq "no" "yes"
.PRECIOUS: examples/%$(EXESUFFIX).rc examples/%$(EXESUFFIX).res
examples/%$(EXESUFFIX).rc:
	$(RCGEN) $(@:%.rc=%) $< > $@
examples/%$(EXESUFFIX): examples/%$(OBJSUFFIX) examples/%$(EXESUFFIX).res \
	$(ALLLIB)
	$(CXX) -o $@ $< $@.res \
		$(DLLPATH) $(CXXFLAGS) \
		$(LINKALL) $(GLDFLAGS) $(LINKQT)
	$(FIXMANIFEST) $@.manifest
	$(MANIFEST) -manifest $@.manifest -outputresource:$@\;1
else
examples/%$(EXESUFFIX): examples/%$(OBJSUFFIX) $(ALLLIB)
	$(CXX) -o $@ $< $(DLLPATH) $(CXXFLAGS) \
	$(LINKALL) $(GLDFLAGS) $(LINKQT)
	$(FIXMANIFEST) $@.manifest
	$(MANIFEST) -manifest $@.manifest -outputresource:$@\;1
endif

ifeq "no" "yes"
TESTRES = $(TESTEXE).res
$(TESTEXE).rc:
	$(RCGEN) $(TESTEXE) $(TESTSRC) $(TESTHDR) > $@
else
TESTRES =
endif
$(TESTEXE): $(TESTOBJ) $(TESTRES) $(ALLLIB)
	$(CXX) -o $@ $(TESTOBJ) $(TESTRES) $(DLLPATH) $(CXXFLAGS) \
	$(LINKALL) $(GLDFLAGS) $(LINKQT)
	$(FIXMANIFEST) $@.manifest $(DLLSUFFIX)
	$(MANIFEST) -manifest $@.manifest -outputresource:$@\;1

.PHONY: flatzinc
ifeq "yes" "yes"
flatzinc: $(FLATZINCEXE)
else
flatzinc:
endif

ifeq "no" "yes"
FLATZINCEXERES = $(FLATZINCEXE).res
$(FLATZINCEXE).rc:
	$(RCGEN) $(FLATZINCEXE) $(FLATZINCEXESRC) > $@
else
FLATZINCEXERES =
endif
$(FLATZINCEXE): $(FLATZINCEXEOBJ) $(FLATZINCEXERES) $(ALLLIB)
	$(CXX) -o $@ $(FLATZINCEXEOBJ) $(FLATZINCEXERES) \
	$(DLLPATH) $(CXXFLAGS) \
	$(LINKALL) $(GLDFLAGS) $(LINKSTATICQT)
	$(FIXMANIFEST) $@.manifest
	$(MANIFEST) -manifest $@.manifest -outputresource:$@\;1


#
# Autoconf
#

$(top_srcdir)/configure: $(top_srcdir)/configure.ac
	(cd $(top_srcdir) && autoconf)
config.status: $(top_srcdir)/configure
	./config.status --recheck
Makefile: $(top_srcdir)/Makefile.in config.status
	./config.status --file $@:$<
doxygen.hh: $(top_srcdir)/doxygen/doxygen.hh.in config.status
	./config.status --file $@:$<
doxygen.conf: $(top_srcdir)/doxygen/doxygen.conf.in config.status
	./config.status --file $@:$<


#
# Documentation
#

export ENABLEDOCCHM	= no
export ENABLEDOCDOCSET	= no
ENABLEDOCSEARCH = no

.PHONY: doc

DOCSRC_NOTGENERATED = \
	misc/doxygen/back.png misc/doxygen/footer.html \
	misc/doxygen/gecode-logo-100.png \
	misc/doxygen/stylesheet.css \
	misc/genlicense.perl misc/genstatistics.perl \
	misc/genchangelog.perl
DOCSRC = $(DOCSRC_NOTGENERATED:%=$(top_srcdir)/%) \
	doxygen.conf.use header.html \
	doxygen.hh license.hh stat.hh changelog.hh

changelog.hh: $(top_srcdir)/changelog.in
	perl $(top_srcdir)/misc/genchangelog.perl < $^ > changelog.hh

ChangeLog: $(top_srcdir)/changelog.in
	perl $(top_srcdir)/misc/gentxtchangelog.perl < $^ > ChangeLog

license.hh: $(ALLGECODEHDR:%=$(top_srcdir)/%) $(ALLSRC:%=$(top_srcdir)/%) \
	$(VARIMPHDR)
	perl $(top_srcdir)/misc/genlicense.perl $^ > license.hh

stat.hh: $(ALLGECODEHDR:%=$(top_srcdir)/%) $(ALLSRC:%=$(top_srcdir)/%) \
	$(TESTHDR:%=$(top_srcdir)/%) $(TESTSRC:%=$(top_srcdir)/%) \
	$(VARIMPHDR)
	perl $(top_srcdir)/misc/genstatistics.perl $^ > stat.hh


ifeq "no" "yes"

DOCTARGET=GecodeReference.chm

header.html: $(top_srcdir)/misc/doxygen/header.html doxygen.conf
	grep -v '<form.*form>' < $< > $@
doxygen.conf.use: doxygen.conf
	(echo "GENERATE_HTMLHELP = YES"; \
	 echo "SEARCHENGINE = NO";\
	 echo "HAVE_DOT = NO") | \
		cat $< - > $@

doc: $(ALLGECODEHDR:%=$(top_srcdir)/%) $(VARIMPHDR) \
	$(ALLSRC:%=$(top_srcdir)/%) $(DOCSRC)
	mkdir -p doc/html
	cp -f $(top_srcdir)/misc/doxygen/back.png \
	$(top_srcdir)/misc/doxygen/gecode-logo-100.png doc/html
	doxygen doxygen.conf.use
	mv doc/html/GecodeReference.chm GecodeReference.chm

else
ifeq "no" "yes"

DOCTARGET=org.gecode.6.3.0.docset

header.html: $(top_srcdir)/misc/doxygen/header.html doxygen.conf
	grep -v '<form.*form>' < $< > $@
doxygen.conf.use: doxygen.conf
	(echo "SEARCHENGINE = NO";\
	 echo "HAVE_DOT = NO";\
	 echo "GENERATE_DOCSET = YES";\
	 echo "DOCSET_BUNDLE_ID = org.gecode.6.3.0";\
	 echo "DOCSET_FEEDNAME = Gecode") | \
		cat $< - > $@

doc: $(ALLGECODEHDR:%=$(top_srcdir)/%) $(VARIMPHDR) \
	$(ALLSRC:%=$(top_srcdir)/%) $(DOCSRC)
	mkdir -p doc/html
	cp -f $(top_srcdir)/misc/doxygen/back.png \
	$(top_srcdir)/misc/doxygen/gecode-logo-100.png doc/html
	doxygen doxygen.conf.use
	cd doc/html && make
	mv doc/html/$(DOCTARGET) .

else
DOCTARGET=doc/html

ifeq "no" "yes"

header.html: $(top_srcdir)/misc/doxygen/header.html doxygen.conf
	cat < $< > $@
doxygen.conf.use: doxygen.conf
	(echo "GENERATE_HTMLHELP = NO"; \
	 echo "SEARCHENGINE = YES"; \
	 echo "SERVER_BASED_SEARCH = YES"; \
	 echo "HAVE_DOT = NO") | \
		cat $< - > $@
else

header.html: $(top_srcdir)/misc/doxygen/header.html doxygen.conf
	grep -v '<form.*form>' < $< > $@
doxygen.conf.use: doxygen.conf
	(echo "GENERATE_HTMLHELP = NO"; \
	 echo "SEARCHENGINE = NO"; \
	 echo "HAVE_DOT = NO") | \
		cat $< - > $@

endif

doc: $(ALLGECODEHDR:%=$(top_srcdir)/%) $(VARIMPHDR) \
	$(ALLSRC:%=$(top_srcdir)/%) $(DOCSRC)
	mkdir -p doc/html
	cp -f $(top_srcdir)/misc/doxygen/back.png \
	$(top_srcdir)/misc/doxygen/gecode-logo-100.png doc/html
	doxygen doxygen.conf.use

ifeq "no" "yes"
	# Work around a doxygen bug (a missing div screws up the layout)
	mv doc/html/search.php doc/html/search2.php
	sed -e 's|<div class="qindex">|<div><div class="qindex">|' \
	  doc/html/search2.php > doc/html/search.php
	rm doc/html/search2.php
endif

endif
endif

#
# Installation
#

.PHONY: install

ifeq "" "yes"
install: installframework
else

ifeq "yes" "yes"
install: installlib installexamples
else
install: installlib
endif

endif

doinstallheaders: $(ALLHDR:%=$(top_srcdir)/%) $(EXTRA_HEADERS) $(VARIMPHDR) $(FLATZINCCONFIG)
	mkdir -p $(DESTDIR)$(includedir) && \
	(cd $(top_srcdir) && tar cf - $(ALLHDR)) | \
	  (cd $(DESTDIR)$(includedir) && tar xf -) && \
	for_varimpheaders="$(VARIMPHDR)" && \
	  for f in $$for_varimpheaders; do \
	    cp $$f $(DESTDIR)$(includedir)/gecode/kernel; done && \
	for_extraheaders="$(EXTRA_HEADERS)" && \
	  for f in $$for_extraheaders; do \
	    cp $$f $(DESTDIR)$(includedir)/$$f; done && \
	mkdir -p $(DESTDIR)$(datadir)/minizinc/solvers && \
	for_flatconf="$(FLATZINCCONFIG)" && \
	  for f in $$for_flatconf; do \
			cp $$f $(DESTDIR)$(datadir)/minizinc/solvers; done && \
	for_mznlib="$(FLATZINCMZNLIB)" && \
	  mkdir -p $(DESTDIR)$(datadir)/minizinc/gecode; \
	  for f in $$for_mznlib; do \
		cp $(top_srcdir)/$$f/*.mzn \
		  $(DESTDIR)$(datadir)/minizinc/gecode; done

doinstalllib:
	mkdir -p $(DESTDIR)$(sharedlibdir) && \
	mkdir -p $(DESTDIR)$(libdir) && \
	mkdir -p $(DESTDIR)$(bindir) && \
	for_libtargets="$(LIBTARGETS) $(PDBTARGETS)" && \
	  for f in $$for_libtargets; do \
	    cp $$f $(DESTDIR)$(sharedlibdir); done && \
	for_liblinktargets="$(LIBLINKTARGETS)" && \
	  for f in $$for_liblinktargets; do \
	    cp -pR $$f $(DESTDIR)$(sharedlibdir); done &&\
	for_liblibtargets="$(LIBLIBTARGETS)" && \
	  for f in $$for_liblibtargets; do \
	    cp $$f $(DESTDIR)$(libdir); done &&\
	for_exetargets="$(EXETARGETS)" && \
	  for f in $$for_exetargets; do \
	    cp $$f $(DESTDIR)$(bindir); done

ifeq "no" "yes"
ranliblib:
	for_static="$(STATICTARGETS)" ; \
	  for f in $$for_static; do \
	    $(RANLIB) $(DESTDIR)$(sharedlibdir)/$$f; done
else
ranliblib:
endif

installlib: compilelib
	$(MAKE) doinstalllib
	$(MAKE) doinstallheaders
	$(MAKE) ranliblib
	$(MAKE) installsubdirs

installexamples: compileexamples
	mkdir -p $(DESTDIR)$(bindir); \
	for_exampleexe="$(EXAMPLEEXE)" ; \
	for f in $$for_exampleexe; do cp $$f $(DESTDIR)$(bindir); done \

installframework: framework
	mkdir -p $(DESTDIR)/Library/Frameworks/gecode.framework && \
	(cd gecode.framework && tar cf - *) | \
	(cd $(DESTDIR)/Library/Frameworks/gecode.framework && tar xf -)

installdoc: doc
	mkdir -p $(DESTDIR)$(docdir); \
	cp -r $(DOCTARGET) $(DESTDIR)$(docdir)

installsubdirs:
	@for_subdirs="$(subdirs)" ; for i in $$for_subdirs; do \
	  cd $$i && if test -f Makefile; then \
	    echo "Making install in directory $$i"; \
	    $(MAKE) install; \
	  fi; \
	done

#
# Source Distribution
#

.PHONY: distdoc

ifeq "no" "yes"
DOCSUFFIX = search-$(VERSION)
else
DOCSUFFIX = $(VERSION)
endif

distdoc: doc
	rm -rf gecode-doc-$(DOCSUFFIX) && \
	mv doc/html gecode-doc-$(DOCSUFFIX) && \
	$(TAR) cf - gecode-doc-$(DOCSUFFIX) | \
		gzip -9 > gecode-doc-$(DOCSUFFIX).tar.gz && \
	7z a -mx9 gecode-doc-$(DOCSUFFIX).7z gecode-doc-$(DOCSUFFIX) && \
	$(RMF) gecode-doc-$(DOCSUFFIX) && \
	if test -f doc/gecode-doc.tag; then \
		mv doc/gecode-doc.tag gecode-doc-$(DOCSUFFIX).tag && \
		gzip -9 gecode-doc-$(DOCSUFFIX).tag; \
	fi


#
# Maintenance targets
#

clean:
	$(RMF) *.stackdump core gmon.out \
		vc70.pdb vc80.pdb vc90.pdb vc100.pdb vc110.pdb \
		vc120.pdb vc130.pdb vc140.pdb
	$(RMF) doxygen.log doxygen.hh doxygen.conf.use stat.hh \
		 changelog.hh doxygen.hh license.hh header.html
	$(RMF) $(ALLOBJ) $(ALLSBJ) $(ALLOBJ:%$(OBJSUFFIX)=%.pdb)
	$(RMF) $(TESTOBJ) $(TESTSBJ) $(TESTOBJ:%$(OBJSUFFIX)=%.pdb)
	$(RMF) $(GISTMOCSRC)
	$(RMF) $(LIBTARGETS:%$(DLLSUFFIX)=%$(MANIFESTSUFFIX)) \
		$(LIBTARGETS:%$(DLLSUFFIX)=%$(RCSUFFIX)) \
		$(LIBTARGETS:%$(DLLSUFFIX)=%$(RESSUFFIX)) \
		$(LIBTARGETS:%$(DLLSUFFIX)=%$(EXPSUFFIX))
	$(RMF) $(EXAMPLEEXE:%.exe=%.pdb) $(EXAMPLEEXE:%=%.manifest) \
		$(EXAMPLEEXE:%=%.rc) $(EXAMPLEEXE:%=%.res)
	$(RMF) $(TESTEXE:%.exe=%.pdb) $(TESTEXE:%=%.manifest) \
		$(TESTEXE:%=%.rc) $(TESTEXE:%=%.res)
	$(RMF) $(FLATZINCEXE:%.exe=%.pdb) $(FLATZINCEXE:%=%.manifest) \
		$(FLATZINCEXE:%=%.rc) $(FLATZINCEXE:%=%.res)

veryclean: clean
	$(RMF) $(LIBTARGETS) \
		$(LIBTARGETS:%$(DLLSUFFIX)=%$(LIBSUFFIX)) \
		$(LIBTARGETS:%$(DLLSUFFIX)=%$(PDBSUFFIX))
	$(RMF) $(LIBTARGETS:%$(DLLSUFFIX)=%$(SOLINKSUFFIX)) \
		$(LIBTARGETS:%$(DLLSUFFIX)=%$(SOSUFFIX))
	$(RMF) $(EXAMPLEEXE)
	$(RMF) $(TESTEXE)
	$(RMF) $(FLATZINCEXE)
	$(RMF) doc GecodeReference.chm ChangeLog
	$(RMF) $(ALLOBJ:%$(OBJSUFFIX)=%.gcno) $(TESTOBJ:%$(OBJSUFFIX)=%.gcno)
	$(RMF) $(ALLOBJ:%$(OBJSUFFIX)=%.gcda) $(TESTOBJ:%$(OBJSUFFIX)=%.gcda)

distclean: veryclean
	$(RMF) $(VARIMP)
	$(RMF) Makefile doxygen.conf
	$(RMF) $(EXTRA_HEADERS)
	$(RMF) config.log config.status Makefile.dep

depend: mkcompiledirs
	@$(MAKE) $(VARIMP) gecode/flatzinc/parser.tab.hpp
	perl $(top_srcdir)/misc/makedepend.perl \
	$(top_srcdir) \
	$(ALLSRC) \
	$(FLATZINC_GENSRC) \
	$(TESTSRC) > Makefile.dep

-include Makefile.dep
