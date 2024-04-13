void APar_Extract_iods_Info(FILE *isofile, AtomicInfo *iods_atom) {
  uint64_t iods_offset = iods_atom->AtomicStart + 8;
  if (iods_atom->AtomicVerFlags == 0 &&
      APar_read8(isofile, iods_offset + 4) == 0x10) {
    iods_offset += 5;
    iods_offset += APar_skip_filler(isofile, iods_offset);
    uint8_t iods_objdescrip_len = APar_read8(isofile, iods_offset);
    iods_offset++;
    if (iods_objdescrip_len >= 7) {
      iods_info.od_profile_level = APar_read8(isofile, iods_offset + 2);
      iods_info.scene_profile_level = APar_read8(isofile, iods_offset + 3);
      iods_info.audio_profile = APar_read8(isofile, iods_offset + 4);
      iods_info.video_profile_level = APar_read8(isofile, iods_offset + 5);
      iods_info.graphics_profile_level = APar_read8(isofile, iods_offset + 6);
    }
  }
  return;
}