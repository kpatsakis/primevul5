void LibRaw::setSonyBodyFeatures(unsigned long long id)
{
  ushort idx;
  static const struct
  {
    ushort scf[11];
    /*
    scf[0]  camera id
    scf[1]  camera format
    scf[2]  camera mount: Minolta A, Sony E, fixed,
    scf[3]  camera type: DSLR, NEX, SLT, ILCE, ILCA, DSC
    scf[4]  lens mount
    scf[5]  tag 0x2010 group (0 if not used)
    scf[6]  offset of Sony ISO in 0x2010 table, 0xffff if not valid
    scf[7]  offset of ShutterCount3 in 0x9050 table, 0xffff if not valid
    scf[8]  offset of MeteringMode in 0x2010 table, 0xffff if not valid
    scf[9]  offset of ExposureProgram in 0x2010 table, 0xffff if not valid
    scf[10] offset of ReleaseMode2 in 0x2010 table, 0xffff if not valid
    */
  } SonyCamFeatures[] = {
      {SonyID_DSLR_A100, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A900, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A700, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A200, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A350, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A300, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A900, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A380, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A330, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A230, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A290, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x10b, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x10c, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A850, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A850, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x10f, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x110, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A550, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A500, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A450, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x114, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x115, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_NEX_5, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_NEX_3, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_SLT_A33, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_SLT_A55, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A560, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSLR_A580, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_NEX_C3, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_SLT_A35, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_SLT_A65, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 2,
       0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {SonyID_SLT_A77, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 2,
       0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {SonyID_NEX_5N, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 1,
       0x113e, 0x01bd, 0x1174, 0x1175, 0x112c},
      {SonyID_NEX_7, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 2,
       0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {SonyID_NEX_VG20, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 2,
       0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {SonyID_SLT_A37, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 3,
       0x11f4, 0x01bd, 0x1154, 0x1155, 0x1108},
      {SonyID_SLT_A57, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 3,
       0x11f4, 0x01bd, 0x1154, 0x1155, 0x1108},
      {SonyID_NEX_F3, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 3,
       0x11f4, 0x01bd, 0x1154, 0x1155, 0x1108},
      {SonyID_SLT_A99, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {SonyID_NEX_6, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {SonyID_NEX_5R, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {SonyID_DSC_RX100, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 5, 0x1254, 0xffff, 0x11ac, 0x11ad, 0x1160},
      {SonyID_DSC_RX1, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 5, 0x1258, 0xffff, 0x11ac, 0x11ad, 0x1160},
      {SonyID_NEX_VG900, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {SonyID_NEX_VG30, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x12d, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_ILCE_3000, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 5,
       0x1280, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {SonyID_SLT_A58, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 5,
       0x1280, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x130, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_NEX_3N, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1280, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {SonyID_ILCE_7, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_NEX_5T, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {SonyID_DSC_RX100M2, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 6, 0x113c, 0xffff, 0x1064, 0x1065, 0x1018},
      {SonyID_DSC_RX10, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_DSC_RX1R, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 5, 0x1258, 0xffff, 0x11ac, 0x11ad, 0x1160},
      {SonyID_ILCE_7R, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_ILCE_6000, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_ILCE_5000, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0x01aa, 0x025c, 0x025d, 0x0210},
      {0x13a, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x13b, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x13c, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSC_RX100M3, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_ILCE_7S, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_ILCA_77M2, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0,
       7, 0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {0x140, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x141, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x142, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x143, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x144, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x145, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x146, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x147, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x148, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x149, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14a, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14b, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14c, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14d, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14e, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14f, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x150, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x151, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x152, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_ILCE_5100, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {SonyID_ILCE_7M2, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_DSC_RX100M4, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_DSC_RX10M2, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x157, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSC_RX1RM2, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x159, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_ILCE_QX1, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {SonyID_ILCE_7RM2, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8,
       0x0346, 0x01cb, 0x025c, 0x025d, 0x0210},
      {0x15c, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x15d, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_ILCE_7SM2, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8,
       0x0346, 0x01cb, 0x025c, 0x025d, 0x0210},
      {0x15f, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x160, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_ILCA_68, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0,
       7, 0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {SonyID_ILCA_99M2, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0, 8,
       0x0346, 0x01cd, 0x025c, 0x025d, 0x0210},
      {SonyID_DSC_RX10M3, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_DSC_RX100M5, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_ILCE_6300, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8,
       0x0346, 0x01cd, 0x025c, 0x025d, 0x0210},
      {SonyID_ILCE_9, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {0x167, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_ILCE_6500, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8,
       0x0346, 0x01cd, 0x025c, 0x025d, 0x0210},
      {0x169, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_ILCE_7RM3, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {SonyID_ILCE_7M3, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {SonyID_DSC_RX0, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {SonyID_DSC_RX10M4, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {SonyID_DSC_RX100M6, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {SonyID_DSC_HX99, LIBRAW_FORMAT_1div2p3INCH, LIBRAW_MOUNT_FixedLens,
       LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b,
       0x024c, 0x0208},
      {0x170, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSC_RX100M5A, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {0x172, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_ILCE_6400, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {SonyID_DSC_RX0M2, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {0x175, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_DSC_RX100M7, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {SonyID_ILCE_7RM4, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {SonyID_ILCE_9M2, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {0x179, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {SonyID_ILCE_6600, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {SonyID_ILCE_6100, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},

  };
  ilm.CamID = id;

  if (id == SonyID_DSC_R1)
  {
    ilm.CameraMount = ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    imSony.CameraType = LIBRAW_SONY_DSC;
    imSony.group2010 = 0;
    imSony.real_iso_offset = 0xffff;
    imSony.ImageCount3_offset = 0xffff;
    return;
  }
  else
    idx = id - 0x100ULL;

  if ((idx >= 0) && (idx < sizeof SonyCamFeatures / sizeof *SonyCamFeatures))
  {
    if (!SonyCamFeatures[idx].scf[2])
      return;
    ilm.CameraFormat = SonyCamFeatures[idx].scf[1];
    ilm.CameraMount = SonyCamFeatures[idx].scf[2];
    imSony.CameraType = SonyCamFeatures[idx].scf[3];
    if (SonyCamFeatures[idx].scf[4])
      ilm.LensMount = SonyCamFeatures[idx].scf[4];
    imSony.group2010 = SonyCamFeatures[idx].scf[5];
    imSony.real_iso_offset = SonyCamFeatures[idx].scf[6];
    imSony.ImageCount3_offset = SonyCamFeatures[idx].scf[7];
    imSony.MeteringMode_offset = SonyCamFeatures[idx].scf[8];
    imSony.ExposureProgram_offset = SonyCamFeatures[idx].scf[9];
    imSony.ReleaseMode2_offset = SonyCamFeatures[idx].scf[10];
  }

  char *sbstr = strstr(software, " v");
  if (sbstr != NULL)
  {
    sbstr += 2;
    imSony.firmware = atof(sbstr);

    if ((id == SonyID_ILCE_7) ||
        (id == SonyID_ILCE_7R))
    {
      if (imSony.firmware < 1.2f)
        imSony.ImageCount3_offset = 0x01aa;
      else
        imSony.ImageCount3_offset = 0x01c0;
    }
    else if (id == SonyID_ILCE_6000)
    {
      if (imSony.firmware < 2.0f)
        imSony.ImageCount3_offset = 0x01aa;
      else
        imSony.ImageCount3_offset = 0x01c0;
    }
    else if ((id == SonyID_ILCE_7S) ||
             (id == SonyID_ILCE_7M2))
    {
      if (imSony.firmware < 1.2f)
        imSony.ImageCount3_offset = 0x01a0;
      else
        imSony.ImageCount3_offset = 0x01b6;
    }
  }
}