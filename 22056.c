pcl_set_readback_loc_type(pcl_args_t * pargs, pcl_state_t * pcs)
{
    pcs->location_type = uint_arg(pargs);
    return 0;
}