static void opj_j2k_dump_MH_info(opj_j2k_t* p_j2k, FILE* out_stream)
{

    fprintf(out_stream, "Codestream info from main header: {\n");

    fprintf(out_stream, "\t tx0=%d, ty0=%d\n", p_j2k->m_cp.tx0, p_j2k->m_cp.ty0);
    fprintf(out_stream, "\t tdx=%d, tdy=%d\n", p_j2k->m_cp.tdx, p_j2k->m_cp.tdy);
    fprintf(out_stream, "\t tw=%d, th=%d\n", p_j2k->m_cp.tw, p_j2k->m_cp.th);
    opj_j2k_dump_tile_info(p_j2k->m_specific_param.m_decoder.m_default_tcp,
                           (OPJ_INT32)p_j2k->m_private_image->numcomps, out_stream);
    fprintf(out_stream, "}\n");
}