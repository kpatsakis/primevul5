Init_String(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)

    rb_cString  = rb_define_class("String", rb_cObject);
    rb_include_module(rb_cString, rb_mComparable);
    rb_define_alloc_func(rb_cString, str_alloc);
    rb_define_singleton_method(rb_cString, "try_convert", rb_str_s_try_convert, 1);
    rb_define_method(rb_cString, "initialize", rb_str_init, -1);
    rb_define_method(rb_cString, "initialize_copy", rb_str_replace, 1);
    rb_define_method(rb_cString, "<=>", rb_str_cmp_m, 1);
    rb_define_method(rb_cString, "==", rb_str_equal, 1);
    rb_define_method(rb_cString, "===", rb_str_equal, 1);
    rb_define_method(rb_cString, "eql?", rb_str_eql, 1);
    rb_define_method(rb_cString, "hash", rb_str_hash_m, 0);
    rb_define_method(rb_cString, "casecmp", rb_str_casecmp, 1);
    rb_define_method(rb_cString, "+", rb_str_plus, 1);
    rb_define_method(rb_cString, "*", rb_str_times, 1);
    rb_define_method(rb_cString, "%", rb_str_format_m, 1);
    rb_define_method(rb_cString, "[]", rb_str_aref_m, -1);
    rb_define_method(rb_cString, "[]=", rb_str_aset_m, -1);
    rb_define_method(rb_cString, "insert", rb_str_insert, 2);
    rb_define_method(rb_cString, "length", rb_str_length, 0);
    rb_define_method(rb_cString, "size", rb_str_length, 0);
    rb_define_method(rb_cString, "bytesize", rb_str_bytesize, 0);
    rb_define_method(rb_cString, "empty?", rb_str_empty, 0);
    rb_define_method(rb_cString, "=~", rb_str_match, 1);
    rb_define_method(rb_cString, "match", rb_str_match_m, -1);
    rb_define_method(rb_cString, "succ", rb_str_succ, 0);
    rb_define_method(rb_cString, "succ!", rb_str_succ_bang, 0);
    rb_define_method(rb_cString, "next", rb_str_succ, 0);
    rb_define_method(rb_cString, "next!", rb_str_succ_bang, 0);
    rb_define_method(rb_cString, "upto", rb_str_upto, -1);
    rb_define_method(rb_cString, "index", rb_str_index_m, -1);
    rb_define_method(rb_cString, "rindex", rb_str_rindex_m, -1);
    rb_define_method(rb_cString, "replace", rb_str_replace, 1);
    rb_define_method(rb_cString, "clear", rb_str_clear, 0);
    rb_define_method(rb_cString, "chr", rb_str_chr, 0);
    rb_define_method(rb_cString, "getbyte", rb_str_getbyte, 1);
    rb_define_method(rb_cString, "setbyte", rb_str_setbyte, 2);

    rb_define_method(rb_cString, "to_i", rb_str_to_i, -1);
    rb_define_method(rb_cString, "to_f", rb_str_to_f, 0);
    rb_define_method(rb_cString, "to_s", rb_str_to_s, 0);
    rb_define_method(rb_cString, "to_str", rb_str_to_s, 0);
    rb_define_method(rb_cString, "inspect", rb_str_inspect, 0);
    rb_define_method(rb_cString, "dump", rb_str_dump, 0);

    rb_define_method(rb_cString, "upcase", rb_str_upcase, 0);
    rb_define_method(rb_cString, "downcase", rb_str_downcase, 0);
    rb_define_method(rb_cString, "capitalize", rb_str_capitalize, 0);
    rb_define_method(rb_cString, "swapcase", rb_str_swapcase, 0);

    rb_define_method(rb_cString, "upcase!", rb_str_upcase_bang, 0);
    rb_define_method(rb_cString, "downcase!", rb_str_downcase_bang, 0);
    rb_define_method(rb_cString, "capitalize!", rb_str_capitalize_bang, 0);
    rb_define_method(rb_cString, "swapcase!", rb_str_swapcase_bang, 0);

    rb_define_method(rb_cString, "hex", rb_str_hex, 0);
    rb_define_method(rb_cString, "oct", rb_str_oct, 0);
    rb_define_method(rb_cString, "split", rb_str_split_m, -1);
    rb_define_method(rb_cString, "lines", rb_str_each_line, -1);
    rb_define_method(rb_cString, "bytes", rb_str_each_byte, 0);
    rb_define_method(rb_cString, "chars", rb_str_each_char, 0);
    rb_define_method(rb_cString, "codepoints", rb_str_each_codepoint, 0);
    rb_define_method(rb_cString, "reverse", rb_str_reverse, 0);
    rb_define_method(rb_cString, "reverse!", rb_str_reverse_bang, 0);
    rb_define_method(rb_cString, "concat", rb_str_concat, 1);
    rb_define_method(rb_cString, "<<", rb_str_concat, 1);
    rb_define_method(rb_cString, "crypt", rb_str_crypt, 1);
    rb_define_method(rb_cString, "intern", rb_str_intern, 0);
    rb_define_method(rb_cString, "to_sym", rb_str_intern, 0);
    rb_define_method(rb_cString, "ord", rb_str_ord, 0);

    rb_define_method(rb_cString, "include?", rb_str_include, 1);
    rb_define_method(rb_cString, "start_with?", rb_str_start_with, -1);
    rb_define_method(rb_cString, "end_with?", rb_str_end_with, -1);

    rb_define_method(rb_cString, "scan", rb_str_scan, 1);

    rb_define_method(rb_cString, "ljust", rb_str_ljust, -1);
    rb_define_method(rb_cString, "rjust", rb_str_rjust, -1);
    rb_define_method(rb_cString, "center", rb_str_center, -1);

    rb_define_method(rb_cString, "sub", rb_str_sub, -1);
    rb_define_method(rb_cString, "gsub", rb_str_gsub, -1);
    rb_define_method(rb_cString, "chop", rb_str_chop, 0);
    rb_define_method(rb_cString, "chomp", rb_str_chomp, -1);
    rb_define_method(rb_cString, "strip", rb_str_strip, 0);
    rb_define_method(rb_cString, "lstrip", rb_str_lstrip, 0);
    rb_define_method(rb_cString, "rstrip", rb_str_rstrip, 0);

    rb_define_method(rb_cString, "sub!", rb_str_sub_bang, -1);
    rb_define_method(rb_cString, "gsub!", rb_str_gsub_bang, -1);
    rb_define_method(rb_cString, "chop!", rb_str_chop_bang, 0);
    rb_define_method(rb_cString, "chomp!", rb_str_chomp_bang, -1);
    rb_define_method(rb_cString, "strip!", rb_str_strip_bang, 0);
    rb_define_method(rb_cString, "lstrip!", rb_str_lstrip_bang, 0);
    rb_define_method(rb_cString, "rstrip!", rb_str_rstrip_bang, 0);

    rb_define_method(rb_cString, "tr", rb_str_tr, 2);
    rb_define_method(rb_cString, "tr_s", rb_str_tr_s, 2);
    rb_define_method(rb_cString, "delete", rb_str_delete, -1);
    rb_define_method(rb_cString, "squeeze", rb_str_squeeze, -1);
    rb_define_method(rb_cString, "count", rb_str_count, -1);

    rb_define_method(rb_cString, "tr!", rb_str_tr_bang, 2);
    rb_define_method(rb_cString, "tr_s!", rb_str_tr_s_bang, 2);
    rb_define_method(rb_cString, "delete!", rb_str_delete_bang, -1);
    rb_define_method(rb_cString, "squeeze!", rb_str_squeeze_bang, -1);

    rb_define_method(rb_cString, "each_line", rb_str_each_line, -1);
    rb_define_method(rb_cString, "each_byte", rb_str_each_byte, 0);
    rb_define_method(rb_cString, "each_char", rb_str_each_char, 0);
    rb_define_method(rb_cString, "each_codepoint", rb_str_each_codepoint, 0);

    rb_define_method(rb_cString, "sum", rb_str_sum, -1);

    rb_define_method(rb_cString, "slice", rb_str_aref_m, -1);
    rb_define_method(rb_cString, "slice!", rb_str_slice_bang, -1);

    rb_define_method(rb_cString, "partition", rb_str_partition, 1);
    rb_define_method(rb_cString, "rpartition", rb_str_rpartition, 1);

    rb_define_method(rb_cString, "encoding", rb_obj_encoding, 0); /* in encoding.c */
    rb_define_method(rb_cString, "force_encoding", rb_str_force_encoding, 1);
    rb_define_method(rb_cString, "valid_encoding?", rb_str_valid_encoding_p, 0);
    rb_define_method(rb_cString, "ascii_only?", rb_str_is_ascii_only_p, 0);

    id_to_s = rb_intern("to_s");

    rb_fs = Qnil;
    rb_define_variable("$;", &rb_fs);
    rb_define_variable("$-F", &rb_fs);

    rb_cSymbol = rb_define_class("Symbol", rb_cObject);
    rb_include_module(rb_cSymbol, rb_mComparable);
    rb_undef_alloc_func(rb_cSymbol);
    rb_undef_method(CLASS_OF(rb_cSymbol), "new");
    rb_define_singleton_method(rb_cSymbol, "all_symbols", rb_sym_all_symbols, 0); /* in parse.y */

    rb_define_method(rb_cSymbol, "==", sym_equal, 1);
    rb_define_method(rb_cSymbol, "===", sym_equal, 1);
    rb_define_method(rb_cSymbol, "inspect", sym_inspect, 0);
    rb_define_method(rb_cSymbol, "to_s", rb_sym_to_s, 0);
    rb_define_method(rb_cSymbol, "id2name", rb_sym_to_s, 0);
    rb_define_method(rb_cSymbol, "intern", sym_to_sym, 0);
    rb_define_method(rb_cSymbol, "to_sym", sym_to_sym, 0);
    rb_define_method(rb_cSymbol, "to_proc", sym_to_proc, 0);
    rb_define_method(rb_cSymbol, "succ", sym_succ, 0);
    rb_define_method(rb_cSymbol, "next", sym_succ, 0);

    rb_define_method(rb_cSymbol, "<=>", sym_cmp, 1);
    rb_define_method(rb_cSymbol, "casecmp", sym_casecmp, 1);
    rb_define_method(rb_cSymbol, "=~", sym_match, 1);

    rb_define_method(rb_cSymbol, "[]", sym_aref, -1);
    rb_define_method(rb_cSymbol, "slice", sym_aref, -1);
    rb_define_method(rb_cSymbol, "length", sym_length, 0);
    rb_define_method(rb_cSymbol, "size", sym_length, 0);
    rb_define_method(rb_cSymbol, "empty?", sym_empty, 0);
    rb_define_method(rb_cSymbol, "match", sym_match, 1);

    rb_define_method(rb_cSymbol, "upcase", sym_upcase, 0);
    rb_define_method(rb_cSymbol, "downcase", sym_downcase, 0);
    rb_define_method(rb_cSymbol, "capitalize", sym_capitalize, 0);
    rb_define_method(rb_cSymbol, "swapcase", sym_swapcase, 0);

    rb_define_method(rb_cSymbol, "encoding", sym_encoding, 0);
}