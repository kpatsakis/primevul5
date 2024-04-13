uint8_t APar_ExtractChannelInfo(FILE *isofile, uint32_t pos) {
  uint8_t packed_channels = APar_read8(isofile, pos);
  uint8_t unpacked_channels =
      (packed_channels << 1); // just shift the first bit off the table
  unpacked_channels =
      (unpacked_channels >> 4); // and slide it on over back on the uint8_t
  return unpacked_channels;
}