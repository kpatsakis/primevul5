static void  opj_j2k_read_float64_to_int32(const void * p_src_data,
        void * p_dest_data, OPJ_UINT32 p_nb_elem)
{
    OPJ_BYTE * l_src_data = (OPJ_BYTE *) p_src_data;
    OPJ_INT32 * l_dest_data = (OPJ_INT32 *) p_dest_data;
    OPJ_UINT32 i;
    OPJ_FLOAT64 l_temp;

    for (i = 0; i < p_nb_elem; ++i) {
        opj_read_double(l_src_data, &l_temp);

        l_src_data += sizeof(OPJ_FLOAT64);

        *(l_dest_data++) = (OPJ_INT32) l_temp;
    }
}