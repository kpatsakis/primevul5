create_device (ClutterDeviceManagerXI2 *manager_xi2,
               ClutterBackendX11       *backend_x11,
               XIDeviceInfo            *info)
{
  ClutterInputDeviceType source, touch_source;
  ClutterInputDevice *retval;
  ClutterInputMode mode;
  gboolean is_enabled;
  guint num_touches = 0;

  if (info->use == XIMasterKeyboard || info->use == XISlaveKeyboard)
    source = CLUTTER_KEYBOARD_DEVICE;
  else if (info->use == XISlavePointer &&
           is_touch_device (info->classes, info->num_classes,
                            &touch_source,
                            &num_touches))
    {
      source = touch_source;
    }
  else
    {
      gchar *name;

      name = g_ascii_strdown (info->name, -1);

      if (strstr (name, "eraser") != NULL)
        source = CLUTTER_ERASER_DEVICE;
      else if (strstr (name, "cursor") != NULL)
        source = CLUTTER_CURSOR_DEVICE;
      else if (strstr (name, "wacom") != NULL || strstr (name, "pen") != NULL)
        source = CLUTTER_PEN_DEVICE;
      else
        source = CLUTTER_POINTER_DEVICE;

      g_free (name);
    }

  switch (info->use)
    {
    case XIMasterKeyboard:
    case XIMasterPointer:
      mode = CLUTTER_INPUT_MODE_MASTER;
      is_enabled = TRUE;
      break;

    case XISlaveKeyboard:
    case XISlavePointer:
      mode = CLUTTER_INPUT_MODE_SLAVE;
      is_enabled = FALSE;
      break;

    case XIFloatingSlave:
    default:
      mode = CLUTTER_INPUT_MODE_FLOATING;
      is_enabled = FALSE;
      break;
    }

  retval = g_object_new (CLUTTER_TYPE_INPUT_DEVICE_XI2,
                         "name", info->name,
                         "id", info->deviceid,
                         "has-cursor", (info->use == XIMasterPointer),
                         "device-manager", manager_xi2,
                         "device-type", source,
                         "device-mode", mode,
                         "backend", backend_x11,
                         "enabled", is_enabled,
                         NULL);

  translate_device_classes (backend_x11->xdpy, retval,
                            info->classes,
                            info->num_classes);

  CLUTTER_NOTE (BACKEND, "Created device '%s' (id: %d, has-cursor: %s)",
                info->name,
                info->deviceid,
                info->use == XIMasterPointer ? "yes" : "no");

  return retval;
}