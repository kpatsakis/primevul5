dwarf_check_lineheader_b(Dwarf_Die die, int *err_count_out,
    Dwarf_Error *err)
{
    int res = 0;

    int only_line_header = 1;
    res = _dwarf_internal_printlines(die,err_count_out,
        only_line_header,err);
    return res;
}