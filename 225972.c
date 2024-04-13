print_experimental_counts(Dwarf_Debug dbg, int line_version,
    Dwarf_Line_Context line_context)
{
    if (line_version == EXPERIMENTAL_LINE_TABLES_VERSION) {
        print_experimental_subprograms_list(dbg,line_context);
    }
}