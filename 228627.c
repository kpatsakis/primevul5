void APar_Extract_d263_Info(char *uint32_buffer,
                            FILE *isofile,
                            short track_level_atom,
                            TrackInfo *track_info) {
  uint64_t offset_into_d263 = 8;
  APar_readX(track_info->encoder_name,
             isofile,
             parsedAtoms[track_level_atom].AtomicStart + offset_into_d263,
             4);
  track_info->level = APar_read8(isofile,
                                 parsedAtoms[track_level_atom].AtomicStart +
                                     offset_into_d263 + 4 + 1);
  track_info->profile = APar_read8(isofile,
                                   parsedAtoms[track_level_atom].AtomicStart +
                                       offset_into_d263 + 4 + 2);
  // possible 'bitr' bitrate box afterwards
  return;
}