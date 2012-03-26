/*
	PREGEN.C
	--------
*/
#include "str.h"
#include "pregen.h"
#include "pregen_field_type.h"

/*
	ANT_PREGEN::ANT_PREGEN()
	------------------------
*/
ANT_pregen::ANT_pregen(ANT_pregen_field_type type, const char *name)
{
field_name = strnew(name);
scores = NULL;
doc_count = 0;

this->type = type;
}

/*
	ANT_PREGEN::~ANT_PREGEN()
	-------------------------
*/
ANT_pregen::~ANT_pregen()
{
delete [] scores;
}

/*
	ANT_PREGEN::READ()
	------------------
	Read a pregen from the given file, return non-zero iff successful.
	On failure, the fields are left in a half-updated (but destructible) state.
*/
int ANT_pregen::read(const char *filename)
{
uint32_t four_byte;
uint32_t field_name_length;

if (file.open(filename, "rbx") == 0)
	return 0;

file.seek(file.file_length() - sizeof(four_byte) - sizeof(four_byte) - sizeof(four_byte) - sizeof(four_byte) - sizeof(four_byte));

file.read(&four_byte);
if (four_byte != PREGEN_FILE_VERSION)
	return 0;

file.read(&four_byte);
doc_count = four_byte;

file.read(&four_byte);
if (four_byte != sizeof(ANT_pregen_t))
	return 0;

file.read(&four_byte);
type = (ANT_pregen_field_type)four_byte;

file.read(&four_byte);
field_name_length = four_byte;

file.seek(sizeof(file_header));

delete [] field_name;
field_name = new char[field_name_length + 1];
if (file.read((unsigned char *)field_name, field_name_length * sizeof(*field_name)) == 0)
	return 0;
field_name[field_name_length] = '\0';

delete [] scores;
scores = new ANT_pregen_t[doc_count];
if (file.read((unsigned char *)scores, doc_count * sizeof(*scores)) == 0)
	return 0;

return 1;
}

/*
	ANT_PREGEN::PREGEN_TYPE_TO_STR()
	--------------------------------
*/
const char *ANT_pregen::pregen_type_to_str(ANT_pregen_field_type type)
{
switch (type)
	{
	case INTEGER:
		return "integer";
	case STRTRUNC:
		return "strtrunc";
	case BINTRUNC:
		return "bintrunc";
	case BASE32:
		return "base32";
	case BASE32_ARITHMETIC:
		return "base32arith";
	case BASE36:
		return "base36";
	case RECENTDATE:
		return "recentdate";
	case INTEGEREXACT:
		return "integerexact";
	case STREXACT:
		return "strexact";
	case STREXACT_RESTRICTED:
		return "strextractrestricted";
	case BASE37:
		return "base37";
	case BASE37_ARITHMETIC:
		return "base37arith";
	case BASE40:
		return "base40";
	case ASCII_PRINTABLES:
		return "asciiprintables";
	case ASCII_PRINTABLES_ARITHMETIC:
		return "asciiprintablesarith";
	case ASCII_PRINTABLES_ARITHMETIC_BIGRAM:
		return "asciiprintablesarithbigram";
	default:
		return "Unknown";
	}
}
