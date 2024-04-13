void LibRaw::parseSonyMakernotes(
    int base, unsigned tag, unsigned type, unsigned len, unsigned dng_writer,
    uchar *&table_buf_0x0116, ushort &table_buf_0x0116_len,
    uchar *&table_buf_0x2010, ushort &table_buf_0x2010_len,
    uchar *&table_buf_0x9050, ushort &table_buf_0x9050_len,
    uchar *&table_buf_0x9400, ushort &table_buf_0x9400_len,
    uchar *&table_buf_0x9402, ushort &table_buf_0x9402_len,
    uchar *&table_buf_0x9403, ushort &table_buf_0x9403_len,
    uchar *&table_buf_0x9406, ushort &table_buf_0x9406_len,
    uchar *&table_buf_0x940c, ushort &table_buf_0x940c_len,
    uchar *&table_buf_0x940e, ushort &table_buf_0x940e_len)
{

  ushort lid, a, b, c, d;
  uchar *table_buf;
  uchar uc;
  uchar s[2];
  int LensDataValid = 0;
  unsigned uitemp;

  if (tag == 0xb001)
  { // Sony ModelID
    unique_id = get2();
    setSonyBodyFeatures(unique_id);

    if (table_buf_0x0116_len)
    {
      process_Sony_0x0116(table_buf_0x0116, table_buf_0x0116_len, unique_id);
      free(table_buf_0x0116);
      table_buf_0x0116_len = 0;
    }

    if (table_buf_0x2010_len)
    {
      process_Sony_0x2010(table_buf_0x2010, table_buf_0x2010_len);
      free(table_buf_0x2010);
      table_buf_0x2010_len = 0;
    }

    if (table_buf_0x9050_len)
    {
      process_Sony_0x9050(table_buf_0x9050, table_buf_0x9050_len, unique_id);
      free(table_buf_0x9050);
      table_buf_0x9050_len = 0;
    }

    if (table_buf_0x9400_len)
    {
      process_Sony_0x9400(table_buf_0x9400, table_buf_0x9400_len, unique_id);
      free(table_buf_0x9400);
      table_buf_0x9400_len = 0;
    }

    if (table_buf_0x9402_len)
    {
      process_Sony_0x9402(table_buf_0x9402, table_buf_0x9402_len);
      free(table_buf_0x9402);
      table_buf_0x9402_len = 0;
    }

    if (table_buf_0x9403_len)
    {
      process_Sony_0x9403(table_buf_0x9403, table_buf_0x9403_len);
      free(table_buf_0x9403);
      table_buf_0x9403_len = 0;
    }

    if (table_buf_0x9406_len)
    {
      process_Sony_0x9406(table_buf_0x9406, table_buf_0x9406_len);
      free(table_buf_0x9406);
      table_buf_0x9406_len = 0;
    }

    if (table_buf_0x940c_len)
    {
      process_Sony_0x940c(table_buf_0x940c, table_buf_0x940c_len);
      free(table_buf_0x940c);
      table_buf_0x940c_len = 0;
    }

    if (table_buf_0x940e_len)
    {
      process_Sony_0x940e(table_buf_0x940e, table_buf_0x940e_len, unique_id);
      free(table_buf_0x940e);
      table_buf_0x940e_len = 0;
    }
  }
  else if (tag == 0xb000)
  {
    FORC4 imSony.FileFormat = imSony.FileFormat * 10 + fgetc(ifp);
  }
  else if (tag == 0xb026)
  {
    uitemp = get4();
    if (uitemp != 0xffffffff)
      imgdata.shootinginfo.ImageStabilization = uitemp;
  }
  else if (((tag == 0x0001) || // Minolta CameraSettings, big endian
            (tag == 0x0003)) &&
           (len >= 196))
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);

    lid = 0x01 << 2;
    imgdata.shootinginfo.ExposureMode =
        (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
        (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];

    lid = 0x06 << 2;
    imgdata.shootinginfo.DriveMode =
        (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
        (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];

    lid = 0x07 << 2;
    imgdata.shootinginfo.MeteringMode =
        (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
        (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];

    lid = 0x25 << 2;
    imSony.MinoltaCamID =
        (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
        (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];
    if (imSony.MinoltaCamID != -1)
      ilm.CamID = imSony.MinoltaCamID;

    lid = 0x30 << 2;
    imgdata.shootinginfo.FocusMode =
        (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
        (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];

    free(table_buf);
  }
  else if ((tag == 0x0004) && // Minolta CameraSettings7D, big endian
           (len >= 227))
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);

    lid = 0x0;
    imgdata.shootinginfo.ExposureMode =
        (ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1];

    lid = 0x0e << 1;
    imgdata.shootinginfo.FocusMode =
        (ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1];

    lid = 0x10 << 1;
    imgdata.shootinginfo.AFPoint =
        (ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1];

    lid = 0x25 << 1;
    switch ((ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1]) {
    case 0:
    case 1:
      imCommon.ColorSpace = LIBRAW_COLORSPACE_sRGB;
      break;
    case 4:
      imCommon.ColorSpace = LIBRAW_COLORSPACE_AdobeRGB;
      break;
    default:
      imCommon.ColorSpace = LIBRAW_COLORSPACE_Unknown;
      break;
    }

    lid = 0x71 << 1;
    imgdata.shootinginfo.ImageStabilization =
        (ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1];

    free(table_buf);
  }
  else if ((tag == 0x0010) && // CameraInfo
           strncasecmp(model, "DSLR-A100", 9) &&
           !strncasecmp(make, "SONY", 4) &&
           ((len == 368) ||  // a700                         : CameraInfo
            (len == 5478) || // a850, a900                   : CameraInfo
            (len == 5506) || // a200, a300, a350             : CameraInfo2
            (len == 6118) || // a230, a290, a330, a380, a390 : CameraInfo2
            (len == 15360))  // a450, a500, a550, a560, a580 : CameraInfo3
                             // a33, a35, a55
                             // NEX-3, NEX-5, NEX-5C, NEX-C3, NEX-VG10E

  )
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);
    if (memcmp(table_buf, "\xff\xff\xff\xff\xff\xff\xff\xff", 8) &&
        memcmp(table_buf, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))
    {
      LensDataValid = 1;
    }
    switch (len)
    {
    case 368:  // a700: CameraInfo
    case 5478: // a850, a900: CameraInfo
      if ((!dng_writer) ||
          (saneSonyCameraInfo(table_buf[0], table_buf[3], table_buf[2],
                              table_buf[5], table_buf[4], table_buf[7])))
      {
        if (LensDataValid)
        {
          if (table_buf[0] | table_buf[3])
            ilm.MinFocal = bcd2dec(table_buf[0]) * 100 + bcd2dec(table_buf[3]);
          if (table_buf[2] | table_buf[5])
            ilm.MaxFocal = bcd2dec(table_buf[2]) * 100 + bcd2dec(table_buf[5]);
          if (table_buf[4])
            ilm.MaxAp4MinFocal = bcd2dec(table_buf[4]) / 10.0f;
          if (table_buf[4])
            ilm.MaxAp4MaxFocal = bcd2dec(table_buf[7]) / 10.0f;
          parseSonyLensFeatures(table_buf[1], table_buf[6]);
        }

        imSony.AFPointSelected = table_buf[21];
        imgdata.shootinginfo.AFPoint = (ushort)table_buf[25];

        if (len == 5478)
        {
          imSony.AFMicroAdjValue = table_buf[304] - 20;
          imSony.AFMicroAdjOn = (((table_buf[305] & 0x80) == 0x80) ? 1 : 0);
          imSony.AFMicroAdjRegisteredLenses = table_buf[305] & 0x7f;
        }
      }
      break;
    default:
      // CameraInfo2 & 3
      if ((!dng_writer) ||
          (saneSonyCameraInfo(table_buf[1], table_buf[2], table_buf[3],
                              table_buf[4], table_buf[5], table_buf[6])))
      {
        if ((LensDataValid) && strncasecmp(model, "NEX-5C", 6))
        {
          if (table_buf[1] | table_buf[2])
            ilm.MinFocal = bcd2dec(table_buf[1]) * 100 + bcd2dec(table_buf[2]);
          if (table_buf[3] | table_buf[4])
            ilm.MaxFocal = bcd2dec(table_buf[3]) * 100 + bcd2dec(table_buf[4]);
          if (table_buf[5])
            ilm.MaxAp4MinFocal = bcd2dec(table_buf[5]) / 10.0f;
          if (table_buf[6])
            ilm.MaxAp4MaxFocal = bcd2dec(table_buf[6]) / 10.0f;
          parseSonyLensFeatures(table_buf[0], table_buf[7]);
        }

        if (!strncasecmp(model, "DSLR-A450", 9) ||
            !strncasecmp(model, "DSLR-A500", 9) ||
            !strncasecmp(model, "DSLR-A550", 9))
        {
          imSony.AFPointSelected = table_buf[0x14];
          imgdata.shootinginfo.FocusMode = table_buf[0x15];
          imgdata.shootinginfo.AFPoint = (ushort)table_buf[0x18];
        }
        else if (!strncasecmp(model, "SLT-", 4) ||
                 !strncasecmp(model, "DSLR-A560", 9) ||
                 !strncasecmp(model, "DSLR-A580", 9))
        {
          imSony.AFPointSelected = table_buf[0x1c];
          imgdata.shootinginfo.FocusMode = table_buf[0x1d];
          imgdata.shootinginfo.AFPoint = (ushort)table_buf[0x20];
        }
      }
    }
    free(table_buf);
  }
  else if ((!dng_writer) && ((tag == 0x0020) || (tag == 0xb0280020)))
  {
    if (!strncasecmp(model, "DSLR-A100", 9))
    { // WBInfoA100
      fseek(ifp, 0x49dc, SEEK_CUR);
      stmread(imgdata.shootinginfo.InternalBodySerial, 13, ifp);
    }
    else if ((len ==
              19154) || // a200 a230 a290 a300 a330 a350 a380 a390 : FocusInfo
             (len == 19148))
    { // a700 a850 a900                          : FocusInfo
      table_buf = (uchar *)malloc(128);
      fread(table_buf, 128, 1, ifp);
      imgdata.shootinginfo.DriveMode = table_buf[14];
      imgdata.shootinginfo.ExposureProgram = table_buf[63];
      free(table_buf);
    }
    else if (len == 20480) // a450 a500 a550 a560 a580 a33 a35 a55 : MoreInfo
                           // NEX-3 NEX-5 NEX-C3 NEX-VG10E         : MoreInfo
    {
      a = get2();
      b = get2();
      c = get2();
      d = get2();
      if ((a) && (c == 1))
      {
        fseek(ifp, d - 8, SEEK_CUR);
        table_buf = (uchar *)malloc(256);
        fread(table_buf, 256, 1, ifp);
        imgdata.shootinginfo.DriveMode = table_buf[1];
        imgdata.shootinginfo.ExposureProgram = table_buf[2];
        imgdata.shootinginfo.MeteringMode = table_buf[3];
        switch (table_buf[6]) {
        case 1:
          imCommon.ColorSpace = LIBRAW_COLORSPACE_sRGB;
          break;
        case 2:
          imCommon.ColorSpace = LIBRAW_COLORSPACE_AdobeRGB;
          break;
        default:
          imCommon.ColorSpace = LIBRAW_COLORSPACE_Unknown;
          break;
        }
        if (strncasecmp(model, "DSLR-A450", 9) &&
            strncasecmp(model, "DSLR-A500", 9) &&
            strncasecmp(model, "DSLR-A550", 9))
          imgdata.shootinginfo.FocusMode = table_buf[0x13];
        else
          imgdata.shootinginfo.FocusMode = table_buf[0x2c];
        free(table_buf);
      }
    }
  }
  else if (tag == 0x0102)
  {
    imSony.Quality = get4();
  }
  else if (tag == 0x0104)
  {
    imCommon.FlashEC = getreal(type);
  }
  else if (tag == 0x0105)
  { // Teleconverter
    ilm.TeleconverterID = get4();
  }
  else if (tag == 0x0107)
  {
    uitemp = get4();
    if (uitemp == 1)
      imgdata.shootinginfo.ImageStabilization = 0;
    else if (uitemp == 5)
      imgdata.shootinginfo.ImageStabilization = 1;
    else
      imgdata.shootinginfo.ImageStabilization = uitemp;
  }
  else if ((tag == 0xb0280088) && (dng_writer == nonDNG))
  {
    thumb_offset = get4() + base;
  }
  else if ((tag == 0xb0280089) && (dng_writer == nonDNG))
  {
    thumb_length = get4();
  }
  else if (((tag == 0x0114) || // CameraSettings
            (tag == 0xb0280114)) &&
           (len < 256000))
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);
    switch (len)
    {
    case 260: // Sony a100, big endian
      imgdata.shootinginfo.ExposureMode =
          ((ushort)table_buf[0]) << 8 | ((ushort)table_buf[1]);
      lid = 0x0a << 1;
      imgdata.shootinginfo.DriveMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1]);
      lid = 0x0c << 1;
      imgdata.shootinginfo.FocusMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1]);
      lid = 0x0d << 1;
      imSony.AFPointSelected = table_buf[lid + 1];
      lid = 0x0e << 1;
      imSony.AFAreaModeSetting = table_buf[lid + 1];
      lid = 0x12 << 1;
      imgdata.shootinginfo.MeteringMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1]);

      lid = 0x17 << 1;
      switch ((ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1]) {
      case 0:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_sRGB;
        break;
      case 2:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_MonochromeGamma;
        break;
      case 5:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_AdobeRGB;
        break;
      default:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_Unknown;
        break;
      }

      break;
    case 448: // Minolta "DYNAX 5D" and its aliases, big endian
      lid = 0x0a << 1;
      imgdata.shootinginfo.ExposureMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1]);
      lid = 0x25 << 1;
      imgdata.shootinginfo.MeteringMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1]);

      lid = 0x2f << 1;
      switch ((ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1]) {
      case 0:
      case 1:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_sRGB;
        break;
      case 2:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_MonochromeGamma;
        break;
      case 4:
      case 5:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_AdobeRGB;
        break;
      default:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_Unknown;
        break;
      }

      lid = 0xbd << 1;
      imgdata.shootinginfo.ImageStabilization =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1]);
      break;
    case 280: // a200 a300 a350 a700
    case 364: // a850 a900
      // CameraSettings and CameraSettings2 are big endian
      if (table_buf[2] | table_buf[3])
      {
        lid = (((ushort)table_buf[2]) << 8) | ((ushort)table_buf[3]);
        ilm.CurAp = libraw_powf64l(2.0f, ((float)lid / 8.0f - 1.0f) / 2.0f);
      }
      lid = 0x04 << 1;
      imgdata.shootinginfo.DriveMode = table_buf[lid + 1];
      lid = 0x1b << 1;
      switch (((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1])) {
      case 0:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_sRGB;
        break;
      case 1:
      case 5:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_AdobeRGB;
        break;
      default:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_Unknown;
        break;
      }
      lid = 0x4d << 1;
      imgdata.shootinginfo.FocusMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1]);
      if (!imCommon.ColorSpace ||
          (imCommon.ColorSpace == LIBRAW_COLORSPACE_Unknown)) {
        lid = 0x83 << 1;
        switch (((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1])) {
        case 6:
          imCommon.ColorSpace = LIBRAW_COLORSPACE_sRGB;
          break;
        case 5:
          imCommon.ColorSpace = LIBRAW_COLORSPACE_AdobeRGB;
          break;
        default:
          imCommon.ColorSpace = LIBRAW_COLORSPACE_Unknown;
          break;
        }
      }
      break;
    case 332: // a230 a290 a330 a380 a390
      // CameraSettings and CameraSettings2 are big endian
      if (table_buf[2] | table_buf[3])
      {
        lid = (((ushort)table_buf[2]) << 8) | ((ushort)table_buf[3]);
        ilm.CurAp = libraw_powf64l(2.0f, ((float)lid / 8.0f - 1.0f) / 2.0f);
      }
      lid = 0x4d << 1;
      imgdata.shootinginfo.FocusMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid + 1]);
      lid = 0x7e << 1;
      imgdata.shootinginfo.DriveMode = table_buf[lid + 1];
      break;
    case 1536: // a560 a580 a33 a35 a55 NEX-3 NEX-5 NEX-5C NEX-C3 NEX-VG10E
    case 2048: // a450 a500 a550
      // CameraSettings3 are little endian
      switch (table_buf[0x0e]) {
      case 1:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_sRGB;
        break;
      case 2:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_AdobeRGB;
        break;
      default:
        imCommon.ColorSpace = LIBRAW_COLORSPACE_Unknown;
        break;
      }
      imgdata.shootinginfo.DriveMode = table_buf[0x34];
      parseSonyLensType2(table_buf[1016], table_buf[1015]);
      if (ilm.LensMount != LIBRAW_MOUNT_Canon_EF)
      {
        switch (table_buf[153])
        {
        case 16:
          ilm.LensMount = LIBRAW_MOUNT_Minolta_A;
          break;
        case 17:
          ilm.LensMount = LIBRAW_MOUNT_Sony_E;
          break;
        }
      }
      break;
    }
    free(table_buf);
  }
  else if ((tag == 0x3000) && (len < 256000))
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);
    for (int i = 0; i < 20; i++)
      imSony.SonyDateTime[i] = table_buf[6 + i];
    free(table_buf);
  }
  else if (tag == 0x0116 && len < 256000)
  {
    table_buf_0x0116 = (uchar *)malloc(len);
    table_buf_0x0116_len = len;
    fread(table_buf_0x0116, len, 1, ifp);
    if (ilm.CamID)
    {
      process_Sony_0x0116(table_buf_0x0116, table_buf_0x0116_len, ilm.CamID);
      free(table_buf_0x0116);
      table_buf_0x0116_len = 0;
    }
  }
  else if (tag == 0x2008)
  {
    imSony.LongExposureNoiseReduction = get4();
  }
  else if (tag == 0x2009)
  {
    imSony.HighISONoiseReduction = get2();
  }
  else if (tag == 0x200a)
  {
    imSony.HDR[0] = get2();
    imSony.HDR[1] = get2();
  }
  else if (tag == 0x2010 && len < 256000)
  {
    table_buf_0x2010 = (uchar *)malloc(len);
    table_buf_0x2010_len = len;
    fread(table_buf_0x2010, len, 1, ifp);
    if (ilm.CamID)
    {
      process_Sony_0x2010(table_buf_0x2010, table_buf_0x2010_len);
      free(table_buf_0x2010);
      table_buf_0x2010_len = 0;
    }
  }
  else if (tag == 0x201a)
  {
    imSony.ElectronicFrontCurtainShutter = get4();
  }
  else if (tag == 0x201b)
  {
    if ((imSony.CameraType != LIBRAW_SONY_DSC) ||
        (ilm.CamID == SonyID_DSC_RX10M4)   ||
        (ilm.CamID == SonyID_DSC_RX100M6)  ||
        (ilm.CamID == SonyID_DSC_RX100M5A) ||
        (ilm.CamID == SonyID_DSC_RX0M2)    ||
        (ilm.CamID == SonyID_DSC_RX100M7))
    {
      fread(&uc, 1, 1, ifp);
      imgdata.shootinginfo.FocusMode = (short)uc;
    }
  }
  else if (tag == 0x201c)
  {
    if ((imSony.CameraType != LIBRAW_SONY_DSC) ||
        (ilm.CamID == SonyID_DSC_RX10M4)   ||
        (ilm.CamID == SonyID_DSC_RX100M6)  ||
        (ilm.CamID == SonyID_DSC_RX100M5A) ||
        (ilm.CamID == SonyID_DSC_RX0M2)    ||
        (ilm.CamID == SonyID_DSC_RX100M7))
    {
      imSony.AFAreaModeSetting = fgetc(ifp);
    }
  }
  else if (tag == 0x201d)
  {
    if (((imSony.AFAreaModeSetting == 3) &&
         ((imSony.CameraType == LIBRAW_SONY_ILCE) ||
          (imSony.CameraType == LIBRAW_SONY_NEX) ||
          (ilm.CamID == SonyID_DSC_RX10M4)    ||
          (ilm.CamID == SonyID_DSC_RX100M6)   ||
          (ilm.CamID == SonyID_DSC_RX100M5A)  ||
          (ilm.CamID == SonyID_DSC_RX0M2)     ||
          (ilm.CamID == SonyID_DSC_RX100M7))) ||
        ((imSony.AFAreaModeSetting == 4) &&
         (imSony.CameraType == LIBRAW_SONY_ILCA)))
    {
      imSony.FlexibleSpotPosition[0] = get2();
      imSony.FlexibleSpotPosition[1] = get2();
    }
  }
  else if (tag == 0x201e)
  {
    if (imSony.CameraType != LIBRAW_SONY_DSC)
    {
      imSony.AFPointSelected = fgetc(ifp);
    }
  }
  else if (tag == 0x2020)
  {
    if (imSony.CameraType != LIBRAW_SONY_DSC)
    {
      fread(imSony.AFPointsUsed, 1, 10, ifp);
    }
  }
  else if (tag == 0x2021)
  {
    if ((imSony.CameraType != LIBRAW_SONY_DSC) ||
        (ilm.CamID == SonyID_DSC_RX10M4)   ||
        (ilm.CamID == SonyID_DSC_RX100M6)  ||
        (ilm.CamID == SonyID_DSC_RX100M5A) ||
        (ilm.CamID == SonyID_DSC_RX0M2)    ||
        (ilm.CamID == SonyID_DSC_RX100M7))
    {
      imSony.AFTracking = fgetc(ifp);
    }
  }
  else if (tag == 0x2027)
  {
    FORC4 imSony.FocusLocation[c] = get2();
  }
  else if (tag == 0x2028)
  {
    if (get2())
    {
      imSony.VariableLowPassFilter = get2();
    }
  }
  else if (tag == 0x2029)
  {
    imSony.RAWFileType = get2();
  }
  else if (tag == 0x202c)
  {
    imSony.MeteringMode2 = get2();
  }
  else if (tag == 0x202f)
  {
    imSony.PixelShiftGroupID = get4();
    imSony.PixelShiftGroupPrefix = imSony.PixelShiftGroupID >> 22;
    imSony.PixelShiftGroupID =
        ((imSony.PixelShiftGroupID >> 17) & (unsigned)0x1f) *
            (unsigned)1000000 +
        ((imSony.PixelShiftGroupID >> 12) & (unsigned)0x1f) * (unsigned)10000 +
        ((imSony.PixelShiftGroupID >> 6) & (unsigned)0x3f) * (unsigned)100 +
        (imSony.PixelShiftGroupID & (unsigned)0x3f);

    imSony.numInPixelShiftGroup = fgetc(ifp);
    imSony.nShotsInPixelShiftGroup = fgetc(ifp);
  }
  else if (tag == 0x9050 && len < 256000)
  { // little endian
    table_buf_0x9050 = (uchar *)malloc(len);
    table_buf_0x9050_len = len;
    fread(table_buf_0x9050, len, 1, ifp);

    if (ilm.CamID)
    {
      process_Sony_0x9050(table_buf_0x9050, table_buf_0x9050_len, ilm.CamID);
      free(table_buf_0x9050);
      table_buf_0x9050_len = 0;
    }
  }
  else if (tag == 0x9400 && len < 256000)
  {
    table_buf_0x9400 = (uchar *)malloc(len);
    table_buf_0x9400_len = len;
    fread(table_buf_0x9400, len, 1, ifp);
    if (ilm.CamID)
    {
      process_Sony_0x9400(table_buf_0x9400, table_buf_0x9400_len, unique_id);
      free(table_buf_0x9400);
      table_buf_0x9400_len = 0;
    }
  }
  else if (tag == 0x9402 && len < 256000)
  {
    table_buf_0x9402 = (uchar *)malloc(len);
    table_buf_0x9402_len = len;
    fread(table_buf_0x9402, len, 1, ifp);
    if (ilm.CamID)
    {
      process_Sony_0x9402(table_buf_0x9402, table_buf_0x9402_len);
      free(table_buf_0x9402);
      table_buf_0x9402_len = 0;
    }
  }
  else if (tag == 0x9403 && len < 256000)
  {
    table_buf_0x9403 = (uchar *)malloc(len);
    table_buf_0x9403_len = len;
    fread(table_buf_0x9403, len, 1, ifp);
    if (ilm.CamID)
    {
      process_Sony_0x9403(table_buf_0x9403, table_buf_0x9403_len);
      free(table_buf_0x9403);
      table_buf_0x9403_len = 0;
    }
  }
  else if ((tag == 0x9405) && (len < 256000) && (len > 0x64))
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);
    uc = table_buf[0x0];
    if (imCommon.real_ISO < 0.1f)
    {
      if ((uc == 0x25) || (uc == 0x3a) || (uc == 0x76) || (uc == 0x7e) ||
          (uc == 0x8b) || (uc == 0x9a) || (uc == 0xb3) || (uc == 0xe1))
      {
        s[0] = SonySubstitution[table_buf[0x04]];
        s[1] = SonySubstitution[table_buf[0x05]];
        imCommon.real_ISO =
            100.0f * libraw_powf64l(2.0f, (16 - ((float)sget2(s)) / 256.0f));
      }
    }
    free(table_buf);
  }
  else if (tag == 0x9406 && len < 256000)
  {
    table_buf_0x9406 = (uchar *)malloc(len);
    table_buf_0x9406_len = len;
    fread(table_buf_0x9406, len, 1, ifp);
    if (ilm.CamID)
    {
      process_Sony_0x9406(table_buf_0x9406, table_buf_0x9406_len);
      free(table_buf_0x9406);
      table_buf_0x9406_len = 0;
    }
  }
  else if (tag == 0x940c && len < 256000)
  {
    table_buf_0x940c = (uchar *)malloc(len);
    table_buf_0x940c_len = len;
    fread(table_buf_0x940c, len, 1, ifp);
    if (ilm.CamID)
    {
      process_Sony_0x940c(table_buf_0x940c, table_buf_0x940c_len);
      free(table_buf_0x940c);
      table_buf_0x940c_len = 0;
    }
  }
  else if (tag == 0x940e && len < 256000)
  {
    table_buf_0x940e = (uchar *)malloc(len);
    table_buf_0x940e_len = len;
    fread(table_buf_0x940e, len, 1, ifp);
    if (ilm.CamID)
    {
      process_Sony_0x940e(table_buf_0x940e, table_buf_0x940e_len, ilm.CamID);
      free(table_buf_0x940e);
      table_buf_0x940e_len = 0;
    }
  }
  else if (((tag == 0xb027) || (tag == 0x010c)) && (ilm.LensID == -1))
  {
    ilm.LensID = get4();
//    printf ("==>> 1: ilm.LensID %lld\n", ilm.LensID);
    if ((ilm.LensID > 0x4900) && (ilm.LensID <= 0x5900))
    {
      ilm.AdapterID = 0x4900;
      ilm.LensID -= ilm.AdapterID;
      ilm.LensMount = LIBRAW_MOUNT_Sigma_X3F;
      strcpy(ilm.Adapter, "MC-11");
    }

    else if ((ilm.LensID > 0xef00) && (ilm.LensID < 0xffff) &&
             (ilm.LensID != 0xff00))
    {
      ilm.AdapterID = 0xef00;
      ilm.LensID -= ilm.AdapterID;
      ilm.LensMount = LIBRAW_MOUNT_Canon_EF;
    }

    else if (((ilm.LensID != -1) && (ilm.LensID < 0xef00)) ||
             (ilm.LensID == 0xff00))
      ilm.LensMount = LIBRAW_MOUNT_Minolta_A;
    /*
        if (tag == 0x010c)
          ilm.CameraMount = LIBRAW_MOUNT_Minolta_A;
    */
  }
  else if (tag == 0xb02a && len < 256000)
  { // Sony LensSpec
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);
    if ((!dng_writer) ||
        (saneSonyCameraInfo(table_buf[1], table_buf[2], table_buf[3],
                            table_buf[4], table_buf[5], table_buf[6])))
    {
      if (table_buf[1] | table_buf[2])
        ilm.MinFocal = bcd2dec(table_buf[1]) * 100 + bcd2dec(table_buf[2]);
      if (table_buf[3] | table_buf[4])
        ilm.MaxFocal = bcd2dec(table_buf[3]) * 100 + bcd2dec(table_buf[4]);
      if (table_buf[5])
        ilm.MaxAp4MinFocal = bcd2dec(table_buf[5]) / 10.0f;
      if (table_buf[6])
        ilm.MaxAp4MaxFocal = bcd2dec(table_buf[6]) / 10.0f;
      parseSonyLensFeatures(table_buf[0], table_buf[7]);
    }
    free(table_buf);
  }
  else if ((tag == 0xb02b) && !imgdata.sizes.raw_inset_crop.cwidth &&
           (len == 2))
  {
    imgdata.sizes.raw_inset_crop.cheight = get4();
    imgdata.sizes.raw_inset_crop.cwidth = get4();
  }
  else if (tag == 0xb041)
  {
    imgdata.shootinginfo.ExposureMode = get2();
  }

  // MetaVersion: (unique_id >= 286)
}