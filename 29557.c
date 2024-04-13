void reds_set_client_mouse_allowed(int is_client_mouse_allowed, int x_res, int y_res)
{
    reds->monitor_mode.x_res = x_res;
    reds->monitor_mode.y_res = y_res;
    reds->dispatcher_allows_client_mouse = is_client_mouse_allowed;
    reds_update_mouse_mode();
    if (reds->is_client_mouse_allowed && inputs_has_tablet()) {
        inputs_set_tablet_logical_size(reds->monitor_mode.x_res, reds->monitor_mode.y_res);
    }
}
