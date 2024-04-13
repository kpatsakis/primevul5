static void cairo_font_face_destroy (void *data)
{
  CairoFont *font = (CairoFont *) data;

  delete font;
}