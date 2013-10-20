/*********************************************************************
*	File name: 		buffer.h
*	Compiler: 		MS Visual Studio 2010
*	Author: 		Kwok Hong Kelvin Chan, 040742238
*	Course: 		CST 8152 – Compilers, Lab Section: 401 
*	Assignment: 	1
*	Date: 			September 24th, 2013
*	Professor: 		Sv. Ranev
*	Purpose: 		Header file for buffer.c, include various MARCO declaration and Buffer structure declarations
*	Function list: 
*********************************************************************/
#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* constant definitions */
/* You may add your own constant definitions here */
#define R_FAIL_1 -1         /* fail return value */
#define R_FAIL_2 -2         /* fail return value */
#define LOAD_FAIL -2       /* load fail error */
#define SET_R_FLAG 1       /* realloc flag set value */

#define NUM_BUFFER_STRUC 1  /* Initial number for Buffer structure  */
#define NUM_NEGATIVE_1 -1   /* MARCO for -1 */
#define NUM_0 0 			/* MARCO for 0 */
#define NUM_1 1 			/* MARCO for 1 */
#define LOWER_RANGE_1 1 	/* Lower range inc_factor for mode 'a' and 'm'*/
#define UPPER_RANGE_100 100 /* Upper range inc_factor for mode 'm'*/
#define UPPER_RANGE_255 255 /* Upper range inc_factor for mode 'a'*/
#define BUFFER_FULL 1 		/* Signal Buffer is full */
#define BUFFER_AVAILABLE 0  /* Signal Buffer is not full */
#define BUFFER_EMPTY 1 		/* Signal Buffer is empty */

#define B_FULL 				/* Signal for using b_isfull as a macro expansion or a function  */
#ifdef B_FULL 				/* If B_FULL is defined */
/*********************************************************************
*	Purpose: Define B_ISFULL as a macro expansion to increase run-time performance, and the function determines if the Buffer is full or not.
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD						
*	Return value: 
					type:		value:
					int 		1, 0
*	Algorithm:		Compare the structure's addc_offset and capacity to get the result to determine if the Buffer is full or not.
*********************************************************************/
#define B_ISFULL(pBD)  ((pBD->addc_offset <= pBD->capacity) ? (BUFFER_AVAILABLE) : (BUFFER_FULL)) 	
#endif						

/* user data type declarations */
typedef struct BufferDescriptor {
char *ca_head;   /* pointer to the beginning of character array (character buffer) */
short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
short addc_offset;  /* the offset (in chars) to the add-character location */
short getc_offset;  /* the offset (in chars) to the get-character location */
short mark_offset; /* the offset (in chars) to the mark location */
char  inc_factor; /* character array increment factor */
char  r_flag;     /* reallocation flag */
char  mode;       /* operational mode indicator*/
int   eob;       /* end-of-buffer flag */
} Buffer;

/* function declarations */
/*
Place your function declarations here.
Do not include the function header comments here.
Place them in the buffer.c file
*/
Buffer * b_create (short init_capacity,char inc_factor,char o_mode);
Buffer *b_addc (Buffer * const pBD, char symbol);
int b_reset(Buffer * const pBD);
void b_destroy(Buffer * const pBD);
int b_isfull(Buffer * const pBD);
short b_getsize(Buffer * const pBD);
short b_getcapacity(Buffer * const pBD);
int b_setmark(Buffer * const pBD, short mark);
short b_getmark(Buffer * const pBD);
int b_getmode(Buffer * const pBD);
int b_load (FILE * const fi, Buffer * const pBD);
int b_eob(Buffer * const pBD);
char b_getc(Buffer * const pBD);
int b_print (Buffer * const pBD);
Buffer *b_pack(Buffer * const pBD);
char b_get_r_flag(Buffer * const pBD);
int b_isempty(Buffer * const pBD);
int b_retract(Buffer * const pBD);
short b_get_getc_offset(Buffer * const pBD);
int b_set_getc_offset(Buffer * const pBD, short offset);
char * b_get_chmemloc(Buffer * const pBD, short offset);
#endif
