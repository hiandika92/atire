/*
	RANKING_FUNCTION_DIVERGENCE.C
	-----------------------------
*/
#include "maths.h"
#include "ranking_function_divergence.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_DIVERGENCE::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------------
	see G. Amati and C.J. van Rijsbergen (2002), Probabilistic Models of Information Retrieval Based on 
	Measuring the Divergence from Randomness, Transactions on Information Systems 20(4):357-389.
*/
void ANT_ranking_function_divergence::relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
long docid;
double tf, rsv, tf_prime, ne, F, F_plus_1, inf_right;
ANT_compressable_integer *current, *end;

/*
	we are using I(ne)B2 defined thus:

	                            N + 1
	Inf1 = I(ne) = tf' * log2(--------)
	                          ne + 0.5

	where
	               N - 1
	ne = N * (1 - (-----)^F )
	                 N

	where
	tf' = normalised term frequency (see below)
	N = Number of documents
	F = collection frequency

	and use First Normalisation B

	                  F + 1
	weight(t,d) = ------------- * Inf1
	              n * (tf' + 1)

	where n = document frequency
	finally we length normalise tf using H2

	                   av_len
	tf' = tf.log2(1 + ------)
	                    len

	where
		len is the length of the document,
		av_len is the average document length,
		ttf is the number of occurences of the term in the document
*/
current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);

F = (double)term_details->collection_frequency;
F_plus_1 = F + 1.0;
ne = documents * (1.0 - pow((documents - 1.0) / documents, F));
inf_right = ANT_log2(((double)documents + 1.0) / (ne + 0.5));

while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		tf_prime = tf * ANT_log2(1.0 + (double)mean_document_length / (double)document_lengths[docid]);
		rsv = tf_prime * inf_right * (F_plus_1 / ((double)term_details->document_frequency * (tf_prime + 1.0)));
		accumulator[docid].add_rsv(rsv);
		}
	current++;		// skip over the zero
	}
}