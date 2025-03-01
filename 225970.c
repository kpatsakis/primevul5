print_line_detail(
    Dwarf_Debug dbg,
    const char *prefix,
    int opcode,
    unsigned curr_line,
    struct Dwarf_Line_Registers_s * regs,
    Dwarf_Bool is_single_table, Dwarf_Bool is_actuals_table)
{
    dwarfstring m1;

    dwarfstring_constructor_static(&m1,locallinebuf,
        sizeof(locallinebuf));
    if(!is_single_table && is_actuals_table) {
        dwarfstring_append_printf_s(&m1,"%-15s ",(char *)prefix);
        dwarfstring_append_printf_i(&m1,"%3d ",opcode);
        dwarfstring_append_printf_u(&m1,"0x%" DW_PR_XZEROS DW_PR_DUx,
            regs->lr_address);
        dwarfstring_append_printf_u(&m1,"/%01u",regs->lr_op_index);
        dwarfstring_append_printf_u(&m1," %5lu", regs->lr_line);
        dwarfstring_append_printf_u(&m1," %3d",regs->lr_isa);
        dwarfstring_append_printf_i(&m1,"   %1d",
            regs->lr_basic_block);
        dwarfstring_append_printf_i(&m1,"%1d\n",
            regs->lr_end_sequence);
        _dwarf_printf(dbg,dwarfstring_string(&m1));
        dwarfstring_destructor(&m1);
        return;
    }
    if(!is_single_table && !is_actuals_table) {
        dwarfstring_append_printf_i(&m1,
            "[%3d] "  /* row number */, curr_line);
        dwarfstring_append_printf_s(&m1,
            "%-15s ",(char *)prefix);
        dwarfstring_append_printf_i(&m1,
            "%3d ",opcode);
        dwarfstring_append_printf_u(&m1,
            "x%" DW_PR_XZEROS DW_PR_DUx, regs->lr_address);
        dwarfstring_append_printf_u(&m1,
            "/%01u", regs->lr_op_index);
        dwarfstring_append_printf_u(&m1," %2lu ",regs->lr_file);
        dwarfstring_append_printf_u(&m1,"%4lu  ",regs->lr_line);
        dwarfstring_append_printf_u(&m1,"%1lu",regs->lr_column);
        if (regs->lr_discriminator ||
            regs->lr_prologue_end ||
            regs->lr_epilogue_begin ||
            regs->lr_isa ||
            regs->lr_is_stmt ||
            regs->lr_call_context ||
            regs->lr_subprogram) {
            dwarfstring_append_printf_u(&m1,
                "   x%02" DW_PR_DUx ,
                regs->lr_discriminator); /* DWARF4 */
            dwarfstring_append_printf_u(&m1,
                "  x%02" DW_PR_DUx,
                regs->lr_call_context); /* EXPERIMENTAL */
            dwarfstring_append_printf_u(&m1,
                "  x%02" DW_PR_DUx ,
                regs->lr_subprogram); /* EXPERIMENTAL */
            dwarfstring_append_printf_i(&m1,
                "  %1d", regs->lr_is_stmt);
            dwarfstring_append_printf_i(&m1,
                "%1d", (int) regs->lr_isa);
            dwarfstring_append_printf_i(&m1,
                "%1d", regs->lr_prologue_end); /* DWARF3 */
            dwarfstring_append_printf_i(&m1,
                "%1d", regs->lr_epilogue_begin); /* DWARF3 */
        }
        dwarfstring_append(&m1,"\n");
        _dwarf_printf(dbg,dwarfstring_string(&m1));
        dwarfstring_destructor(&m1);
        return;
    }
    /*  In the first quoted line below:
        3d looks better than 2d, but best to do that as separate
        change and test from two-level-line-tables.  */
    dwarfstring_append_printf_s(&m1,
        "%-15s ",(char *)prefix);
    dwarfstring_append_printf_i(&m1,
        "%2d ",opcode);
    dwarfstring_append_printf_u(&m1,
        "0x%" DW_PR_XZEROS DW_PR_DUx " ",
        regs->lr_address);
    dwarfstring_append_printf_u(&m1,
        "%2lu   ", regs->lr_file);
    dwarfstring_append_printf_u(&m1,
        "%4lu ", regs->lr_line);
    dwarfstring_append_printf_u(&m1,
        "%2lu   ", regs->lr_column);
    dwarfstring_append_printf_i(&m1,
        "%1d ",regs->lr_is_stmt);
    dwarfstring_append_printf_i(&m1,
        "%1d ", regs->lr_basic_block);
    dwarfstring_append_printf_i(&m1,
        "%1d",regs->lr_end_sequence);
    if (regs->lr_discriminator ||
        regs->lr_prologue_end ||
        regs->lr_epilogue_begin ||
        regs->lr_isa) {
        dwarfstring_append_printf_i(&m1,
            " %1d", regs->lr_prologue_end); /* DWARF3 */
        dwarfstring_append_printf_i(&m1,
            " %1d", regs->lr_epilogue_begin); /* DWARF3 */
        dwarfstring_append_printf_i(&m1,
            " %1d", regs->lr_isa); /* DWARF3 */
        dwarfstring_append_printf_u(&m1,
            " 0x%" DW_PR_DUx , regs->lr_discriminator); /* DWARF4 */
    }
    dwarfstring_append(&m1, "\n");
    _dwarf_printf(dbg,dwarfstring_string(&m1));
    dwarfstring_destructor(&m1);
}