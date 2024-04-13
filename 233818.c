is_touch_device (XIAnyClassInfo         **classes,
                 guint                    n_classes,
                 ClutterInputDeviceType  *device_type,
                 guint                   *n_touch_points)
{
#ifdef HAVE_XINPUT_2_2
  guint i;

  for (i = 0; i < n_classes; i++)
    {
      XITouchClassInfo *class = (XITouchClassInfo *) classes[i];

      if (class->type != XITouchClass)
        continue;

      if (class->num_touches > 0)
        {
          if (class->mode == XIDirectTouch)
            *device_type = CLUTTER_TOUCHSCREEN_DEVICE;
          else if (class->mode == XIDependentTouch)
            *device_type = CLUTTER_TOUCHPAD_DEVICE;
          else
            continue;

          *n_touch_points = class->num_touches;

          return TRUE;
        }
    }
#endif

  return FALSE;
}