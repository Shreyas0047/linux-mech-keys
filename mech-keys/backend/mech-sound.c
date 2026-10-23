#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <linux/input.h>

#define MAX_SOUNDS 512
#define MAX_DEVICES 32
#define MAX_PATH_LEN 512

typedef struct {
    int keycode;
    char path[MAX_PATH_LEN];
} KeySound;

typedef struct {
    char name[128];
    char path[MAX_PATH_LEN];
    int fd;
} InputDevice;

static ma_engine g_engine;
static int g_engine_inited = 0;
static KeySound g_sounds[MAX_SOUNDS];
static int g_sound_count = 0;
static char g_default_wav[MAX_PATH_LEN] = "";
static float g_volume = 1.0f;
static int g_muted = 0;
static char g_pack_dir[MAX_PATH_LEN] = "";
static InputDevice g_devices[MAX_DEVICES];
static int g_device_count = 0;
static volatile int g_running = 1;
static FILE *g_log = NULL;

void sig_handler(int sig) {
    (void)sig;
    g_running = 0;
}

void log_msg(const char *fmt, ...) {
    if (!g_log) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(g_log, fmt, args);
    va_end(args);
    fflush(g_log);
}

void unload_soundpack() {
    g_sound_count = 0;
    g_default_wav[0] = '\0';
}

int load_soundpack(const char *dir_path) {
    if (!g_engine_inited) return 0;
    unload_soundpack();

    strncpy(g_pack_dir, dir_path, sizeof(g_pack_dir) - 1);
    log_msg("Loading sound pack from %s\n", dir_path);

    // Read all .wav files in directory
    DIR *d = opendir(dir_path);
    if (d) {
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL && g_sound_count < MAX_SOUNDS) {
            if (strstr(ent->d_name, ".wav")) {
                char wav_full[MAX_PATH_LEN];
                snprintf(wav_full, sizeof(wav_full), "%s/%s", dir_path, ent->d_name);
                int keycode = atoi(ent->d_name);

                g_sounds[g_sound_count].keycode = keycode;
                strncpy(g_sounds[g_sound_count].path, wav_full, MAX_PATH_LEN - 1);

                // Pick representative fallback sound (e.g. keycode 57 for spacebar or 30 for 'a')
                if (keycode == 57 || keycode == 30 || g_sound_count == 0) {
                    strncpy(g_default_wav, wav_full, MAX_PATH_LEN - 1);
                }

                g_sound_count++;
            }
        }
        closedir(d);
    }

    log_msg("Loaded %d sound files. Default sound: %s\n", g_sound_count, g_default_wav);
    fprintf(stderr, "Loaded sound pack '%s' (%d key sounds)\n", dir_path, g_sound_count);
    return g_sound_count > 0;
}

void play_key(int keycode, int is_press) {
    if (g_muted || !is_press || g_sound_count == 0) {
        log_msg("Skip play: muted=%d, is_press=%d, sounds=%d\n", g_muted, is_press, g_sound_count);
        return;
    }

    const char *wav = NULL;
    for (int i = 0; i < g_sound_count; i++) {
        if (g_sounds[i].keycode == keycode) {
            wav = g_sounds[i].path;
            break;
        }
    }

    if (!wav) {
        if (strlen(g_default_wav) > 0) {
            wav = g_default_wav;
        } else {
            int idx = keycode % g_sound_count;
            wav = g_sounds[idx].path;
        }
    }

    if (wav) {
        ma_result r = ma_engine_play_sound(&g_engine, wav, NULL);
        log_msg("PLAY: keycode=%d, file=%s, res=%d\n", keycode, wav, r);
    }
}

