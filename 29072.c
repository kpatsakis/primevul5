void GfxDeviceCMYKColorSpace::getCMYK(GfxColor *color, GfxCMYK *cmyk) {
  cmyk->c = clip01(color->c[0]);
  cmyk->m = clip01(color->c[1]);
  cmyk->y = clip01(color->c[2]);
  cmyk->k = clip01(color->c[3]);
}
