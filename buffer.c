/*********************************************************************
*	File name: 		buffer.c
*	Compiler: 		MS Visual Studio 2010
*	Author: 		Kwok Hong Kelvin Chan, 040742238
*	Course: 		ST 8152 – Compilers, Lab Section: 401 
*	Assignment: 	1
*	Date: 			September 24th, 2013
*	Professor: 		Sv. Ranev
*	Purpose: 		Create a Buffer structure and make the Buffer functional.
*	Function list: 
					b_create ();
					b_addc ();
					b_reset();
					b_destroy();
					b_isfull();
					b_getsize();
					b_getcapacity();
					b_setmark();
					b_getmark();
					b_getmode();
					b_load ();
					b_eob();
					b_getc();
					b_print ();
					b_pack();
					b_get_r_flag();
					b_isempty();
					b_retract();
					b_get_getc_offset();
					b_set_getc_offset();
					b_get_chmemloc();
*********************************************************************/

#include "buffer.h" /* for function prototypes, Buffer structure, and macro declaration */

/*********************************************************************
*	Purpose: Create a Buffer structure and set structure's members
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
					calloc()
					malloc()
					free()
*	Parameters: 
					Type:		Parameter:				Values/Range:
					short 		init_capacity 			signed 0 to SHRT_MAX
					char 		inc_factor 				unsigned 0 to 255 signed -128-127
					char 		o_mode 					'f', 'a', 'm'

*	Return value: 
					type:		value:
					Buffer *	pBD
					NULL 		NULL

*	Algorithm: 		calloc a Buffer structure, malloc a character array, 
					and set the structure member mode and inc_factor to 
					the appropriate data depends on the input parameter o_mode.
*********************************************************************/

Buffer * b_create (short init_capacity,char inc_factor,char o_mode){
	/* Initialize/create one Buffer structure using calloc, and store the return pointer to pBD for manipulating the Buffer's structure members */
	Buffer * pBD = (Buffer *) calloc(NUM_BUFFER_STRUC, sizeof(Buffer));
	/* 
	 *	Check whether if the Buffer pointer is created successfully, if not, return NULL. 	
	 */
	 if (pBD != NULL && init_capacity > NUM_0 && init_capacity < SHRT_MAX){
		/* Initialize/create a character array using malloc, and store the return pointer to temp to ensure memory is allocated successfully */
	 	char * temp = (char *) malloc(init_capacity);
		/* 
		 *	Check whether if the character array is created successfully, if not, free the Buffer pointer memory and return NULL. 	
		 */
		 if (temp != NULL){
		 	pBD->ca_head = temp; /* assign the temp character pointer to Buffer's structure member ca_head */
		 }
		 else{
		 	free(pBD); 	/* free the Buffer allocated memory */
		 	return NULL;
		 }
		/*
		*	switch cases for o_mode to determine various mode condition (fixed, additive self-incrementing, and multiplicative self-incrementing)
		*/
		switch ( o_mode ) {
			case 'f':
			/*
			*	for mode equal to 'f' check if inc_factor is not equal to 0, then initialize Buffer's mode and inc_factor to 0.
			*
			*/
			if (inc_factor != NUM_0){
				pBD->mode = NUM_0;
				pBD->inc_factor = NUM_0;
			}else{
				free(pBD->ca_head); /* free the character array allocated memory */
				free(pBD); /* free the Buffer allocated memory */
				return NULL;
			}
			break;
			case 'a':
			/*
			*	for mode equal to 'a' check if inc_factor is equal to 0, then initialize Buffer's mode and inc_factor to 0.
			*	if the inc_factor is between 1 to 255 inclusive, then initialize Buffer's mode to 1 and inc_factor to parameter inc_factor.
			*/
			if (inc_factor == NUM_0){
				pBD->mode = NUM_0;
				pBD->inc_factor = NUM_0;
			}
			if (inc_factor >= LOWER_RANGE_1 && inc_factor <= UPPER_RANGE_255){	
				pBD->mode = NUM_1;
				pBD->inc_factor = inc_factor;
			}else{
				free(pBD->ca_head); /* free the character array allocated memory */
				free(pBD); /* free the Buffer allocated memory */
				return NULL;
			}
			break;
			case 'm':
			/*
			*	for mode equal to 'm' check if inc_factor is equal to 0, then initialize Buffer's mode and inc_factor to 0.
			*	if the inc_factor is between 1 to 100 inclusive, then initialize Buffer's mode to -1 and inc_factor to parameter inc_factor.
			*/
			if (inc_factor == NUM_0){
				pBD->mode = NUM_0;
				pBD->inc_factor = NUM_0;
			}
			if (inc_factor >= LOWER_RANGE_1 && inc_factor <= UPPER_RANGE_100){	
				pBD->mode = NUM_NEGATIVE_1;
				pBD->inc_factor = inc_factor;
			}
			else{
				free(pBD->ca_head); /* free the character array allocated memory */
				free(pBD); /* free the Buffer allocated memory */
				return NULL;
			}
			break;
			/*
			*	when all cases failed, it free all allocated memory.
			*/
			default:
			free(pBD->ca_head); /* free the character array allocated memory */
			free(pBD); /* free the Buffer allocated memory */
			return NULL;
			break;
		}
		/*
		*	double check parameter init_capacity is greater than 0 or not before assign the Buffer's capacity to it.
		*/
		if (init_capacity > NUM_0){
			pBD->capacity = init_capacity;
			return pBD;
		}
		else{
			free(pBD->ca_head); /* free the character array allocated memory */
			free(pBD); /* free the Buffer allocated memory */
			return NULL;
		}
	}else{
		return NULL;
	}
}

