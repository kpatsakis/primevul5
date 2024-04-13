get_event_stage (ClutterEventTranslator *translator,
                 XIEvent                *xi_event)
{
  Window xwindow = None;

  switch (xi_event->evtype)
    {
    case XI_KeyPress:
    case XI_KeyRelease:
    case XI_ButtonPress:
    case XI_ButtonRelease:
    case XI_Motion:
#ifdef HAVE_XINPUT_2_2
    case XI_TouchBegin:
    case XI_TouchUpdate:
    case XI_TouchEnd:
#endif /* HAVE_XINPUT_2_2 */
      {
        XIDeviceEvent *xev = (XIDeviceEvent *) xi_event;

        xwindow = xev->event;
      }
      break;

    case XI_Enter:
    case XI_Leave:
    case XI_FocusIn:
    case XI_FocusOut:
      {
        XIEnterEvent *xev = (XIEnterEvent *) xi_event;

        xwindow = xev->event;
      }
      break;

    default:
      break;
    }

  if (xwindow == None)
    return NULL;

  return clutter_x11_get_stage_from_window (xwindow);
}