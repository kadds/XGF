#pragma once
#include <unordered_map>
class Batch;
typedef std::unordered_map<const char *, Batch *> Batches;
extern const char BATCHES_BATCH_PT[];
extern const char BATCHES_BATCH_PC[];