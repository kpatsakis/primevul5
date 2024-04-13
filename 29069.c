void GfxDeviceGrayColorSpace::getCMYK(GfxColor *color, GfxCMYK *cmyk) {
  cmyk->c = cmyk->m = cmyk->y = 0;
  cmyk->k = clip01(gfxColorComp1 - color->c[0]);
}
