#include "database.h"

#include "log/my_log.h"

static BOOK_INFO_t book_info[BOOK_DATABASE_CAPACITY];
static uint8_t     book_index;

void input_booK(uint32_t* UID, uint8_t RSSI) {
  if (book_index >= BOOK_DATABASE_CAPACITY) {
    LOGW("book capacity overflow");
    return;
  }

  book_info[book_index].index = book_index;
  //   book_info[book_index].UID_H = UID[0];
  //   book_info[book_index].UID_L = UID[1];
  //   book_info[book_index].RSSI  = RSSI;
}

// void input_book(BOOK_DATA_T* book) {
//   if (book_index >= BOOK_DATABASE_CAPACITY) {
//     LOGW("book capacity overflow");
//     return;
//   }

//   book_info[book_index].index = book_index;
//   book_info[book_index].data  = *book;

//   LOGW("book input sucess index is : %d", book_index);
// }

// void book_data_pack(BOOK_DATA_T* data, uint32_t* UID, uint8_t RSSI, uint16_t x, uint16_t y) {
//   data->UID_H = UID[0];
//   data->UID_L = UID[1];
//   data->RSSI  = RSSI;
//   data->x_cor = x;
//   data->y_cor = y;
// }

// void book_info_process(uint8_t* recv_data, uint16_t len) {  //
//   BOOK_DATA_T book
// }