clutter_device_manager_xi2_select_events (ClutterDeviceManager *manager,
                                          Window                xwindow,
                                          XIEventMask          *event_mask)
{
  Display *xdisplay;

  xdisplay = clutter_x11_get_default_display ();

  XISelectEvents (xdisplay, xwindow, event_mask, 1);
}