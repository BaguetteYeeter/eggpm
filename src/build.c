#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ftw.h>

#include "utils.h"
#include "conf.h"

struct build_pkg {
    char* name;
    char* version;
    char* arch;
    char* description;
    char* makedepends;
    char* rundepends;
    char* url;
    char* checksum;
    char** stages;
};

static unsigned int totalSize = 0;
int sum(const char *fpath, const struct stat *sb, int typeflag) {
    if (S_ISDIR(sb->st_mode)) {return 0;}
    totalSize += sb->st_size;
    return 0;
}

//i hate ftw so much, it makes no sense
long int getSize(char* directory) {
    totalSize = 0;
    if (ftw(directory, &sum, 1)) {
        exit(1);
    }
    return (long int) totalSize;
}

void build_package(char* name) {
    char* path = catstring(name, "/build.sh", NULL);

    if (access(path, F_OK) != 0) {
        printf("Can't find build file\n");
        exit(1);
    }

    printf("\nBuilding package `%s`...\n", name);

    char** stages = (char**) malloc(sizeof(char*) * 100);
    for (int i = 0; i < 100; i++) {
        stages[i] = NULL;
    }
    struct build_pkg pkg = {"", "", get_arch(), "", "", "", "", "", stages};

    printf("\n---Reading package info---\n");

    FILE *fp = popen(catstring("exec bash -c 'source ", path, " && set'", NULL), "r");

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0;
        char** parts = split_string_no(line, "=", 1);
        if (strcmp(parts[0], "name") == 0) {
            pkg.name = parts[1];
        } else if (strcmp(parts[0], "version") == 0) {
            pkg.version = parts[1];
        } else if (strcmp(parts[0], "url") == 0) {
            pkg.url = parts[1];
        } else if (strcmp(parts[0], "checksum") == 0) {
            pkg.checksum = parts[1];
        } else if (strcmp(parts[0], "description") == 0) {
            pkg.description = parts[1];
        } else if (strcmp(parts[0], "makedepends") == 0) {
            pkg.makedepends = parts[1];
        } else if (strcmp(parts[0], "rundepends") == 0) {
            pkg.rundepends = parts[1];
        } else if (strstart(parts[0], "stage") == 0) {
            char *pattern = "stage([0-9][0-9])";
            regex_t regex;
            int reti = regcomp(&regex, pattern, REG_EXTENDED);
            regmatch_t matches[2];
            reti = regexec(&regex, parts[0], 2, matches, 0);
            int length = matches[1].rm_eo - matches[1].rm_so;
            char stageno[length + 1];
            snprintf(stageno, length + 1, "%.*s", length, parts[0] + matches[1].rm_so);
            int sno = strtol((char*) stageno, NULL, 10);
            if (parts[1][0] == '\'') {
                parts[1]++;
                parts[1][strlen(parts[1])-1] = 0;
            }
            if (stages[sno] == NULL) {
                stages[sno] = parts[1];
            } else {
                stages[sno] = catstring(stages[sno], "\n", parts[1], NULL);
            }
        }
    }

    pclose(fp);

    printf("\n---Downloading files---\n");
    printf("%s... ", pkg.url);
    fflush(stdout);
    char* filename = get_filename_url(pkg.url);
    char* real_filename = catstring(name, "/", filename, NULL);
    download_file(pkg.url, real_filename, pkg.checksum);
    printf("done\n");

    printf("\n---Extracting files---\n");
    printf("%s... ", filename);
    fflush(stdout);
    system(catstring("tar -xf ", real_filename, " -C ", name, NULL));
    printf("done\n");

    system(catstring("mkdir -p ", name, "/build", NULL));

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    chdir(name);

    printf("\n---Running stages---\n");
    for (int i = 0; i < 100; i++) {
        if (stages[i] == NULL) {
            continue;
        }

        printf(">>> %s\n", stages[i]);
        if (strstart(stages[i], "cd ") == 0) {
            char** parts = split_string_no(stages[i], " ", 1);
            chdir(parts[1]);
            continue;
        }

        fp = popen(stages[i], "r");

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            fputs(buffer, stdout);
        }

        int status = pclose(fp);
        status = WEXITSTATUS(status);
        if (status != 0) {
            printf("Last command failed");
            exit(status);
        }
    }

    long size = getSize("build");
    char* pkgxml = (char*) malloc(sizeof(char) * 1024);

    snprintf(
        pkgxml, 1024,
        "<package>\n"
        "    <name>%s</name>\n"
        "    <version>%s</version>\n"
        "    <architecture>%s</architecture>\n"
        "    <description>%s</description>\n"
        "    <size>%ld</size>\n"
        "    <dependencies>%s</dependencies>\n"
        "</package>",
        pkg.name, pkg.version, pkg.arch, pkg.description, size, pkg.rundepends
    );

    fp = fopen("build/info.xml", "wb");
    fwrite(pkgxml, 1, strlen(pkgxml), fp);
    fclose(fp);

    printf("\n---Packaging---\n");
    char* pkgpath = catstring(pkg.name, "-", pkg.version, "-", pkg.arch, ".eggpm", NULL);
    printf("%s... ", pkgpath);
    fflush(stdout);
    system("mkdir -p dist");
    system(catstring("tar -cJf dist/", pkgpath, " -C build .", NULL));
    printf("done\n");

    printf("\n---Deleting old files---\n");
    system(catstring("rm ", filename, NULL));
    system(catstring("rm -rf build"));

    chdir(cwd);

    printf("\n---------------------------------------\nPackage successfully created at `%s/dist/%s`\n", name, pkgpath);

    fp = fopen(catstring(name, "/dist/", pkgpath, NULL), "rb");
    char* checksum = calculate_sha256(fp);
    fclose(fp);

    printf("\nIf you're building this for a repo, here is package information\n");
    printf(
        "<package name=\"%s\">\n"
        "    <name>%s</name>\n"
        "    <version>%s</version>\n"
        "    <architecture>%s</architecture>\n"
        "    <description>%s</description>\n"
        "    <size>%ld</size>\n"
        "    <dependencies>%s</dependencies>\n"
        "    <url>YOURURL/%s</url>\n"
        "    <checksum>%s</checksum>\n"
        "</package>\n",
        pkg.name, pkg.name, pkg.version, pkg.arch, pkg.description, size, pkg.rundepends, pkgpath, checksum
    );
}
