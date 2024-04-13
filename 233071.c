static void test_thumb_ite() {
    uc_engine *uc;
    uc_err err;

    uint32_t sp = 0x1234;
    uint32_t r2 = 0, r3 = 1;
    uint32_t step_r2, step_r3;

    int i, addr=ADDRESS;

    printf("Emulate a THUMB ITE block as a whole or per instruction.\n");
    err = uc_open(UC_ARCH_ARM, UC_MODE_THUMB, &uc);
    if (err) {
        printf("Failed on uc_open() with error returned: %u (%s)\n",
                err, uc_strerror(err));
        return;
    }

    uc_mem_map(uc, ADDRESS, 2 * 1024 * 1024, UC_PROT_ALL);

    uc_mem_write(uc, ADDRESS, ARM_THUM_COND_CODE, sizeof(ARM_THUM_COND_CODE) - 1);

    uc_reg_write(uc, UC_ARM_REG_SP, &sp);

    uc_reg_write(uc, UC_ARM_REG_R2, &r2);
    uc_reg_write(uc, UC_ARM_REG_R3, &r3);

    // Run once.
    printf("Running the entire binary.\n");
    err = uc_emu_start(uc, ADDRESS | 1, ADDRESS + sizeof(ARM_THUM_COND_CODE) - 1, 0, 0);
    if (err) {
        printf("Failed on uc_emu_start() with error returned: %u\n", err);
    }
    uc_reg_read(uc, UC_ARM_REG_R2, &r2);
    uc_reg_read(uc, UC_ARM_REG_R3, &r3);

    printf(">>> R2: %d\n", r2);
    printf(">>> R3: %d\n\n", r3);

    // Step each instruction.
    printf("Running the binary one instruction at a time.\n");
    for (i = 0; i < sizeof(ARM_THUM_COND_CODE) / 2; i++) {
        err = uc_emu_start(uc, addr | 1, ADDRESS + sizeof(ARM_THUM_COND_CODE) - 1, 0, 1);
        if (err) {
            printf("Failed on uc_emu_start() with error returned: %u\n", err);
        }
        uc_reg_read(uc, UC_ARM_REG_PC, &addr);
    }

    uc_reg_read(uc, UC_ARM_REG_R2, &step_r2);
    uc_reg_read(uc, UC_ARM_REG_R3, &step_r3);

    printf(">>> R2: %d\n", step_r2);
    printf(">>> R3: %d\n\n", step_r3);

    if (step_r2 != r2 || step_r3 != r3) {
        printf("Failed with ARM ITE blocks stepping!\n");
    }

    uc_close(uc);
}