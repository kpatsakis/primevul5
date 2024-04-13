clutter_device_manager_xi2_get_core_device (ClutterDeviceManager   *manager,
                                            ClutterInputDeviceType  device_type)
{
  ClutterDeviceManagerXI2 *manager_xi2 = CLUTTER_DEVICE_MANAGER_XI2 (manager);
  ClutterBackendX11 *backend_x11;
  ClutterInputDevice *device;
  int device_id;

  backend_x11 =
    CLUTTER_BACKEND_X11 (_clutter_device_manager_get_backend (manager));

  XIGetClientPointer (backend_x11->xdpy, None, &device_id);

  device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                GINT_TO_POINTER (device_id));

  switch (device_type)
    {
    case CLUTTER_POINTER_DEVICE:
      return device;

    case CLUTTER_KEYBOARD_DEVICE:
      return clutter_input_device_get_associated_device (device);

    default:
      break;
    }

  return NULL;
}