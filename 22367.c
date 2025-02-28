static int em_grp45(struct x86_emulate_ctxt *ctxt)
{
	int rc = X86EMUL_CONTINUE;

	switch (ctxt->modrm_reg) {
	case 0:	/* inc */
		emulate_1op(ctxt, "inc");
		break;
	case 1:	/* dec */
		emulate_1op(ctxt, "dec");
		break;
	case 2: /* call near abs */ {
		long int old_eip;
		old_eip = ctxt->_eip;
		ctxt->_eip = ctxt->src.val;
		ctxt->src.val = old_eip;
		rc = em_push(ctxt);
		break;
	}
	case 4: /* jmp abs */
		ctxt->_eip = ctxt->src.val;
		break;
	case 5: /* jmp far */
		rc = em_jmp_far(ctxt);
		break;
	case 6:	/* push */
		rc = em_push(ctxt);
		break;
	}
	return rc;
}