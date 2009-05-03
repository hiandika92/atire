/*
	COMPRESSION_FACTORY.C
	---------------------
*/
#include <stdio.h>
#include <limits.h>
#include "compression_factory.h"
#include "compress_relative10.h"
#include "compress_carryover12.h"
#include "compress_golomb.h"
#include "compress_variable_byte.h"

/*
	static
	------
	Declaration of an object of each of the known compression types,
	pointers to these are used below as the objects to do the compression
	and decompression on demand.
*/
static ANT_compress_variable_byte variable_byte;
static ANT_compress_simple9 simple9;
static ANT_compress_relative10 relative10;
static ANT_compress_carryover12 carryover12;
static ANT_compress_elias_delta elias_delta;
static ANT_compress_elias_gamma elias_gamma;
static ANT_compress_golomb golomb;

/*
	ANT_compression_factory::techniques[]
	-------------------------------------
	List of all known (to ANT) compression schemes
*/
ANT_compress *ANT_compression_factory::technique[] = 
{
&variable_byte,
&simple9,
&relative10,
&carryover12,
&elias_delta,
&elias_gamma,
&golomb
} ;

/*
	ANT_compression_factory::names[]
*/
char *ANT_compression_factory::technique_name[] =
{
"Variable-Byte",
"Simple-9",
"Relative-10",
"Carryover-12",
"Elias-Delta",
"Elias-Gamma",
"Golomb"
} ;

long ANT_compression_factory::number_of_techniques = sizeof(ANT_compression_factory::technique) / sizeof(*ANT_compression_factory::technique);

/*
	ANT_COMPRESSION_FACTORY::COMPRESS()
	-----------------------------------
*/
long long ANT_compression_factory::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long which, preferred = 0;
long long min_size, size;

min_size = LLONG_MAX;

for (which = 0; which < number_of_techniques; which++)
	{
	size = technique[which]->compress(destination, destination_length, source, source_integers);
	if (size < min_size)
		{
		min_size = size;
		preferred = which;
		}
//	printf("%d %lld\n", which, size);
	}
*destination = (unsigned char)preferred;
return technique[preferred]->compress(destination + 1, destination_length - 1, source, source_integers) + 1;
}

/*
	ANT_COMPRESSION_FACTORY::DECOMPRESS()
	-------------------------------------
*/
void ANT_compression_factory::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
technique[*source]->decompress(destination, source + 1, destination_integers);
}