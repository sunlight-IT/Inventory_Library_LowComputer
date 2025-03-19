#pragma once
#include "communicate/middle_communicate_both.h"
#define DATABASE_SIZE 128

// typedef struct book_list_t {
//   Book_Info_Lower_t*  book_index;
//   struct book_list_t* next;
// } Book_list_t;

typedef struct book_list_t {
  Book_Info_Lower_t book_index;
  int16_t           prev;
  int16_t           next;
} Book_list_t;
void               input_book_info(Book_Info_Upper_t *book);
Book_Info_Upper_t *output_book_info(uint32_t index);