/*********************************************************************
*	Purpose: Add a character to the character array inside the Buffer's structure
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
					B_ISFULL()
					b_isfull()
					free()
					realloc()
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD						
					char 		symbol 				unsigned 0 to 255 signed -128-127

*	Return value: 
					type:		value:
					Buffer *	pBD
					NULL 		NULL

*	Algorithm: 		Check if the Buffer is full. If it is, check the Buffer's mode, and calculate the new capacity, 
					and relloc the new memory space 
*********************************************************************/
Buffer *b_addc (Buffer * const pBD, char symbol){
	int is_full = BUFFER_AVAILABLE; /* store the return value from function b_isfull to see if Buffer is full */
	short available_space; /* temporarily variable to store a calculation result */
	float new_increment;  /* temporarily variable to store a calculation result */
	short new_capacity;  /* variable to temporarily store the new calculated capacity */
	char * temp; /* temporarily character pointer to store the realloc function result */
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
		pBD->r_flag = NUM_0; /* Set the Buffer's r_flag to 0 indicating the Buffer memory has not moved */
		/*
		*	if B_FULL is defined, then it will use the macro expansion version, else use the normal function
		*/
		#ifdef B_FULL
			is_full = B_ISFULL(pBD); /* macro expansion function to check if the Buffer is full */
		#else
			is_full = b_isfull(pBD); /* normal function to check if the Buffer is full */
		#endif
		/*
		*	switch cases for is_full variable and apply various actions when the Buffer is full, and when it is not.
		*/
		switch (is_full){
			case BUFFER_FULL: /* case when the Buffer is full */
			/*
			*	switch cases for current Buffer's mode (1, 0, and -1) to apply various actions.
			*/
			switch (pBD->mode){
				/* 
				* for Buffer's mode equal to -1, new capacity is calculated by a set function, and if the calculated result is greater than SHRT_MAX
				* then the new capacity is equal to SHRT_MAX.
				*/
				case NUM_NEGATIVE_1 : 
				if (pBD->capacity < SHRT_MAX){
					available_space = SHRT_MAX - pBD->capacity;
					new_increment = (float) available_space * pBD->inc_factor/100;
					/*
					*	check if the calculated new_increment is less than SHRT_MAX
					*	if it is, return NULL to cancel incrementing the Buffer.
					*/
					if (new_increment < SHRT_MAX){
						/*
						*	if current Buffer's capacity plus the value of new_increment,
						*	the new_capacity is equal to SHRT_MAX.
						*/
						if ((pBD->capacity + new_increment) > SHRT_MAX){
							new_capacity = SHRT_MAX;
						}else{
							/*
							*	Avoid underflow issue from new_increment,
							*	actual new_increment value could be between 1 and 0 (ex: 0.9),
							*	and by casting to short, the new_increment will become 0, ignore the decimal.
							* 	therefore I am manually adding one to ensure this process works accordingly.
							*/
							if ((short)new_increment == NUM_0){
								new_capacity = pBD->capacity + NUM_1;
							}else{
								new_capacity = pBD->capacity + (short)new_increment;
							}
						}
					}else{
						return NULL;
					}
				}else{
					return NULL;
				}
				break;
				/* 
				* for Buffer's mode equal to 0, return NULL.
				*/
				case NUM_0 :
				return NULL;
				break;
				/* 
				* for Buffer's mode equal to 1, new capacity is calculated adding the Buffer's capacity to Buffer's inc_factor.
				*/
				case NUM_1 :
				if (pBD->inc_factor > NUM_0){
					new_capacity = pBD->capacity + pBD->inc_factor;
					/*
					*	ensure the new capacity is between the range of 0 to SHRT_MAX.
					*/
					if (new_capacity < 0 || new_capacity > SHRT_MAX){
						return NULL;
					}else{
						break;
					}
				}
				else{
					return NULL;
				}
				break;
				default:
				return NULL;
				break;
			}
			/* reallocate memory for a character array using realloc, and store the return pointer to temp to ensure memory is allocated successfully */
			temp = (char *) realloc(pBD->ca_head, new_capacity);
			/* 
			 *	Check whether if the character array is created successfully, if not, free the Buffer pointer memory and return NULL. 	
			 */
			 if (temp != NULL){
				/* 
				 *	Check whether if pointer to the character array has been reallocated or not, if so, set the Buffer's reallocation flag r_flag to 1.
				 */
				 if (pBD->ca_head != temp){
				 	pBD->r_flag = SET_R_FLAG;
				 }
				pBD->ca_head = temp; /* assign the temp character pointer to Buffer's structure member ca_head */
				pBD->ca_head[pBD->addc_offset] = symbol; /* add a character to the character array with the index given by Buffer's addc_offset. */
				 ++pBD->addc_offset;
				pBD->capacity = new_capacity; /* reassign the Buffer's capacity with the newly calculated capacity. */
				 return pBD;
				}else{
					return NULL;
				}
				break;
			case BUFFER_AVAILABLE: /* case when the Buffer is available */
			// pBD->r_flag = SET_R_FLAG;
			pBD->ca_head[pBD->addc_offset] = symbol; /* add a character to the character array with the index given by Buffer's addc_offset. */
				++pBD->addc_offset;
				return pBD;
				break;
				default:
				return NULL;
				break;
			}

	}else{
		return NULL;
	}
}

