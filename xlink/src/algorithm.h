/*
 * algorithm.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "links.h"

namespace QLINK {
	class algorithm_config;

	/*
	 *
	 */
	class algorithm {
	public:
		enum run_mode { WITH_CMDLINE_OPTIONS, SIMPLE };

	protected:
		run_mode 				mode_;
		int						topic_param_start_;

		links					*links_;

		/*
		 * pointers for the convenient access members of links
		 */
//		ANT_link *all_links_in_file; //[MAX_LINKS_IN_FILE];
//		long long *all_links_in_file_length;
//
//		ANT_link *links_in_orphan; //[MAX_LINKS_IN_FILE];
//		long *links_in_orphan_length;
		algorithm_config		*config_;

	public:
		algorithm(links *links_list);
		algorithm();
		virtual ~algorithm();

		virtual int init_params() {}
		virtual void process_topic(char *filename) {}
		virtual void process_topic_content(char *file) {}
		virtual void process_topic(ltw_topic *a_topic) {}
		virtual void process_links(ltw_topic *a_topic, algorithm *bep_algor) {}
		virtual void process_links(const char *file, algorithm *bep_algor) {}

		int topic_param_start() { return topic_param_start_; }
		void set_links_container(links *container);

	//protected:

	};

}

#endif /* ALGORITHM_H_ */