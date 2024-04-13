static void opj_j2k_dump_MH_index(opj_j2k_t* p_j2k, FILE* out_stream)
{
    opj_codestream_index_t* cstr_index = p_j2k->cstr_index;
    OPJ_UINT32 it_marker, it_tile, it_tile_part;

    fprintf(out_stream, "Codestream index from main header: {\n");

    fprintf(out_stream, "\t Main header start position=%" PRIi64 "\n"
            "\t Main header end position=%" PRIi64 "\n",
            cstr_index->main_head_start, cstr_index->main_head_end);

    fprintf(out_stream, "\t Marker list: {\n");

    if (cstr_index->marker) {
        for (it_marker = 0; it_marker < cstr_index->marknum ; it_marker++) {
            fprintf(out_stream, "\t\t type=%#x, pos=%" PRIi64 ", len=%d\n",
                    cstr_index->marker[it_marker].type,
                    cstr_index->marker[it_marker].pos,
                    cstr_index->marker[it_marker].len);
        }
    }

    fprintf(out_stream, "\t }\n");

    if (cstr_index->tile_index) {

        /* Simple test to avoid to write empty information*/
        OPJ_UINT32 l_acc_nb_of_tile_part = 0;
        for (it_tile = 0; it_tile < cstr_index->nb_of_tiles ; it_tile++) {
            l_acc_nb_of_tile_part += cstr_index->tile_index[it_tile].nb_tps;
        }

        if (l_acc_nb_of_tile_part) {
            fprintf(out_stream, "\t Tile index: {\n");

            for (it_tile = 0; it_tile < cstr_index->nb_of_tiles ; it_tile++) {
                OPJ_UINT32 nb_of_tile_part = cstr_index->tile_index[it_tile].nb_tps;

                fprintf(out_stream, "\t\t nb of tile-part in tile [%d]=%d\n", it_tile,
                        nb_of_tile_part);

                if (cstr_index->tile_index[it_tile].tp_index) {
                    for (it_tile_part = 0; it_tile_part < nb_of_tile_part; it_tile_part++) {
                        fprintf(out_stream, "\t\t\t tile-part[%d]: star_pos=%" PRIi64 ", end_header=%"
                                PRIi64 ", end_pos=%" PRIi64 ".\n",
                                it_tile_part,
                                cstr_index->tile_index[it_tile].tp_index[it_tile_part].start_pos,
                                cstr_index->tile_index[it_tile].tp_index[it_tile_part].end_header,
                                cstr_index->tile_index[it_tile].tp_index[it_tile_part].end_pos);
                    }
                }

                if (cstr_index->tile_index[it_tile].marker) {
                    for (it_marker = 0; it_marker < cstr_index->tile_index[it_tile].marknum ;
                            it_marker++) {
                        fprintf(out_stream, "\t\t type=%#x, pos=%" PRIi64 ", len=%d\n",
                                cstr_index->tile_index[it_tile].marker[it_marker].type,
                                cstr_index->tile_index[it_tile].marker[it_marker].pos,
                                cstr_index->tile_index[it_tile].marker[it_marker].len);
                    }
                }
            }
            fprintf(out_stream, "\t }\n");
        }
    }

    fprintf(out_stream, "}\n");

}