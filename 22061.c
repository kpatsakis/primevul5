pcstatus_do_registration(pcl_parser_state_t * pcl_parser_state,
                         gs_memory_t * mem)
{                               /* Register commands */
    DEFINE_CLASS('*') {
    's', 'T',
            PCL_COMMAND("Set Readback Location Type",
                            pcl_set_readback_loc_type,
                            pca_neg_error | pca_big_error)}, {
    's', 'U',
            PCL_COMMAND("Set Readback Location Unit",
                            pcl_set_readback_loc_unit,
                            pca_neg_error | pca_big_error)}, {
    's', 'I',
            PCL_COMMAND("Inquire Readback Entity",
                            pcl_inquire_readback_entity,
                            pca_neg_error | pca_big_error)}, {
    's', 'M',
            PCL_COMMAND("Free Space", pcl_free_space,
                            pca_neg_ok | pca_big_ok)},
        END_CLASS
        DEFINE_CLASS_COMMAND_ARGS('&', 'r', 'F', "Flush All Pages",
                                  pcl_flush_all_pages,
                                  pca_neg_error | pca_big_error)
        DEFINE_CLASS_COMMAND_ARGS('*', 's', 'X', "Echo",
                                  pcl_echo, pca_neg_ok | pca_big_error)
        return 0;
}