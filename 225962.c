_dwarf_print_lines(Dwarf_Die die, Dwarf_Error * error)
{
    int only_line_header = 0;
    int err_count = 0;
    int res = _dwarf_internal_printlines(die,
        &err_count,
        only_line_header,error);
    /* No way to get error count back in this interface */
    return res;
}