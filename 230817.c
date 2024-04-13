pcre2_jit_compile(pcre2_code *code, uint32_t options)
{
#ifndef SUPPORT_JIT

(void)code;
(void)options;
return PCRE2_ERROR_JIT_BADOPTION;

#else  /* SUPPORT_JIT */

pcre2_real_code *re = (pcre2_real_code *)code;
executable_functions *functions;
int result;

if (code == NULL)
  return PCRE2_ERROR_NULL;

if ((options & ~PUBLIC_JIT_COMPILE_OPTIONS) != 0)
  return PCRE2_ERROR_JIT_BADOPTION;

if ((re->flags & PCRE2_NOJIT) != 0) return 0;

functions = (executable_functions *)re->executable_jit;

if ((options & PCRE2_JIT_COMPLETE) != 0 && (functions == NULL
    || functions->executable_funcs[0] == NULL)) {
  result = jit_compile(code, PCRE2_JIT_COMPLETE);
  if (result != 0)
    return result;
  }

if ((options & PCRE2_JIT_PARTIAL_SOFT) != 0 && (functions == NULL
    || functions->executable_funcs[1] == NULL)) {
  result = jit_compile(code, PCRE2_JIT_PARTIAL_SOFT);
  if (result != 0)
    return result;
  }

if ((options & PCRE2_JIT_PARTIAL_HARD) != 0 && (functions == NULL
    || functions->executable_funcs[2] == NULL)) {
  result = jit_compile(code, PCRE2_JIT_PARTIAL_HARD);
  if (result != 0)
    return result;
  }

return 0;

#endif  /* SUPPORT_JIT */
}