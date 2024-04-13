void APar_ExtractMovieDetails(char *uint32_buffer,
                              FILE *isofile,
                              AtomicInfo *mvhd_atom) {
  if (mvhd_atom->AtomicVerFlags & 0x01000000) {
    movie_info.creation_time =
        APar_read64(uint32_buffer, isofile, mvhd_atom->AtomicStart + 12);
    movie_info.modified_time =
        APar_read64(uint32_buffer, isofile, mvhd_atom->AtomicStart + 20);
    movie_info.timescale =
        APar_read32(uint32_buffer, isofile, mvhd_atom->AtomicStart + 28);
    movie_info.duration =
        APar_read32(uint32_buffer, isofile, mvhd_atom->AtomicStart + 32);
    movie_info.timescale =
        APar_read32(uint32_buffer, isofile, mvhd_atom->AtomicStart + 36);
    movie_info.duration =
        APar_read32(uint32_buffer, isofile, mvhd_atom->AtomicStart + 40);
    movie_info.playback_rate =
        APar_read32(uint32_buffer, isofile, mvhd_atom->AtomicStart + 44);
    movie_info.volume =
        APar_read16(uint32_buffer, isofile, mvhd_atom->AtomicStart + 48);
  } else {
    movie_info.creation_time = (uint64_t)APar_read32(
        uint32_buffer, isofile, mvhd_atom->AtomicStart + 12);
    movie_info.modified_time = (uint64_t)APar_read32(
        uint32_buffer, isofile, mvhd_atom->AtomicStart + 16);
    movie_info.timescale =
        APar_read32(uint32_buffer, isofile, mvhd_atom->AtomicStart + 20);
    movie_info.duration =
        APar_read32(uint32_buffer, isofile, mvhd_atom->AtomicStart + 24);
    movie_info.playback_rate =
        APar_read32(uint32_buffer, isofile, mvhd_atom->AtomicStart + 28);
    movie_info.volume =
        APar_read16(uint32_buffer, isofile, mvhd_atom->AtomicStart + 32);
  }

  movie_info.seconds = (float)movie_info.duration / (float)movie_info.timescale;
#if defined(_MSC_VER)
  __int64 media_bits = (__int64)mdatData * 8;
#else
  uint64_t media_bits = (uint64_t)mdatData * 8;
#endif
  movie_info.simple_bitrate_calc =
      ((double)media_bits / movie_info.seconds) / 1000.0;

  return;
}