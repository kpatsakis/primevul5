static BOOL check_opcode_types(compiler_common *common, PCRE2_SPTR cc, PCRE2_SPTR ccend)
{
int count;
PCRE2_SPTR slot;
PCRE2_SPTR assert_back_end = cc - 1;
PCRE2_SPTR assert_na_end = cc - 1;

/* Calculate important variables (like stack size) and checks whether all opcodes are supported. */
while (cc < ccend)
  {
  switch(*cc)
    {
    case OP_SET_SOM:
    common->has_set_som = TRUE;
    common->might_be_empty = TRUE;
    cc += 1;
    break;

    case OP_REFI:
#ifdef SUPPORT_UNICODE
    if (common->iref_ptr == 0)
      {
      common->iref_ptr = common->ovector_start;
      common->ovector_start += 3 * sizeof(sljit_sw);
      }
#endif /* SUPPORT_UNICODE */
    /* Fall through. */
    case OP_REF:
    common->optimized_cbracket[GET2(cc, 1)] = 0;
    cc += 1 + IMM2_SIZE;
    break;

    case OP_ASSERT_NA:
    case OP_ASSERTBACK_NA:
    slot = bracketend(cc);
    if (slot > assert_na_end)
      assert_na_end = slot;
    cc += 1 + LINK_SIZE;
    break;

    case OP_CBRAPOS:
    case OP_SCBRAPOS:
    common->optimized_cbracket[GET2(cc, 1 + LINK_SIZE)] = 0;
    cc += 1 + LINK_SIZE + IMM2_SIZE;
    break;

    case OP_COND:
    case OP_SCOND:
    /* Only AUTO_CALLOUT can insert this opcode. We do
       not intend to support this case. */
    if (cc[1 + LINK_SIZE] == OP_CALLOUT || cc[1 + LINK_SIZE] == OP_CALLOUT_STR)
      return FALSE;
    cc += 1 + LINK_SIZE;
    break;

    case OP_CREF:
    common->optimized_cbracket[GET2(cc, 1)] = 0;
    cc += 1 + IMM2_SIZE;
    break;

    case OP_DNREF:
    case OP_DNREFI:
    case OP_DNCREF:
    count = GET2(cc, 1 + IMM2_SIZE);
    slot = common->name_table + GET2(cc, 1) * common->name_entry_size;
    while (count-- > 0)
      {
      common->optimized_cbracket[GET2(slot, 0)] = 0;
      slot += common->name_entry_size;
      }
    cc += 1 + 2 * IMM2_SIZE;
    break;

    case OP_RECURSE:
    /* Set its value only once. */
    if (common->recursive_head_ptr == 0)
      {
      common->recursive_head_ptr = common->ovector_start;
      common->ovector_start += sizeof(sljit_sw);
      }
    cc += 1 + LINK_SIZE;
    break;

    case OP_CALLOUT:
    case OP_CALLOUT_STR:
    if (common->capture_last_ptr == 0)
      {
      common->capture_last_ptr = common->ovector_start;
      common->ovector_start += sizeof(sljit_sw);
      }
    cc += (*cc == OP_CALLOUT) ? PRIV(OP_lengths)[OP_CALLOUT] : GET(cc, 1 + 2*LINK_SIZE);
    break;

    case OP_ASSERTBACK:
    slot = bracketend(cc);
    if (slot > assert_back_end)
      assert_back_end = slot;
    cc += 1 + LINK_SIZE;
    break;

    case OP_THEN_ARG:
    common->has_then = TRUE;
    common->control_head_ptr = 1;
    /* Fall through. */

    case OP_COMMIT_ARG:
    case OP_PRUNE_ARG:
    if (cc < assert_na_end)
      return FALSE;
    /* Fall through */
    case OP_MARK:
    if (common->mark_ptr == 0)
      {
      common->mark_ptr = common->ovector_start;
      common->ovector_start += sizeof(sljit_sw);
      }
    cc += 1 + 2 + cc[1];
    break;

    case OP_THEN:
    common->has_then = TRUE;
    common->control_head_ptr = 1;
    cc += 1;
    break;

    case OP_SKIP:
    if (cc < assert_back_end)
      common->has_skip_in_assert_back = TRUE;
    if (cc < assert_na_end)
      return FALSE;
    cc += 1;
    break;

    case OP_SKIP_ARG:
    common->control_head_ptr = 1;
    common->has_skip_arg = TRUE;
    if (cc < assert_back_end)
      common->has_skip_in_assert_back = TRUE;
    if (cc < assert_na_end)
      return FALSE;
    cc += 1 + 2 + cc[1];
    break;

    case OP_PRUNE:
    case OP_COMMIT:
    case OP_ASSERT_ACCEPT:
    if (cc < assert_na_end)
      return FALSE;
    cc++;
    break;

    default:
    cc = next_opcode(common, cc);
    if (cc == NULL)
      return FALSE;
    break;
    }
  }
return TRUE;
}