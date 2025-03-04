_dwarf_internal_printlines(Dwarf_Die die,
    int * err_count_out,
    int only_line_header,
    Dwarf_Error * error)
{
    /*  This pointer is used to scan the portion of the .debug_line
        section for the current cu. */
    Dwarf_Small *line_ptr = 0;
    Dwarf_Small *orig_line_ptr = 0;

    /*  Pointer to a DW_AT_stmt_list attribute in case
        it exists in the die. */
    Dwarf_Attribute stmt_list_attr = 0;

    /*  Pointer to DW_AT_comp_dir attribute in die. */
    Dwarf_Attribute comp_dir_attr = 0;

    /*  Pointer to name of compilation directory. */
    Dwarf_Small *comp_dir = NULL;

    /*  Offset into .debug_line specified by a DW_AT_stmt_list
        attribute. */
    Dwarf_Unsigned line_offset = 0;

    /*  These variables are used to decode leb128 numbers. Leb128_num
        holds the decoded number, and leb128_length is its length in
        bytes. */
    Dwarf_Half attrform = 0;

    /*  In case there are wierd bytes 'after' the line table
        prologue this lets us print something. This is a gcc
        compiler bug and we expect the bytes count to be 12.  */
    Dwarf_Small* bogus_bytes_ptr = 0;
    Dwarf_Unsigned bogus_bytes_count = 0;
    Dwarf_Half address_size = 0;
    Dwarf_Unsigned fission_offset = 0;
    unsigned line_version = 0;


    /* The Dwarf_Debug this die belongs to. */
    Dwarf_Debug dbg = 0;
    Dwarf_CU_Context cu_context = 0;
    Dwarf_Line_Context line_context = 0;
    int resattr = DW_DLV_ERROR;
    int lres =    DW_DLV_ERROR;
    int res  =    DW_DLV_ERROR;
    Dwarf_Small *line_ptr_actuals  = 0;
    Dwarf_Small *line_ptr_end = 0;
    Dwarf_Small *section_start = 0;

    /* ***** BEGIN CODE ***** */

    if (error != NULL) {
        *error = NULL;
    }

    CHECK_DIE(die, DW_DLV_ERROR);
    cu_context = die->di_cu_context;
    dbg = cu_context->cc_dbg;

    res = _dwarf_load_section(dbg, &dbg->de_debug_line,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    if (!dbg->de_debug_line.dss_size) {
        return (DW_DLV_NO_ENTRY);
    }

    address_size = _dwarf_get_address_size(dbg, die);
    resattr = dwarf_attr(die, DW_AT_stmt_list, &stmt_list_attr,
        error);
    if (resattr != DW_DLV_OK) {
        return resattr;
    }
    /*  The list of relevant FORMs is small.
        DW_FORM_data4, DW_FORM_data8, DW_FORM_sec_offset
    */
    lres = dwarf_whatform(stmt_list_attr,&attrform,error);
    if (lres != DW_DLV_OK) {
        dwarf_dealloc(dbg,stmt_list_attr, DW_DLA_ATTR);
        return lres;
    }
    if (attrform != DW_FORM_data4 && attrform != DW_FORM_data8 &&
        attrform != DW_FORM_sec_offset ) {
        dwarf_dealloc(dbg,stmt_list_attr, DW_DLA_ATTR);
        _dwarf_error(dbg, error, DW_DLE_LINE_OFFSET_BAD);
        return (DW_DLV_ERROR);
    }
    lres = dwarf_global_formref(stmt_list_attr, &line_offset, error);
    if (lres != DW_DLV_OK) {
        dwarf_dealloc(dbg,stmt_list_attr, DW_DLA_ATTR);
        return lres;
    }

    if (line_offset >= dbg->de_debug_line.dss_size) {
        dwarf_dealloc(dbg,stmt_list_attr, DW_DLA_ATTR);
        _dwarf_error(dbg, error, DW_DLE_LINE_OFFSET_BAD);
        return (DW_DLV_ERROR);
    }
    section_start =  dbg->de_debug_line.dss_data;
    {
        Dwarf_Unsigned fission_size = 0;
        int resfis = _dwarf_get_fission_addition_die(die,
            DW_SECT_LINE,
            &fission_offset,&fission_size,error);
        if(resfis != DW_DLV_OK) {
            dwarf_dealloc(dbg,stmt_list_attr, DW_DLA_ATTR);
            return resfis;
        }
    }

    orig_line_ptr = section_start + line_offset + fission_offset;
    line_ptr = orig_line_ptr;
    dwarf_dealloc(dbg, stmt_list_attr, DW_DLA_ATTR);

    /*  If die has DW_AT_comp_dir attribute, get the string
        that names the compilation directory. */
    resattr = dwarf_attr(die, DW_AT_comp_dir, &comp_dir_attr, error);
    if (resattr == DW_DLV_ERROR) {
        return resattr;
    }
    if (resattr == DW_DLV_OK) {
        int cres = DW_DLV_ERROR;
        char *cdir = 0;

        cres = dwarf_formstring(comp_dir_attr, &cdir, error);
        if (cres == DW_DLV_ERROR) {
            return cres;
        } else if (cres == DW_DLV_OK) {
            comp_dir = (Dwarf_Small *) cdir;
        }
    }
    if (resattr == DW_DLV_OK) {
        dwarf_dealloc(dbg, comp_dir_attr, DW_DLA_ATTR);
    }
    line_context = (Dwarf_Line_Context)
        _dwarf_get_alloc(dbg, DW_DLA_LINE_CONTEXT, 1);
    if (line_context == NULL) {
        _dwarf_error(dbg, error, DW_DLE_ALLOC_FAIL);
        return (DW_DLV_ERROR);
    }
    {
        Dwarf_Small *newlinep = 0;
        int dres = _dwarf_read_line_table_header(dbg,
            cu_context,
            section_start,
            line_ptr,
            dbg->de_debug_line.dss_size,
            &newlinep,
            line_context,
            &bogus_bytes_ptr,
            &bogus_bytes_count,
            error,
            err_count_out);
        if (dres == DW_DLV_ERROR) {
            dwarf_srclines_dealloc_b(line_context);
            return dres;
        }
        if (dres == DW_DLV_NO_ENTRY) {
            dwarf_srclines_dealloc_b(line_context);
            return dres;
        }
        line_ptr_end = line_context->lc_line_ptr_end;
        line_ptr = newlinep;
        if (line_context->lc_actuals_table_offset > 0) {
            line_ptr_actuals = line_context->lc_line_prologue_start +
                line_context->lc_actuals_table_offset;
        }
    }
    line_version = line_context->lc_version_number;
    line_context->lc_compilation_directory = comp_dir;
    if (only_line_header) {
        /* Just checking for header errors, nothing more here.*/
        dwarf_srclines_dealloc_b(line_context);
        return DW_DLV_OK;
    }
    do_line_print_now(dbg,line_version,comp_dir,line_context);
    print_include_directory_details(dbg,line_version,line_context);
    print_file_entry_details(dbg,line_version,line_context);
    print_experimental_counts(dbg, line_version,line_context);
    res = print_actuals_and_locals(dbg, line_context,
        bogus_bytes_count,bogus_bytes_ptr,
        orig_line_ptr,
        line_ptr,
        section_start,
        line_ptr_actuals,
        line_ptr_end,
        address_size,
        err_count_out,
        error);
    if (res  !=  DW_DLV_OK) {
        return res;
    }
    return DW_DLV_OK;
}