uint8_t APar_skip_filler(FILE *isofile, uint32_t start_position) {
  uint8_t skip_bytes = 0;

  while (true) {
    uint8_t eval_byte = APar_read8(isofile, start_position + skip_bytes);

    if (eval_byte == 0x80 || eval_byte == 0x81 ||
        eval_byte == 0xFE) { // seems sometimes QT writes 0x81
      skip_bytes++;
    } else {
      break;
    }
  }
  return skip_bytes;
}