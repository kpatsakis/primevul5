relate_masters (gpointer key,
                gpointer value,
                gpointer data)
{
  ClutterDeviceManagerXI2 *manager_xi2 = data;
  ClutterInputDevice *device, *relative;

  device = g_hash_table_lookup (manager_xi2->devices_by_id, key);
  relative = g_hash_table_lookup (manager_xi2->devices_by_id, value);

  _clutter_input_device_set_associated_device (device, relative);
  _clutter_input_device_set_associated_device (relative, device);
}