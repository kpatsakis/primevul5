clutter_device_manager_xi2_translate_event (ClutterEventTranslator *translator,
                                            gpointer                native,
                                            ClutterEvent           *event)
{
  ClutterDeviceManagerXI2 *manager_xi2 = CLUTTER_DEVICE_MANAGER_XI2 (translator);
  ClutterTranslateReturn retval = CLUTTER_TRANSLATE_CONTINUE;
  ClutterBackendX11 *backend_x11;
  ClutterStageX11 *stage_x11 = NULL;
  ClutterStage *stage = NULL;
  ClutterInputDevice *device, *source_device;
  XGenericEventCookie *cookie;
  XIEvent *xi_event;
  XEvent *xevent;

  backend_x11 = CLUTTER_BACKEND_X11 (clutter_get_default_backend ());

  xevent = native;

  cookie = &xevent->xcookie;

  if (cookie->type != GenericEvent ||
      cookie->extension != manager_xi2->opcode)
    return CLUTTER_TRANSLATE_CONTINUE;

  xi_event = (XIEvent *) cookie->data;

  if (!xi_event)
    return CLUTTER_TRANSLATE_REMOVE;

  if (!(xi_event->evtype == XI_HierarchyChanged ||
        xi_event->evtype == XI_DeviceChanged))
    {
      stage = get_event_stage (translator, xi_event);
      if (stage == NULL || CLUTTER_ACTOR_IN_DESTRUCTION (stage))
        return CLUTTER_TRANSLATE_CONTINUE;
      else
        stage_x11 = CLUTTER_STAGE_X11 (_clutter_stage_get_window (stage));
    }

  event->any.stage = stage;

  switch (xi_event->evtype)
    {
    case XI_HierarchyChanged:
      {
        XIHierarchyEvent *xev = (XIHierarchyEvent *) xi_event;

        translate_hierarchy_event (backend_x11, manager_xi2, xev);
      }
      retval = CLUTTER_TRANSLATE_REMOVE;
      break;

    case XI_DeviceChanged:
      {
        XIDeviceChangedEvent *xev = (XIDeviceChangedEvent *) xi_event;

        device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                      GINT_TO_POINTER (xev->deviceid));
        source_device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                             GINT_TO_POINTER (xev->sourceid));
        if (device)
          {
            _clutter_input_device_reset_axes (device);
            translate_device_classes (backend_x11->xdpy,
                                      device,
                                      xev->classes,
                                      xev->num_classes);
          }

        if (source_device)
          _clutter_input_device_reset_scroll_info (source_device);
      }
      retval = CLUTTER_TRANSLATE_REMOVE;
      break;

    case XI_KeyPress:
    case XI_KeyRelease:
      {
        XIDeviceEvent *xev = (XIDeviceEvent *) xi_event;
        ClutterEventX11 *event_x11;
        char buffer[7] = { 0, };
        gunichar n;

        event->key.type = event->type = (xev->evtype == XI_KeyPress)
                                      ? CLUTTER_KEY_PRESS
                                      : CLUTTER_KEY_RELEASE;

        event->key.time = xev->time;
        event->key.stage = stage;
        event->key.modifier_state =
          _clutter_input_device_xi2_translate_state (&xev->mods, &xev->buttons, &xev->group);
        event->key.hardware_keycode = xev->detail;

          /* keyval is the key ignoring all modifiers ('1' vs. '!') */
        event->key.keyval =
          _clutter_keymap_x11_translate_key_state (backend_x11->keymap,
                                                   event->key.hardware_keycode,
                                                   &event->key.modifier_state,
                                                   NULL);

        /* KeyEvents have platform specific data associated to them */
        event_x11 = _clutter_event_x11_new ();
        _clutter_event_set_platform_data (event, event_x11);

        event_x11->key_group =
          _clutter_keymap_x11_get_key_group (backend_x11->keymap,
                                             event->key.modifier_state);
        event_x11->key_is_modifier =
          _clutter_keymap_x11_get_is_modifier (backend_x11->keymap,
                                               event->key.hardware_keycode);
        event_x11->num_lock_set =
          _clutter_keymap_x11_get_num_lock_state (backend_x11->keymap);
        event_x11->caps_lock_set =
          _clutter_keymap_x11_get_caps_lock_state (backend_x11->keymap);

        source_device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                             GINT_TO_POINTER (xev->sourceid));
        clutter_event_set_source_device (event, source_device);

        device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                      GINT_TO_POINTER (xev->deviceid));
        clutter_event_set_device (event, device);

        /* XXX keep this in sync with the evdev device manager */
        n = print_keysym (event->key.keyval, buffer, sizeof (buffer));
        if (n == 0)
          {
            /* not printable */
            event->key.unicode_value = (gunichar) '\0';
          }
        else
          {
            event->key.unicode_value = g_utf8_get_char_validated (buffer, n);
            if (event->key.unicode_value == -1 ||
                event->key.unicode_value == -2)
              event->key.unicode_value = (gunichar) '\0';
          }

        CLUTTER_NOTE (EVENT,
                      "%s: win:0x%x device:%d source:%d, key: %12s (%d)",
                      event->any.type == CLUTTER_KEY_PRESS
                        ? "key press  "
                        : "key release",
                      (unsigned int) stage_x11->xwin,
                      xev->deviceid,
                      xev->sourceid,
                      event->key.keyval ? buffer : "(none)",
                      event->key.keyval);

        if (xi_event->evtype == XI_KeyPress)
          _clutter_stage_x11_set_user_time (stage_x11, event->key.time);

        retval = CLUTTER_TRANSLATE_QUEUE;
      }
      break;

    case XI_ButtonPress:
    case XI_ButtonRelease:
      {
        XIDeviceEvent *xev = (XIDeviceEvent *) xi_event;

        source_device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                             GINT_TO_POINTER (xev->sourceid));
        device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                      GINT_TO_POINTER (xev->deviceid));

        /* Set the stage for core events coming out of nowhere (see bug #684509) */
        if (clutter_input_device_get_device_mode (device) == CLUTTER_INPUT_MODE_MASTER &&
            clutter_input_device_get_pointer_stage (device) == NULL &&
            stage != NULL)
          _clutter_input_device_set_stage (device, stage);

        switch (xev->detail)
          {
          case 4:
          case 5:
          case 6:
          case 7:
            /* we only generate Scroll events on ButtonPress */
            if (xi_event->evtype == XI_ButtonRelease)
              return CLUTTER_TRANSLATE_REMOVE;

            event->scroll.type = event->type = CLUTTER_SCROLL;

            if (xev->detail == 4)
              event->scroll.direction = CLUTTER_SCROLL_UP;
            else if (xev->detail == 5)
              event->scroll.direction = CLUTTER_SCROLL_DOWN;
            else if (xev->detail == 6)
              event->scroll.direction = CLUTTER_SCROLL_LEFT;
            else
              event->scroll.direction = CLUTTER_SCROLL_RIGHT;

            event->scroll.stage = stage;

            event->scroll.time = xev->time;
            event->scroll.x = xev->event_x;
            event->scroll.y = xev->event_y;
            event->scroll.modifier_state =
              _clutter_input_device_xi2_translate_state (&xev->mods,
                                                         &xev->buttons,
                                                         &xev->group);

            clutter_event_set_source_device (event, source_device);
            clutter_event_set_device (event, device);

            event->scroll.axes = translate_axes (event->scroll.device,
                                                 event->scroll.x,
                                                 event->scroll.y,
                                                 stage_x11,
                                                 &xev->valuators);

            CLUTTER_NOTE (EVENT,
                          "scroll: win:0x%x, device:%d '%s', time:%d "
                          "(direction:%s, "
                          "x:%.2f, y:%.2f, "
                          "emulated:%s)",
                          (unsigned int) stage_x11->xwin,
                          device->id,
                          device->device_name,
                          event->any.time,
                          event->scroll.direction == CLUTTER_SCROLL_UP ? "up" :
                          event->scroll.direction == CLUTTER_SCROLL_DOWN ? "down" :
                          event->scroll.direction == CLUTTER_SCROLL_LEFT ? "left" :
                          event->scroll.direction == CLUTTER_SCROLL_RIGHT ? "right" :
                          "invalid",
                          event->scroll.x,
                          event->scroll.y,
#ifdef HAVE_XINPUT_2_2
                          (xev->flags & XIPointerEmulated) ? "yes" : "no"
#else
                          "no"
#endif
                          );
            break;

          default:
            event->button.type = event->type =
              (xi_event->evtype == XI_ButtonPress) ? CLUTTER_BUTTON_PRESS
                                                   : CLUTTER_BUTTON_RELEASE;

            event->button.stage = stage;

            event->button.time = xev->time;
            event->button.x = xev->event_x;
            event->button.y = xev->event_y;
            event->button.button = xev->detail;
            event->button.modifier_state =
              _clutter_input_device_xi2_translate_state (&xev->mods,
                                                         &xev->buttons,
                                                         &xev->group);

            clutter_event_set_source_device (event, source_device);
            clutter_event_set_device (event, device);

            event->button.axes = translate_axes (event->button.device,
                                                 event->button.x,
                                                 event->button.y,
                                                 stage_x11,
                                                 &xev->valuators);

            CLUTTER_NOTE (EVENT,
                          "%s: win:0x%x, device:%d '%s', time:%d "
                          "(button:%d, "
                          "x:%.2f, y:%.2f, "
                          "axes:%s, "
                          "emulated:%s)",
                          event->any.type == CLUTTER_BUTTON_PRESS
                            ? "button press  "
                            : "button release",
                          (unsigned int) stage_x11->xwin,
                          device->id,
                          device->device_name,
                          event->any.time,
                          event->button.button,
                          event->button.x,
                          event->button.y,
                          event->button.axes != NULL ? "yes" : "no",
#ifdef HAVE_XINPUT_2_2
                          (xev->flags & XIPointerEmulated) ? "yes" : "no"
#else
                          "no"
#endif
                          );
            break;
          }

        if (source_device != NULL && device->stage != NULL)
          _clutter_input_device_set_stage (source_device, device->stage);

