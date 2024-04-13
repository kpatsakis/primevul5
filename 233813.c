clutter_device_manager_xi2_set_property (GObject      *gobject,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
  ClutterDeviceManagerXI2 *manager_xi2 = CLUTTER_DEVICE_MANAGER_XI2 (gobject);

  switch (prop_id)
    {
    case PROP_OPCODE:
      manager_xi2->opcode = g_value_get_int (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}