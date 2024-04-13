void ssh_reseed(void){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    RAND_add(&tv, sizeof(tv), 0.0);
}