do_line_print_now(Dwarf_Debug dbg,
    int line_version,
    Dwarf_Small *comp_dir,
    Dwarf_Line_Context line_context)
{
    dwarfstring m7;
    Dwarf_Unsigned i = 0;

    dwarfstring_constructor(&m7);
    dwarfstring_append_printf_i(&m7,
        "total line info length %ld bytes,",
        line_context->lc_total_length);

    dwarfstring_append_printf_u(&m7,
        " line offset 0x%" DW_PR_XZEROS DW_PR_DUx,
        line_context->lc_section_offset);

    dwarfstring_append_printf_u(&m7,
        " %" DW_PR_DUu "\n",
        line_context->lc_section_offset);

    if (line_version <= DW_LINE_VERSION5) {
        dwarfstring_append_printf_i(&m7,
            "  line table version     %d\n",
        (int) line_context->lc_version_number);
    } else {
        dwarfstring_append_printf_u(&m7,
            "  line table version 0x%x\n",
            (int) line_context->lc_version_number);
    }
    if (line_version == DW_LINE_VERSION5) {
        dwarfstring_append_printf_i(&m7,
            "  address size          %d\n",
            line_context->lc_address_size);
        dwarfstring_append_printf_i(&m7,
            "  segment selector size %d\n",
            line_context->lc_segment_selector_size);
    }
    _dwarf_printf(dbg,dwarfstring_string(&m7));
    dwarfstring_reset(&m7);
    dwarfstring_append_printf_i(&m7,
        "  line table length field length %d\n",
        line_context->lc_length_field_length);
    dwarfstring_append_printf_i(&m7,
        "  prologue length       %d\n",
        line_context->lc_prologue_length);
    dwarfstring_append_printf_s(&m7,
        "  compilation_directory %s\n",
        comp_dir ? ((char *) comp_dir) : "");

    dwarfstring_append_printf_i(&m7,
        "  min instruction length %d\n",
        line_context->lc_minimum_instruction_length);
    _dwarf_printf(dbg,dwarfstring_string(&m7));
    dwarfstring_reset(&m7);
    if (line_version == DW_LINE_VERSION5 ||
        line_version == DW_LINE_VERSION4 ||
        line_version == EXPERIMENTAL_LINE_TABLES_VERSION) {
        dwarfstring_append_printf_u(&m7,
            "  maximum ops per instruction %u\n",
            line_context->lc_maximum_ops_per_instruction);
        _dwarf_printf(dbg,dwarfstring_string(&m7));
        dwarfstring_reset(&m7);
    }
    if (line_version == EXPERIMENTAL_LINE_TABLES_VERSION) {
        dwarfstring_append_printf_u(&m7, "  actuals table offset "
            "0x%" DW_PR_XZEROS DW_PR_DUx "\n",
            line_context->lc_actuals_table_offset);
        dwarfstring_append_printf_u(&m7,"  logicals table offset "
            "0x%" DW_PR_XZEROS DW_PR_DUx "\n",
            line_context->lc_logicals_table_offset);
        _dwarf_printf(dbg,dwarfstring_string(&m7));
        dwarfstring_reset(&m7);
    }
    dwarfstring_append_printf_i(&m7,
        "  default is stmt        %d\n",
        (int)line_context->lc_default_is_stmt);
    dwarfstring_append_printf_i(&m7,
        "  line base              %d\n",
        (int)line_context->lc_line_base);
    dwarfstring_append_printf_i(&m7,
        "  line_range             %d\n",
        (int)line_context->lc_line_range);
    dwarfstring_append_printf_i(&m7,
        "  opcode base            %d\n",
        (int)line_context->lc_opcode_base);
    dwarfstring_append_printf_i(&m7,
        "  standard opcode count  %d\n",
        (int)line_context->lc_std_op_count);
    _dwarf_printf(dbg,dwarfstring_string(&m7));
    dwarfstring_reset(&m7);

    for (i = 1; i < line_context->lc_opcode_base; i++) {
        dwarfstring_append_printf_i(&m7,
            "  opcode[%2d] length", (int) i);
        dwarfstring_append_printf_i(&m7,
            "  %d\n",
            (int) line_context->lc_opcode_length_table[i - 1]);
        _dwarf_printf(dbg,dwarfstring_string(&m7));
        dwarfstring_reset(&m7);
    }
    dwarfstring_destructor(&m7);
}