static char *flip_ximage_byte_order(XImage *xim) {
	char *order;
	if (xim->byte_order == LSBFirst) {
		order = "MSBFirst";
		xim->byte_order = MSBFirst;
		xim->bitmap_bit_order = MSBFirst;
	} else {
		order = "LSBFirst";
		xim->byte_order = LSBFirst;
		xim->bitmap_bit_order = LSBFirst;
	}
	return order;
}