int validated_offset(uint32_t addr, uint32_t start, uint32_t end){
    return addr <= end && addr >= start? 0:-1;
}