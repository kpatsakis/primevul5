set_vertex_data(ImageRenderData *rd, const ImageRef *ref, const ImageRect *dest_rect) {
#define R(n, a, b) rd->vertices[n*4] = ref->src_rect.a; rd->vertices[n*4 + 1] = ref->src_rect.b; rd->vertices[n*4 + 2] = dest_rect->a; rd->vertices[n*4 + 3] = dest_rect->b;
        R(0, right, top); R(1, right, bottom); R(2, left, bottom); R(3, left, top);
#undef R
}