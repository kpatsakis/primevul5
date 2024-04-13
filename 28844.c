poppler_color_copy (PopplerColor *color)
{
  PopplerColor *new_color;

  new_color = g_new (PopplerColor, 1);
  *new_color = *color;

  return new_color;
}
