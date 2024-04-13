void GfxDeviceGrayColorSpace::getRGB(GfxColor *color, GfxRGB *rgb) {
  rgb->r = rgb->g = rgb->b = clip01(color->c[0]);
}
