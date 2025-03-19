#include "middle_book_database.h"

#include <string.h>

#include "component/endianness_change.h"
#include "log/my_log.h"
#define INVALID_INDEX -1

static Book_Info_Upper_t book_info[DATABASE_SIZE];
static uint8_t           book_head;
static uint8_t           book_tail;
static uint16_t          book_data_have;
static uint16_t          book_overflow;

static Book_list_t book_dabase[DATABASE_SIZE];
static int32_t     free_head = 0;
static int32_t     data_head = INVALID_INDEX;

// 存入信息
// 取出信息

void init_static_list(void) {
  for (int i = 0; i < DATA_REGION_SIZE - 1; i++) {
    book_dabase[i].next = i + 1;
  }
  book_dabase[DATA_REGION_SIZE - 1].next = 0;
}

// uint8_t node_alloc() { if (free_head == INVALID_INDEX) }
void input_book_info(Book_Info_Upper_t *book) {
  if (book == NULL) {
    LOGE("book info is empty");
    return;
  }

  // if ((book_head + 1) % DATA_REGION_SIZE == book_tail) {
  //   LOGE("book database has full");
  // }

  if ((book_head + 1) % DATA_REGION_SIZE == 0) {
    ++book_overflow;
    LOGE("database overflow %d", book_overflow);
  }

  memcpy(&book_info[book_head], book, sizeof(Book_Info_Upper_t));
  for (int i = 0; i < sizeof(Book_Info_Upper_t); i++) {
    LOGI("%x", ((uint8_t *)(&book_info[book_head]))[i]);
  }
  book_data_have = 1;
  book_head      = (book_head + 1) % DATA_REGION_SIZE;
  LOGI("input book sucess");
}

// Book_Info_Upper_t output_book_info(void) {
//   Book_Info_Upper_t book_temp;
//   if (book_tail == book_head) {
//     LOGE("book database is empty");
//     return;
//   }
//   book_temp = book_info[book_tail];
//   book_tail = (book_tail + 1) % DATA_REGION_SIZE;

//   LOGI("output book sucess");
//   return book_temp;
// }

Book_Info_Upper_t *output_book_info(uint32_t index) {
  Book_Info_Upper_t book_temp;
  // if (book_tail == book_head) {
  //   LOGE("book database is empty");
  //   return NULL;
  // }

  if (0 == book_data_have) LOGE("book database is empty");

  // for (int i = 0; i < sizeof(Book_Info_Upper_t); i++) {
  //   LOGI("%x", ((uint8_t *)(&book_info[index]))[i]);
  // }

  for (int i = 0; i < DATA_REGION_SIZE; i++) {
    if (book_info[i].index == endianness_change_little(index, 4)) {
      // book_tail = (book_tail + 1) % DATA_REGION_SIZE;
      LOGI("output book sucess");
      for (int i = 0; i < sizeof(Book_Info_Upper_t); i++) {
        LOGI("%x", ((uint8_t *)(&book_info[index]))[i]);
      }
      return &book_info[i];
    }
  }

  LOGE("book index not exist");
  return NULL;
}
