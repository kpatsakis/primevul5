clutter_device_manager_xi2_get_devices (ClutterDeviceManager *manager)
{
  ClutterDeviceManagerXI2 *manager_xi2 = CLUTTER_DEVICE_MANAGER_XI2 (manager);
  GSList *all_devices = NULL;
  GList *l;

  if (manager_xi2->all_devices != NULL)
    return manager_xi2->all_devices;

  for (l = manager_xi2->master_devices; l != NULL; l = l->next)
    all_devices = g_slist_prepend (all_devices, l->data);

  for (l = manager_xi2->slave_devices; l != NULL; l = l->next)
    all_devices = g_slist_prepend (all_devices, l->data);

  manager_xi2->all_devices = g_slist_reverse (all_devices);

  return manager_xi2->all_devices;
}