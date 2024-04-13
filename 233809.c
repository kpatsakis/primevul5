clutter_device_manager_xi2_init (ClutterDeviceManagerXI2 *self)
{
  self->devices_by_id = g_hash_table_new_full (NULL, NULL,
                                               NULL,
                                               (GDestroyNotify) g_object_unref);
}