/*
	RANKING_FUNCTION_TOPSIG_NEGATIVE.H
	----------------------------------
*/
#ifndef RANKING_FUNCTION_TOPSIG_NEGATIVE_H_
#define RANKING_FUNCTION_TOPSIG_NEGATIVE_H_

#include "ranking_function_topsig.h"

/*
	class ANT_RANKING_FUNCTION_TOPSIG_NEGATIVE
	------------------------------------------
*/
class ANT_ranking_function_topsig_negative : public ANT_ranking_function_topsig
{
public:
	ANT_ranking_function_topsig_negative(ANT_search_engine *engine, long width) : ANT_ranking_function_topsig(engine, width) {}
	ANT_ranking_function_topsig_negative(long long documents, ANT_compressable_integer *document_lengths) : ANT_ranking_function_topsig(documents, document_lengths) {}
	virtual ~ANT_ranking_function_topsig_negative() {}

#ifdef IMPACT_HEADER
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters);
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency);
} ;



#endif /* RANKING_FUNCTION_TOPSIG_NEGATIVE_H_ */
