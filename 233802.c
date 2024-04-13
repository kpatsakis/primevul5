relate_slaves (gpointer key,
               gpointer value,
               gpointer data)
{
  ClutterDeviceManagerXI2 *manager_xi2 = data;
  ClutterInputDevice *master, *slave;

  slave = g_hash_table_lookup (manager_xi2->devices_by_id, key);
  master = g_hash_table_lookup (manager_xi2->devices_by_id, value);

  _clutter_input_device_set_associated_device (slave, master);
  _clutter_input_device_add_slave (master, slave);
}