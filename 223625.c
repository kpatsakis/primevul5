    int RemoteIo::open()
    {
        close(); // reset the IO position
        bigBlock_ = nullptr;
        if (p_->isMalloced_ == false) {
            long length = p_->getFileLength();
            if (length < 0) { // unable to get the length of remote file, get the whole file content.
                std::string data;
                p_->getDataByRange(-1, -1, data);
                p_->size_ = data.length();
                size_t nBlocks = (p_->size_ + p_->blockSize_ - 1) / p_->blockSize_;
                p_->blocksMap_  = new BlockMap[nBlocks];
                p_->isMalloced_ = true;
                byte* source = (byte*)data.c_str();
                size_t remain = p_->size_, iBlock = 0, totalRead = 0;
                while (remain) {
                    size_t allow = std::min(remain, p_->blockSize_);
                    p_->blocksMap_[iBlock].populate(&source[totalRead], allow);
                    remain -= allow;
                    totalRead += allow;
                    iBlock++;
                }
            } else if (length == 0) { // file is empty
                throw Error(kerErrorMessage, "the file length is 0");
            } else {
                p_->size_ = (size_t) length;
                size_t nBlocks = (p_->size_ + p_->blockSize_ - 1) / p_->blockSize_;
                p_->blocksMap_  = new BlockMap[nBlocks];
                p_->isMalloced_ = true;
            }
        }
        return 0; // means OK
    }