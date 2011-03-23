/*
	RANKING_FUNCTION_DLH13.H
	------------------------
*/
#ifndef RANKING_FUNCTION_DLH13_H_
#define RANKING_FUNCTION_DLH13_H_

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_DLH13
	--------------------------------
*/
class ANT_ranking_function_DLH13 : public ANT_ranking_function
{
public:
	ANT_ranking_function_DLH13(ANT_search_engine *engine) : ANT_ranking_function(engine) {}
	ANT_ranking_function_DLH13(long long documents, ANT_compressable_integer *document_lengths) : ANT_ranking_function(documents, document_lengths) {}
	virtual ~ANT_ranking_function_DLH13() {}

	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;

#endif /* RANKING_FUNCTION_DLH13_H_ */