/*
	INEXQRELS_TO_RUN.C
	------------------
	The format of the qrels is:

	<qid> Q0 <file> <judgment> <length> [ <bep_offset> [ <offset>:<length> ]+ ]

	where

	<qid> query id
	<file> UID of wikipedia document
	<judgment> number of relevant characters (0 = document is not relevant)
	<length> document length

	relevant documents also have Best Entry Points (BEPs) and offset:length pairs thus:

	<bep_offset> BEP offset
	<offset>:<length> each passage as an offset-length pair
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/disk.h"

class ANT_INEX_assessment
{
public:
	long topic, file, relevant, length;
	double proportional_relevance;
public:
	ANT_INEX_assessment(long topic, long file, long relevant, long length);

	static int docsort(const void *a, const void *b);
} ;


/*
	ANT_INEX_ASSESSMENT()
	---------------------
*/
ANT_INEX_assessment::ANT_INEX_assessment(long topic, long file, long relevant, long length)
{
this->topic = topic;
this->file = file;
this->relevant = relevant;
this->length = length;
this->proportional_relevance = (double)relevant / (double)length;
}

/*
	ANT_INEX_ASSESSMENT::DOCSORT()
	------------------------------
*/
int ANT_INEX_assessment::docsort(const void *a, const void *b)
{
int cmp;
ANT_INEX_assessment *one, *two;

one = *(ANT_INEX_assessment **)a;
two = *(ANT_INEX_assessment **)b;

if ((cmp = one->topic - two->topic) == 0)
	{
	if ((cmp = two->proportional_relevance - one->proportional_relevance > 0.0 ? 1 : two->proportional_relevance - one->proportional_relevance < 0.0 ? -1 : 0) == 0)
		if ((cmp = one->file - two->file) == 0)
			exit(printf("There appear to be two assessments for file %d which is not allowed\n", one->file));
	}

return cmp;
}

/*
	READ_ASSESSMENTS()
	------------------
*/
ANT_INEX_assessment **read_assessments(char *filename, long *length)
{
char *file, **lines, **current;
long number_of_lines, qid, document, relevant, doc_length;
ANT_INEX_assessment *another, **assessment_list;
long assessment_list_length, assessment_list_used, assessment_list_chunk_size;

if ((file = ANT_disk::read_entire_file(filename)) == NULL)
	exit(printf("Cannot open infile:%s\n", filename));

lines = ANT_disk::buffer_to_list(file, &number_of_lines);

assessment_list_length = 0;
assessment_list_used = 0;
assessment_list_chunk_size = (1024 * 1024);
assessment_list = NULL;
for (current = lines; *current != NULL; current++)
	{
	sscanf(*current, "%d %*s %d %d %d", &qid, &document, &relevant, &doc_length);
	if (relevant != 0)
		{
		another = new ANT_INEX_assessment(qid, document, relevant, doc_length);
		if (assessment_list_used >= assessment_list_length)
			assessment_list = (ANT_INEX_assessment **)realloc(assessment_list, sizeof(*assessment_list) * ((assessment_list_length += assessment_list_chunk_size) + 1)); // +1 for the NULL termination
		assessment_list[assessment_list_used] = another;
		assessment_list_used++;
		}
	}
assessment_list[assessment_list_used] = NULL;		// NULL terminate the list
delete [] file;
delete [] lines;

*length = assessment_list_used;
return assessment_list;
}


/*
	WRITE_IDEAL_DOC_RUN()
	---------------------
*/
void write_ideal_doc_run(char *outfilename, ANT_INEX_assessment **assessment_list, long assessment_list_length)
{
ANT_INEX_assessment **current;
FILE *outfile;
long current_topic = -1;

qsort(assessment_list, assessment_list_length, sizeof(*assessment_list), ANT_INEX_assessment::docsort);

if ((outfile = fopen(outfilename, "wt")) == NULL)
	exit(printf("Cannot open output file:%s\n", outfilename));

current_topic = -1;

fprintf(outfile, "<inex-submission participant-id=\"4\" run-id=\"IdealDoc\" task=\"RelevantInContext \" query=\"automatic\" result-type=\"fol\">\n");
fprintf(outfile, "<topic-fields title=\"yes\" castitle=\"yes\" description=\"no\" narrative=\"no\"/>\n");
fprintf(outfile, "<description>Ideal whole document run derived from the qrels</description>\n");
fprintf(outfile, "<collections>\n");
fprintf(outfile, "  <collection>wikipedia</collection>\n");
fprintf(outfile, "</collections>\n");

for (current = assessment_list; *current != NULL; current++)
	{
	if ((*current)->topic != current_topic)
		{
		if (current_topic != -1)
			fprintf(outfile, "</topic>");
		fprintf(outfile, "<topic topic-id=\"%ld\">\n", (*current)->topic);
		}

	fprintf(outfile, "  <result>\n");
	fprintf(outfile, "    <file>%ld</file>\n", (*current)->file);
	fprintf(outfile, "    <fol offset=\"0\" length=\"%ld\"/>\n", (*current)->length);
	fprintf(outfile, "    <rsv>%f</rsv>\n", (*current)->proportional_relevance);
	fprintf(outfile, "  </result>\n");
	current_topic = (*current)->topic;
	}

fprintf(outfile, "</topic>\n</inex-submission>");

fclose(outfile);
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_INEX_assessment **assessment_list;
long assessment_list_length;

if (argc != 3)
	exit(printf("Usage:%s <INEX2008qrelFile> <outfile>\n", argv[0]));

assessment_list = read_assessments(argv[1], &assessment_list_length);

write_ideal_doc_run(argv[2], assessment_list, assessment_list_length);

return 0;
}
