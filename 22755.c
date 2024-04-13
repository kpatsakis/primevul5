onig_reg_init(regex_t* reg, OnigOptionType option,
	      OnigCaseFoldType case_fold_flag,
	      OnigEncoding enc, const OnigSyntaxType* syntax)
{
  if (! onig_inited)
    onig_init();

  if (IS_NULL(reg))
    return ONIGERR_INVALID_ARGUMENT;

  if (ONIGENC_IS_UNDEF(enc))
    return ONIGERR_DEFAULT_ENCODING_IS_NOT_SETTED;

  if ((option & (ONIG_OPTION_DONT_CAPTURE_GROUP|ONIG_OPTION_CAPTURE_GROUP))
      == (ONIG_OPTION_DONT_CAPTURE_GROUP|ONIG_OPTION_CAPTURE_GROUP)) {
    return ONIGERR_INVALID_COMBINATION_OF_OPTIONS;
  }

  (reg)->state = ONIG_STATE_MODIFY;

  if ((option & ONIG_OPTION_NEGATE_SINGLELINE) != 0) {
    option |= syntax->options;
    option &= ~ONIG_OPTION_SINGLELINE;
  }
  else
    option |= syntax->options;

  (reg)->enc              = enc;
  (reg)->options          = option;
  (reg)->syntax           = syntax;
  (reg)->optimize         = 0;
  (reg)->exact            = (UChar* )NULL;
  (reg)->int_map          = (int* )NULL;
  (reg)->int_map_backward = (int* )NULL;
  (reg)->chain            = (regex_t* )NULL;

  (reg)->p                = (UChar* )NULL;
  (reg)->alloc            = 0;
  (reg)->used             = 0;
  (reg)->name_table       = (void* )NULL;

  (reg)->case_fold_flag   = case_fold_flag;
  return 0;
}