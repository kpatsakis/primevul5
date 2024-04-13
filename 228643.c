void APar_Extract_AMR_Info(char *uint32_buffer,
                           FILE *isofile,
                           short track_level_atom,
                           TrackInfo *track_info) {
  uint32_t amr_specific_offet = 8;
  APar_readX(track_info->encoder_name,
             isofile,
             parsedAtoms[track_level_atom].AtomicStart + amr_specific_offet,
             4);
  if (track_info->track_codec == 0x73616D72 ||
      track_info->track_codec == 0x73617762 ||
      track_info->track_codec ==
          0x73766D72) { // samr,sawb & svmr contain modes only
    track_info->amr_modes = APar_read16(
        uint32_buffer,
        isofile,
        parsedAtoms[track_level_atom].AtomicStart + amr_specific_offet + 4 + 1);
  }
  return;
}