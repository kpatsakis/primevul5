pcre2_jit_compile(pcre2_code *code, uint32_t options)
{
pcre2_real_code *re = (pcre2_real_code *)code;
#ifdef SUPPORT_JIT
executable_functions *functions;
static int executable_allocator_is_working = 0;
#endif

if (code == NULL)
  return PCRE2_ERROR_NULL;

if ((options & ~PUBLIC_JIT_COMPILE_OPTIONS) != 0)
  return PCRE2_ERROR_JIT_BADOPTION;

/* Support for invalid UTF was first introduced in JIT, with the option
PCRE2_JIT_INVALID_UTF. Later, support was added to the interpreter, and the
compile-time option PCRE2_MATCH_INVALID_UTF was created. This is now the
preferred feature, with the earlier option deprecated. However, for backward
compatibility, if the earlier option is set, it forces the new option so that
if JIT matching falls back to the interpreter, there is still support for
invalid UTF. However, if this function has already been successfully called
without PCRE2_JIT_INVALID_UTF and without PCRE2_MATCH_INVALID_UTF (meaning that
non-invalid-supporting JIT code was compiled), give an error.

If in the future support for PCRE2_JIT_INVALID_UTF is withdrawn, the following
actions are needed:

  1. Remove the definition from pcre2.h.in and from the list in
     PUBLIC_JIT_COMPILE_OPTIONS above.

  2. Replace PCRE2_JIT_INVALID_UTF with a local flag in this module.

  3. Replace PCRE2_JIT_INVALID_UTF in pcre2_jit_test.c.

  4. Delete the following short block of code. The setting of "re" and
     "functions" can be moved into the JIT-only block below, but if that is
     done, (void)re and (void)functions will be needed in the non-JIT case, to
     avoid compiler warnings.
*/

#ifdef SUPPORT_JIT
functions = (executable_functions *)re->executable_jit;
#endif

if ((options & PCRE2_JIT_INVALID_UTF) != 0)
  {
  if ((re->overall_options & PCRE2_MATCH_INVALID_UTF) == 0)
    {
#ifdef SUPPORT_JIT
    if (functions != NULL) return PCRE2_ERROR_JIT_BADOPTION;
#endif
    re->overall_options |= PCRE2_MATCH_INVALID_UTF;
    }
  }

/* The above tests are run with and without JIT support. This means that
PCRE2_JIT_INVALID_UTF propagates back into the regex options (ensuring
interpreter support) even in the absence of JIT. But now, if there is no JIT
support, give an error return. */

#ifndef SUPPORT_JIT
return PCRE2_ERROR_JIT_BADOPTION;
#else  /* SUPPORT_JIT */

/* There is JIT support. Do the necessary. */

if ((re->flags & PCRE2_NOJIT) != 0) return 0;

if (executable_allocator_is_working == 0)
  {
  /* Checks whether the executable allocator is working. This check
     might run multiple times in multi-threaded environments, but the
     result should not be affected by it. */
  void *ptr = SLJIT_MALLOC_EXEC(32, NULL);

  executable_allocator_is_working = -1;

  if (ptr != NULL)
    {
    SLJIT_FREE_EXEC(((sljit_u8*)(ptr)) + SLJIT_EXEC_OFFSET(ptr), NULL);
    executable_allocator_is_working = 1;
    }
  }

if (executable_allocator_is_working < 0)
  return PCRE2_ERROR_NOMEMORY;

if ((re->overall_options & PCRE2_MATCH_INVALID_UTF) != 0)
  options |= PCRE2_JIT_INVALID_UTF;

if ((options & PCRE2_JIT_COMPLETE) != 0 && (functions == NULL
    || functions->executable_funcs[0] == NULL)) {
  uint32_t excluded_options = (PCRE2_JIT_PARTIAL_SOFT | PCRE2_JIT_PARTIAL_HARD);
  int result = jit_compile(code, options & ~excluded_options);
  if (result != 0)
    return result;
  }

if ((options & PCRE2_JIT_PARTIAL_SOFT) != 0 && (functions == NULL
    || functions->executable_funcs[1] == NULL)) {
  uint32_t excluded_options = (PCRE2_JIT_COMPLETE | PCRE2_JIT_PARTIAL_HARD);
  int result = jit_compile(code, options & ~excluded_options);
  if (result != 0)
    return result;
  }

if ((options & PCRE2_JIT_PARTIAL_HARD) != 0 && (functions == NULL
    || functions->executable_funcs[2] == NULL)) {
  uint32_t excluded_options = (PCRE2_JIT_COMPLETE | PCRE2_JIT_PARTIAL_SOFT);
  int result = jit_compile(code, options & ~excluded_options);
  if (result != 0)
    return result;
  }

return 0;

#endif  /* SUPPORT_JIT */
}