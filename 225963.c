dwarf_check_lineheader(Dwarf_Die die, int *err_count_out)
{
    int res = 0;
    Dwarf_Error err = 0;

    int only_line_header = 1;
    res = _dwarf_internal_printlines(die,err_count_out,
        only_line_header,&err);
    if (res == DW_DLV_ERROR) {
        Dwarf_CU_Context c = 0;
        Dwarf_Debug dbg = 0;

        c = die->di_cu_context;
        if (!c) {
            return;
        }
        dbg = c->cc_dbg;
        dwarf_dealloc(dbg,err,DW_DLA_ERROR);
        err = 0;
    }
}