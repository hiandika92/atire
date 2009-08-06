/*
	LINK_EXTRACT.C
	--------------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../source/disk.h"
#include "../source/directory_recursive_iterator.h"
#include "link_parts.h"

char target[1024];
char anchor_text[1024 * 1024];

/*
	STRIP_SPACE_INLINE()
	--------------------
*/
char *strip_space_inline(char *source)
{
char *end, *start = source;

while (isspace(*start))
	start++;

if (start > source)
	memmove(source, start, strlen(start) + 1);		// copy the '\0'

end = source + strlen(source) - 1;
while ((end >= source) && (isspace(*end)))
	*end-- = '\0';

return source;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
//ANT_disk disk;
ANT_directory_recursive_iterator disk;  // make the recursive pattern matching as for default files reading
char *file, *start, *end, *from, *ch;
char *target_start, *target_end, *target_dot;
char *slash;
long param, file_number, current_docid;

if (argc < 2)
	exit(printf("Usage:%s <filespec> ...\n", argv[0]));

file_number = 1;
for (param = 1; param < argc; param++)
	{
	//file = disk.read_entire_file(disk.get_first_filename(argv[param]));
	file = disk.read_entire_file(disk.first(argv[param]));
	while (file != NULL)
		{
		current_docid = get_doc_id(file);
		from = file;
		while (from != NULL)
			{
			if (((start = strstr(from, "<collectionlink")) != NULL) || ((start = strstr(from, "<link")) != NULL))
				if (((end = strstr(start, "</collectionlink>")) != NULL) || ((end = strstr(start, "</link>")) != NULL))
					{
					start = strstr(start, "xlink:href=");
					if (start != NULL && start < end)
						{
						target_start = strchr(start, '"') + 1;

						/* skip / or \ */
						while ((slash = strpbrk(target_start, "\\/")) && slash < end)
							target_start = slash + 1;

						target_dot = strchr(target_start, '.');
						target_end = strchr(target_start, '"');
						if (target_dot != NULL && target_dot < target_end)
							target_end = target_dot;
						strncpy(target, target_start, target_end - target_start);
						target[target_end - target_start] = '\0';

						start = strchr(start, '>') + 1;
						strncpy(anchor_text, start, end - start);
						anchor_text[end - start] = '\0';
						strip_space_inline(anchor_text);
						for (ch = anchor_text; *ch != '\0'; ch++)
							if (isspace(*ch))
								*ch = ' ';		// convert all spaces (tabs, cr, lf) into a space;
						if (*target >= '0' && *target <= '9') // make sure this is a valid link
							printf("%d:%s:%s\n", current_docid, target, anchor_text);
						}

					if (start != NULL && start < end)
						start = end;		// for the next time around the loop
					}
			from = start;
			}

		if (file_number % 1000 == 0)
			fprintf(stderr, "Files processed:%d\n", file_number);
		file_number++;

		delete [] file;
		//file = disk.read_entire_file(disk.get_next_filename());
		file = disk.read_entire_file(disk.next());
		}
	}

fprintf(stderr, "%s Completed\n", argv[0]);
return 0;
}

