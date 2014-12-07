int is_num(const char *arg) {
    for(; *arg; arg++) {
        if (*arg <= '9' && *arg >= '0') {
            continue;
        }
        return 0;
    }
    return 1;
}