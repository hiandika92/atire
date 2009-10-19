/*
	SEARCH_ENGINE_RESULT.H
	----------------------
*/
#ifndef SEARCH_ENGINE_RESULT_H_
#define SEARCH_ENGINE_RESULT_H_


#include <stdio.h>
#include "search_engine_accumulator.h"

class ANT_memory;

/*
	class ANT_SEARCH_ENGINE_RESULT
	------------------------------
*/
class ANT_search_engine_result
{
friend class ANT_search_engine;
friend class ANT_search_engine_result_iterator;

private:
	ANT_search_engine_accumulator *accumulator;
	ANT_search_engine_accumulator **accumulator_pointers;
	long long documents;

public:
	ANT_search_engine_result(ANT_memory *memory, long long documents);
	virtual ~ANT_search_engine_result() {}

	void *operator new(size_t bytes, ANT_memory *allocator);
	ANT_search_engine_accumulator &operator[](size_t index) { return accumulator[index]; }
	void add_rsv(size_t index, double score) { accumulator[index].add_rsv(score); }
	void add_rsv(size_t index, long score) { accumulator[index].add_rsv(score); }
	long is_zero_rsv(size_t index) { return accumulator[index].is_zero_rsv(); }
	
	void init_accumulators(void);
	long long init_pointers(void);
} ;


#endif /* SEARCH_ENGINE_RESULT_H_ */