/*********************************************************************
*	Purpose: Re-initializes all offset Buffer's members (addc_offset, getc_offset and mark_offset)
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD						
					
*	Return value: 
					type:		value:
					int 		-1, 0

*	Algorithm: 		check if the input Buffer's pointer and its character array pointer to be NULL, 
					if not, reset all offset Buffer's members (addc_offset, getc_offset and mark_offset) to be 0.
*********************************************************************/
int b_reset(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
		/* 
	 	*	Check whether if the character array pointer exists, if so, reset Buffer's offset to be 0.
	 	*/
	 	if (pBD->ca_head != NULL){
	 		pBD->addc_offset = NUM_0;
	 		pBD->getc_offset = NUM_0;
	 		pBD->mark_offset = NUM_0;
	 		return NUM_0;
	 	}else{
	 		return R_FAIL_1;
	 	}  
	 }else{
	 	return R_FAIL_1;
	 }
}	

/*********************************************************************
*	Purpose: Destroy the Buffer's character array
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
						free()
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD						
					
*	Return value: 	
*	Algorithm: 		check if the input Buffer's pointer and its character array pointer to be NULL, 
					if not, free the allocated memory for the Buffer's character array.
*********************************************************************/
void b_destroy(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
		/* 
	 	*	Check whether if the character array pointer exists, if so, free the allocated memory.
	 	*/
	 	if (pBD->ca_head != NULL){
	 		free(pBD->ca_head);
	 	}
	 	free(pBD);
	 }
}

