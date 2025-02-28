tsize_t t2p_write_pdf_xobject_cs(T2P* t2p, TIFF* output){

	tsize_t written=0;
	char buffer[128];
	int buflen=0;

	float X_W=1.0;
	float Y_W=1.0;
	float Z_W=1.0;
	
	if( (t2p->pdf_colorspace & T2P_CS_ICCBASED) != 0){
		written += t2p_write_pdf_xobject_icccs(t2p, output);
		return(written);
	}
	if( (t2p->pdf_colorspace & T2P_CS_PALETTE) != 0){
		written += t2pWriteFile(output, (tdata_t) "[ /Indexed ", 11);
		t2p->pdf_colorspace ^= T2P_CS_PALETTE;
		written += t2p_write_pdf_xobject_cs(t2p, output);
		t2p->pdf_colorspace |= T2P_CS_PALETTE;
		buflen=snprintf(buffer, sizeof(buffer), "%u", (0x0001 << t2p->tiff_bitspersample)-1 );
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) " ", 1);
		buflen=snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)t2p->pdf_palettecs ); 
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) " 0 R ]\n", 7);
		return(written);
	}
	if(t2p->pdf_colorspace & T2P_CS_BILEVEL){
			written += t2pWriteFile(output, (tdata_t) "/DeviceGray \n", 13);
	}
	if(t2p->pdf_colorspace & T2P_CS_GRAY){
			if(t2p->pdf_colorspace & T2P_CS_CALGRAY){
				written += t2p_write_pdf_xobject_calcs(t2p, output);
			} else {
				written += t2pWriteFile(output, (tdata_t) "/DeviceGray \n", 13);
			}
	}
	if(t2p->pdf_colorspace & T2P_CS_RGB){
			if(t2p->pdf_colorspace & T2P_CS_CALRGB){
				written += t2p_write_pdf_xobject_calcs(t2p, output);
			} else {
				written += t2pWriteFile(output, (tdata_t) "/DeviceRGB \n", 12);
			}
	}
	if(t2p->pdf_colorspace & T2P_CS_CMYK){
			written += t2pWriteFile(output, (tdata_t) "/DeviceCMYK \n", 13);
	}
	if(t2p->pdf_colorspace & T2P_CS_LAB){
			written += t2pWriteFile(output, (tdata_t) "[/Lab << \n", 10);
			written += t2pWriteFile(output, (tdata_t) "/WhitePoint ", 12);
			X_W = t2p->tiff_whitechromaticities[0];
			Y_W = t2p->tiff_whitechromaticities[1];
			Z_W = 1.0F - (X_W + Y_W);
			normalizePoint(X_W, Y_W, Z_W);
			buflen=snprintf(buffer, sizeof(buffer), "[%.4f %.4f %.4f] \n", X_W, Y_W, Z_W);
			check_snprintf_ret(t2p, buflen, buffer);
			written += t2pWriteFile(output, (tdata_t) buffer, buflen);
			written += t2pWriteFile(output, (tdata_t) "/Range ", 7);
			buflen=snprintf(buffer, sizeof(buffer), "[%d %d %d %d] \n", 
				t2p->pdf_labrange[0], 
				t2p->pdf_labrange[1], 
				t2p->pdf_labrange[2], 
				t2p->pdf_labrange[3]);
			check_snprintf_ret(t2p, buflen, buffer);
			written += t2pWriteFile(output, (tdata_t) buffer, buflen);
			written += t2pWriteFile(output, (tdata_t) ">>] \n", 5);
			
	}
	
	return(written);
}