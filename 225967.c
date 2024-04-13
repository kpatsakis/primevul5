dwarf_print_lines(Dwarf_Die die,
    Dwarf_Error * error,
    int *error_count)
{
    int only_line_header = 0;
    int res = _dwarf_internal_printlines(die,
        error_count,
        only_line_header,error);
    return res;
}