static sljit_s32 SLJIT_FUNC do_callout(struct jit_arguments *arguments, pcre2_callout_block *callout_block, PCRE2_SPTR *jit_ovector)
{
PCRE2_SPTR begin;
PCRE2_SIZE *ovector;
sljit_u32 oveccount, capture_top;

if (arguments->callout == NULL)
  return 0;

SLJIT_COMPILE_ASSERT(sizeof (PCRE2_SIZE) <= sizeof (sljit_sw), pcre2_size_must_be_lower_than_sljit_sw_size);

begin = arguments->begin;
ovector = (PCRE2_SIZE*)(callout_block + 1);
oveccount = callout_block->capture_top;

SLJIT_ASSERT(oveccount >= 1);

callout_block->version = 2;
callout_block->callout_flags = 0;

/* Offsets in subject. */
callout_block->subject_length = arguments->end - arguments->begin;
callout_block->start_match = jit_ovector[0] - begin;
callout_block->current_position = (PCRE2_SPTR)callout_block->offset_vector - begin;
callout_block->subject = begin;

/* Convert and copy the JIT offset vector to the ovector array. */
callout_block->capture_top = 1;
callout_block->offset_vector = ovector;

ovector[0] = PCRE2_UNSET;
ovector[1] = PCRE2_UNSET;
ovector += 2;
jit_ovector += 2;
capture_top = 1;

/* Convert pointers to sizes. */
while (--oveccount != 0)
  {
  capture_top++;

  ovector[0] = (PCRE2_SIZE)(jit_ovector[0] - begin);
  ovector[1] = (PCRE2_SIZE)(jit_ovector[1] - begin);

  if (ovector[0] != PCRE2_UNSET)
    callout_block->capture_top = capture_top;

  ovector += 2;
  jit_ovector += 2;
  }

return (arguments->callout)(callout_block, arguments->callout_data);
}