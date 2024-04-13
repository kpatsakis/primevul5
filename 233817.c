clutter_event_translator_iface_init (ClutterEventTranslatorIface *iface)
{
  iface->translate_event = clutter_device_manager_xi2_translate_event;
}