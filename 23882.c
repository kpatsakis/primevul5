static void* kssl_calloc(size_t nmemb, size_t size)
{
	void* p;
	
	p=OPENSSL_malloc(nmemb*size);
	if (p){
		memset(p, 0, nmemb*size);
	}
	return p;
}