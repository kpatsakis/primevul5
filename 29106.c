void GfxIndexedColorSpace::getGray(GfxColor *color, GfxGray *gray) {
  GfxColor color2;

  base->getGray(mapColorToBase(color, &color2), gray);
}
