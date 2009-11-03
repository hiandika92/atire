/*
 * algorithm_ant_link_this.h
 *
 *  Created on: Aug 11, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_ANT_LINK_THIS_H_
#define ALGORITHM_ANT_LINK_THIS_H_

/*
 *
 */
#include "algorithm.h"
#include "ant_link.h"
#include <vector>

namespace QLINK {

	class algorithm_ant_link_this: public algorithm
	{
	private:
	//	#define MAX_LINKS_IN_FILE (1024 * 1024)
	//	ANT_link *all_links_in_file; //[MAX_LINKS_IN_FILE];
	//	long long *all_links_in_file_length;
	//
	//	ANT_link *links_in_orphan; //[MAX_LINKS_IN_FILE];
	//	long *links_in_orphan_length;

		long lowercase_only;					// are we in lowercase or mixed-case matching mode?

		//std::vector<ANT_link_term *>	link_index_;
		ANT_link_term *	link_index_;

		char *runname;
		double proper_noun_boost;
		long targets_per_link;
		long anchors_per_run;
		long print_mode;

	protected:
		char buffer[1024 * 1024];
		char orphan_name_[2048];
		long orphan_docid_;
		const char *text_;
		long terms_in_index;

	private:
		void read_index(const char *filename, long *terms_in_collection);
		//void push_link(char *place_in_file, char *buffer, long docid, double gamma, ANT_link_term *node);
		//void deduplicate_links(void);
		//void print_header(char *runname);
		//void print_footer(void);
		//void print_links(long orphan_docid, char *orphan_name, long links_to_print, long max_targets_per_anchor, long mode);
		ANT_link_term *find_term_in_list(char *value, ANT_link_term *list, long list_length, long this_docid);
		void generate_collection_link_set(char *original_file);
		void add_or_subtract_orphan_links(long add_or_subtract, ANT_link_term *link_index, long terms_in_index);
		long ispropper_noun(char *phrase);
		void usage(char *exename);
		int init_params(int argc, char *argv[]);
		void init_variables();

	protected:
		virtual void process_terms(char **term_list, const char *source);
		void search_anchor_from_link_analysis(links* lx, char **term_list, const char *source);

	public:
		algorithm_ant_link_this(links *links_list);
		algorithm_ant_link_this();
		virtual ~algorithm_ant_link_this();

		int init_params();
		//void process_topic(char *filename);
		virtual void process_topic_text();
		virtual void process_topic(ltw_topic *a_topic);
		//void find_anchor_and_beps();
	};

}
#endif /* ALGORITHM_ANT_LINK_THIS_H_ */