EXPORTED void annotatemore_close(void)
{
    /* close all the open databases */
    while (all_dbs_head)
        annotate_closedb(all_dbs_head);

    annotatemore_dbopen = 0;
}