void GfxICCBasedColorSpace::getGray(GfxColor *color, GfxGray *gray) {
#ifdef USE_CMS
  if (transform != 0 && displayPixelType == PT_GRAY) {
    Guchar in[gfxColorMaxComps];
    Guchar out[gfxColorMaxComps];
    
    for (int i = 0;i < nComps;i++) {
	in[i] = colToByte(color->c[i]);
    }
    transform->doTransform(in,out,1);
    *gray = byteToCol(out[0]);
  } else {
    GfxRGB rgb;
    getRGB(color,&rgb);
    *gray = clip01((GfxColorComp)(0.3 * rgb.r +
		   0.59 * rgb.g +
		   0.11 * rgb.b + 0.5));
  }
#else
  alt->getGray(color, gray);
#endif
}
