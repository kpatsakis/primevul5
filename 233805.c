clutter_device_manager_xi2_class_init (ClutterDeviceManagerXI2Class *klass)
{
  ClutterDeviceManagerClass *manager_class;
  GObjectClass *gobject_class;

  obj_props[PROP_OPCODE] =
    g_param_spec_int ("opcode",
                      "Opcode",
                      "The XI2 opcode",
                      -1, G_MAXINT,
                      -1,
                      CLUTTER_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->constructed = clutter_device_manager_xi2_constructed;
  gobject_class->set_property = clutter_device_manager_xi2_set_property;

  g_object_class_install_properties (gobject_class, PROP_LAST, obj_props);
  
  manager_class = CLUTTER_DEVICE_MANAGER_CLASS (klass);
  manager_class->add_device = clutter_device_manager_xi2_add_device;
  manager_class->remove_device = clutter_device_manager_xi2_remove_device;
  manager_class->get_devices = clutter_device_manager_xi2_get_devices;
  manager_class->get_core_device = clutter_device_manager_xi2_get_core_device;
  manager_class->get_device = clutter_device_manager_xi2_get_device;
}