inflate_png(GraphicsManager UNUSED *self, Image *img, uint8_t *buf, size_t bufsz) {
    png_read_data d = {.err_handler=png_error_handler};
    inflate_png_inner(&d, buf, bufsz);
    if (d.ok) {
        free_load_data(&img->load_data);
        img->load_data.buf = d.decompressed;
        img->load_data.buf_capacity = d.sz;
        img->load_data.buf_used = d.sz;
        img->load_data.data_sz = d.sz;
        img->width = d.width; img->height = d.height;
    }
    else free(d.decompressed);
    free(d.row_pointers);
    return d.ok;
}