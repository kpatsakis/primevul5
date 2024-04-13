remove_device (ClutterDeviceManagerXI2 *manager_xi2,
               gint                     device_id)
{
  ClutterInputDevice *device;

  device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                GINT_TO_POINTER (device_id));

  if (device != NULL)
    {
      manager_xi2->master_devices =
        g_list_remove (manager_xi2->master_devices, device);
      manager_xi2->slave_devices =
        g_list_remove (manager_xi2->slave_devices, device);

      /* blow the cache */
      g_slist_free (manager_xi2->all_devices);
      manager_xi2->all_devices = NULL;

      g_signal_emit_by_name (manager_xi2, "device-removed", device);

      g_object_run_dispose (G_OBJECT (device));

      g_hash_table_remove (manager_xi2->devices_by_id,
                           GINT_TO_POINTER (device_id));
    }
}