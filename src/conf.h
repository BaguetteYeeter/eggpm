#ifndef CONF_H
#define CONF_H

struct conf {
    char** repositories;
    int repoc;
};

struct conf readconf();

#endif