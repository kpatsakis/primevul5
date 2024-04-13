pcstatus_do_reset(pcl_state_t * pcs, pcl_reset_type_t type)
{
    if (type & (pcl_reset_initial | pcl_reset_printer)) {
        if (type & pcl_reset_initial) {
            pcs->status.buffer = 0;
            pcs->status.write_pos = 0;
            pcs->status.read_pos = 0;
        }
        pcs->location_type = 0;
        pcs->location_unit = 0;
    }

    return 0;
}