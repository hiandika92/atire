#include <limits>
#include <limits.h>
#include <string.h>
#include <cassert>
#include <time.h>
#include <algorithm>
#include <string.h>
#include <cmath>

#include "maths.h"
#include "pregen.h"
#include "arithmetic_coding.h"
#include "str.h"

template<typename T> T min (T a, T b)
{
return a < b ? a : b;
}

template<typename T> T max (T a, T b)
{
return a > b ? a : b;
}

pregen_t ANT_pregen_writer_normal::generate_integer(ANT_string_pair field)
{
/* atol can happily wander off the end of the field. It'll be stopped by the null-termination
 * of the document name and not cause a problem.
 */
long value = atol(field.start);

return value > std::numeric_limits<pregen_t>::max() ? std::numeric_limits<pregen_t>::max() :
		value <= 0 ? 1 :
		(pregen_t) value;
}

pregen_t ANT_pregen_writer_normal::generate_bintrunc(ANT_string_pair field)
{
pregen_t result = 0;
size_t bytes = min(field.length(), sizeof(result));

/* Work correctly on both little and big-endian systems (don't just cast string to integer) */
for (unsigned int i = 0; i < bytes; i++)
	result = (result << CHAR_BIT) | field[i];

/* Left-align the result */
if (bytes < sizeof(result))
	result = result << (CHAR_BIT * (sizeof(result) - bytes));

return result;
}

/*
 * For severely constrained accumulator sizes, compress UNIX timestamps to give higher accuracy
 * for recent dates than old ones.
 */
pregen_t ANT_pregen_writer_normal::generate_recentdate(ANT_string_pair field)
{
long long date = atol(field.start);

//Don't need to do any fancy conversion
if (sizeof(pregen_t) >= 4)
	return (pregen_t) date;

return 0; //TODO implement me
}

template <typename T>
pregen_t ANT_pregen_writer_normal::generate_sliding_radix(ANT_string_pair field)
{
pregen_t result = 0;
/*
uint32_t character;

unsigned char buffer[UTF8_LONGEST_DECOMPOSITION_LEN];
unsigned char *buffer_pos;
size_t buffer_remain;

 How many letters can we fit in the result? If accumulator is signed, don't use the sign bit (we don't want
 * negative RSVs)
const unsigned int dest_bits = sizeof(result) * CHAR_BIT - (std::numeric_limits<pregen_t>::is_signed ? 1 : 0);

unsigned int dest_chars_remain = (int) (dest_bits / LOG_BASE_2_OF_RADIX);

while (field.string_length > 0 && (character = utf8_to_wide(field.start)) != 0 && dest_chars_remain > 0)
	{
	buffer_pos = buffer;
	buffer_remain = sizeof(buffer);

	ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_remain, character);

	 Write as much of that UTF-8 normalization as we can fit into the result
	for (int i = 0; i < (buffer_pos - buffer) && dest_chars_remain > 0; i++)
		{
		unsigned char encoded = T::encode(buffer[i]);

		if (encoded != CHAR_ENCODE_FAIL)
			{
			dest_chars_remain--;
			result = result * T::num_symbols + encoded;
			}
		}

	field.start += utf8_bytes(field.start);
	field.string_length -= utf8_bytes(field.start);
	}

//"left justify" the resulting bits so that longer strings aren't always larger than shorter ones
while (dest_chars_remain)
	{
	result *= T::num_symbols;
	dest_chars_remain--;
	}

if (result == 0) Strings of all zero symbols become zero, which we don't want as an RSV. Avoid.
	return 1;*/

return result;
}

/**
 * Generalized encoding of a UTF-8 string using a character encoding function
 * provided as type parameter T, e.g. ANT_encode_char_base36. The string is
 * first Unicode-normalized and lowercased, then encoded using the given function.
 *
 * T must have a static method num_symbols, and a method encode(unsigned char), which
 * takes a character to encode and returns a value in the range [0..num_symbols).
 */
