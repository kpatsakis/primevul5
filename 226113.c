void CLASS parse_fuji (int offset)
{
  unsigned entries, tag, len, save, c;

  fseek (ifp, offset, SEEK_SET);
  entries = get4();
  if (entries > 255) return;
  while (entries--) {
    tag = get2();
    len = get2();
    save = ftell(ifp);

    if (tag == 0x100) {
      raw_height = get2();
      raw_width  = get2();
    } else if (tag == 0x121) {
      height = get2();
      if ((width = get2()) == 4284) width += 3;
    } else if (tag == 0x130) {
      fuji_layout = fgetc(ifp) >> 7;
      fuji_width = !(fgetc(ifp) & 8);
    } else if (tag == 0x131) {
      filters = 9;
      FORC(36)
        {
           int q = fgetc(ifp);
           xtrans_abs[0][35 - c] = MAX(0,MIN(q,2)); /* & 3;*/
        }
    } else if (tag == 0x2ff0) {
      FORC4 cam_mul[c ^ 1] = get2();
    }
// IB start
#ifdef LIBRAW_LIBRARY_BUILD
    else if (tag == 0x9650)
    {
      short a = (short)get2();
      float b =fMAX(1.0f, get2());
      imgdata.makernotes.fuji.FujiExpoMidPointShift = a / b;
    } else if (tag == 0x2100) {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Daylight][c ^ 1] = get2();
    } else if (tag == 0x2200) {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Cloudy][c ^ 1] = get2();
    } else if (tag == 0x2300) {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_D][c ^ 1] = get2();
    } else if (tag == 0x2301) {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_N][c ^ 1] = get2();
    } else if (tag == 0x2302) {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_WW][c ^ 1] = get2();
    } else if (tag == 0x2310) {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_L][c ^ 1] = get2();
    } else if (tag == 0x2400) {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Tungsten][c ^ 1] = get2();
    }
#endif
// IB end
    else if (tag == 0xc000) {
      c = order;
      order = 0x4949;
      if ((tag = get4()) > 10000) tag = get4();
      if (tag > 10000) tag = get4();
      width = tag;
      height = get4();
#ifdef LIBRAW_LIBRARY_BUILD
      libraw_internal_data.unpacker_data.posRAFData = save;
      libraw_internal_data.unpacker_data.lenRAFData = (len>>1);
#endif
	  order = c;
    }
    fseek (ifp, save+len, SEEK_SET);
  }
  height <<= fuji_layout;
  width  >>= fuji_layout;
}