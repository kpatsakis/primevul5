static inline void stgi(void)
{
	asm volatile (__ex("stgi"));
}