template <typename T>
pregen_t ANT_pregen_writer_normal::generate_radix(ANT_string_pair field)
{
pregen_t result = 0;

uint32_t character;

unsigned char buffer[UTF8_LONGEST_DECOMPOSITION_LEN];
unsigned char *buffer_pos;
size_t buffer_remain;

int dest_chars_remain = ANT_compiletime_int_floor_log_to_base<pregen_t, T::num_symbols>::value;
const int final_digit_radix = ANT_compiletime_int_floor_log_to_base_remainder<pregen_t, T::num_symbols,
		ANT_compiletime_int_floor_log_to_base_has_remainder<pregen_t, T::num_symbols>::value>::value;

while (field.string_length > 0 && (character = utf8_to_wide(field.start)) != 0 && dest_chars_remain >= 0)
	{
	buffer_pos = buffer;
	buffer_remain = sizeof(buffer);

	ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_remain, character);

	/* Write as much of that UTF-8 normalization as we can fit into the result */
	for (int i = 0; i < (buffer_pos - buffer) && dest_chars_remain >= 0; i++)
		{
		unsigned char encoded = T::encode(buffer[i]);

		if (encoded != CHAR_ENCODE_FAIL)
			{
			if (dest_chars_remain == 0)
				{
				//Final digit isn't the full radix, scale down
				result = (pregen_t) (result * final_digit_radix + (encoded * (final_digit_radix - 1)) / (T::num_symbols - 1));
				}
			else
				result = (pregen_t) (result * T::num_symbols + encoded);

			dest_chars_remain--;
			}
		}

	field.start += utf8_bytes(field.start);
	field.string_length -= utf8_bytes(field.start);
	}

//"left justify" the resulting bits so that longer strings aren't always larger than shorter ones
if (dest_chars_remain >= 0)
	{
	while (dest_chars_remain > 0)
		{
		result = (pregen_t) (result * T::num_symbols);
		dest_chars_remain--;
		}
	//Final digit isn't the full radix, scale down
	result = (pregen_t) (result * final_digit_radix);
	}

if (result == 0) /*Strings of all zero symbols become zero, which we don't want as an RSV. Avoid. */
	return 1;

return result;
}

template <typename T>
pregen_t ANT_pregen_writer_normal::generate_arithmetic(ANT_string_pair field, ANT_arithmetic_model *model)
{
uint32_t character;

unsigned char buffer[UTF8_LONGEST_DECOMPOSITION_LEN];
unsigned char *buffer_pos;
size_t buffer_remain;
pregen_t result;

ANT_arithmetic_encoder<pregen_t> encoder(model);

while (field.string_length > 0 && (character = utf8_to_wide(field.start)) != 0)
	{
	buffer_pos = buffer;
	buffer_remain = sizeof(buffer);

	ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_remain, character);

	/* Write as much of that UTF-8 normalization as we can fit into the result */
	for (int i = 0; i < (buffer_pos - buffer); i++)
		{
		unsigned char symbol = T::encode(buffer[i]);

		if (symbol != CHAR_ENCODE_FAIL)
			if (!encoder.encode_symbol(symbol))
				goto break_outer; //Ran out of room in the encoded result, so stop encoding
		}
	field.start += utf8_bytes(field.start);
	field.string_length -= utf8_bytes(field.start);
	}

break_outer:
result = encoder.done();

if (result == 0) /*Strings of all zero symbols become zero, which we don't want as an RSV. Avoid. */
	return 1;

return result;
}

pregen_t ANT_pregen_writer_normal::generate(ANT_string_pair field)
{
switch (type)
	{
	case INTEGER:
		return generate_integer(field);
	case STRTRUNC:
		return generate_radix<ANT_encode_char_8bit>(field);
	case BINTRUNC:
		return generate_bintrunc(field);
	case ASCII_5BIT:
		return generate_radix<ANT_encode_char_5bit>(field);
	case BASE36:
		return generate_radix<ANT_encode_char_base36>(field);
	case BASE37:
		return generate_radix<ANT_encode_char_base37>(field);
	case BASE40:
		return generate_radix<ANT_encode_char_base40>(field);
	case ASCII_PRINTABLES:
		return generate_radix<ANT_encode_char_printable_ascii>(field);
	case BASE37_ARITHMETIC:
		return generate_arithmetic<ANT_encode_char_base37>(field, arithmetic_model);
	case ASCII_PRINTABLES_ARITHMETIC:
		return generate_arithmetic<ANT_encode_char_printable_ascii>(field, arithmetic_model);
	default:
		assert(0);
		return 0;
	}
}

ANT_pregens_writer::ANT_pregens_writer()
{
field_count = 0;
}

