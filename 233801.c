clutter_device_manager_xi2_constructed (GObject *gobject)
{
  ClutterDeviceManagerXI2 *manager_xi2 = CLUTTER_DEVICE_MANAGER_XI2 (gobject);
  ClutterDeviceManager *manager = CLUTTER_DEVICE_MANAGER (gobject);
  ClutterBackendX11 *backend_x11;
  GHashTable *masters, *slaves;
  XIDeviceInfo *info;
  XIEventMask event_mask;
  unsigned char mask[2] = { 0, };
  int n_devices, i;

  backend_x11 =
    CLUTTER_BACKEND_X11 (_clutter_device_manager_get_backend (manager));

  masters = g_hash_table_new (NULL, NULL);
  slaves = g_hash_table_new (NULL, NULL);

  info = XIQueryDevice (backend_x11->xdpy, XIAllDevices, &n_devices);

  for (i = 0; i < n_devices; i++)
    {
      XIDeviceInfo *xi_device = &info[i];

      add_device (manager_xi2, backend_x11, xi_device, TRUE);

      if (xi_device->use == XIMasterPointer ||
          xi_device->use == XIMasterKeyboard)
        {
          g_hash_table_insert (masters,
                               GINT_TO_POINTER (xi_device->deviceid),
                               GINT_TO_POINTER (xi_device->attachment));
        }
      else if (xi_device->use == XISlavePointer ||
               xi_device->use == XISlaveKeyboard)
        {
          g_hash_table_insert (slaves,
                               GINT_TO_POINTER (xi_device->deviceid),
                               GINT_TO_POINTER (xi_device->attachment));
        }
    }

  XIFreeDeviceInfo (info);

  g_hash_table_foreach (masters, relate_masters, manager_xi2);
  g_hash_table_destroy (masters);

  g_hash_table_foreach (slaves, relate_slaves, manager_xi2);
  g_hash_table_destroy (slaves);

  XISetMask (mask, XI_HierarchyChanged);
  XISetMask (mask, XI_DeviceChanged);

  event_mask.deviceid = XIAllDevices;
  event_mask.mask_len = sizeof (mask);
  event_mask.mask = mask;

  clutter_device_manager_xi2_select_events (manager,
                                            clutter_x11_get_root_window (),
                                            &event_mask);

  if (G_OBJECT_CLASS (clutter_device_manager_xi2_parent_class)->constructed)
    G_OBJECT_CLASS (clutter_device_manager_xi2_parent_class)->constructed (gobject);
}