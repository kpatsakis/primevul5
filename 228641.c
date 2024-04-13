void APar_ShowMPEG4VisualProfileInfo(TrackInfo *track_info) {
  fprintf(stdout, "  MPEG-4 Visual ");
  uint8_t mp4v_profile = 0;
  if (movie_info.contains_iods) {
    mp4v_profile = iods_info.video_profile_level;
  } else {
    mp4v_profile = track_info->m4v_profile;
  }

  // unparalleled joy - Annex G table g1 - a binary listing (this from
  // 14496-2:2001)
  if (mp4v_profile == 0x01) {
    fprintf(stdout, "Simple Profile, Level 1"); // 00000001
  } else if (mp4v_profile == 0x02) {
    fprintf(stdout, "Simple Profile, Level 2"); // 00000010
  } else if (mp4v_profile == 0x03) {
    fprintf(stdout,
            "Simple Profile, Level 3"); // most files will land here  //00000011

  } else if (mp4v_profile == 0x08) { // Compressor can create these in 3gp files
    fprintf(stdout, "Simple Profile, Level 0"); // ISO 14496-2:2004(e)
                                                // //00001000

    // Reserved 00000100 - 00000111
  } else if (mp4v_profile == 0x10) {
    fprintf(stdout, "Simple Scalable Profile, Level 0"); // 00010000
  } else if (mp4v_profile == 0x11) {
    fprintf(stdout, "Simple Scalable Profile, Level 1"); // 00010001
  } else if (mp4v_profile == 0x12) {
    fprintf(stdout, "Simple Scalable Profile, Level 2"); // 00010010

    // Reserved 00010011 - 00100000
  } else if (mp4v_profile == 0x21) {
    fprintf(stdout, "Core Profile, Level 1"); // 00100001
  } else if (mp4v_profile == 0x22) {
    fprintf(stdout, "Core Profile, Level 2"); // 00100010

    // Reserved 00100011 - 00110001
  } else if (mp4v_profile == 0x32) {
    fprintf(stdout, "Main Profile, Level 2"); // 00110010
  } else if (mp4v_profile == 0x33) {
    fprintf(stdout, "Main Profile, Level 3"); // 00110011
  } else if (mp4v_profile == 0x34) {
    fprintf(stdout, "Main Profile, Level 4"); // 00110100

    // Reserved 00110101 - 01000001
  } else if (mp4v_profile == 0x42) {
    fprintf(stdout, "N-bit Profile, Level 2"); // 01000010

    // Reserved 01000011 - 01010000
  } else if (mp4v_profile == 0x51) {
    fprintf(stdout, "Scalable Texture Profile, Level 1"); // 01010001

    // Reserved 01010010 - 01100000
  } else if (mp4v_profile == 0x61) {
    fprintf(stdout, "Simple Face Animation, Level 1"); // 01100001
  } else if (mp4v_profile == 0x62) {
    fprintf(stdout, "Simple Face Animation, Level 2"); // 01100010

  } else if (mp4v_profile == 0x63) {
    fprintf(stdout, "Simple FBA Profile, Level 1"); // 01100011
  } else if (mp4v_profile == 0x64) {
    fprintf(stdout, "Simple FBA Profile, Level 2"); // 01100100

    // Reserved 01100101 - 01110000
  } else if (mp4v_profile == 0x71) {
    fprintf(stdout, "Basic Animated Texture Profile, Level 1"); // 01110001
  } else if (mp4v_profile == 0x72) {
    fprintf(stdout, "Basic Animated Texture Profile, Level 2"); // 01110010

    // Reserved 01110011 - 10000000
  } else if (mp4v_profile == 0x81) {
    fprintf(stdout, "Hybrid Profile, Level 1"); // 10000001
  } else if (mp4v_profile == 0x82) {
    fprintf(stdout, "Hybrid Profile, Level 2"); // 10000010

    // Reserved 10000011 - 10010000
  } else if (mp4v_profile == 0x91) {
    fprintf(stdout, "Advanced Real Time Simple Profile, Level 1"); // 10010001
  } else if (mp4v_profile == 0x92) {
    fprintf(stdout, "Advanced Real Time Simple Profile, Level 2"); // 10010010
  } else if (mp4v_profile == 0x93) {
    fprintf(stdout, "Advanced Real Time Simple Profile, Level 3"); // 10010011
  } else if (mp4v_profile == 0x94) {
    fprintf(stdout, "Advanced Real Time Simple Profile, Level 4"); // 10010100

    // Reserved 10010101 - 10100000
  } else if (mp4v_profile == 0xA1) {
    fprintf(stdout, "Core Scalable Profile, Level 1"); // 10100001
  } else if (mp4v_profile == 0xA2) {
    fprintf(stdout, "Core Scalable Profile, Level 2"); // 10100010
  } else if (mp4v_profile == 0xA3) {
    fprintf(stdout, "Core Scalable Profile, Level 3"); // 10100011

    // Reserved 10100100 - 10110000
  } else if (mp4v_profile == 0xB1) {
    fprintf(stdout, "Advanced Coding Efficiency Profile, Level 1"); // 10110001
  } else if (mp4v_profile == 0xB2) {
    fprintf(stdout, "Advanced Coding Efficiency Profile, Level 2"); // 10110010
  } else if (mp4v_profile == 0xB3) {
    fprintf(stdout, "Advanced Coding Efficiency Profile, Level 3"); // 10110011
  } else if (mp4v_profile == 0xB4) {
    fprintf(stdout, "Advanced Coding Efficiency Profile, Level 4"); // 10110100

    // Reserved 10110101  11000000
  } else if (mp4v_profile == 0xC1) {
    fprintf(stdout, "Advanced Core Profile, Level 1"); // 11000001
  } else if (mp4v_profile == 0xC2) {
    fprintf(stdout, "Advanced Core Profile, Level 2"); // 11000010

    // Reserved 11000011  11010000
  } else if (mp4v_profile == 0xD1) {
    fprintf(stdout, "Advanced Scalable Texture, Level 1"); // 11010001
  } else if (mp4v_profile == 0xD2) {
    fprintf(stdout, "Advanced Scalable Texture, Level 2"); // 11010010
  } else if (mp4v_profile == 0xD2) {
    fprintf(stdout, "Advanced Scalable Texture, Level 3"); // 11010011

    // from a draft document - 1999 (earlier than the 2000 above!!)
  } else if (mp4v_profile == 0xE1) {
    fprintf(stdout, "Simple Studio Profile, Level 1"); // 11100001
  } else if (mp4v_profile == 0xE2) {
    fprintf(stdout, "Simple Studio Profile, Level 2"); // 11100010
  } else if (mp4v_profile == 0xE3) {
    fprintf(stdout, "Simple Studio Profile, Level 3"); // 11100011
  } else if (mp4v_profile == 0xE4) {
    fprintf(stdout, "Simple Studio Profile, Level 4"); // 11100100

  } else if (mp4v_profile == 0xE5) {
    fprintf(stdout, "Core Studio Profile, Level 1"); // 11100101
  } else if (mp4v_profile == 0xE6) {
    fprintf(stdout, "Core Studio Profile, Level 2"); // 11100110
  } else if (mp4v_profile == 0xE7) {
    fprintf(stdout, "Core Studio Profile, Level 3"); // 11100111
  } else if (mp4v_profile == 0xE8) {
    fprintf(stdout, "Core Studio Profile, Level 4"); // 11101000

    // Reserved 11101001 - 11101111
    // ISO 14496-2:2004(e)
  } else if (mp4v_profile == 0xF0) {
    fprintf(stdout, "Advanced Simple Profile, Level 0"); // 11110000
  } else if (mp4v_profile == 0xF1) {
    fprintf(stdout, "Advanced Simple Profile, Level 1"); // 11110001
  } else if (mp4v_profile == 0xF2) {
    fprintf(
        stdout,
        "Advanced Simple Profile, Level 2"); // 11110010  ////3gp files that QT
                                             // says is H.263 have esds to 0xF2
                                             // & their ObjectType set to 0x20
                                             // (mpeg-4 visual)
                                             ////...and its been figured out -
                                             /// FILE EXTENSION of all things
                                             /// determines mpeg-4 ASP or H.263
  } else if (mp4v_profile == 0xF3) {
    fprintf(stdout, "Advanced Simple Profile, Level 3"); // 11110011
  } else if (mp4v_profile == 0xF4) {
    fprintf(stdout, "Advanced Simple Profile, Level 4"); // 11110100
  } else if (mp4v_profile == 0xF5) {
    fprintf(stdout, "Advanced Simple Profile, Level 5"); // 11110101

    // Reserved 11110110
  } else if (mp4v_profile == 0xF7) {
    fprintf(stdout, "Advanced Simple Profile, Level 3b"); // 11110111

  } else if (mp4v_profile == 0xF7) {
    fprintf(stdout, "Fine Granularity Scalable Profile/Level 0"); // 11111000
  } else if (mp4v_profile == 0xF7) {
    fprintf(stdout, "Fine Granularity Scalable Profile/Level 1"); // 11111001
  } else if (mp4v_profile == 0xF7) {
    fprintf(stdout, "Fine Granularity Scalable Profile/Level 2"); // 11111010
  } else if (mp4v_profile == 0xF7) {
    fprintf(stdout, "Fine Granularity Scalable Profile/Level 3"); // 11111011
  } else if (mp4v_profile == 0xF7) {
    fprintf(stdout, "Fine Granularity Scalable Profile/Level 4"); // 11111100
  } else if (mp4v_profile == 0xF7) {
    fprintf(stdout, "Fine Granularity Scalable Profile/Level 5"); // 11111101

    // Reserved 11111110
    // Reserved for Escape 11111111

  } else {
    fprintf(stdout, "Unknown profile: 0x%X", mp4v_profile);
  }
  return;
}