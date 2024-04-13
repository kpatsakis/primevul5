pcl_flush_all_pages(pcl_args_t * pargs, pcl_state_t * pcs)
{
    switch (uint_arg(pargs)) {
        case 0:
            {                   /* Flush all complete pages. */
                /* This is a driver function.... */
                return 0;
            }
        case 1:
            {                   /* Flush all pages, including an incomplete one. */
                int code = pcl_end_page_if_marked(pcs);

                if (code >= 0)
                    code = pcl_home_cursor(pcs);
                return code;
            }
        default:
            return e_Range;
    }
}