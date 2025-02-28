static int jpc_dec_tileinit(jpc_dec_t *dec, jpc_dec_tile_t *tile)
{
	jpc_dec_tcomp_t *tcomp;
	int compno;
	unsigned rlvlno;
	jpc_dec_rlvl_t *rlvl;
	jpc_dec_band_t *band;
	jpc_dec_prc_t *prc;
	int bndno;
	jpc_tsfb_band_t *bnd;
	int bandno;
	jpc_dec_ccp_t *ccp;
	int prccnt;
	jpc_dec_cblk_t *cblk;
	int cblkcnt;
	uint_fast32_t tlprcxstart;
	uint_fast32_t tlprcystart;
	uint_fast32_t brprcxend;
	uint_fast32_t brprcyend;
	uint_fast32_t tlcbgxstart;
	uint_fast32_t tlcbgystart;
	uint_fast32_t brcbgxend;
	uint_fast32_t brcbgyend;
	uint_fast32_t cbgxstart;
	uint_fast32_t cbgystart;
	uint_fast32_t cbgxend;
	uint_fast32_t cbgyend;
	uint_fast32_t tlcblkxstart;
	uint_fast32_t tlcblkystart;
	uint_fast32_t brcblkxend;
	uint_fast32_t brcblkyend;
	uint_fast32_t cblkxstart;
	uint_fast32_t cblkystart;
	uint_fast32_t cblkxend;
	uint_fast32_t cblkyend;
	uint_fast32_t tmpxstart;
	uint_fast32_t tmpystart;
	uint_fast32_t tmpxend;
	uint_fast32_t tmpyend;
	jpc_dec_cp_t *cp;
	jpc_tsfb_band_t bnds[JPC_MAXBANDS];
	jpc_pchg_t *pchg;
	int pchgno;
	jpc_dec_cmpt_t *cmpt;

	cp = tile->cp;
	tile->realmode = 0;
	if (cp->mctid == JPC_MCT_ICT) {
		tile->realmode = 1;
	}

	for (compno = 0, tcomp = tile->tcomps, cmpt = dec->cmpts; compno <
	  dec->numcomps; ++compno, ++tcomp, ++cmpt) {
		ccp = &tile->cp->ccps[compno];
		if (ccp->qmfbid == JPC_COX_INS) {
			tile->realmode = 1;
		}
		tcomp->numrlvls = ccp->numrlvls;
		if (!(tcomp->rlvls = jas_alloc2(tcomp->numrlvls,
		  sizeof(jpc_dec_rlvl_t)))) {
			return -1;
		}
		for (rlvlno = 0, rlvl = tcomp->rlvls; rlvlno < tcomp->numrlvls;
		  ++rlvlno, ++rlvl) {
			rlvl->bands = NULL;
		}
		if (!(tcomp->data = jas_seq2d_create(JPC_CEILDIV(tile->xstart,
		  cmpt->hstep), JPC_CEILDIV(tile->ystart, cmpt->vstep),
		  JPC_CEILDIV(tile->xend, cmpt->hstep), JPC_CEILDIV(tile->yend,
		  cmpt->vstep)))) {
			return -1;
		}
		if (!(tcomp->tsfb = jpc_cod_gettsfb(ccp->qmfbid,
		  tcomp->numrlvls - 1))) {
			return -1;
		}
		{
			jpc_tsfb_getbands(tcomp->tsfb, jas_seq2d_xstart(tcomp->data),
			  jas_seq2d_ystart(tcomp->data), jas_seq2d_xend(tcomp->data),
			  jas_seq2d_yend(tcomp->data), bnds);
		}
		for (rlvlno = 0, rlvl = tcomp->rlvls; rlvlno < tcomp->numrlvls;
		  ++rlvlno, ++rlvl) {
			rlvl->bands = 0;
			rlvl->xstart = JPC_CEILDIVPOW2(tcomp->xstart,
			  tcomp->numrlvls - 1 - rlvlno);
			rlvl->ystart = JPC_CEILDIVPOW2(tcomp->ystart,
			  tcomp->numrlvls - 1 - rlvlno);
			rlvl->xend = JPC_CEILDIVPOW2(tcomp->xend,
			  tcomp->numrlvls - 1 - rlvlno);
			rlvl->yend = JPC_CEILDIVPOW2(tcomp->yend,
			  tcomp->numrlvls - 1 - rlvlno);
			rlvl->prcwidthexpn = ccp->prcwidthexpns[rlvlno];
			rlvl->prcheightexpn = ccp->prcheightexpns[rlvlno];
			tlprcxstart = JPC_FLOORDIVPOW2(rlvl->xstart,
			  rlvl->prcwidthexpn) << rlvl->prcwidthexpn;
			tlprcystart = JPC_FLOORDIVPOW2(rlvl->ystart,
			  rlvl->prcheightexpn) << rlvl->prcheightexpn;
			brprcxend = JPC_CEILDIVPOW2(rlvl->xend,
			  rlvl->prcwidthexpn) << rlvl->prcwidthexpn;
			brprcyend = JPC_CEILDIVPOW2(rlvl->yend,
			  rlvl->prcheightexpn) << rlvl->prcheightexpn;
			rlvl->numhprcs = (brprcxend - tlprcxstart) >>
			  rlvl->prcwidthexpn;
			rlvl->numvprcs = (brprcyend - tlprcystart) >>
			  rlvl->prcheightexpn;
			rlvl->numprcs = rlvl->numhprcs * rlvl->numvprcs;

			if (rlvl->numprcs >= 64 * 1024) {
				/* avoid out-of-memory due to
				   malicious file; this limit is
				   rather arbitrary; "good" files I
				   have seen have values 1..12 */
				return -1;
			}

			if (rlvl->xstart >= rlvl->xend || rlvl->ystart >= rlvl->yend) {
				rlvl->bands = 0;
				rlvl->numprcs = 0;
				rlvl->numhprcs = 0;
				rlvl->numvprcs = 0;
				continue;
			}	
			if (!rlvlno) {
				tlcbgxstart = tlprcxstart;
				tlcbgystart = tlprcystart;
				brcbgxend = brprcxend;
				brcbgyend = brprcyend;
				rlvl->cbgwidthexpn = rlvl->prcwidthexpn;
				rlvl->cbgheightexpn = rlvl->prcheightexpn;
			} else {
				tlcbgxstart = JPC_CEILDIVPOW2(tlprcxstart, 1);
				tlcbgystart = JPC_CEILDIVPOW2(tlprcystart, 1);
				brcbgxend = JPC_CEILDIVPOW2(brprcxend, 1);
				brcbgyend = JPC_CEILDIVPOW2(brprcyend, 1);
				rlvl->cbgwidthexpn = rlvl->prcwidthexpn - 1;
				rlvl->cbgheightexpn = rlvl->prcheightexpn - 1;
				if (rlvl->cbgwidthexpn < 0 || rlvl->cbgheightexpn < 0) {
					/* the control block width/height offset
					   exponent must not be negative */
					return -1;
				}
			}
			rlvl->cblkwidthexpn = JAS_MIN(ccp->cblkwidthexpn,
			  rlvl->cbgwidthexpn);
			rlvl->cblkheightexpn = JAS_MIN(ccp->cblkheightexpn,
			  rlvl->cbgheightexpn);

			rlvl->numbands = (!rlvlno) ? 1 : 3;
			if (!(rlvl->bands = jas_alloc2(rlvl->numbands,
			  sizeof(jpc_dec_band_t)))) {
				return -1;
			}
			for (bandno = 0, band = rlvl->bands;
			  bandno < rlvl->numbands; ++bandno, ++band) {
				bndno = (!rlvlno) ? 0 : (3 * (rlvlno - 1) +
				  bandno + 1);
				bnd = &bnds[bndno];

				band->orient = bnd->orient;
				band->stepsize = ccp->stepsizes[bndno];
				band->analgain = JPC_NOMINALGAIN(ccp->qmfbid,
				  tcomp->numrlvls - 1, rlvlno, band->orient);
				band->absstepsize = jpc_calcabsstepsize(band->stepsize,
				  cmpt->prec + band->analgain);
				band->numbps = ccp->numguardbits +
				  JPC_QCX_GETEXPN(band->stepsize) - 1;
				band->roishift = (ccp->roishift + band->numbps >= JPC_PREC) ?
				  (JPC_PREC - 1 - band->numbps) : ccp->roishift;
				band->data = 0;
				band->prcs = 0;
				if (bnd->xstart == bnd->xend || bnd->ystart == bnd->yend) {
					continue;
				}
				if (!(band->data = jas_seq2d_create(0, 0, 0, 0))) {
					return -1;
				}
				if (jas_seq2d_bindsub(band->data, tcomp->data, bnd->locxstart,
						      bnd->locystart, bnd->locxend, bnd->locyend)) {
					return -1;
				}
				jas_seq2d_setshift(band->data, bnd->xstart, bnd->ystart);

				assert(rlvl->numprcs);

				if (!(band->prcs = jas_alloc2(rlvl->numprcs,
				  sizeof(jpc_dec_prc_t)))) {
					return -1;
				}

/************************************************/
				cbgxstart = tlcbgxstart;
				cbgystart = tlcbgystart;
				for (prccnt = rlvl->numprcs, prc = band->prcs;
				  prccnt > 0; --prccnt, ++prc) {
					cbgxend = cbgxstart + (1 << rlvl->cbgwidthexpn);
					cbgyend = cbgystart + (1 << rlvl->cbgheightexpn);
					prc->xstart = JAS_MAX(cbgxstart, JAS_CAST(uint_fast32_t,
					  jas_seq2d_xstart(band->data)));
					prc->ystart = JAS_MAX(cbgystart, JAS_CAST(uint_fast32_t,
					  jas_seq2d_ystart(band->data)));
					prc->xend = JAS_MIN(cbgxend, JAS_CAST(uint_fast32_t,
					  jas_seq2d_xend(band->data)));
					prc->yend = JAS_MIN(cbgyend, JAS_CAST(uint_fast32_t,
					  jas_seq2d_yend(band->data)));
					if (prc->xend > prc->xstart && prc->yend > prc->ystart) {
						tlcblkxstart = JPC_FLOORDIVPOW2(prc->xstart,
						  rlvl->cblkwidthexpn) << rlvl->cblkwidthexpn;
						tlcblkystart = JPC_FLOORDIVPOW2(prc->ystart,
						  rlvl->cblkheightexpn) << rlvl->cblkheightexpn;
						brcblkxend = JPC_CEILDIVPOW2(prc->xend,
						  rlvl->cblkwidthexpn) << rlvl->cblkwidthexpn;
						brcblkyend = JPC_CEILDIVPOW2(prc->yend,
						  rlvl->cblkheightexpn) << rlvl->cblkheightexpn;
						prc->numhcblks = (brcblkxend - tlcblkxstart) >>
						  rlvl->cblkwidthexpn;
						prc->numvcblks = (brcblkyend - tlcblkystart) >>
						  rlvl->cblkheightexpn;
						prc->numcblks = prc->numhcblks * prc->numvcblks;
						assert(prc->numcblks > 0);

						if (!(prc->incltagtree = jpc_tagtree_create(
						  prc->numhcblks, prc->numvcblks))) {
							return -1;
						}
						if (!(prc->numimsbstagtree = jpc_tagtree_create(
						  prc->numhcblks, prc->numvcblks))) {
							return -1;
						}
						if (!(prc->cblks = jas_alloc2(prc->numcblks,
						  sizeof(jpc_dec_cblk_t)))) {
							return -1;
						}

						cblkxstart = cbgxstart;
						cblkystart = cbgystart;
						for (cblkcnt = prc->numcblks, cblk = prc->cblks;
						  cblkcnt > 0;) {
							cblkxend = cblkxstart + (1 << rlvl->cblkwidthexpn);
							cblkyend = cblkystart + (1 << rlvl->cblkheightexpn);
							tmpxstart = JAS_MAX(cblkxstart, prc->xstart);
							tmpystart = JAS_MAX(cblkystart, prc->ystart);
							tmpxend = JAS_MIN(cblkxend, prc->xend);
							tmpyend = JAS_MIN(cblkyend, prc->yend);
							if (tmpxend > tmpxstart && tmpyend > tmpystart) {
								cblk->firstpassno = -1;
								cblk->mqdec = 0;
								cblk->nulldec = 0;
								cblk->flags = 0;
								cblk->numpasses = 0;
								cblk->segs.head = 0;
								cblk->segs.tail = 0;
								cblk->curseg = 0;
								cblk->numimsbs = 0;
								cblk->numlenbits = 3;
								cblk->flags = 0;
								if (!(cblk->data = jas_seq2d_create(0, 0, 0,
								  0))) {
									return -1;
								}
								if (jas_seq2d_bindsub(cblk->data, band->data,
										      tmpxstart, tmpystart, tmpxend, tmpyend)) {
									return -1;
								}
								++cblk;
								--cblkcnt;
							}
							cblkxstart += 1 << rlvl->cblkwidthexpn;
							if (cblkxstart >= cbgxend) {
								cblkxstart = cbgxstart;
								cblkystart += 1 << rlvl->cblkheightexpn;
							}
						}

					} else {
						prc->cblks = 0;
						prc->incltagtree = 0;
						prc->numimsbstagtree = 0;
					}
					cbgxstart += 1 << rlvl->cbgwidthexpn;
					if (cbgxstart >= brcbgxend) {
						cbgxstart = tlcbgxstart;
						cbgystart += 1 << rlvl->cbgheightexpn;
					}

				}
/********************************************/
			}
		}
	}

	if (!(tile->pi = jpc_dec_pi_create(dec, tile))) {
		return -1;
	}

	for (pchgno = 0; pchgno < jpc_pchglist_numpchgs(tile->cp->pchglist);
	  ++pchgno) {
		pchg = jpc_pchg_copy(jpc_pchglist_get(tile->cp->pchglist, pchgno));
		assert(pchg);
		jpc_pi_addpchg(tile->pi, pchg);
	}
	jpc_pi_init(tile->pi);

	return 0;
}