#ifdef HAVE_XINPUT_2_2
        if (xev->flags & XIPointerEmulated)
          _clutter_event_set_pointer_emulated (event, TRUE);
#endif /* HAVE_XINPUT_2_2 */

        if (xi_event->evtype == XI_ButtonPress)
          _clutter_stage_x11_set_user_time (stage_x11, event->button.time);

        retval = CLUTTER_TRANSLATE_QUEUE;
      }
      break;

    case XI_Motion:
      {
        XIDeviceEvent *xev = (XIDeviceEvent *) xi_event;
        gdouble delta_x, delta_y;

        source_device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                             GINT_TO_POINTER (xev->sourceid));
        device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                      GINT_TO_POINTER (xev->deviceid));

        /* Set the stage for core events coming out of nowhere (see bug #684509) */
        if (clutter_input_device_get_device_mode (device) == CLUTTER_INPUT_MODE_MASTER &&
            clutter_input_device_get_pointer_stage (device) == NULL &&
            stage != NULL)
          _clutter_input_device_set_stage (device, stage);

        if (scroll_valuators_changed (source_device,
                                      &xev->valuators,
                                      &delta_x, &delta_y))
          {
            event->scroll.type = event->type = CLUTTER_SCROLL;
            event->scroll.direction = CLUTTER_SCROLL_SMOOTH;

            event->scroll.stage = stage;
            event->scroll.time = xev->time;
            event->scroll.x = xev->event_x;
            event->scroll.y = xev->event_y;
            event->scroll.modifier_state =
              _clutter_input_device_xi2_translate_state (&xev->mods,
                                                         &xev->buttons,
                                                         &xev->group);

            clutter_event_set_scroll_delta (event, delta_x, delta_y);
            clutter_event_set_source_device (event, source_device);
            clutter_event_set_device (event, device);

            CLUTTER_NOTE (EVENT,
                          "smooth scroll: win:0x%x device:%d '%s' (x:%.2f, y:%.2f, delta:%f, %f)",
                          (unsigned int) stage_x11->xwin,
                          event->scroll.device->id,
                          event->scroll.device->device_name,
                          event->scroll.x,
                          event->scroll.y,
                          delta_x, delta_y);

            retval = CLUTTER_TRANSLATE_QUEUE;
            break;
          }

        event->motion.type = event->type = CLUTTER_MOTION;

        event->motion.stage = stage;

        event->motion.time = xev->time;
        event->motion.x = xev->event_x;
        event->motion.y = xev->event_y;
        event->motion.modifier_state =
          _clutter_input_device_xi2_translate_state (&xev->mods,
                                                     &xev->buttons,
                                                     &xev->group);

        clutter_event_set_source_device (event, source_device);
        clutter_event_set_device (event, device);

        event->motion.axes = translate_axes (event->motion.device,
                                             event->motion.x,
                                             event->motion.y,
                                             stage_x11,
                                             &xev->valuators);

        if (source_device != NULL && device->stage != NULL)
          _clutter_input_device_set_stage (source_device, device->stage);