int is_keyboard_device(const char *dev_path, char *name_out, size_t name_max) {
    int fd = open(dev_path, O_RDONLY | O_NONBLOCK);
    if (fd < 0) return 0;

    unsigned long evbit = 0;
    ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);

    if (!(evbit & (1 << EV_KEY))) {
        close(fd);
        return 0;
    }

    unsigned long keybit[KEY_MAX / 8 / sizeof(long) + 1] = {0};
    ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit);

    int has_a = (keybit[30 / (sizeof(long) * 8)] >> (30 % (sizeof(long) * 8))) & 1;
    int has_space = (keybit[57 / (sizeof(long) * 8)] >> (57 % (sizeof(long) * 8))) & 1;

    if (!has_a && !has_space) {
        close(fd);
        return 0;
    }

    if (name_out && name_max > 0) {
        char name[128] = "Unknown Keyboard";
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        strncpy(name_out, name, name_max - 1);
        name_out[name_max - 1] = '\0';
    }

    close(fd);
    return 1;
}

void list_devices_json() {
    printf("[\n");
    int first = 1;
    for (int i = 0; i < 32; i++) {
        char path[64];
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        char name[128];
        if (is_keyboard_device(path, name, sizeof(name))) {
            if (!first) printf(",\n");
            printf("  {\"path\": \"%s\", \"name\": \"%s\"}", path, name);
            first = 0;
        }
    }
    printf("\n]\n");
}

int add_device(int epoll_fd, const char *dev_path) {
    if (g_device_count >= MAX_DEVICES) return -1;

    int fd = open(dev_path, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        log_msg("Could not open device %s: %s\n", dev_path, strerror(errno));
        return -1;
    }

    char name[128] = "";
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        log_msg("epoll_ctl ADD failed for %s: %s\n", dev_path, strerror(errno));
        close(fd);
        return -1;
    }

    g_devices[g_device_count].fd = fd;
    strncpy(g_devices[g_device_count].path, dev_path, MAX_PATH_LEN - 1);
    strncpy(g_devices[g_device_count].name, name, sizeof(g_devices[g_device_count].name) - 1);
    g_device_count++;

    log_msg("Listening to keyboard: %s (%s) on fd %d\n", name, dev_path, fd);
    fprintf(stderr, "Listening to keyboard: %s (%s)\n", name, dev_path);
    return fd;
}

int auto_detect_devices(int epoll_fd) {
    int found = 0;
    for (int i = 0; i < 32; i++) {
        char path[64];
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        char name[128];
        if (is_keyboard_device(path, name, sizeof(name))) {
            if (add_device(epoll_fd, path) >= 0) {
                found++;
            }
        }
    }
    return found;
}

void handle_ipc_command(const char *cmd) {
    log_msg("IPC RECV: %s\n", cmd);
    if (strncmp(cmd, "VOLUME ", 7) == 0) {
        g_volume = atof(cmd + 7);
        if (g_volume < 0.0f) g_volume = 0.0f;
        if (g_volume > 1.0f) g_volume = 1.0f;
        ma_engine_set_volume(&g_engine, g_volume);
    } else if (strncmp(cmd, "MUTE ", 5) == 0) {
        g_muted = atoi(cmd + 5);
    } else if (strncmp(cmd, "PACK ", 5) == 0) {
        char pack[MAX_PATH_LEN];
        strncpy(pack, cmd + 5, sizeof(pack) - 1);
        pack[sizeof(pack) - 1] = '\0';
        size_t len = strlen(pack);
        while (len > 0 && (pack[len - 1] == '\r' || pack[len - 1] == '\n')) {
            pack[--len] = '\0';
        }
        load_soundpack(pack);
    } else if (strcmp(cmd, "TEST") == 0) {
        play_key(57, 1);
    } else if (strcmp(cmd, "PING") == 0) {
        printf("PONG\n");
        fflush(stdout);
    } else if (strcmp(cmd, "QUIT") == 0) {
        g_running = 0;
    }
}