/*********************************************************************
*	Purpose: Determine if the Buffer is full
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD						
					
*	Return value: 
					type:		value:
					int 		-1, 0, 1

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, compare Buffer's addc_offset with Buffer's capacity to determine if the Buffer is full or not.
*********************************************************************/
int b_isfull(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
		/*
		*	Check if Buffer's addc_offset greater or equal to Buffer's capacity. If it is, indicate the Buffer is full.
		*/
		if (pBD->addc_offset >= pBD->capacity){
			return BUFFER_FULL;
		}else{
			return BUFFER_AVAILABLE;
		}
	}else{
		return R_FAIL_1;
	}
}

/*********************************************************************
*	Purpose: Get the Buffer's character array size
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD						
					
*	Return value: 
					type:		value:
					short 		-1 / addc_offset

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, return the Buffer's character array size
*********************************************************************/
short b_getsize(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
	 	return pBD->addc_offset;
	 }else{
	 	return R_FAIL_1;
	 }
}

/*********************************************************************
*	Purpose: Get the Buffer's capacity
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD						
					
*	Return value: 
					type:		value:
					short 		-1 / capacity

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, return the Buffer's capacity
*********************************************************************/
short b_getcapacity(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
	 	return pBD->capacity;
	 }else{
	 	return R_FAIL_1;
	 }
}

/*********************************************************************
*	Purpose: Set the Buffer's mark
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					short 		mark					
					
*	Return value: 
					type:		value:
					int 		-1 / mark

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, return the Buffer's capacity
*********************************************************************/
int b_setmark(Buffer * const pBD, short mark){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	
	 *  Also, check if parameter mark is within the range of Buffer's capacity.
	 */
	 if (pBD != NULL && mark <= pBD->capacity && mark >=NUM_0){
	 	pBD->mark_offset = mark;

	 	return mark;
	 }else{
	 	return R_FAIL_1;
	 }
}

/*********************************************************************
*	Purpose: Get the Buffer's mark
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD				
					
*	Return value: 
					type:		value:
					short 		-1 / mark_offset

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, return the Buffer's mark_offset
*********************************************************************/
short b_getmark(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
	 	return pBD->mark_offset;
	 }else{
	 	return R_FAIL_1;
	 }
}

/*********************************************************************
*	Purpose: Get the Buffer's mode
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					int 		-1 / mode

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, return the Buffer's mode
*********************************************************************/
int b_getmode(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
	 	return pBD->mode;
	 }else{
	 	return R_FAIL_1;
	 }
}

/*********************************************************************
*	Purpose: Load File content into the Buffer
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
						fgetc()
						feof()
						b_addc()

*	Parameters: 
					Type:		Parameter:			Values/Range:
					FILE * 		fi
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					int 		-2 / -1 / number_of_char

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, read characters from file as long as it does not get to end of file.
					Meanwhile, increment number_of_char to keep count the total of characters that added to the Buffer
*********************************************************************/
int b_load (FILE * const fi, Buffer * const pBD){
	int number_of_char = 0; /* variable to store the total number of characters added */
	char symbol; /* variable to store the character read from the file */
	Buffer * temp; /* temporarily character pointer to store the b_addc function result */
	/* 
	 *	Check whether if the Buffer pointer and the File pointer is NULL, if so, return NULL.
	 */
	 if (fi != NULL && pBD != NULL){
		/* 
		 *	while true, using fgetc to read one character at a time from the file and add it into the Buffer using b_addc.
		 */
		 while (NUM_1) {
			symbol = fgetc (fi); /* read one character from file */
			/*
			* check if the file reaches end of file, if not, continue reading character from file and store it in the Buffer
			*/
			if (feof(fi)){
				return number_of_char;
			}
			temp = b_addc(pBD, symbol);
			/* 
			 *	Check whether the character is added properly to the Buffer's character array, if so, increment the number_of_char.
			 */
			 if (temp == NULL){
			 	return LOAD_FAIL;
			 }
			 else{
			 	++number_of_char;
			 }
			}
			return number_of_char;
	}else{
		return R_FAIL_1;
	}
}

