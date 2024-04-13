static inline void stgi(void)
{
	asm volatile (__ex(SVM_STGI));
}