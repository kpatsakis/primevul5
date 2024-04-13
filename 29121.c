void GfxICCBasedColorSpace::getRGB(GfxColor *color, GfxRGB *rgb) {
#ifdef USE_CMS
  if (transform != 0
       && (displayProfile == NULL || displayPixelType == PT_RGB)) {
    Guchar in[gfxColorMaxComps];
    Guchar out[gfxColorMaxComps];
    
    for (int i = 0;i < nComps;i++) {
	in[i] = colToByte(color->c[i]);
    }
    transform->doTransform(in,out,1);
    rgb->r = byteToCol(out[0]);
    rgb->g = byteToCol(out[1]);
    rgb->b = byteToCol(out[2]);
  } else {
    alt->getRGB(color, rgb);
  }
#else
  alt->getRGB(color, rgb);
#endif
}
