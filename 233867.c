bliptype_name (int const type)
{
	switch (type) {
	case 2:	 return "emf.gz";
	case 3:	 return "wmf.gz";
	case 4:	 return "pict.gz";
	case 5:	 return "jpg";
	case 6:	 return "png";
	case 7:	 return "dib";
	default: return "Unknown";
	}
}