struct options {
    char** packages;
    int packc;
    int update_repo;
    int build_package;
    int install;
};

struct options parse(int argc, char *argv[]);