void j2k_destroy_cstr_index(opj_codestream_index_t *p_cstr_ind)
{
    if (p_cstr_ind) {

        if (p_cstr_ind->marker) {
            opj_free(p_cstr_ind->marker);
            p_cstr_ind->marker = NULL;
        }

        if (p_cstr_ind->tile_index) {
            OPJ_UINT32 it_tile = 0;

            for (it_tile = 0; it_tile < p_cstr_ind->nb_of_tiles; it_tile++) {

                if (p_cstr_ind->tile_index[it_tile].packet_index) {
                    opj_free(p_cstr_ind->tile_index[it_tile].packet_index);
                    p_cstr_ind->tile_index[it_tile].packet_index = NULL;
                }

                if (p_cstr_ind->tile_index[it_tile].tp_index) {
                    opj_free(p_cstr_ind->tile_index[it_tile].tp_index);
                    p_cstr_ind->tile_index[it_tile].tp_index = NULL;
                }

                if (p_cstr_ind->tile_index[it_tile].marker) {
                    opj_free(p_cstr_ind->tile_index[it_tile].marker);
                    p_cstr_ind->tile_index[it_tile].marker = NULL;

                }
            }

            opj_free(p_cstr_ind->tile_index);
            p_cstr_ind->tile_index = NULL;
        }

        opj_free(p_cstr_ind);
    }
}