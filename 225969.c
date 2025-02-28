print_actuals_and_locals(Dwarf_Debug dbg,
    Dwarf_Line_Context line_context,
    Dwarf_Unsigned bogus_bytes_count,
    Dwarf_Small *bogus_bytes_ptr,
    Dwarf_Small *orig_line_ptr,
    Dwarf_Small *line_ptr,
    Dwarf_Small *section_start,
    Dwarf_Small *line_ptr_actuals,
    Dwarf_Small *line_ptr_end,
    Dwarf_Half   address_size,
    int *        err_count_out,
    Dwarf_Error *error)
{
    int res = 0;
    dwarfstring m8;
    Dwarf_Unsigned offset = 0;

    dwarfstring_constructor(&m8);
    if (bogus_bytes_count > 0) {
        Dwarf_Unsigned wcount = bogus_bytes_count;
        Dwarf_Unsigned boffset = bogus_bytes_ptr - section_start;

        dwarfstring_append_printf_u(&m8,
            "*** DWARF CHECK: the line table prologue  header_length "
            " is %" DW_PR_DUu " too high, we pretend it is smaller.",
            wcount);
        dwarfstring_append_printf_u(&m8,
            "Section offset: 0x%"
            DW_PR_XZEROS DW_PR_DUx,
            boffset);
        dwarfstring_append_printf_u(&m8,
            " (%" DW_PR_DUu ") ***\n",
            boffset);
        *err_count_out += 1;
    }
    offset = line_ptr - section_start;
    dwarfstring_append_printf_u(&m8,
        "  statement prog offset in section: 0x%"
        DW_PR_XZEROS DW_PR_DUx,
        offset);
    dwarfstring_append_printf_u(&m8,
        " (%" DW_PR_DUu ")\n",
        offset);
    _dwarf_printf(dbg,dwarfstring_string(&m8));
    dwarfstring_reset(&m8);

    {
        Dwarf_Bool doaddrs = false;
        Dwarf_Bool dolines = true;

        if (!line_ptr_actuals) {
            /* Normal single level line table. */

            Dwarf_Bool is_single_table = true;
            Dwarf_Bool is_actuals_table = false;
            print_line_header(dbg, is_single_table, is_actuals_table);
            res = read_line_table_program(dbg,
                line_ptr, line_ptr_end, orig_line_ptr,
                section_start,
                line_context,
                address_size, doaddrs, dolines,
                is_single_table,
                is_actuals_table,
                error,
                err_count_out);
            if (res != DW_DLV_OK) {
                dwarfstring_destructor(&m8);
                dwarf_srclines_dealloc_b(line_context);
                return res;
            }
        } else {
            Dwarf_Bool is_single_table = false;
            Dwarf_Bool is_actuals_table = false;
            if (line_context->lc_version_number !=
                EXPERIMENTAL_LINE_TABLES_VERSION) {
                dwarf_srclines_dealloc_b(line_context);
                dwarfstring_destructor(&m8);
                _dwarf_error(dbg, error, DW_DLE_VERSION_STAMP_ERROR);
                return (DW_DLV_ERROR);
            }
            /* Read Logicals */
            print_line_header(dbg, is_single_table, is_actuals_table);
            res = read_line_table_program(dbg,
                line_ptr, line_ptr_actuals, orig_line_ptr,
                section_start,
                line_context,
                address_size, doaddrs, dolines,
                is_single_table,
                is_actuals_table,
                error,err_count_out);
            if (res != DW_DLV_OK) {
                dwarfstring_destructor(&m8);
                dwarf_srclines_dealloc_b(line_context);
                return res;
            }
            if (line_context->lc_actuals_table_offset > 0) {
                is_actuals_table = true;
                /* Read Actuals */

                print_line_header(dbg, is_single_table,
                    is_actuals_table);
                res = read_line_table_program(dbg,
                    line_ptr_actuals, line_ptr_end, orig_line_ptr,
                    section_start,
                    line_context,
                    address_size, doaddrs, dolines,
                    is_single_table,
                    is_actuals_table,
                    error,
                    err_count_out);
                if (res != DW_DLV_OK) {
                    dwarfstring_destructor(&m8);
                    dwarf_srclines_dealloc_b(line_context);
                    return res;
                }
            }
        }
    }
    dwarfstring_destructor(&m8);
    dwarf_srclines_dealloc_b(line_context);
    return DW_DLV_OK;
}