#ifdef HAVE_XINPUT_2_2
        if (xev->flags & XIPointerEmulated)
          _clutter_event_set_pointer_emulated (event, TRUE);
#endif /* HAVE_XINPUT_2_2 */

        CLUTTER_NOTE (EVENT, "motion: win:0x%x device:%d '%s' (x:%.2f, y:%.2f, axes:%s)",
                      (unsigned int) stage_x11->xwin,
                      event->motion.device->id,
                      event->motion.device->device_name,
                      event->motion.x,
                      event->motion.y,
                      event->motion.axes != NULL ? "yes" : "no");

        retval = CLUTTER_TRANSLATE_QUEUE;
      }
      break;

#ifdef HAVE_XINPUT_2_2
    case XI_TouchBegin:
    case XI_TouchEnd:
      {
        XIDeviceEvent *xev = (XIDeviceEvent *) xi_event;

        source_device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                             GINT_TO_POINTER (xev->sourceid));

        if (xi_event->evtype == XI_TouchBegin)
          event->touch.type = event->type = CLUTTER_TOUCH_BEGIN;
        else
          event->touch.type = event->type = CLUTTER_TOUCH_END;

        event->touch.stage = stage;
        event->touch.time = xev->time;
        event->touch.x = xev->event_x;
        event->touch.y = xev->event_y;
        event->touch.modifier_state =
          _clutter_input_device_xi2_translate_state (&xev->mods,
                                                     &xev->buttons,
                                                     &xev->group);

        clutter_event_set_source_device (event, source_device);

        device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                      GINT_TO_POINTER (xev->deviceid));
        clutter_event_set_device (event, device);

        event->touch.axes = translate_axes (event->touch.device,
                                            event->motion.x,
                                            event->motion.y,
                                            stage_x11,
                                            &xev->valuators);

        if (xi_event->evtype == XI_TouchBegin)
          {
            event->touch.modifier_state |= CLUTTER_BUTTON1_MASK;

            _clutter_stage_x11_set_user_time (stage_x11, event->touch.time);
          }

        event->touch.sequence = GUINT_TO_POINTER (xev->detail);

        if (xev->flags & XITouchEmulatingPointer)
          _clutter_event_set_pointer_emulated (event, TRUE);

        CLUTTER_NOTE (EVENT, "touch %s: win:0x%x device:%d '%s' (seq:%d, x:%.2f, y:%.2f, axes:%s)",
                      event->type == CLUTTER_TOUCH_BEGIN ? "begin" : "end",
                      (unsigned int) stage_x11->xwin,
                      event->touch.device->id,
                      event->touch.device->device_name,
                      GPOINTER_TO_UINT (event->touch.sequence),
                      event->touch.x,
                      event->touch.y,
                      event->touch.axes != NULL ? "yes" : "no");

        retval = CLUTTER_TRANSLATE_QUEUE;
      }
      break;

    case XI_TouchUpdate:
      {
        XIDeviceEvent *xev = (XIDeviceEvent *) xi_event;

        source_device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                             GINT_TO_POINTER (xev->sourceid));

        event->touch.type = event->type = CLUTTER_TOUCH_UPDATE;
        event->touch.stage = stage;
        event->touch.time = xev->time;
        event->touch.sequence = GUINT_TO_POINTER (xev->detail);
        event->touch.x = xev->event_x;
        event->touch.y = xev->event_y;

        clutter_event_set_source_device (event, source_device);

        device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                      GINT_TO_POINTER (xev->deviceid));
        clutter_event_set_device (event, device);

        event->touch.axes = translate_axes (event->touch.device,
                                            event->motion.x,
                                            event->motion.y,
                                            stage_x11,
                                            &xev->valuators);

        event->touch.modifier_state =
          _clutter_input_device_xi2_translate_state (&xev->mods,
                                                     &xev->buttons,
                                                     &xev->group);
        event->touch.modifier_state |= CLUTTER_BUTTON1_MASK;

        if (xev->flags & XITouchEmulatingPointer)
          _clutter_event_set_pointer_emulated (event, TRUE);

        CLUTTER_NOTE (EVENT, "touch update: win:0x%x device:%d '%s' (seq:%d, x:%.2f, y:%.2f, axes:%s)",
                      (unsigned int) stage_x11->xwin,
                      event->touch.device->id,
                      event->touch.device->device_name,
                      GPOINTER_TO_UINT (event->touch.sequence),
                      event->touch.x,
                      event->touch.y,
                      event->touch.axes != NULL ? "yes" : "no");

        retval = CLUTTER_TRANSLATE_QUEUE;
      }
      break;
