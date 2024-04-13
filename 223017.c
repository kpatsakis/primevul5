void j2k_dump_image_header(opj_image_t* img_header, OPJ_BOOL dev_dump_flag,
                           FILE* out_stream)
{
    char tab[2];

    if (dev_dump_flag) {
        fprintf(stdout, "[DEV] Dump an image_header struct {\n");
        tab[0] = '\0';
    } else {
        fprintf(out_stream, "Image info {\n");
        tab[0] = '\t';
        tab[1] = '\0';
    }

    fprintf(out_stream, "%s x0=%d, y0=%d\n", tab, img_header->x0, img_header->y0);
    fprintf(out_stream,     "%s x1=%d, y1=%d\n", tab, img_header->x1,
            img_header->y1);
    fprintf(out_stream, "%s numcomps=%d\n", tab, img_header->numcomps);

    if (img_header->comps) {
        OPJ_UINT32 compno;
        for (compno = 0; compno < img_header->numcomps; compno++) {
            fprintf(out_stream, "%s\t component %d {\n", tab, compno);
            j2k_dump_image_comp_header(&(img_header->comps[compno]), dev_dump_flag,
                                       out_stream);
            fprintf(out_stream, "%s}\n", tab);
        }
    }

    fprintf(out_stream, "}\n");
}