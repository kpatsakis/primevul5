scroll_valuators_changed (ClutterInputDevice *device,
                          XIValuatorState    *valuators,
                          gdouble            *dx_p,
                          gdouble            *dy_p)
{
  gboolean retval = FALSE;
  guint n_axes, n_val, i;
  double *values;

  n_axes = clutter_input_device_get_n_axes (device);
  values = valuators->values;

  *dx_p = *dy_p = 0.0;

  n_val = 0;

  for (i = 0; i < MIN (valuators->mask_len * 8, n_axes); i++)
    {
      ClutterScrollDirection direction;
      gdouble delta;

      if (!XIMaskIsSet (valuators->mask, i))
        continue;

      if (_clutter_input_device_get_scroll_delta (device, i,
                                                  values[n_val],
                                                  &direction,
                                                  &delta))
        {
          retval = TRUE;

          if (direction == CLUTTER_SCROLL_UP ||
              direction == CLUTTER_SCROLL_DOWN)
            *dy_p = delta;
          else
            *dx_p = delta;
        }

      n_val += 1;
    }

  return retval;
}