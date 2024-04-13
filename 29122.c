void GfxIndexedColorSpace::getRGB(GfxColor *color, GfxRGB *rgb) {
  GfxColor color2;

  base->getRGB(mapColorToBase(color, &color2), rgb);
}
