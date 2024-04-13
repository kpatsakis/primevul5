static int get_framesize(compiler_common *common, PCRE2_SPTR cc, PCRE2_SPTR ccend, BOOL recursive, BOOL *needs_control_head)
{
int length = 0;
int possessive = 0;
BOOL stack_restore = FALSE;
BOOL setsom_found = recursive;
BOOL setmark_found = recursive;
/* The last capture is a local variable even for recursions. */
BOOL capture_last_found = FALSE;

#if defined DEBUG_FORCE_CONTROL_HEAD && DEBUG_FORCE_CONTROL_HEAD
SLJIT_ASSERT(common->control_head_ptr != 0);
*needs_control_head = TRUE;
#else
*needs_control_head = FALSE;
#endif

if (ccend == NULL)
  {
  ccend = bracketend(cc) - (1 + LINK_SIZE);
  if (!recursive && (*cc == OP_CBRAPOS || *cc == OP_SCBRAPOS))
    {
    possessive = length = (common->capture_last_ptr != 0) ? 5 : 3;
    /* This is correct regardless of common->capture_last_ptr. */
    capture_last_found = TRUE;
    }
  cc = next_opcode(common, cc);
  }

SLJIT_ASSERT(cc != NULL);
while (cc < ccend)
  switch(*cc)
    {
    case OP_SET_SOM:
    SLJIT_ASSERT(common->has_set_som);
    stack_restore = TRUE;
    if (!setsom_found)
      {
      length += 2;
      setsom_found = TRUE;
      }
    cc += 1;
    break;

    case OP_MARK:
    case OP_COMMIT_ARG:
    case OP_PRUNE_ARG:
    case OP_THEN_ARG:
    SLJIT_ASSERT(common->mark_ptr != 0);
    stack_restore = TRUE;
    if (!setmark_found)
      {
      length += 2;
      setmark_found = TRUE;
      }
    if (common->control_head_ptr != 0)
      *needs_control_head = TRUE;
    cc += 1 + 2 + cc[1];
    break;

    case OP_RECURSE:
    stack_restore = TRUE;
    if (common->has_set_som && !setsom_found)
      {
      length += 2;
      setsom_found = TRUE;
      }
    if (common->mark_ptr != 0 && !setmark_found)
      {
      length += 2;
      setmark_found = TRUE;
      }
    if (common->capture_last_ptr != 0 && !capture_last_found)
      {
      length += 2;
      capture_last_found = TRUE;
      }
    cc += 1 + LINK_SIZE;
    break;

    case OP_CBRA:
    case OP_CBRAPOS:
    case OP_SCBRA:
    case OP_SCBRAPOS:
    stack_restore = TRUE;
    if (common->capture_last_ptr != 0 && !capture_last_found)
      {
      length += 2;
      capture_last_found = TRUE;
      }
    length += 3;
    cc += 1 + LINK_SIZE + IMM2_SIZE;
    break;

    case OP_THEN:
    stack_restore = TRUE;
    if (common->control_head_ptr != 0)
      *needs_control_head = TRUE;
    cc ++;
    break;

    default:
    stack_restore = TRUE;
    /* Fall through. */

    case OP_NOT_WORD_BOUNDARY:
    case OP_WORD_BOUNDARY:
    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    case OP_ALLANY:
    case OP_ANYBYTE:
    case OP_NOTPROP:
    case OP_PROP:
    case OP_ANYNL:
    case OP_NOT_HSPACE:
    case OP_HSPACE:
    case OP_NOT_VSPACE:
    case OP_VSPACE:
    case OP_EXTUNI:
    case OP_EODN:
    case OP_EOD:
    case OP_CIRC:
    case OP_CIRCM:
    case OP_DOLL:
    case OP_DOLLM:
    case OP_CHAR:
    case OP_CHARI:
    case OP_NOT:
    case OP_NOTI:

    case OP_EXACT:
    case OP_POSSTAR:
    case OP_POSPLUS:
    case OP_POSQUERY:
    case OP_POSUPTO:

    case OP_EXACTI:
    case OP_POSSTARI:
    case OP_POSPLUSI:
    case OP_POSQUERYI:
    case OP_POSUPTOI:

    case OP_NOTEXACT:
    case OP_NOTPOSSTAR:
    case OP_NOTPOSPLUS:
    case OP_NOTPOSQUERY:
    case OP_NOTPOSUPTO:

    case OP_NOTEXACTI:
    case OP_NOTPOSSTARI:
    case OP_NOTPOSPLUSI:
    case OP_NOTPOSQUERYI:
    case OP_NOTPOSUPTOI:

    case OP_TYPEEXACT:
    case OP_TYPEPOSSTAR:
    case OP_TYPEPOSPLUS:
    case OP_TYPEPOSQUERY:
    case OP_TYPEPOSUPTO:

    case OP_CLASS:
    case OP_NCLASS:
    case OP_XCLASS:

    case OP_CALLOUT:
    case OP_CALLOUT_STR:

    cc = next_opcode(common, cc);
    SLJIT_ASSERT(cc != NULL);
    break;
    }

/* Possessive quantifiers can use a special case. */
if (SLJIT_UNLIKELY(possessive == length))
  return stack_restore ? no_frame : no_stack;

if (length > 0)
  return length + 1;
return stack_restore ? no_frame : no_stack;
}