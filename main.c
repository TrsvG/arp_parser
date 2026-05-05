#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/arp_parser.h"

uint8_t *read_file(const char *filename, size_t *out_size);
void handle_parse_error(int error_code);

int main() {
  const char *filename = "arp_test.bin";
  size_t data_size = 0;

  uint8_t *buffer = read_file(filename, &data_size);
  if (!buffer) {
    return 1;
  }

  struct arp_packet packet;
  int res = parse_arp(buffer, data_size, &packet);

  if (res == 0) {
    print_arp(&packet);
  } else {
    handle_parse_error(res);
  }

  free(buffer);
  return (res == 0) ? 0 : 1;
}

uint8_t *read_file(const char *filename, size_t *out_size) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    perror("Ошибка открытия файла");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  uint8_t *buffer = malloc(file_size);
  if (!buffer) {
    fclose(file);
    return NULL;
  }

  size_t read_count = fread(buffer, 1, file_size, file);
  fclose(file);

  if (read_count < (size_t)file_size) {
    free(buffer);
    return NULL;
  }

  *out_size = read_count;
  return buffer;
}

void handle_parse_error(int error_code) {
  fprintf(stderr, "Ошибка парсинга (%d): ", error_code);
  switch (error_code) {
    case -1:
      fprintf(stderr, "Недостаточно данных.\n");
      break;
    case -2:
      fprintf(stderr, "Неподдерживаемый размер адресов.\n");
      break;
    case -3:
      fprintf(stderr, "Неверный тип сети/протокола.\n");
      break;
    case -4:
      fprintf(stderr, "Неизвестная операция ARP.\n");
      break;
    default:
      fprintf(stderr, "Неизвестный код ошибки.\n");
  }
}
