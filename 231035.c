static inline void clgi(void)
{
	asm volatile (__ex(SVM_CLGI));
}