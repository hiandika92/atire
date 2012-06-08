/*
	RELEVANT_DOCUMENT.C
	-------------------
*/

#include "relevant_document.h"

/*
	ANT_RELEVANT_DOCUMENT::COMPARE()
	--------------------------------
*/
int ANT_relevant_document::compare(const void *a, const void *b)
{
ANT_relevant_document *one, *two;

one = (ANT_relevant_document *)a;
two = (ANT_relevant_document *)b;

// sort on topic, subtopic, docid
if (two->topic < one->topic) return -1;
if (two->topic > one->topic) return 1;
if (two->subtopic < one->subtopic) return -1;
if (two->subtopic > one->subtopic) return 1;
if (two->docid < one->docid) return -1;
if (two->docid > one->docid) return 1;
return 0;
}

/*
	ANT_RELEVANT_DOCUMENT::IANDCG_SEARCH()
	--------------------------------------
*/
//int ANT_relevant_document::iandcg_search(const void *a, const void *b)
//{
//ANT_relevant_document *one, *two;
//
//one = (ANT_relevant_document *)a;
//two = (ANT_relevant_document *)b;
//}
