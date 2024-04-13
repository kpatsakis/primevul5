	__acquires(&pkc->blk_fill_in_prog_lock)
{
	struct tpacket3_hdr *ppd;

	ppd  = (struct tpacket3_hdr *)curr;
	ppd->tp_next_offset = TOTAL_PKT_LEN_INCL_ALIGN(len);
	pkc->prev = curr;
	pkc->nxt_offset += TOTAL_PKT_LEN_INCL_ALIGN(len);
	BLOCK_LEN(pbd) += TOTAL_PKT_LEN_INCL_ALIGN(len);
	BLOCK_NUM_PKTS(pbd) += 1;
	read_lock(&pkc->blk_fill_in_prog_lock);
	prb_run_all_ft_ops(pkc, ppd);
}