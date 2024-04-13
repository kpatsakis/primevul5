    void MemIo::Impl::reserve(size_t wcount)
    {
        const size_t need = wcount + idx_;
        size_t blockSize =     32*1024;   // 32768           `
        const size_t maxBlockSize = 4*1024*1024;

        if (!isMalloced_) {
            // Minimum size for 1st block
            size_t size  = std::max(blockSize * (1 + need / blockSize), size_);
            byte* data = (byte*)std::malloc(size);
            if (  data == nullptr ) {
                throw Error(kerMallocFailed);
            }
            if (data_ != nullptr) {
                std::memcpy(data, data_, size_);
            }
            data_ = data;
            sizeAlloced_ = size;
            isMalloced_ = true;
        }

        if (need > size_) {
            if (need > sizeAlloced_) {
                blockSize = 2*sizeAlloced_ ;
                if ( blockSize > maxBlockSize ) blockSize = maxBlockSize ;
                // Allocate in blocks
                size_t want      = blockSize * (1 + need / blockSize );
                data_ = (byte*)std::realloc(data_, want);
                if ( data_ == nullptr ) {
                    throw Error(kerMallocFailed);
                }
                sizeAlloced_ = want;
                isMalloced_ = true;
            }
            size_ = need;
        }
    }