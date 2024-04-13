add_device (ClutterDeviceManagerXI2 *manager_xi2,
            ClutterBackendX11       *backend_x11,
            XIDeviceInfo            *info,
            gboolean                 in_construction)
{
  ClutterInputDevice *device;

  device = create_device (manager_xi2, backend_x11, info);

  /* we don't go through the DeviceManager::add_device() vfunc because
   * that emits the signal, and we only do it conditionally
   */
  g_hash_table_replace (manager_xi2->devices_by_id,
                        GINT_TO_POINTER (info->deviceid),
                        g_object_ref (device));

  if (info->use == XIMasterPointer ||
      info->use == XIMasterKeyboard)
    {
      manager_xi2->master_devices =
        g_list_prepend (manager_xi2->master_devices, device);
    }
  else if (info->use == XISlavePointer ||
           info->use == XISlaveKeyboard ||
           info->use == XIFloatingSlave)
    {
      manager_xi2->slave_devices =
        g_list_prepend (manager_xi2->slave_devices, device);
    }
  else
    g_warning ("Unhandled device: %s",
               clutter_input_device_get_device_name (device));

  /* relationships between devices and signal emissions are not
   * necessary while we're constructing the device manager instance
   */
  if (!in_construction)
    {
      if (info->use == XISlavePointer || info->use == XISlaveKeyboard)
        {
          ClutterInputDevice *master;

          master = g_hash_table_lookup (manager_xi2->devices_by_id,
                                        GINT_TO_POINTER (info->attachment));
          _clutter_input_device_set_associated_device (device, master);
          _clutter_input_device_add_slave (master, device);
        }

      /* blow the cache */
      g_slist_free (manager_xi2->all_devices);
      manager_xi2->all_devices = NULL;

      g_signal_emit_by_name (manager_xi2, "device-added", device);
    }

  return device;
}