int main(int argc, char **argv) {
    g_log = fopen("/tmp/mech-sound.log", "w");
    log_msg("mech-sound starting up...\n");

    char specific_device[MAX_PATH_LEN] = "";
    char pack_path[MAX_PATH_LEN] = "";
    float initial_volume = 1.0f;
    int initial_mute = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--list-devices") == 0 || strcmp(argv[i], "-l") == 0) {
            list_devices_json();
            return 0;
        } else if ((strcmp(argv[i], "--device") == 0 || strcmp(argv[i], "-d") == 0) && i + 1 < argc) {
            strncpy(specific_device, argv[++i], sizeof(specific_device) - 1);
        } else if ((strcmp(argv[i], "--pack") == 0 || strcmp(argv[i], "-p") == 0) && i + 1 < argc) {
            strncpy(pack_path, argv[++i], sizeof(pack_path) - 1);
        } else if ((strcmp(argv[i], "--volume") == 0 || strcmp(argv[i], "-v") == 0) && i + 1 < argc) {
            initial_volume = atof(argv[++i]);
        } else if (strcmp(argv[i], "--mute") == 0 || strcmp(argv[i], "-m") == 0) {
            initial_mute = 1;
        }
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    ma_engine_config engineConfig = ma_engine_config_init();
    ma_result res = ma_engine_init(&engineConfig, &g_engine);
    if (res != MA_SUCCESS) {
        log_msg("Audio engine init failed: %d\n", res);
        fprintf(stderr, "Failed to initialize audio engine: error %d\n", res);
        return 1;
    }
    g_engine_inited = 1;
    g_volume = initial_volume;
    g_muted = initial_mute;
    ma_engine_set_volume(&g_engine, g_volume);
    log_msg("Audio engine initialized, vol=%.2f, mute=%d\n", g_volume, g_muted);

    if (strlen(pack_path) > 0) {
        load_soundpack(pack_path);
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        log_msg("epoll_create1 failed: %s\n", strerror(errno));
        return 1;
    }

    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK);
    struct epoll_event ev_stdin;
    ev_stdin.events = EPOLLIN;
    ev_stdin.data.fd = STDIN_FILENO;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev_stdin);

    if (strlen(specific_device) > 0) {
        add_device(epoll_fd, specific_device);
    } else {
        auto_detect_devices(epoll_fd);
    }

    log_msg("Total input devices added: %d\n", g_device_count);

    printf("READY\n");
    fflush(stdout);

    char stdin_buf[1024];
    int stdin_buf_pos = 0;
    struct epoll_event events[32];

    while (g_running) {
        int n = epoll_wait(epoll_fd, events, 32, 100);
        if (n < 0) {
            if (errno == EINTR) continue;
            break;
        }

        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == STDIN_FILENO) {
                char temp[256];
                int r = read(STDIN_FILENO, temp, sizeof(temp));
                if (r > 0) {
                    for (int j = 0; j < r; j++) {
                        if (temp[j] == '\n' || temp[j] == '\r') {
                            if (stdin_buf_pos > 0) {
                                stdin_buf[stdin_buf_pos] = '\0';
                                handle_ipc_command(stdin_buf);
                                stdin_buf_pos = 0;
                            }
                        } else if (stdin_buf_pos < (int)sizeof(stdin_buf) - 1) {
                            stdin_buf[stdin_buf_pos++] = temp[j];
                        }
                    }
                } else if (r == 0) {
                    log_msg("stdin closed, exiting\n");
                    g_running = 0;
                }
            } else {
                struct input_event ev[16];
                int bytes = read(fd, ev, sizeof(ev));
                if (bytes >= (int)sizeof(struct input_event)) {
                    int count = bytes / sizeof(struct input_event);
                    for (int k = 0; k < count; k++) {
                        if (ev[k].type == EV_KEY) {
                            log_msg("KEY EVENT: fd=%d, code=%d, val=%d\n", fd, ev[k].code, ev[k].value);
                            // Only play sound on initial key press (value == 1).
                            // Ignore repeat events (value == 2) so holding a key doesn't trigger continuous sounds.
                            if (ev[k].value == 1) {
                                play_key(ev[k].code, 1);
                            }
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < g_device_count; i++) {
        close(g_devices[i].fd);
    }
    close(epoll_fd);
    unload_soundpack();
    ma_engine_uninit(&g_engine);
    log_msg("mech-sound exited cleanly\n");
    if (g_log) fclose(g_log);

    return 0;
}
