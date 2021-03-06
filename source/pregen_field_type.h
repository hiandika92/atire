/*
	PREGEN_FIELD_TYPE.H
	-------------------
*/
#ifndef PREGEN_FIELD_TYPE_H_
#define PREGEN_FIELD_TYPE_H_

enum ANT_pregen_field_type	{
	INTEGER, 
	BINTRUNC, 
	STRTRUNC, 
	BASE32, 
	BASE32_ARITHMETIC, 
	BASE36,
	BASE37, 
	BASE40, 
	RECENTDATE, 
	INTEGEREXACT, 
	STREXACT, 
	STREXACT_RESTRICTED, 
	BASE37_ARITHMETIC,
	ASCII_PRINTABLES, 
	ASCII_PRINTABLES_ARITHMETIC, 
	ASCII_PRINTABLES_ARITHMETIC_BIGRAM,	
	OCTECT_ARITHMETIC_BIGRAM
	};


#endif /* PREGEN_FIELD_TYPE_H_ */
