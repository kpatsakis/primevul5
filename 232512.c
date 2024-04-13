tsize_t t2p_write_pdf(T2P* t2p, TIFF* input, TIFF* output){

	tsize_t written=0;
	ttile_t i2=0;
	tsize_t streamlen=0;
	uint16 i=0;

	t2p_read_tiff_init(t2p, input);
	if(t2p->t2p_error!=T2P_ERR_OK){return(0);}
	t2p->pdf_xrefoffsets= (uint32*) _TIFFmalloc(TIFFSafeMultiply(tmsize_t,t2p->pdf_xrefcount,sizeof(uint32)) );
	if(t2p->pdf_xrefoffsets==NULL){
		TIFFError(
			TIFF2PDF_MODULE, 
			"Can't allocate %u bytes of memory for t2p_write_pdf", 
			(unsigned int) (t2p->pdf_xrefcount * sizeof(uint32)) );
		t2p->t2p_error = T2P_ERR_ERROR;
		return(written);
	}
	t2p->pdf_xrefcount=0;
	t2p->pdf_catalog=1;
	t2p->pdf_info=2;
	t2p->pdf_pages=3;
	written += t2p_write_pdf_header(t2p, output);
	t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
	t2p->pdf_catalog=t2p->pdf_xrefcount;
	written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
	written += t2p_write_pdf_catalog(t2p, output);
	written += t2p_write_pdf_obj_end(output);
	t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
	t2p->pdf_info=t2p->pdf_xrefcount;
	written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
	written += t2p_write_pdf_info(t2p, input, output);
	written += t2p_write_pdf_obj_end(output);
	t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
	t2p->pdf_pages=t2p->pdf_xrefcount;
	written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
	written += t2p_write_pdf_pages(t2p, output);
	written += t2p_write_pdf_obj_end(output);
	for(t2p->pdf_page=0;t2p->pdf_page<t2p->tiff_pagecount;t2p->pdf_page++){
		t2p_read_tiff_data(t2p, input);
		if(t2p->t2p_error!=T2P_ERR_OK){return(0);}
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_page(t2p->pdf_xrefcount, t2p, output);
		written += t2p_write_pdf_obj_end(output);
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_dict_start(output);
		written += t2p_write_pdf_stream_dict(0, t2p->pdf_xrefcount+1, output);
		written += t2p_write_pdf_stream_dict_end(output);
		written += t2p_write_pdf_stream_start(output);
		streamlen=written;
		written += t2p_write_pdf_page_content_stream(t2p, output);
		streamlen=written-streamlen;
		written += t2p_write_pdf_stream_end(output);
		written += t2p_write_pdf_obj_end(output);
		t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
		written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
		written += t2p_write_pdf_stream_length(streamlen, output);
		written += t2p_write_pdf_obj_end(output);
		if(t2p->tiff_transferfunctioncount != 0){
			t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
			written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
			written += t2p_write_pdf_transfer(t2p, output);
			written += t2p_write_pdf_obj_end(output);
			for(i=0; i < t2p->tiff_transferfunctioncount; i++){
				t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
				written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
				written += t2p_write_pdf_stream_dict_start(output);
				written += t2p_write_pdf_transfer_dict(t2p, output, i);
				written += t2p_write_pdf_stream_dict_end(output);
				written += t2p_write_pdf_stream_start(output);
				/* streamlen=written; */ /* value not used */
				written += t2p_write_pdf_transfer_stream(t2p, output, i);
				/* streamlen=written-streamlen; */ /* value not used */
				written += t2p_write_pdf_stream_end(output);
				written += t2p_write_pdf_obj_end(output);
			}
		}
		if( (t2p->pdf_colorspace & T2P_CS_PALETTE) != 0){
			t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
			t2p->pdf_palettecs=t2p->pdf_xrefcount;
			written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
			written += t2p_write_pdf_stream_dict_start(output);
			written += t2p_write_pdf_stream_dict(t2p->pdf_palettesize, 0, output);
			written += t2p_write_pdf_stream_dict_end(output);
			written += t2p_write_pdf_stream_start(output);
			/* streamlen=written; */ /* value not used */
			written += t2p_write_pdf_xobject_palettecs_stream(t2p, output);
			/* streamlen=written-streamlen; */ /* value not used */
			written += t2p_write_pdf_stream_end(output);
			written += t2p_write_pdf_obj_end(output);
		}
		if( (t2p->pdf_colorspace & T2P_CS_ICCBASED) != 0){
			t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
			t2p->pdf_icccs=t2p->pdf_xrefcount;
			written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
			written += t2p_write_pdf_stream_dict_start(output);
			written += t2p_write_pdf_xobject_icccs_dict(t2p, output);
			written += t2p_write_pdf_stream_dict_end(output);
			written += t2p_write_pdf_stream_start(output);
			/* streamlen=written; */ /* value not used */
			written += t2p_write_pdf_xobject_icccs_stream(t2p, output);
			/* streamlen=written-streamlen; */ /* value not used */
			written += t2p_write_pdf_stream_end(output);
			written += t2p_write_pdf_obj_end(output);
		}
		if(t2p->tiff_tiles[t2p->pdf_page].tiles_tilecount !=0){
			for(i2=0;i2<t2p->tiff_tiles[t2p->pdf_page].tiles_tilecount;i2++){
				t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
				written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
				written += t2p_write_pdf_stream_dict_start(output);
				written += t2p_write_pdf_xobject_stream_dict(
					i2+1, 
					t2p, 
					output);
				written += t2p_write_pdf_stream_dict_end(output);
				written += t2p_write_pdf_stream_start(output);
				streamlen=written;
				t2p_read_tiff_size_tile(t2p, input, i2);
				written += t2p_readwrite_pdf_image_tile(t2p, input, output, i2);
				t2p_write_advance_directory(t2p, output);
				if(t2p->t2p_error!=T2P_ERR_OK){return(0);}
				streamlen=written-streamlen;
				written += t2p_write_pdf_stream_end(output);
				written += t2p_write_pdf_obj_end(output);
				t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
				written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
				written += t2p_write_pdf_stream_length(streamlen, output);
				written += t2p_write_pdf_obj_end(output);
			}
		} else {
			t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
			written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
			written += t2p_write_pdf_stream_dict_start(output);
			written += t2p_write_pdf_xobject_stream_dict(
				0, 
				t2p, 
				output);
			written += t2p_write_pdf_stream_dict_end(output);
			written += t2p_write_pdf_stream_start(output);
			streamlen=written;
			t2p_read_tiff_size(t2p, input);
			if (t2p->tiff_maxdatasize && (t2p->tiff_datasize > t2p->tiff_maxdatasize)) {
				TIFFError(TIFF2PDF_MODULE,
					"Allocation of " TIFF_UINT64_FORMAT " bytes is forbidden. Limit is " TIFF_UINT64_FORMAT ". Use -m option to change limit",
					(uint64)t2p->tiff_datasize, (uint64)t2p->tiff_maxdatasize);
				t2p->t2p_error = T2P_ERR_ERROR;
				return (0);
			}
			written += t2p_readwrite_pdf_image(t2p, input, output);
			t2p_write_advance_directory(t2p, output);
			if(t2p->t2p_error!=T2P_ERR_OK){return(0);}
			streamlen=written-streamlen;
			written += t2p_write_pdf_stream_end(output);
			written += t2p_write_pdf_obj_end(output);
			t2p->pdf_xrefoffsets[t2p->pdf_xrefcount++]=written;
			written += t2p_write_pdf_obj_start(t2p->pdf_xrefcount, output);
			written += t2p_write_pdf_stream_length(streamlen, output);
			written += t2p_write_pdf_obj_end(output);
		}
	}
	t2p->pdf_startxref = written;
	written += t2p_write_pdf_xreftable(t2p, output);
	written += t2p_write_pdf_trailer(t2p, output);
	t2p_disable(output);

	return(written);
}