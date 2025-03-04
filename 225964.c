print_include_directory_details(Dwarf_Debug dbg,
    unsigned int line_version,
    Dwarf_Line_Context line_context)
{
    Dwarf_Unsigned u = 0;
    dwarfstring    m4;
    Dwarf_Unsigned indexbase = 0;
    Dwarf_Unsigned indexlimit = 0;

    dwarfstring_constructor_static(&m4,locallinebuf,
        sizeof(locallinebuf));
    if (line_version == DW_LINE_VERSION5) {
        unsigned i = 0;
        unsigned dfcount =
            line_context->lc_directory_entry_format_count;

        dwarfstring_constructor(&m4);
        dwarfstring_append_printf_u(&m4,
            "  directory entry format count %u\n",dfcount);
        _dwarf_printf(dbg,dwarfstring_string(&m4));
        dwarfstring_reset(&m4);
        for ( ; i < dfcount;++i) {
            struct Dwarf_Unsigned_Pair_s *valpair = 0;
            const char *tname = 0;
            const char *fname = 0;
            int res;

            valpair = line_context->lc_directory_format_values +i;
            dwarfstring_append_printf_u(&m4,
                "  format [%2u] ",i);
            res = dwarf_get_LNCT_name(valpair->up_first,
                &tname);
            if ( res != DW_DLV_OK) {
                tname = "<unknown type>";
            }
            dwarfstring_append_printf_u (&m4,
                " type 0x%" DW_PR_XZEROS DW_PR_DUx,
                valpair->up_first);
            dwarfstring_append_printf_s (&m4,
                " %-20s\n",(char *)tname);
            res = dwarf_get_FORM_name(valpair->up_second,&fname);
            if ( res != DW_DLV_OK) {
                fname = "<unknown form>";
            }
            dwarfstring_append_printf_u(&m4,
                "               code 0x%" DW_PR_XZEROS DW_PR_DUx ,
                valpair->up_second);
            dwarfstring_append_printf_s(&m4,
                " %-20s\n", (char *)fname);
            _dwarf_printf(dbg,dwarfstring_string(&m4));
            dwarfstring_reset(&m4);

        }
    }
    /*  Common print of the directories.
        For DWARF 2,3,4 it has always started
        the indexing at 0 even though the directory index
        in line entries starts at 1 (zero meaning
        current directory at compile time).
        That is odd, given the non-dash-v printed
        starting at 1.  So lets adjust for consistency. */
    if (line_version == DW_LINE_VERSION5) {
        dwarfstring_append_printf_i(&m4,
            "  include directories count %d\n",
            (int) line_context->lc_include_directories_count);
    } else {
        if(!line_context->lc_include_directories_count) {
            dwarfstring_append_printf_i(&m4,
                "  include directories count %d\n",
                (int) line_context->lc_include_directories_count);
        } else {
            dwarfstring_append_printf_i(&m4,
                "  include directories count %d"
                " (index starts at 1)\n",
                (int) line_context->lc_include_directories_count);
        }
    }
    _dwarf_printf(dbg,dwarfstring_string(&m4));
    dwarfstring_reset(&m4);
    if (line_version == DW_LINE_VERSION5) {
        indexbase = 0;
        indexlimit =  line_context->lc_include_directories_count;
    } else {
        indexbase = 1;
        indexlimit = 1 + line_context->lc_include_directories_count;
    }
    for (u = indexbase; u < indexlimit; ++u) {
        dwarfstring_append_printf_u(&m4,
            "  include dir[%u] ",u);
        dwarfstring_append_printf_s(&m4,
            "%s\n",(char *)
            line_context->lc_include_directories[u-indexbase]);
        _dwarf_printf(dbg,dwarfstring_string(&m4));
        dwarfstring_reset(&m4);
    }
    dwarfstring_destructor(&m4);
}