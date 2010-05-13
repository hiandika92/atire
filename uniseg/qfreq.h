/*
 * qfreq.h
 *
 *  Created on: Jan 15, 2009
 *      Author: monfee
 */

#ifndef QFREQ_H_
#define QFREQ_H_

#include "freq.h"
#include <climits>
#include <vector>

class FreqFile;

class QFreq {

private:
	Freq 						freq_stat_;
	Freq 						freq_training_;
	//int 						k_;

public:
	QFreq();
	~QFreq();

	void load_freq(int n = INT_MAX, bool force = false);
	Freq& freq();
	Freq& freq_stat() { return freq_stat_; }

	Freq& freq_training() { return freq_training_; }
	//int k() { return k_; }
	//int count() { return k_; }
	//bool is_loaded() { return loaded_; }

	void load(word_ptr_type word);
	word_ptr_type find(string_type word);

	static QFreq& instance();
};

#endif /* QFREQ_H_ */