ANT_pregens_writer::~ANT_pregens_writer()
{
for (int i = 0; i < field_count; i++)
	delete fields[i];
}

int ANT_pregens_writer::add_field(const char *filename, const char *field_name, pregen_field_type type)
{
ANT_pregen_writer * field;

switch (type)
	{
	case STREXACT:
		field = new ANT_pregen_writer_exact_strings(field_name);
		break;
	case INTEGEREXACT:
		field = new ANT_pregen_writer_exact_integers(field_name);
		break;
	default:
		field = new ANT_pregen_writer_normal(type, field_name);
	}

if (!field->open_write(filename))
	{
	delete field;
	return 0;
	}

fields[field_count] = field;
field_count++;

return 1;
}

void ANT_pregen_writer_normal::add_field(long long docindex, ANT_string_pair & content)
{
long long skip_docs;

/* It's possible that previous documents didn't contain this field, so be prepared to zero-pad. */
skip_docs = docindex - doc_count;

if (skip_docs > 0)
	add_score(0, skip_docs);
else
	assert (skip_docs == 0); //docindex should never go backwards. We don't support that.

add_score(generate(content));
}

void ANT_pregens_writer::process_document(long long doc_index, ANT_string_pair doc_name)
{
//TODO version which doesn't cause copying...
char *dup = doc_name.str();

process_document(doc_index, dup);

delete [] dup;
}

void ANT_pregens_writer::process_document(long long doc_index, char *doc_name)
{
char * pos = doc_name;
enum { IDLE, INSIDE_TAG, INSIDE_OPEN_TAG, INSIDE_CLOSE_TAG} state = IDLE;

int tag_depth = 0;
ANT_string_pair tag_name, tag_body, close_tag_name;

/* Parse document name and find matching top-level XML fields to use */
while (*pos)
	{
	switch (state)
		{
		case IDLE:
			if (*pos == '<')
				{
				tag_body.string_length = pos - tag_body.start;
				state = INSIDE_TAG;
				}
			break;
		case INSIDE_TAG:
			if (*pos == '/')
				{
				state = INSIDE_CLOSE_TAG;
				close_tag_name.start = pos + 1;
				break;
				}

			tag_name.start = pos;
			state = INSIDE_OPEN_TAG;
			//Fall-through

		case INSIDE_OPEN_TAG:
			if (*pos == '>')
				{
				tag_name.string_length = pos - tag_name.start;
				tag_body.start = pos + 1;
				tag_depth++;
				state = IDLE;
				}
			break;

		case INSIDE_CLOSE_TAG:
			if (*pos == '>')
				{
				tag_depth--;

				if (tag_depth == 0)
					{
					for (int i = 0; i < field_count; i++)
						if (tag_name.true_strcmp(fields[i]->field_name) == 0)
							fields[i]->add_field(doc_index, tag_body);
					state = IDLE;
					}
				}
			break;
		}
	++pos;
	}
}

void ANT_pregens_writer::close()
{
for (int i = 0; i < field_count; i++)
	fields[i]->close_write();
}

ANT_pregen::ANT_pregen(pregen_field_type type, const char *name)
{
this->field_name = strnew(name);
scores = NULL;
doc_count = 0;

this->type = type;
}

ANT_pregen_writer::ANT_pregen_writer(pregen_field_type type, const char *name)
{
this->field_name = strnew(name);
doc_count = 0;

this->type = type;
}

void ANT_pregen_writer_normal::add_score(pregen_t score, long long count)
{
for (int i = 0; i < count; i++)
	file.write((unsigned char *)&score, sizeof(score));
doc_count = (uint32_t) (doc_count + count);
}

void ANT_pregen_writer_normal::close_write()
{
struct pregen_file_header header;

header.doc_count = doc_count;

//Update doc count field in header before we close the file
file.seek(offsetof(struct pregen_file_header, doc_count));
file.write((unsigned char *)&header.doc_count, sizeof(header.doc_count));

file.close();
}

