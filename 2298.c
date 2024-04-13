static SLJIT_INLINE BOOL is_powerof2(unsigned int value)
{
return (value & (value - 1)) == 0;
}