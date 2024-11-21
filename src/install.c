#include <archive.h>
#include <archive_entry.h>
#include <libxml/parser.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "parser.h"
#include "repo.h"
#include "utils.h"

char* get_pkg_filename(struct repo_package pkg) {
    if (pkg.local) {
        return pkg.url;
    }
    char *pattern = "https?://[A-Za-z0-9\\.\\-]+.*/([A-Za-z0-9_\\.\\-]+)/?";

    regex_t regex;
    int reti = regcomp(&regex, pattern, REG_EXTENDED);

    regmatch_t matches[2];
    reti = regexec(&regex, pkg.url, 2, matches, 0);

    if (reti) {
        printf("URL issue %d %s\n", reti, pkg.url);
        exit(1);
    }

    int length = matches[1].rm_eo - matches[1].rm_so;
    char* path = (char*) malloc(sizeof(char)*(length+1));
    snprintf(path, length + 1, "%.*s", length, pkg.url + matches[1].rm_so);

    regfree(&regex);

    char* directory = catstring(VAR_PREFIX, "/cache/eggpm", NULL);
    system(catstring("mkdir -p ", directory, NULL));

    return catstring(directory, "/", path, NULL);
}

void read_xml(char* data, long size, struct repo_package* pkg) {
    xmlInitParser();

    xmlDoc* doc = xmlReadMemory(data, size, "info.xml", NULL, 0);

    xmlNode *root_element = xmlDocGetRootElement(doc);
    for (xmlNode *child = root_element->children; child; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
            xmlChar* value = xmlNodeGetContent(child);
            if (xmlStrcmp(child->name, (const xmlChar *)"name") == 0) {
                pkg->name = (char*) value;
            } else if (xmlStrcmp(child->name, (const xmlChar *)"version") == 0) {
                pkg->version = (char*) value;
            } else if (xmlStrcmp(child->name, (const xmlChar *)"architecture") == 0) {
                pkg->architecture = (char*) value;
            } else if (xmlStrcmp(child->name, (const xmlChar *)"description") == 0) {
                pkg->description = (char*) value;
            } else if (xmlStrcmp(child->name, (const xmlChar *)"size") == 0) {
                pkg->size = strtol((char*) value, NULL, 10);
            } else if (xmlStrcmp(child->name, (const xmlChar *)"dependencies") == 0) {
                pkg->rundepends = (char*) value;
            }
        }
    }
}

int get_info_xml(char* filename, struct repo_package* out_pkg) {
    struct archive *archive = archive_read_new();
    archive_read_support_filter_xz(archive);
    archive_read_support_format_tar(archive);

    if (archive_read_open_filename(archive, filename, 16384) != ARCHIVE_OK) {
        return 1;
    }

    struct archive_entry *entry;
    int found = 1;
    while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
        const char *current_file = archive_entry_pathname(entry);
        if (strcmp(current_file, "./info.xml") == 0) {
            long size = archive_entry_size(entry);
            char* data = (char*) malloc(size);
            archive_read_data(archive, data, size);
            out_pkg->local = 1;
            out_pkg->repository = "local";
            out_pkg->url = filename;
            read_xml(data, size, out_pkg);
            found = 0;
            break;
        }
    }
    return found;
}

void download_package(struct repo_package pkg) {
    download_file(pkg.url, get_pkg_filename(pkg), pkg.checksum);
}

static int copy_data(struct archive *ar, struct archive *aw) {
    int r;
    const void *buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF) {
            return (ARCHIVE_OK);
        }
        if (r < ARCHIVE_OK) {
            return (r);
        }
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) {
            fprintf(stderr, "%s\n", archive_error_string(aw));
            return (r);
        }
    }
}

void install_package(struct repo_package pkg, struct options opts) {
    char* filename = get_pkg_filename(pkg);

    //temporary so it doesnt break my system
    system(catstring("mkdir -p ", opts.root, NULL));

    struct archive *archive, *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    archive = archive_read_new();
    archive_read_support_filter_xz(archive);
    archive_read_support_format_tar(archive);

    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);

    if ((r = archive_read_open_filename(archive, filename, 16384))) {
        printf("Failed to read %s\n", filename);
        exit(1);
    }

    for (;;) {
        r = archive_read_next_header(archive, &entry);

        if (r == ARCHIVE_EOF) {
            break;
        }
        if (r < ARCHIVE_OK) {
            fprintf(stderr, "%s\n", archive_error_string(archive));
        }
        if (r < ARCHIVE_WARN) {
            printf("Archive error 1\n");
            exit(1);
        }

        const char *original_path = archive_entry_pathname(entry);
        if (strcmp(original_path, "./info.xml") == 0) {
            continue;
        }
        size_t size = strlen(opts.root) + strlen(original_path) + 2;
        char *new_path = malloc(sizeof(char) * size);
        snprintf(new_path, size, "%s/%s", opts.root, original_path);
        archive_entry_set_pathname(entry, new_path);
        free(new_path);

        r = archive_write_header(ext, entry);

        if (r < ARCHIVE_OK) {
            fprintf(stderr, "%s\n", archive_error_string(ext));
        } else if (archive_entry_size(entry) > 0) {
            r = copy_data(archive, ext);
            if (r < ARCHIVE_OK) {
                fprintf(stderr, "%s\n", archive_error_string(ext));   
            }
            if (r < ARCHIVE_WARN) {
                printf("Archive error 2\n");
                exit(1);
            }
        }

        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK) {
            fprintf(stderr, "%s\n", archive_error_string(ext));
        }
        if (r < ARCHIVE_WARN) {
            printf("Archive error 3\n");
            exit(1);
        }
    }

    archive_read_free(archive);
}