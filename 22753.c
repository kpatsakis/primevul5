onigenc_unicode_apply_all_case_fold(OnigCaseFoldType flag,
				    OnigApplyAllCaseFoldFunc f, void* arg,
				    OnigEncoding enc ARG_UNUSED)
{
  const CaseUnfold_11_Type* p11;
  OnigCodePoint code;
  int i, j, k, r;

  /* if (CaseFoldInited == 0) init_case_fold_table(); */

  for (i = 0; i < numberof(CaseUnfold_11); i++) {
    p11 = &CaseUnfold_11[i];
    for (j = 0; j < p11->to.n; j++) {
      code = p11->from;
      r = (*f)(p11->to.code[j], &code, 1, arg);
      if (r != 0) return r;

      code = p11->to.code[j];
      r = (*f)(p11->from, &code, 1, arg);
      if (r != 0) return r;

      for (k = 0; k < j; k++) {
	r = (*f)(p11->to.code[j], (OnigCodePoint* )(&p11->to.code[k]), 1, arg);
	if (r != 0) return r;

	r = (*f)(p11->to.code[k], (OnigCodePoint* )(&p11->to.code[j]), 1, arg);
	if (r != 0) return r;
      }
    }
  }

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
    code = 0x0131;
    r = (*f)(0x0049, &code, 1, arg);
    if (r != 0) return r;
    code = 0x0049;
    r = (*f)(0x0131, &code, 1, arg);
    if (r != 0) return r;

    code = 0x0130;
    r = (*f)(0x0069, &code, 1, arg);
    if (r != 0) return r;
    code = 0x0069;
    r = (*f)(0x0130, &code, 1, arg);
    if (r != 0) return r;
  }
  else {
#endif
    for (i = 0; i < numberof(CaseUnfold_11_Locale); i++) {
      p11 = &CaseUnfold_11_Locale[i];
      for (j = 0; j < p11->to.n; j++) {
	code = p11->from;
	r = (*f)(p11->to.code[j], &code, 1, arg);
	if (r != 0) return r;

	code = p11->to.code[j];
	r = (*f)(p11->from, &code, 1, arg);
	if (r != 0) return r;

	for (k = 0; k < j; k++) {
	  r = (*f)(p11->to.code[j], (OnigCodePoint* )(&p11->to.code[k]),
		   1, arg);
	  if (r != 0) return r;

	  r = (*f)(p11->to.code[k], (OnigCodePoint* )(&p11->to.code[j]),
		   1, arg);
	  if (r != 0) return r;
	}
      }
    }
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  }
#endif

  if ((flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
    for (i = 0; i < numberof(CaseUnfold_12); i++) {
      for (j = 0; j < CaseUnfold_12[i].to.n; j++) {
	r = (*f)(CaseUnfold_12[i].to.code[j],
		 (OnigCodePoint* )CaseUnfold_12[i].from, 2, arg);
	if (r != 0) return r;

	for (k = 0; k < CaseUnfold_12[i].to.n; k++) {
	  if (k == j) continue;

	  r = (*f)(CaseUnfold_12[i].to.code[j],
		   (OnigCodePoint* )(&CaseUnfold_12[i].to.code[k]), 1, arg);
	  if (r != 0) return r;
	}
      }
    }

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) == 0) {
#endif
      for (i = 0; i < numberof(CaseUnfold_12_Locale); i++) {
	for (j = 0; j < CaseUnfold_12_Locale[i].to.n; j++) {
	  r = (*f)(CaseUnfold_12_Locale[i].to.code[j],
		   (OnigCodePoint* )CaseUnfold_12_Locale[i].from, 2, arg);
	  if (r != 0) return r;

	  for (k = 0; k < CaseUnfold_12_Locale[i].to.n; k++) {
	    if (k == j) continue;

	    r = (*f)(CaseUnfold_12_Locale[i].to.code[j],
		     (OnigCodePoint* )(&CaseUnfold_12_Locale[i].to.code[k]),
		     1, arg);
	    if (r != 0) return r;
	  }
	}
      }
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
    }
#endif

    for (i = 0; i < numberof(CaseUnfold_13); i++) {
      for (j = 0; j < CaseUnfold_13[i].to.n; j++) {
	r = (*f)(CaseUnfold_13[i].to.code[j],
		 (OnigCodePoint* )CaseUnfold_13[i].from, 3, arg);
	if (r != 0) return r;

	for (k = 0; k < CaseUnfold_13[i].to.n; k++) {
	  if (k == j) continue;

	  r = (*f)(CaseUnfold_13[i].to.code[j],
		   (OnigCodePoint* )(&CaseUnfold_13[i].to.code[k]), 1, arg);
	  if (r != 0) return r;
	}
      }
    }
  }

  return 0;
}