/*
	ANT_PREGEN_FIELD::READ()
	------------------------

	Read a pregen from the given file, return non-zero iff successful.

	On failure, the fields are left in a half-updated (but destructible) state.
*/
int ANT_pregen::read(const char *filename)
{
struct pregen_file_header header;

if (!file.open(filename, "rbx"))
	return 0;

if (!file.read((unsigned char *) &header, sizeof(header)))
	return 0;

if (header.version != PREGEN_FILE_VERSION)
	return 0;

doc_count = header.doc_count;
type = (pregen_field_type) header.field_type;

delete [] field_name;
field_name = new char[header.field_name_length + 1];
if (!file.read((unsigned char *) field_name, header.field_name_length * sizeof(*field_name)))
	return 0;
field_name[header.field_name_length] = '\0';

delete [] scores;
scores = new pregen_t[header.doc_count];
if (!file.read((unsigned char *) scores, header.doc_count * sizeof(*scores)))
	return 0;

return 1;
}

int ANT_pregen_writer_normal::open_write(const char *filename)
{
struct pregen_file_header header;

if (!file.open(filename, "wbx"))
	return 0;

//Write header to file to begin with

header.doc_count = 0; //We will fill that in later
header.version = PREGEN_FILE_VERSION;
header.field_type = type;
header.field_name_length = (uint32_t) strlen(field_name);

file.write((unsigned char *)&header, sizeof(header));

file.write((unsigned char *)field_name, header.field_name_length * sizeof(*field_name));

return 1;
}

ANT_pregen_writer::~ANT_pregen_writer()
{
delete [] field_name;
}

ANT_pregen::~ANT_pregen()
{
delete [] scores;
}

void ANT_pregen_writer_exact_strings::add_exact_string(char *str)
{
ensure_storage();

exact_strings[doc_count].first = doc_count;
exact_strings[doc_count].second = str;
doc_count++;
}

void ANT_pregen_writer_exact_strings::ensure_storage()
{
if (doc_count >= doc_capacity)
	{
	doc_capacity = doc_capacity * 2 + 1;

	std::pair<long long, char *>* new_strings = new std::pair<long long, char *>[doc_capacity];
	memcpy(new_strings, exact_strings, sizeof(exact_strings[0]) * doc_count);

	delete [] exact_strings;
	exact_strings = new_strings;
	}
}

void ANT_pregen_writer_exact_integers::ensure_storage()
{
if (doc_count >= doc_capacity)
	{
	doc_capacity = doc_capacity * 2 + 1;

	std::pair<long long, long long> *new_integers = new std::pair<long long, long long>[doc_capacity];
	memcpy(new_integers, exact_integers, sizeof(exact_integers[0]) * doc_count);

	delete [] exact_integers;
	exact_integers = new_integers;
	}
}

void ANT_pregen_writer_exact_integers::add_exact_integer(long long i)
{
ensure_storage();

exact_integers[doc_count].first = doc_count;
exact_integers[doc_count].second = i;

doc_count++;
}

bool exact_str_less(const std::pair<long long, char*>& a, const std::pair<long long, char*>& b)
{
//TODO Unicode comparison function
return strcmp(a.second, b.second) < 0;
}

ANT_pregen_writer_exact_strings::ANT_pregen_writer_exact_strings(const char *name) : ANT_pregen_writer(STREXACT, name), memory(100*1024*1024)
{
compare = exact_str_less;
doc_capacity = 1024;
exact_strings = new std::pair<long long, char*>[doc_capacity];
}

ANT_pregen_writer_exact_integers::ANT_pregen_writer_exact_integers(const char *name) : ANT_pregen_writer(INTEGEREXACT, name)
{
doc_capacity = 1024;
exact_integers = new std::pair<long long,long long>[doc_capacity];
}

ANT_pregen_writer_exact_strings::~ANT_pregen_writer_exact_strings()
{
delete [] exact_strings;
}

ANT_pregen_writer_exact_integers::~ANT_pregen_writer_exact_integers()
{
delete [] exact_integers;
}

void ANT_pregen_writer_exact_integers::add_field(long long  docindex, ANT_string_pair & content)
{
long long skip_docs;

/* It's possible that previous documents didn't contain this field, so be prepared to zero-pad. */
skip_docs = docindex - doc_count;

assert(skip_docs >= 0);

while (skip_docs > 0)
	{
	add_exact_integer(0);
	skip_docs--;
	}

//TODO long long conversion
add_exact_integer(atol(content.start));
}

bool exact_int_less(const std::pair<long long, long long>& a, const std::pair<long long, long long>& b)
{
return a.second < b.second;
}

