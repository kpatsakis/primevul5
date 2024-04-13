cmp_by_zindex_and_image(const void *a_, const void *b_) {
    const ImageRenderData *a = (const ImageRenderData*)a_, *b = (const ImageRenderData*)b_;
    int ans = a->z_index - b->z_index;
    if (ans == 0) ans = a->image_id - b->image_id;
    return ans;
}