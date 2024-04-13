void GfxIndexedColorSpace::getCMYK(GfxColor *color, GfxCMYK *cmyk) {
  GfxColor color2;

  base->getCMYK(mapColorToBase(color, &color2), cmyk);
}