int ANT_pregen_writer_exact_integers::open_write(const char *filename)
{
if (!file.open(filename, "wbx"))
	return 0;

return 1;
}

void ANT_pregen_writer_exact_integers::close_write()
{
struct pregen_file_header header;
pregen_t *outvalues;

/* Sort the documents in order based on their exact source values, use that order
 * to define the RSV of each document */
header.doc_count = doc_count;
header.version = PREGEN_FILE_VERSION;
header.field_type = type;
header.field_name_length = (uint32_t) strlen(field_name);

file.write((unsigned char *)&header, sizeof(header));

file.write((unsigned char *)field_name, header.field_name_length * sizeof(*field_name));

std::sort(&exact_integers[0], &exact_integers[doc_count], exact_int_less);

outvalues = new pregen_t[doc_count];

/* Each document's RSV is its position in the ranking (also add 1 to avoid generating a
 * zero RSV) */
for (unsigned int i = 0; i < doc_count; i++)
	outvalues[exact_integers[i].first] = (pregen_t) (i + 1);

file.write((unsigned char *)outvalues, doc_count * sizeof(*outvalues));

delete [] outvalues;

file.close();
}

void ANT_pregen_writer_exact_strings::add_field(long long  docindex, ANT_string_pair & _content)
{
ANT_string_pair content = _content; //we want a copy we can modify
long long skip_docs;

unsigned char *buffer;
unsigned char * buffer_pos;
size_t buffer_remain;
uint32_t character;
char * string;

/* It's possible that previous documents didn't contain this field, so be prepared to zero-pad. */
skip_docs = docindex - doc_count;

assert(skip_docs >= 0);

while (skip_docs > 0)
	{
	add_exact_string(strnew(""));
	skip_docs--;
	}

/* Apply UTF-8 decomposition/normalization to the string (for case folding, etc) */

/* Allocate enough room for worst-case result (actually pretty damn pessimistic) */
buffer_remain = content.string_length * UTF8_LONGEST_DECOMPOSITION_LEN + 1;
buffer = new unsigned char[buffer_remain];
buffer_remain--; //leave room for null terminator

buffer_pos = buffer;

while (content.string_length > 0 && (character = utf8_to_wide(content.start)) != 0 && buffer_remain > 0)
	{
	ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_remain, character);

	content.start += utf8_bytes(character);
	content.string_length -= utf8_bytes(character);
	}
*buffer_pos = '\0';

//Make a copy of that processed string in our memory pool
string = (char*)memory.malloc(buffer_pos - buffer + 1);
memcpy(string, buffer, buffer_pos - buffer + 1);

add_exact_string(string);

delete[] buffer;
}

int ANT_pregen_writer_exact_strings::open_write(const char *filename)
{
if (!file.open(filename, "wbx"))
	return 0;

return 1;
}

void ANT_pregen_writer_exact_strings::print_strings()
{
for (long long i = 0; i < doc_count; i++)
	printf("%6ld %s\n", (long) exact_strings[i].first, exact_strings[i].second);
}

void ANT_pregen_writer_exact_strings::set_comparison_function(comparison_function compare)
{
this->compare = compare;
}

void ANT_pregen_writer_exact_strings::close_write()
{
struct pregen_file_header header;
pregen_t *outvalues;

/* Sort the documents in order based on their exact source values, use that order
 * to define the RSV of each document */
header.doc_count = doc_count;
header.version = PREGEN_FILE_VERSION;
header.field_type = type;
header.field_name_length = (uint32_t) strlen(field_name);

file.write((unsigned char *)&header, sizeof(header));

file.write((unsigned char *)field_name, header.field_name_length * sizeof(*field_name));

std::sort(&exact_strings[0], &exact_strings[doc_count], compare);

outvalues = new pregen_t[doc_count];

/* Each document's RSV is its position in the ranking (also add 1 to avoid generating a
 * zero RSV) */
pregen_t out = 1;

for (unsigned int i = 0; i < doc_count; i++)
	{
	/* Don't increase RSV if this string is the same as the previous (search engine will tiebreak) */
	if (i > 0 && compare(exact_strings[i-1], exact_strings[i]) != 0)
		out++;

	outvalues[exact_strings[i].first] = out;
	}

file.write((unsigned char *)outvalues, doc_count * sizeof(*outvalues));

delete [] outvalues;

file.close();
}
