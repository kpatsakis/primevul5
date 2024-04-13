static BOOL is_char7_bitset(const sljit_u8 *bitset, BOOL nclass)
{
/* Tells whether the character codes below 128 are enough
to determine a match. */
const sljit_u8 value = nclass ? 0xff : 0;
const sljit_u8 *end = bitset + 32;

bitset += 16;
do
  {
  if (*bitset++ != value)
    return FALSE;
  }
while (bitset < end);
return TRUE;
}