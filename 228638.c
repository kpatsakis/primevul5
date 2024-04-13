void APar_ShowMPEG4AACProfileInfo(TrackInfo *track_info) {
  if (track_info->descriptor_object_typeID == 1) {
    fprintf(stdout, "  MPEG-4 AAC Main Profile");
  } else if (track_info->descriptor_object_typeID == 2) {
    fprintf(
        stdout,
        "  MPEG-4 AAC Low Complexity/LC Profile"); // most files will land here
  } else if (track_info->descriptor_object_typeID == 3) {
    fprintf(stdout, "  MPEG-4 AAC Scaleable Sample Rate/SSR Profile");
  } else if (track_info->descriptor_object_typeID == 4) {
    fprintf(stdout, "  MPEG-4 AAC Long Term Prediction Profile");
  } else if (track_info->descriptor_object_typeID == 5) {
    fprintf(stdout, "  MPEG-4 AAC High Efficiency/HE Profile");
  } else if (track_info->descriptor_object_typeID == 6) {
    fprintf(stdout, "  MPEG-4 AAC Scalable Profile");
  } else if (track_info->descriptor_object_typeID == 7) {
    fprintf(stdout,
            "  MPEG-4 AAC Transform domain Weighted INterleave Vector "
            "Quantization/TwinVQ Profile");
  } else if (track_info->descriptor_object_typeID == 8) {
    fprintf(stdout, "  MPEG-4 AAC Code Excited Linear Predictive/CELP Profile");
  } else if (track_info->descriptor_object_typeID == 9) {
    fprintf(stdout, "  MPEG-4 AAC HVXC Profile");

  } else if (track_info->descriptor_object_typeID == 12) {
    fprintf(stdout, "  MPEG-4 AAC TTSI Profile");
  } else if (track_info->descriptor_object_typeID == 13) {
    fprintf(stdout, "  MPEG-4 AAC Main Synthesis Profile");
  } else if (track_info->descriptor_object_typeID == 14) {
    fprintf(stdout, "  MPEG-4 AAC Wavetable Synthesis Profile");
  } else if (track_info->descriptor_object_typeID == 15) {
    fprintf(stdout, "  MPEG-4 AAC General MIDI Profile");
  } else if (track_info->descriptor_object_typeID == 16) {
    fprintf(stdout, "  MPEG-4 AAC Algorithmic Synthesis & Audio FX Profile");
  } else if (track_info->descriptor_object_typeID == 17) {
    fprintf(stdout,
            "  MPEG-4 AAC AAC Low Complexity/LC (+error recovery) Profile");

  } else if (track_info->descriptor_object_typeID == 19) {
    fprintf(stdout,
            "  MPEG-4 AAC Long Term Prediction (+error recovery) Profile");
  } else if (track_info->descriptor_object_typeID == 20) {
    fprintf(stdout, "  MPEG-4 AAC Scalable (+error recovery) Profile");
  } else if (track_info->descriptor_object_typeID == 21) {
    fprintf(stdout,
            "  MPEG-4 AAC Transform domain Weighted INterleave Vector "
            "Quantization/TwinVQ (+error recovery) Profile");
  } else if (track_info->descriptor_object_typeID == 22) {
    fprintf(stdout,
            "  MPEG-4 AAC Bit Sliced Arithmetic Coding/BSAC (+error "
            "recovery) Profile");
  } else if (track_info->descriptor_object_typeID == 23) {
    fprintf(stdout, "  MPEG-4 AAC Low Delay/LD (+error recovery) Profile");
  } else if (track_info->descriptor_object_typeID == 24) {
    fprintf(stdout,
            "  MPEG-4 AAC Code Excited Linear Predictive/CELP (+error "
            "recovery) Profile");
  } else if (track_info->descriptor_object_typeID == 25) {
    fprintf(stdout, "  MPEG-4 AAC HXVC (+error recovery) Profile");
  } else if (track_info->descriptor_object_typeID == 26) {
    fprintf(stdout,
            "  MPEG-4 AAC Harmonic and Individual Lines plus "
            "Noise/HILN (+error recovery) Profile");
  } else if (track_info->descriptor_object_typeID == 27) {
    fprintf(stdout, "  MPEG-4 AAC Parametric (+error recovery) Profile");

  } else if (track_info->descriptor_object_typeID == 31) {
    fprintf(
        stdout,
        "  MPEG-4 ALS Audio Lossless Coding"); // I think that mp4alsRM18 writes
                                               // the channels wrong after
                                               // objectedID: 0xF880 has 0
                                               // channels; 0xF890 is 2ch
  } else {
    fprintf(stdout,
            "  MPEG-4 Unknown profile: 0x%X",
            track_info->descriptor_object_typeID);
  }
  return;
}