#endif /* HAVE_XINPUT_2_2 */

    case XI_Enter:
    case XI_Leave:
      {
        XIEnterEvent *xev = (XIEnterEvent *) xi_event;

        device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                      GINT_TO_POINTER (xev->deviceid));

        source_device = g_hash_table_lookup (manager_xi2->devices_by_id,
                                             GINT_TO_POINTER (xev->sourceid));

        if (xi_event->evtype == XI_Enter)
          {
            event->crossing.type = event->type = CLUTTER_ENTER;

            event->crossing.stage = stage;
            event->crossing.source = CLUTTER_ACTOR (stage);
            event->crossing.related = NULL;

            event->crossing.time = xev->time;
            event->crossing.x = xev->event_x;
            event->crossing.y = xev->event_y;

            _clutter_input_device_set_stage (device, stage);
          }
        else
          {
            if (device->stage == NULL)
              {
                CLUTTER_NOTE (EVENT,
                              "Discarding Leave for ButtonRelease "
                              "event off-stage");

                retval = CLUTTER_TRANSLATE_REMOVE;
                break;
              }

            event->crossing.type = event->type = CLUTTER_LEAVE;

            event->crossing.stage = stage;
            event->crossing.source = CLUTTER_ACTOR (stage);
            event->crossing.related = NULL;

            event->crossing.time = xev->time;
            event->crossing.x = xev->event_x;
            event->crossing.y = xev->event_y;

            _clutter_input_device_set_stage (device, NULL);
          }

        _clutter_input_device_reset_scroll_info (source_device);

        clutter_event_set_device (event, device);
        clutter_event_set_source_device (event, source_device);

        retval = CLUTTER_TRANSLATE_QUEUE;
      }
      break;

    case XI_FocusIn:
    case XI_FocusOut:
      retval = CLUTTER_TRANSLATE_CONTINUE;
      break;
    }

  return retval;
}