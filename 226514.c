static void jpc_dec_destroy(jpc_dec_t *dec)
{
	if (dec->cstate) {
		jpc_cstate_destroy(dec->cstate);
	}
	if (dec->pkthdrstreams) {
		jpc_streamlist_destroy(dec->pkthdrstreams);
	}
	if (dec->ppmstab) {
		jpc_ppxstab_destroy(dec->ppmstab);
	}
	if (dec->image) {
		jas_image_destroy(dec->image);
	}

	if (dec->cp) {
		jpc_dec_cp_destroy(dec->cp);
	}

	if (dec->cmpts) {
		jas_free(dec->cmpts);
	}

	if (dec->tiles) {
		int tileno;
		jpc_dec_tile_t *tile;

		for (tileno = 0, tile = dec->tiles; tileno < dec->numtiles; ++tileno,
		  ++tile) {
			if (tile->state != JPC_TILE_DONE) {
				jpc_dec_tilefini(dec, tile);
			}
		}

		jas_free(dec->tiles);
	}

	jas_free(dec);
}