pcl_inquire_readback_entity(pcl_args_t * pargs, pcl_state_t * pcs)
{
    uint i = uint_arg(pargs);
    int unit = pcs->location_unit;
    stream st;

    static const char *entity_types[] = {
        "FONTS", "MACROS", "PATTERNS", "SYMBOLSETS", "FONTS EXTENDED"
    };
    pcl_data_storage_t storage;
    int code = 0;
    long pos;

    if (i > 4)
        return e_Range;
    status_begin(&st, pcs);
    stprintf(&st, "INFO %s\r\n", entity_types[i]);
    switch (pcs->location_type) {
        case 0:                /* invalid location */
            code = -1;
            break;
        case 1:                /* currently selected */
            storage = (pcl_data_storage_t) 0;   /* indicates currently selected */
            break;
        case 2:                /* all locations */
            storage = (pcl_data_storage_t) ~ 0;
            break;
        case 3:                /* internal */
            if (unit != 0) {
                code = -1;
                break;
            }
            storage = pcds_internal;
            break;
        case 4:                /* downloaded */
            if (unit > 2)
                code = -1;
            else {
                static const pcl_data_storage_t dl_masks[] =
                    { pcds_downloaded, pcds_temporary, pcds_permanent
                };
                storage = dl_masks[unit];
            }
            break;
        case 5:                /* cartridges */
            if (unit == 0)
                storage = (pcl_data_storage_t) pcds_all_cartridges;
            else if (unit <= pcds_cartridge_max)
                storage = (pcl_data_storage_t)
                    (1 << (pcds_cartridge_shift + unit - 1));
            else
                code = -1;
            break;
        case 6:                /* SIMMs */
            if (unit == 0)
                storage = (pcl_data_storage_t) pcds_all_simms;
            else if (unit <= pcds_simm_max)
                storage =
                    (pcl_data_storage_t) (1 << (pcds_simm_shift + unit - 1));
            else
                code = -1;
            break;
        default:
            code = -1;
            stputs(&st, "ERROR=INVALID ENTITY\r\n");
            break;
    }
    if (code >= 0) {
        pos = stell(&st);
        code = (*status_write[i]) (&st, pcs, storage);
        if (code >= 0) {
            if (stell(&st) == pos)
                stputs(&st, "ERROR=NONE\r\n");
            else if (storage == 0)      /* currently selected */
                stprintf(&st, "LOCTYPE=%d\r\nLOCUNIT=%d\r\n",
                         pcs->location_type, unit);
        }
    }
    if (code < 0) {
        if (code == e_Memory)
            stputs(&st, "ERROR=INTERNAL ERROR\r\n");
        else
            stputs(&st, "ERROR=INVALID LOCATION\r\n");
    }
    status_end(&st, pcs);
    return 0;
}