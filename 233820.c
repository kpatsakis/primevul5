translate_device_classes (Display             *xdisplay,
                          ClutterInputDevice  *device,
                          XIAnyClassInfo     **classes,
                          guint                n_classes)
{
  gint i;

  for (i = 0; i < n_classes; i++)
    {
      XIAnyClassInfo *class_info = classes[i];

      switch (class_info->type)
        {
        case XIKeyClass:
          {
            XIKeyClassInfo *key_info = (XIKeyClassInfo *) class_info;
            gint j;

            _clutter_input_device_set_n_keys (device,
                                              key_info->num_keycodes);

            for (j = 0; j < key_info->num_keycodes; j++)
              {
                clutter_input_device_set_key (device, j,
                                              key_info->keycodes[i],
                                              0);
              }
          }
          break;

        case XIValuatorClass:
          translate_valuator_class (xdisplay, device,
                                    (XIValuatorClassInfo *) class_info);
          break;

#ifdef HAVE_XINPUT_2_2
        case XIScrollClass:
          {
            XIScrollClassInfo *scroll_info = (XIScrollClassInfo *) class_info;
            ClutterScrollDirection direction;

            if (scroll_info->scroll_type == XIScrollTypeVertical)
              direction = CLUTTER_SCROLL_DOWN;
            else
              direction = CLUTTER_SCROLL_RIGHT;

            CLUTTER_NOTE (BACKEND, "Scroll valuator %d: %s, increment: %f",
                          scroll_info->number,
                          scroll_info->scroll_type == XIScrollTypeVertical
                            ? "vertical"
                            : "horizontal",
                          scroll_info->increment);

            _clutter_input_device_add_scroll_info (device,
                                                   scroll_info->number,
                                                   direction,
                                                   scroll_info->increment);
          }
          break;
#endif /* HAVE_XINPUT_2_2 */

        default:
          break;
        }
    }
}