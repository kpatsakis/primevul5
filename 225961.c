print_file_entry_details(Dwarf_Debug dbg,
    unsigned int line_version,
    Dwarf_Line_Context line_context)
{
    dwarfstring m5;

    dwarfstring_constructor_static(&m5,locallinebuf,
        sizeof(locallinebuf));
    if (line_version == DW_LINE_VERSION5) {
        unsigned i = 0;
        unsigned dfcount = line_context->lc_file_name_format_count;

        dwarfstring_append_printf_u(&m5,
            "  file entry format count      %u\n",dfcount);
        for ( ; i < dfcount;++i) {
            struct Dwarf_Unsigned_Pair_s *valpair = 0;
            const char *tname = 0;
            const char *fname = 0;
            int res;

            valpair = line_context->lc_file_format_values +i;
            dwarfstring_append_printf_u(&m5,
                "  format [%2u] ",i);
            res = dwarf_get_LNCT_name(valpair->up_first,&tname);
            if ( res != DW_DLV_OK) {
                tname = "<unknown type>";
            }
            dwarfstring_append_printf_u(&m5,
                " type 0x%" DW_PR_XZEROS DW_PR_DUx,
                valpair->up_first);
            dwarfstring_append_printf_s(&m5,
                " %-20s\n",(char *)tname);
            res = dwarf_get_FORM_name(valpair->up_second,&fname);
            if ( res != DW_DLV_OK) {
                fname = "<unknown form>";
            }
            dwarfstring_append_printf_u(&m5,
                "               code 0x%"
                DW_PR_XZEROS DW_PR_DUx,
                valpair->up_second);
            dwarfstring_append_printf_s(&m5, " %-20s\n",
                (char *)fname);
            _dwarf_printf(dbg,dwarfstring_string(&m5));
            dwarfstring_reset(&m5);
        }
        dwarfstring_destructor(&m5);
        print_just_file_entry_details(dbg,line_context);
    } else {
        print_just_file_entry_details(dbg,line_context);
        dwarfstring_destructor(&m5);
    }

}