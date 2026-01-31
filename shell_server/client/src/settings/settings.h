#ifndef SETTINGS_H
#define SETTINGS_H

struct settings {
    char *user_hash;
    char *machine_hash;
    char *version;
};

struct settings *settings_create(void);
void settings_destroy(struct settings *settings);

#endif /* ! SETTINGS_H */
