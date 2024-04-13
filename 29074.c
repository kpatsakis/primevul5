void GfxICCBasedColorSpace::getCMYK(GfxColor *color, GfxCMYK *cmyk) {
#ifdef USE_CMS
  if (transform != NULL && displayPixelType == PT_CMYK) {
    Guchar in[gfxColorMaxComps];
    Guchar out[gfxColorMaxComps];
    
    for (int i = 0;i < nComps;i++) {
	in[i] = colToByte(color->c[i]);
    }
    transform->doTransform(in,out,1);
    cmyk->c = byteToCol(out[0]);
    cmyk->m = byteToCol(out[1]);
    cmyk->y = byteToCol(out[2]);
    cmyk->k = byteToCol(out[3]);
  } else {
    GfxRGB rgb;
    GfxColorComp c, m, y, k;

    getRGB(color,&rgb);
    c = clip01(gfxColorComp1 - rgb.r);
    m = clip01(gfxColorComp1 - rgb.g);
    y = clip01(gfxColorComp1 - rgb.b);
    k = c;
    if (m < k) {
      k = m;
    }
    if (y < k) {
      k = y;
    }
    cmyk->c = c - k;
    cmyk->m = m - k;
    cmyk->y = y - k;
    cmyk->k = k;
  }
#else
  alt->getCMYK(color, cmyk);
#endif
}
