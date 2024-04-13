void APar_ExtractBrands(char *filepath) {
  FILE *a_file = APar_OpenISOBaseMediaFile(filepath, true);
  char buffer[16] = {};
  uint32_t atom_length = 0;
  uint8_t file_type_offset = 0;
  uint32_t compatible_brand = 0;
  bool cb_V2ISOBMFF = false;

  APar_read32(buffer, a_file, 4);
  if (memcmp(buffer, "ftyp", 4) == 0) {
    atom_length = APar_read32(buffer, a_file, 0);
  } else {
    APar_readX(buffer, a_file, 0, 12);
    if (memcmp(buffer,
               "\x00\x00\x00\x0C\x6A\x50\x20\x20\x0D\x0A\x87\x0A",
               12) == 0) {
      APar_readX(buffer, a_file, 12, 12);
      if (memcmp(buffer + 4, "ftypmjp2", 8) == 0 ||
          memcmp(buffer + 4, "ftypmj2s", 8) == 0) {
        atom_length = UInt32FromBigEndian(buffer);
        file_type_offset = 12;
      }
    }
  }

  if (atom_length > 0) {
    memset(buffer, 0, 16);
    APar_readX(buffer, a_file, 8 + file_type_offset, 4);
    printBOM();
    fprintf(stdout, " Major Brand: %s", buffer);
    APar_IdentifyBrand(buffer);

    if (memcmp(buffer, "isom", 4) == 0) {
      APar_ScanAtoms(filepath); // scan_file = true;
    }

    uint32_t minor_version = APar_read32(buffer, a_file, 12 + file_type_offset);
    fprintf(stdout, "  -  version %" PRIu32 "\n", minor_version);

    fprintf(stdout, " Compatible Brands:");
    for (uint64_t i = 16 + file_type_offset; i < atom_length; i += 4) {
      APar_readX(buffer, a_file, i, 4);
      compatible_brand = UInt32FromBigEndian(buffer);
      if (compatible_brand != 0) {
        fprintf(stdout, " %s", buffer);
        if (compatible_brand == 0x6D703432 || compatible_brand == 0x69736F32) {
          cb_V2ISOBMFF = true;
        }
      }
    }
    fprintf(stdout, "\n");
  }

  APar_OpenISOBaseMediaFile(filepath, false);

  fprintf(stdout, " Tagging schemes available:\n");
  switch (metadata_style) {
  case ITUNES_STYLE: {
    fprintf(stdout, "   iTunes-style metadata allowed.\n");
    break;
  }
  case THIRD_GEN_PARTNER:
  case THIRD_GEN_PARTNER_VER1_REL6:
  case THIRD_GEN_PARTNER_VER1_REL7:
  case THIRD_GEN_PARTNER_VER2: {
    fprintf(stdout, "   3GP-style asset metadata allowed.\n");
    break;
  }
  case THIRD_GEN_PARTNER_VER2_REL_A: {
    fprintf(stdout,
            "   3GP-style asset metadata allowed [& unimplemented GAD "
            "(Geographical Area Description) asset].\n");
    break;
  }
  }
  if (cb_V2ISOBMFF || metadata_style == THIRD_GEN_PARTNER_VER1_REL7) {
    fprintf(stdout,
            "   ID3 tags on ID32 atoms @ file/movie/track level allowed.\n");
  }
  fprintf(stdout,
          "   ISO-copyright notices @ movie and/or track level "
          "allowed.\n   uuid private user extension tags allowed.\n");
}