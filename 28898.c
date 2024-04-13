poppler_rectangle_copy (PopplerRectangle *rectangle)
{
  PopplerRectangle *new_rectangle;

  g_return_val_if_fail (rectangle != NULL, NULL);
  
  new_rectangle = g_new (PopplerRectangle, 1);
  *new_rectangle = *rectangle;

  return new_rectangle;
}
