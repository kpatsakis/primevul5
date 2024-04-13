clutter_device_manager_xi2_get_device (ClutterDeviceManager *manager,
                                       gint                  id)
{
  ClutterDeviceManagerXI2 *manager_xi2 = CLUTTER_DEVICE_MANAGER_XI2 (manager);

  return g_hash_table_lookup (manager_xi2->devices_by_id,
                              GINT_TO_POINTER (id));
}