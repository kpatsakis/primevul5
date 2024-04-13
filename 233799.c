translate_axes (ClutterInputDevice *device,
                gdouble             x,
                gdouble             y,
                ClutterStageX11    *stage_x11,
                XIValuatorState    *valuators)
{
  guint n_axes = clutter_input_device_get_n_axes (device);
  guint i;
  gdouble *retval;
  double *values;

  retval = g_new0 (gdouble, n_axes);
  values = valuators->values;

  for (i = 0; i < valuators->mask_len * 8; i++)
    {
      ClutterInputAxis axis;
      gdouble val;

      if (!XIMaskIsSet (valuators->mask, i))
        continue;

      axis = clutter_input_device_get_axis (device, i);
      val = *values++;

      switch (axis)
        {
        case CLUTTER_INPUT_AXIS_X:
          retval[i] = x;
          break;

        case CLUTTER_INPUT_AXIS_Y:
          retval[i] = y;
          break;

        default:
          _clutter_input_device_translate_axis (device, i, val, &retval[i]);
          break;
        }
    }

  return retval;
}