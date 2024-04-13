void GfxDeviceCMYKColorSpace::getDefaultColor(GfxColor *color) {
  color->c[0] = 0;
  color->c[1] = 0;
  color->c[2] = 0;
  color->c[3] = gfxColorComp1;
}
