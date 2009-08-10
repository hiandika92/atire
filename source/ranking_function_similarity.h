/*
	RANKING_FUNCTION_SIMILARITY.H
	-----------------------
*/
#ifndef __RANKING_FUNCTION_SIMILARITY_H__
#define __RANKING_FUNCTION_SIMILARITY_H__

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_SIMILARITY
	-------------------------------
*/
class ANT_ranking_function_similarity : public ANT_ranking_function
{
private:
	double k1;
	double b;
	float *document_prior_probability;

public:
	ANT_ranking_function_similarity(ANT_search_engine *engine, double k1 = 0.9, double b = 0.4);
	virtual ~ANT_ranking_function_similarity();
    virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point);

    virtual void relevance_rank_tf(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf *tf_array, long long trim_point);
} ;

#endif __RANKING_FUNCTION_SIMILARITY_H__