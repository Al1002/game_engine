#pragma once

#include <memory>   // smart pointers

#include <string>
#include <list>          // s.e.
#include <queue>
#include <map>
#include <set>
#include <unordered_set> // hash table
#include <utility>

#include <functional>
#include <iostream> // cout
#include <random>
#if defined(__MINGW32__) || defined(__MINGW64__)
#include "mingw-threads/mingw.mutex.h"
#else
#include <mutex>
#include <atomic>
#endif
// smart pointer relevant
using std::shared_ptr;
using std::weak_ptr;
/// \cond
using std::enable_shared_from_this; // allows safe taking of shared_ptr<>(this) instance
/// \endcond
using std::static_pointer_cast;
using std::dynamic_pointer_cast; 
using std::make_shared;
using std::make_unique;
using std::move;

// general ADTs
using std::string;
using std::list;
using std::vector;
using std::queue;
using std::map;
using std::set;
using std::unordered_set;

using std::function;
using std::pair;
using std::mutex;
