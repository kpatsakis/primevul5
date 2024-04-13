    int RemoteIo::getb()
    {
        assert(p_->isMalloced_);
        if (p_->idx_ == p_->size_) {
            p_->eof_ = true;
            return EOF;
        }

        size_t expectedBlock = (p_->idx_ + 1)/p_->blockSize_;
        // connect to the remote machine & populate the blocks just in time.
        p_->populateBlocks(expectedBlock, expectedBlock);

        byte* data = p_->blocksMap_[expectedBlock].getData();
        return data[p_->idx_++ - expectedBlock*p_->blockSize_];
    }