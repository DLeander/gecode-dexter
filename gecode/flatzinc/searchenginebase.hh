#ifndef SEARCH_ENGINE_BASE_HH
#define SEARCH_ENGINE_BASE_HH

// Includes
// #include <gecode/flatzinc.hh>
// #include <gecode/flatzinc/registry.hh>
// #include <gecode/flatzinc/plugin.hh>
// #include <gecode/flatzinc/branch.hh>
#include <gecode/search.hh>

#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <limits>
#include <unordered_set>

namespace Gecode { namespace FlatZinc {
    class Printer;
    class FlatZincSpace;
    class FlatZincOptions;
}}

using namespace std;
using namespace Gecode;
using namespace Gecode::FlatZinc;

// Base class for all search engines
class BaseEngine {
public:
    virtual ~BaseEngine() {}  // Virtual destructor
    virtual FlatZincSpace* next() = 0;  // Pure virtual function
    virtual Gecode::Search::Statistics statistics() = 0;  // Pure virtual function
    virtual bool stopped() = 0;  // Pure virtual function
};

// Derived class for BAB search engine
class BABEngine : public BaseEngine {
public:
    BABEngine(FlatZincSpace* space, const Search::Options& options)
        : engine(space, options) {}

    FlatZincSpace* next() override {
        return engine.next();
    }

    Gecode::Search::Statistics statistics() override {
        return engine.statistics();
    }

    bool stopped() override {
        return engine.stopped();
    }

private:
    BAB<FlatZincSpace> engine;
};

// Derived class for DFS search engine
class DFSEngine : public BaseEngine {
public:
    DFSEngine(FlatZincSpace* space, const Search::Options& options)
        : engine(space, options) {}

    FlatZincSpace* next() override {
        return engine.next();
    }

    Gecode::Search::Statistics statistics() override {
        return engine.statistics();
    }

    bool stopped() override {
        return engine.stopped();
    }

private:
    DFS<FlatZincSpace> engine;
};

// Derived class for RBS search engine
class RBSEngine : public BaseEngine {
public:
    RBSEngine(FlatZincSpace* space, const Search::Options& options, std::atomic<bool>* optimum_found)
        : engine(space, options) {}

    FlatZincSpace* next() override {
        return engine.next();
    }

    Gecode::Search::Statistics statistics() override {
        return engine.statistics();
    }

    bool stopped() override {
        return engine.stopped();
    }

private:
    RBS<FlatZincSpace> engine;
};

#endif  // SEARCH_ENGINE_BASE_HH