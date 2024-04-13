calcuate_sample_size(char *uint32_buffer, FILE *isofile, short stsz_atom) {
  uint32_t sample_size = 0;
  uint32_t sample_count = 0;
  uint64_t total_size = 0;

  sample_size = APar_read32(
      uint32_buffer, isofile, parsedAtoms[stsz_atom].AtomicStart + 12);
  sample_count = APar_read32(
      uint32_buffer, isofile, parsedAtoms[stsz_atom].AtomicStart + 16);

  if (sample_size == 0) {
    for (uint64_t atom_offset = 20;
         atom_offset < parsedAtoms[stsz_atom].AtomicLength;
         atom_offset += 4) {
      total_size +=
          APar_read32(uint32_buffer,
                      isofile,
                      parsedAtoms[stsz_atom].AtomicStart + atom_offset);
    }
  } else {
    total_size = sample_size * sample_count;
  }
  return total_size;
}