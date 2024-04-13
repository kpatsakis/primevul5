print_experimental_subprograms_list(Dwarf_Debug dbg,
    Dwarf_Line_Context line_context)
{
    /*  Print the subprograms list. */
    Dwarf_Unsigned count = line_context->lc_subprogs_count;
    Dwarf_Unsigned exu = 0;
    Dwarf_Subprog_Entry sub = line_context->lc_subprogs;
    dwarfstring m6;

    dwarfstring_constructor_static(&m6,locallinebuf,
        sizeof(locallinebuf));
    dwarfstring_append_printf_u(&m6,
        "  subprograms count %" DW_PR_DUu "\n",count);
    if (count > 0) {
        dwarfstring_append(&m6,"    indx  file   line   name\n");
    }
    _dwarf_printf(dbg,dwarfstring_string(&m6));
    dwarfstring_reset(&m6);
    for (exu = 0 ; exu < count ; exu++,sub++) {
        dwarfstring_append_printf_u(&m6,
            "    [%2" DW_PR_DUu,exu+1);
        dwarfstring_append_printf_u(&m6,
            "] %4" DW_PR_DUu,sub->ds_decl_file);
        dwarfstring_append_printf_u(&m6,
            "    %4" DW_PR_DUu ,sub->ds_decl_line);
        dwarfstring_append_printf_s(&m6,
            " %s\n",(char *)sub->ds_subprog_name);
        _dwarf_printf(dbg,dwarfstring_string(&m6));
        dwarfstring_reset(&m6);
    }
    dwarfstring_destructor(&m6);
}