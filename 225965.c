print_just_file_entry_details(Dwarf_Debug dbg,
    Dwarf_Line_Context line_context)
{
    unsigned fiu = 0;
    Dwarf_File_Entry fe = line_context->lc_file_entries;
    Dwarf_File_Entry fe2 = fe;
    dwarfstring m3;

    dwarfstring_constructor_static(&m3,locallinebuf,
        sizeof(locallinebuf));
    dwarfstring_append_printf_i(&m3,
        "  file names count      %d\n",
        line_context->lc_file_entry_count);
    _dwarf_printf(dbg,dwarfstring_string(&m3));
    dwarfstring_reset(&m3);
    for (fiu = 0 ; fe2 ; fe2 = fe->fi_next,++fiu ) {
        Dwarf_Unsigned tlm2 = 0;
        unsigned filenum = 0;

        fe = fe2;
        tlm2 = fe->fi_time_last_mod;
        filenum = fiu+1;

        /*  The space character at the end of line is silly,
            but lets leave it there for the moment to avoid
            changing output.  */
        if (line_context->lc_file_entry_count > 9) {
            dwarfstring_append_printf_u(&m3,
                "  file[%2u] ",fiu);
        } else {
            dwarfstring_append_printf_u(&m3,
                "  file[%u]  ", fiu);
        }
        /*  DWARF5 can have a null fi_file_name
            if  the format code in the
            line table header is unknown, such
            as in a corrupt object file. */
        dwarfstring_append_printf_s(&m3,
            "%-20s ",
            fe->fi_file_name?
            (char *) fe->fi_file_name:
            "<no file name>");
        dwarfstring_append_printf_u(&m3,
            "(file-number: %u)\n",
            filenum);
        _dwarf_printf(dbg,dwarfstring_string(&m3));
        dwarfstring_reset(&m3);
        if (fe->fi_dir_index_present) {
            Dwarf_Unsigned di = 0;
            di = fe->fi_dir_index;
            dwarfstring_append_printf_i(&m3,
                "    dir index %d\n", di);
        }
        if (fe->fi_time_last_mod_present) {
            time_t tt = (time_t) tlm2;

            /* ctime supplies newline */
            dwarfstring_append_printf_u(&m3,
                "    last time 0x%x ",tlm2);
            dwarfstring_append(&m3,(char *)ctime(&tt));
        }
        if (fe->fi_file_length_present) {
            Dwarf_Unsigned fl = 0;

            fl = fe->fi_file_length;
            dwarfstring_append_printf_i(&m3,
                "    file length %ld ",fl);
            dwarfstring_append_printf_u(&m3,
                "0x%lx\n",fl);
        }
        if (fe->fi_md5_present) {
            char *c = (char *)&fe->fi_md5_value;
            char *end = c+sizeof(fe->fi_md5_value);
            dwarfstring_append(&m3, "    file md5 value 0x");
            while(c < end) {
                dwarfstring_append_printf_u(&m3,
                    "%02x",0xff&*c);
                ++c;
            }
            dwarfstring_append(&m3,"\n");
        }
        if (dwarfstring_strlen(&m3)) {
            _dwarf_printf(dbg,dwarfstring_string(&m3));
            dwarfstring_reset(&m3);
        }
    }
    dwarfstring_destructor(&m3);
}