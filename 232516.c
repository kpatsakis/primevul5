tsize_t t2p_write_pdf_xobject_calcs(T2P* t2p, TIFF* output){

	tsize_t written=0;
	char buffer[256];
	int buflen=0;
	
	float X_W=0.0;
	float Y_W=0.0;
	float Z_W=0.0;
	float X_R=0.0;
	float Y_R=0.0;
	float Z_R=0.0;
	float X_G=0.0;
	float Y_G=0.0;
	float Z_G=0.0;
	float X_B=0.0;
	float Y_B=0.0;
	float Z_B=0.0;
	float x_w=0.0;
	float y_w=0.0;
	float z_w=0.0;
	float x_r=0.0;
	float y_r=0.0;
	float x_g=0.0;
	float y_g=0.0;
	float x_b=0.0;
	float y_b=0.0;
	float R=1.0;
	float G=1.0;
	float B=1.0;
	
	written += t2pWriteFile(output, (tdata_t) "[", 1);
	if(t2p->pdf_colorspace & T2P_CS_CALGRAY){
		written += t2pWriteFile(output, (tdata_t) "/CalGray ", 9);
		X_W = t2p->tiff_whitechromaticities[0];
		Y_W = t2p->tiff_whitechromaticities[1];
		Z_W = 1.0F - (X_W + Y_W);
		normalizePoint(X_W, Y_W, Z_W);
	}
	if(t2p->pdf_colorspace & T2P_CS_CALRGB){
		written += t2pWriteFile(output, (tdata_t) "/CalRGB ", 8);
		x_w = t2p->tiff_whitechromaticities[0];
		y_w = t2p->tiff_whitechromaticities[1];
		x_r = t2p->tiff_primarychromaticities[0];
		y_r = t2p->tiff_primarychromaticities[1];
		x_g = t2p->tiff_primarychromaticities[2];
		y_g = t2p->tiff_primarychromaticities[3];
		x_b = t2p->tiff_primarychromaticities[4];
		y_b = t2p->tiff_primarychromaticities[5];
		z_w = y_w * ((x_g - x_b)*y_r - (x_r-x_b)*y_g + (x_r-x_g)*y_b);
		Y_R = (y_r/R) * ((x_g-x_b)*y_w - (x_w-x_b)*y_g + (x_w-x_g)*y_b) / z_w;
		X_R = Y_R * x_r / y_r;
		Z_R = Y_R * (((1-x_r)/y_r)-1);
		Y_G = ((0.0F-(y_g))/G) * ((x_r-x_b)*y_w - (x_w-x_b)*y_r + (x_w-x_r)*y_b) / z_w;
		X_G = Y_G * x_g / y_g;
		Z_G = Y_G * (((1-x_g)/y_g)-1);
		Y_B = (y_b/B) * ((x_r-x_g)*y_w - (x_w-x_g)*y_r + (x_w-x_r)*y_g) / z_w;
		X_B = Y_B * x_b / y_b;
		Z_B = Y_B * (((1-x_b)/y_b)-1);
		X_W = (X_R * R) + (X_G * G) + (X_B * B);
		Y_W = (Y_R * R) + (Y_G * G) + (Y_B * B);
		Z_W = (Z_R * R) + (Z_G * G) + (Z_B * B);
		normalizePoint(X_W, Y_W, Z_W);
	}
	written += t2pWriteFile(output, (tdata_t) "<< \n", 4);
	if(t2p->pdf_colorspace & T2P_CS_CALGRAY){
		written += t2pWriteFile(output, (tdata_t) "/WhitePoint ", 12);
		buflen=snprintf(buffer, sizeof(buffer), "[%.4f %.4f %.4f] \n", X_W, Y_W, Z_W);
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) "/Gamma 2.2 \n", 12);
	}
	if(t2p->pdf_colorspace & T2P_CS_CALRGB){
		written += t2pWriteFile(output, (tdata_t) "/WhitePoint ", 12);
		buflen=snprintf(buffer, sizeof(buffer), "[%.4f %.4f %.4f] \n", X_W, Y_W, Z_W);
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) "/Matrix ", 8);
		buflen=snprintf(buffer, sizeof(buffer), "[%.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f] \n", 
			X_R, Y_R, Z_R, 
			X_G, Y_G, Z_G, 
			X_B, Y_B, Z_B); 
		check_snprintf_ret(t2p, buflen, buffer);
		written += t2pWriteFile(output, (tdata_t) buffer, buflen);
		written += t2pWriteFile(output, (tdata_t) "/Gamma [2.2 2.2 2.2] \n", 22);
	}
	written += t2pWriteFile(output, (tdata_t) ">>] \n", 5);

	return(written);
}