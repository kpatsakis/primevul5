pcl_set_readback_loc_unit(pcl_args_t * pargs, pcl_state_t * pcs)
{
    pcs->location_unit = uint_arg(pargs);
    return 0;
}