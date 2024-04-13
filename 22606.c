onig_compile(regex_t* reg, const UChar* pattern, const UChar* pattern_end,
	      OnigErrorInfo* einfo, const char *sourcefile, int sourceline)
{
#define COMPILE_INIT_SIZE  20

  int r;
  OnigDistance init_size;
  Node*  root;
  ScanEnv  scan_env = {0};
#ifdef USE_SUBEXP_CALL
  UnsetAddrList  uslist;
#endif

  if (IS_NOT_NULL(einfo)) einfo->par = (UChar* )NULL;

  scan_env.sourcefile = sourcefile;
  scan_env.sourceline = sourceline;
  reg->state = ONIG_STATE_COMPILING;

#ifdef ONIG_DEBUG
  if (!onig_is_prelude()) print_enc_string(stderr, reg->enc, pattern, pattern_end);
#endif

  if (reg->alloc == 0) {
    init_size = (pattern_end - pattern) * 2;
    if (init_size <= 0) init_size = COMPILE_INIT_SIZE;
    r = BBUF_INIT(reg, init_size);
    if (r != 0) goto end;
  }
  else
    reg->used = 0;

  reg->num_mem            = 0;
  reg->num_repeat         = 0;
  reg->num_null_check     = 0;
  reg->repeat_range_alloc = 0;
  reg->repeat_range       = (OnigRepeatRange* )NULL;
#ifdef USE_COMBINATION_EXPLOSION_CHECK
  reg->num_comb_exp_check = 0;
#endif

  r = onig_parse_make_tree(&root, pattern, pattern_end, reg, &scan_env);
  if (r != 0) goto err;

#ifdef ONIG_DEBUG_PARSE_TREE
# if 0
  fprintf(stderr, "ORIGINAL PARSE TREE:\n");
  if (!onig_is_prelude()) {
    print_tree(stderr, root);
  }
# endif
#endif

#ifdef USE_NAMED_GROUP
  /* mixed use named group and no-named group */
  if (scan_env.num_named > 0 &&
      IS_SYNTAX_BV(scan_env.syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
      !ONIG_IS_OPTION_ON(reg->options, ONIG_OPTION_CAPTURE_GROUP)) {
    if (scan_env.num_named != scan_env.num_mem)
      r = disable_noname_group_capture(&root, reg, &scan_env);
    else
      r = numbered_ref_check(root);

    if (r != 0) goto err;
  }
#endif

#ifdef USE_SUBEXP_CALL
  if (scan_env.num_call > 0) {
    r = unset_addr_list_init(&uslist, scan_env.num_call);
    if (r != 0) goto err;
    scan_env.unset_addr_list = &uslist;
    r = setup_subexp_call(root, &scan_env);
    if (r != 0) goto err_unset;
    r = subexp_recursive_check_trav(root, &scan_env);
    if (r  < 0) goto err_unset;
    r = subexp_inf_recursive_check_trav(root, &scan_env);
    if (r != 0) goto err_unset;

    reg->num_call = scan_env.num_call;
  }
  else
    reg->num_call = 0;
#endif

  r = setup_tree(root, reg, 0, &scan_env);
  if (r != 0) goto err_unset;

#ifdef ONIG_DEBUG_PARSE_TREE
  if (!onig_is_prelude()) print_tree(stderr, root);
#endif

  reg->capture_history  = scan_env.capture_history;
  reg->bt_mem_start     = scan_env.bt_mem_start;
  reg->bt_mem_start    |= reg->capture_history;
  if (IS_FIND_CONDITION(reg->options))
    BIT_STATUS_ON_ALL(reg->bt_mem_end);
  else {
    reg->bt_mem_end  = scan_env.bt_mem_end;
    reg->bt_mem_end |= reg->capture_history;
  }

#ifdef USE_COMBINATION_EXPLOSION_CHECK
  if (scan_env.backrefed_mem == 0
#ifdef USE_SUBEXP_CALL
      || scan_env.num_call == 0
#endif
      ) {
    setup_comb_exp_check(root, 0, &scan_env);
#ifdef USE_SUBEXP_CALL
    if (scan_env.has_recursion != 0) {
      scan_env.num_comb_exp_check = 0;
    }
    else
#endif
    if (scan_env.comb_exp_max_regnum > 0) {
      int i;
      for (i = 1; i <= scan_env.comb_exp_max_regnum; i++) {
	if (BIT_STATUS_AT(scan_env.backrefed_mem, i) != 0) {
	  scan_env.num_comb_exp_check = 0;
	  break;
	}
      }
    }
  }

  reg->num_comb_exp_check = scan_env.num_comb_exp_check;
#endif

  clear_optimize_info(reg);
#ifndef ONIG_DONT_OPTIMIZE
  r = set_optimize_info_from_tree(root, reg, &scan_env);
  if (r != 0) goto err_unset;
#endif

  if (IS_NOT_NULL(scan_env.mem_nodes_dynamic)) {
    xfree(scan_env.mem_nodes_dynamic);
    scan_env.mem_nodes_dynamic = (Node** )NULL;
  }

  r = compile_tree(root, reg);
  if (r == 0) {
    r = add_opcode(reg, OP_END);
#ifdef USE_SUBEXP_CALL
    if (scan_env.num_call > 0) {
      r = unset_addr_list_fix(&uslist, reg);
      unset_addr_list_end(&uslist);
      if (r) goto err;
    }
#endif

    if ((reg->num_repeat != 0) || (reg->bt_mem_end != 0))
      reg->stack_pop_level = STACK_POP_LEVEL_ALL;
    else {
      if (reg->bt_mem_start != 0)
	reg->stack_pop_level = STACK_POP_LEVEL_MEM_START;
      else
	reg->stack_pop_level = STACK_POP_LEVEL_FREE;
    }
  }
#ifdef USE_SUBEXP_CALL
  else if (scan_env.num_call > 0) {
    unset_addr_list_end(&uslist);
  }
#endif
  onig_node_free(root);

#ifdef ONIG_DEBUG_COMPILE
#ifdef USE_NAMED_GROUP
  if (!onig_is_prelude()) onig_print_names(stderr, reg);
#endif
  if (!onig_is_prelude()) print_compiled_byte_code_list(stderr, reg);
#endif

 end:
  reg->state = ONIG_STATE_NORMAL;
  return r;

 err_unset:
#ifdef USE_SUBEXP_CALL
  if (scan_env.num_call > 0) {
    unset_addr_list_end(&uslist);
  }
#endif
 err:
  if (IS_NOT_NULL(scan_env.error)) {
    if (IS_NOT_NULL(einfo)) {
      einfo->enc     = scan_env.enc;
      einfo->par     = scan_env.error;
      einfo->par_end = scan_env.error_end;
    }
  }

  onig_node_free(root);
  if (IS_NOT_NULL(scan_env.mem_nodes_dynamic))
      xfree(scan_env.mem_nodes_dynamic);
  return r;
}