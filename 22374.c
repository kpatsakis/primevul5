static int em_grp2(struct x86_emulate_ctxt *ctxt)
{
	switch (ctxt->modrm_reg) {
	case 0:	/* rol */
		emulate_2op_SrcB(ctxt, "rol");
		break;
	case 1:	/* ror */
		emulate_2op_SrcB(ctxt, "ror");
		break;
	case 2:	/* rcl */
		emulate_2op_SrcB(ctxt, "rcl");
		break;
	case 3:	/* rcr */
		emulate_2op_SrcB(ctxt, "rcr");
		break;
	case 4:	/* sal/shl */
	case 6:	/* sal/shl */
		emulate_2op_SrcB(ctxt, "sal");
		break;
	case 5:	/* shr */
		emulate_2op_SrcB(ctxt, "shr");
		break;
	case 7:	/* sar */
		emulate_2op_SrcB(ctxt, "sar");
		break;
	}
	return X86EMUL_CONTINUE;
}