/*********************************************************************
*	Purpose: Get the Buffer's eob
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					int 		-1 / eob

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, return the Buffer's eob
*********************************************************************/
int b_eob(Buffer * const pBD){
	if (pBD != NULL){
		return pBD->eob;
	}else{
		return R_FAIL_1;
	}
}

/*********************************************************************
*	Purpose: Get the character from the Buffer with the index getc_offset
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					char 		-2 / -1 / symbol

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, return the character from index equal to getc_offset
*********************************************************************/
char b_getc(Buffer * const pBD){
	char symbol; /* variable to store the character read from the Buffer */
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
		/*
		*	If Buffer's getc_offset equal to addc_offset, then indicate end of Buffer.
		*	Else return the character that is store in the index getc_offset, and increment the getc_offset.
		*/
		if (pBD->getc_offset == pBD->addc_offset){
			pBD->eob = NUM_1; /* indicate end of Buffer*/
			return NUM_NEGATIVE_1;
		}else{
			pBD->eob = NUM_0;
			symbol = pBD->ca_head[pBD->getc_offset]; /* get character from Buffer with index of getc_offset */
			++pBD->getc_offset; 
			return symbol;
		}
	}else{
		return LOAD_FAIL;
	}
}

/*********************************************************************
*	Purpose: Print out the characters that are stored in the Buffer.
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
						b_isempty
						b_getc()
						b_eob()
						printf()

*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					int 		-1 / printed_char

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, check whether the Buffer is empty.
					if not, print out the Buffer content, and increment the printed character variable. 
*********************************************************************/
int b_print (Buffer * const pBD){
	int is_empty; /* store the return value from function b_isempty to see if Buffer is empty */
	int printed_char = 0; /* variable to store the total number of characters printed */
	char symbol; /* variable to store the character read from the Buffer */
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
		is_empty = b_isempty(pBD); /* check if Buffer is empty */
		/*
		* Switch statement to only print the Buffer's content when the Buffer is not empty.
		*/
		switch (is_empty){
			case BUFFER_EMPTY:
			printf("The buffer is empty.\n");
			return NUM_0;
			break;
			case BUFFER_AVAILABLE:
			b_set_getc_offset(pBD, NUM_0); /* reset the getc_offset to 0, and get ready to print */
			/* 
			 *	while true, using b_getc to get one character at a time from the Buffer, check it reaches the end of Buffer.
			 *	If not, print the character, and increment printed_char.
			 */
			 while (NUM_1){
				symbol = b_getc(pBD); /* using b_getc to get a character from the Buffer */
				/*
				*	check if it reaches the end of Buffer
				*/
				if (b_eob(pBD) != NUM_1){
					printf("%c", symbol);
					++printed_char;
				}else{
					break;
				}
			} 
			printf("\n");
			return printed_char;
			break;
			default:
			return R_FAIL_1;
			break;
		}
	}else{
		return R_FAIL_1;
	}
}

/*********************************************************************
*	Purpose: Add one character space to the Buffer if possible.
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
						realloc()
						
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					Buffer * 	NULL / pBD

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, attemp to reallocate a memory space for one extra character. 
					On success, reset the Buffer's structure member correspond to the new mempry space.
*********************************************************************/
Buffer *b_pack(Buffer * const pBD){
	short new_capacity; /* variable to store the new calculated new capacity */
	char * temp; /* temporary character pointer to store the realloc function result */
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	 if (pBD != NULL){
		new_capacity = pBD->addc_offset + 1; /* calculate new capacity by adding 1 to the addc_offset */
		/*
		*  Ensure the new calculated capacity is smaller than SHRT_MAX and greater than zero.
		*/
	 	if (new_capacity < SHRT_MAX && new_capacity > NUM_0){
	 		temp = (char *) realloc(pBD->ca_head, new_capacity); /* temporary character array pointer to store the return value from the reallocation memory address*/
	 		/*
	 		*	check against NULL to make sure the reallocation perform properly.
	 		*/
	 		if (temp != NULL){
	 			/* 
				 *	Check whether if pointer to the character array has been reallocated or not, if so, set the Buffer's reallocation flag r_flag to 1.
				 */
	 			if (pBD->ca_head != temp){
	 				pBD->r_flag = SET_R_FLAG;
	 			}
	 			pBD->ca_head = temp;
	 			pBD->capacity = new_capacity;
	 			return pBD;
	 		}else{
	 			return NULL;
	 		}
	 	}else{
	 		return NULL;
	 	}
	 }else{
	 	return NULL;
	 }
}

