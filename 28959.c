cairo_surface_t *cairo_surface_create_similar_clip (cairo_t *cairo, cairo_content_t content)
{
  double x1, y1, x2, y2;
  int width, height;
  cairo_clip_extents (cairo, &x1, &y1, &x2, &y2);
  cairo_matrix_t matrix;
  cairo_get_matrix (cairo, &matrix);
  cairo_user_to_device(cairo, &x1, &y1);
  cairo_user_to_device(cairo, &x2, &y2);
  width = splashCeil(x2) - splashFloor(x1);
  height = splashFloor(y1) - splashCeil(y2);
  cairo_surface_t *target = cairo_get_target (cairo);
  cairo_surface_t *result;

  result = cairo_surface_create_similar (target, content, width, height);
  double x_offset, y_offset;
    cairo_surface_get_device_offset(target, &x_offset, &y_offset);
    cairo_surface_set_device_offset(result, x_offset, y_offset);

 
  return result;
}
