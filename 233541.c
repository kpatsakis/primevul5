calc_crc32( u32  crc,  u8  *p,  u32  len )
{
	register u32  _crc;
	_crc = crc;
	
	__asm__ __volatile__ (
		"xorl	%%ebx, %%ebx\n"
		"movl	%2, %%esi\n" 
		"movl	%3, %%ecx\n" 
		"movl	$crc32tab, %%edi\n"
		"shrl	$2, %%ecx\n"
		"jz	1f\n"

		".align 4\n"
	"0:\n"
		"movb	%%al, %%bl\n"
		"movl	(%%esi), %%edx\n"
		"shrl	$8, %%eax\n"
		"xorb	%%dl, %%bl\n"
		"shrl	$8, %%edx\n"
		"xorl	(%%edi,%%ebx,4), %%eax\n"

		"movb	%%al, %%bl\n"
		"shrl	$8, %%eax\n"
		"xorb	%%dl, %%bl\n"
		"shrl	$8, %%edx\n"
		"xorl	(%%edi,%%ebx,4), %%eax\n"

		"movb	%%al, %%bl\n"
		"shrl	$8, %%eax\n"
		"xorb	%%dl, %%bl\n"
		"movb	%%dh, %%dl\n" 
		"xorl	(%%edi,%%ebx,4), %%eax\n"

		"movb	%%al, %%bl\n"
		"shrl	$8, %%eax\n"
		"xorb	%%dl, %%bl\n"
		"addl	$4, %%esi\n"
		"xorl	(%%edi,%%ebx,4), %%eax\n"

		"decl	%%ecx\n"
		"jnz	0b\n"

	"1:\n"
		"movl	%3, %%ecx\n"
		"andl	$3, %%ecx\n"
		"jz	2f\n"

		"movb	%%al, %%bl\n"
		"shrl	$8, %%eax\n"
		"xorb	(%%esi), %%bl\n"
		"xorl	(%%edi,%%ebx,4), %%eax\n"

		"decl	%%ecx\n"
		"jz	2f\n"

		"movb	%%al, %%bl\n"
		"shrl	$8, %%eax\n"
		"xorb	1(%%esi), %%bl\n"
		"xorl	(%%edi,%%ebx,4), %%eax\n"

		"decl	%%ecx\n"
		"jz	2f\n"

		"movb	%%al, %%bl\n"
		"shrl	$8, %%eax\n"
		"xorb	2(%%esi), %%bl\n"
		"xorl	(%%edi,%%ebx,4), %%eax\n"
	"2:\n"
		: "=a" (_crc)
		: "0" (_crc), "g" (p), "g" (len)
		: "bx", "cx", "dx", "si", "di"
	);

	return  _crc;
}