/*********************************************************************
*	Purpose: Get Buffer's reallocation flag r_flag.
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					char 		-1 / r_flag

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, return the Buffer's reallocation flag r_flag
*********************************************************************/
char b_get_r_flag(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	if (pBD != NULL){
		return pBD->r_flag;
	}else{
		return R_FAIL_1;
	}
}

/*********************************************************************
*	Purpose: Check if Buffer is empty or not.
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					int 		-1 / 1 / 0

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, check if Buffer's addc_offset is equal to zero.
					If so, return Buffer is empty.
*********************************************************************/
int b_isempty(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	if (pBD != NULL){
		/* 
		 *	Check whether if the Buffer's addc_offset is equal to zero.
		 *  If so, the Buffer is empty.
		 */
		if (pBD->addc_offset == NUM_0){
			return BUFFER_EMPTY;
		}else{
			return BUFFER_AVAILABLE;
		}
	}else{
		return R_FAIL_1;
	}
}

/*********************************************************************
*	Purpose: Retract the Buffer's getc_offset by one.
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					int 		-1 / 0

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, decrement the Buffer's getc_offset by one.
*********************************************************************/
int b_retract(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	if (pBD != NULL){
		--pBD->getc_offset;
		return NUM_0;
	}else{
		return R_FAIL_1;
	}
}

/*********************************************************************
*	Purpose: Get Buffer's member getc_offset
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD	
					
*	Return value: 
					type:		value:
					short 		-1 / getc_offset

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, return Buffer's getc_offset.
*********************************************************************/
short b_get_getc_offset(Buffer * const pBD){
	/* 
	 *	Check whether if the Buffer pointer is NULL, if so, return NULL. 	 	
	 */
	if (pBD != NULL){
		return pBD->getc_offset;
	}else{
		return R_FAIL_1;
	}
}

/*********************************************************************
*	Purpose: Set Buffer's member getc_offset
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD
					short 		offset
					
*	Return value: 
					type:		value:
					int 		-1 / 0

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, set Buffer's from parameter offset.
*********************************************************************/
int b_set_getc_offset(Buffer * const pBD, short offset){
	/* 
	 *	Check whether if the Buffer pointer is NULL, and
	 *	ensure the parameter offset is great or equal to zero,
	 *	or it is smaller than current Buffer's addc_offset.	 	
	 */
	if (pBD != NULL && offset >= 0 && offset < pBD->addc_offset){
		pBD->getc_offset = offset;
		return NUM_0;
	}else{
		return R_FAIL_1;
	}
}

/*********************************************************************
*	Purpose: Set a character pointer to the Buffer's character array with a specify location indicated by the parameter offset
*	Author: Kwok Hong Kelvin Chan
*	History/Versions: 1.0, September 24th, 2013
*	Called functions: 
*	Parameters: 
					Type:		Parameter:			Values/Range:
					Buffer * 	pBD
					short 		offset
					
*	Return value: 
					type:		value:
					char * 		NULL / ptr_char

*	Algorithm: 		Check if the input Buffer's pointer is NULL, 
					if not, assign the new character array pointer to
					the index provided by parameter offset.
*********************************************************************/
char * b_get_chmemloc(Buffer * const pBD, short offset){
	char * ptr_char; /* character pointer for the offset location */
	/* 
	 *	Check whether if the Buffer pointer is NULL, and
	 *	ensure the parameter offset is great or equal to zero,
	 *	or it is smaller than current Buffer's addc_offset.	 	
	 */
	if (pBD != NULL  && offset >= 0 && offset < pBD->addc_offset){
		ptr_char = &pBD->ca_head[offset]; /* assign the pointer to the address of character array specific offset. */ 
		if (ptr_char != NULL){
			return ptr_char;	
		}
		else{
			return NULL;
		}
	}else{
		return NULL;
	}
}
