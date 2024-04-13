void GfxDeviceRGBColorSpace::getRGB(GfxColor *color, GfxRGB *rgb) {
  rgb->r = clip01(color->c[0]);
  rgb->g = clip01(color->c[1]);
  rgb->b = clip01(color->c[2]);
}
