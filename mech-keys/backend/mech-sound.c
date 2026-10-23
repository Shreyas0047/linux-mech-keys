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

#define MAX_SOUNDS 256
#define MAX_DEVICES 32
#define MAX_PATH_LEN 512

typedef struct {
    int keycode;
    ma_sound sound;
    int loaded;
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
static ma_sound g_default_sound;
static int g_has_default = 0;
static float g_volume = 1.0f;
static int g_muted = 0;
static char g_pack_dir[MAX_PATH_LEN] = "";
static InputDevice g_devices[MAX_DEVICES];
static int g_device_count = 0;
static volatile int g_running = 1;

void sig_handler(int sig) {
    (void)sig;
    g_running = 0;
}

// Unload current sound pack
void unload_soundpack() {
    for (int i = 0; i < g_sound_count; i++) {
        if (g_sounds[i].loaded) {
            ma_sound_uninit(&g_sounds[i].sound);
            g_sounds[i].loaded = 0;
        }
    }
    g_sound_count = 0;
    if (g_has_default) {
        ma_sound_uninit(&g_default_sound);
        g_has_default = 0;
    }
}

// Load a sound pack from a directory
int load_soundpack(const char *dir_path) {
    if (!g_engine_inited) return 0;
    unload_soundpack();

    strncpy(g_pack_dir, dir_path, sizeof(g_pack_dir) - 1);

    // Try reading config.json for mappings
    char config_path[MAX_PATH_LEN];
    snprintf(config_path, sizeof(config_path), "%s/config.json", dir_path);

    FILE *f = fopen(config_path, "r");
    if (f) {
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (sz > 0 && sz < 2 * 1024 * 1024) {
            char *buf = malloc(sz + 1);
            if (buf) {
                size_t read_bytes = fread(buf, 1, sz, f);
                buf[read_bytes] = '\0';

                // Simple JSON parser for "defines":{"1":"1.wav",...}
                char *defines = strstr(buf, "\"defines\"");
                if (defines) {
                    char *p = strchr(defines, '{');
                    if (p) {
                        p++;
                        while (*p && *p != '}' && g_sound_count < MAX_SOUNDS) {
                            while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ',') p++;
                            if (*p == '}') break;
                            if (*p == '"') {
                                p++;
                                int keycode = atoi(p);
                                p = strchr(p, '"');
                                if (!p) break;
                                p++;
                                p = strchr(p, ':');
                                if (!p) break;
                                p++;
                                while (*p == ' ' || *p == '\t') p++;
                                if (*p == '"') {
                                    p++;
                                    char *val_end = strchr(p, '"');
                                    if (!val_end) break;
                                    char wav_name[128];
                                    int len = val_end - p;
                                    if (len >= (int)sizeof(wav_name)) len = sizeof(wav_name) - 1;
                                    strncpy(wav_name, p, len);
                                    wav_name[len] = '\0';
                                    p = val_end + 1;

                                    char wav_full[MAX_PATH_LEN];
                                    snprintf(wav_full, sizeof(wav_full), "%s/%s", dir_path, wav_name);

                                    ma_result res = ma_sound_init_from_file(&g_engine, wav_full, MA_SOUND_FLAG_DECODE, NULL, NULL, &g_sounds[g_sound_count].sound);
                                    if (res == MA_SUCCESS) {
                                        g_sounds[g_sound_count].keycode = keycode;
                                        g_sounds[g_sound_count].loaded = 1;
                                        g_sound_count++;
                                    }
                                }
                            } else {
                                p++;
                            }
                        }
                    }
                }
                free(buf);
            }
        }
        fclose(f);
    }

    // If config.json didn't provide individual keycodes or it's a folder of wavs, load all wav files
    if (g_sound_count == 0) {
        DIR *d = opendir(dir_path);
        if (d) {
            struct dirent *ent;
            while ((ent = readdir(d)) != NULL && g_sound_count < MAX_SOUNDS) {
                if (strstr(ent->d_name, ".wav")) {
                    char wav_full[MAX_PATH_LEN];
                    snprintf(wav_full, sizeof(wav_full), "%s/%s", dir_path, ent->d_name);
                    int keycode = atoi(ent->d_name);
                    ma_result res = ma_sound_init_from_file(&g_engine, wav_full, MA_SOUND_FLAG_DECODE, NULL, NULL, &g_sounds[g_sound_count].sound);
                    if (res == MA_SUCCESS) {
                        g_sounds[g_sound_count].keycode = keycode;
                        g_sounds[g_sound_count].loaded = 1;
                        g_sound_count++;
                    }
                }
            }
            closedir(d);
        }
    }

    // Set fallback default sound
    if (g_sound_count > 0) {
        // Find a representative sound (like keycode 30 'a' or just the first loaded)
        for (int i = 0; i < g_sound_count; i++) {
            if (g_sounds[i].keycode == 30 || g_sounds[i].keycode == 57 || i == 0) {
                // Initialize default sound from the same file or reuse
                g_has_default = 1;
                break;
            }
        }
    }

    fprintf(stderr, "Loaded sound pack '%s' (%d key sounds)\n", dir_path, g_sound_count);
    return g_sound_count > 0;
}

void play_key(int keycode, int is_press) {
    if (g_muted || !is_press || g_sound_count == 0) return;

    ma_sound *to_play = NULL;

    // Find specific sound for keycode
    for (int i = 0; i < g_sound_count; i++) {
        if (g_sounds[i].keycode == keycode) {
            to_play = &g_sounds[i].sound;
            break;
        }
    }

    // If no exact keycode, pick from common key sound or random key sound
    if (!to_play) {
        int idx = (keycode % g_sound_count);
        to_play = &g_sounds[idx].sound;
    }

    if (to_play) {
        // Add subtle pitch variation (+-2%) for natural feel
        float pitch = 0.98f + ((float)(rand() % 40) / 1000.0f);
        ma_sound_set_pitch(to_play, pitch);
        ma_sound_seek_to_pcm_frame(to_play, 0);
        ma_sound_start(to_play);
    }
}

// Discover keyboards
int is_keyboard_device(const char *dev_path, char *name_out, size_t name_max) {
    int fd = open(dev_path, O_RDONLY | O_NONBLOCK);
    if (fd < 0) return 0;

    unsigned long evbit = 0;
    ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);

    // Check EV_KEY
    if (!(evbit & (1 << EV_KEY))) {
        close(fd);
        return 0;
    }

    // Check if it has letter 'A' (KEY_A = 30) or 'SPACE' (KEY_SPACE = 57)
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
        fprintf(stderr, "Warning: Could not open %s: %s\n", dev_path, strerror(errno));
        return -1;
    }

    char name[128] = "";
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        close(fd);
        return -1;
    }

    g_devices[g_device_count].fd = fd;
    strncpy(g_devices[g_device_count].path, dev_path, MAX_PATH_LEN - 1);
    strncpy(g_devices[g_device_count].name, name, sizeof(g_devices[g_device_count].name) - 1);
    g_device_count++;

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
    if (strncmp(cmd, "VOLUME ", 7) == 0) {
        g_volume = atof(cmd + 7);
        if (g_volume < 0.0f) g_volume = 0.0f;
        if (g_volume > 1.0f) g_volume = 1.0f;
        ma_engine_set_volume(&g_engine, g_volume);
        fprintf(stderr, "Volume set to %.2f\n", g_volume);
    } else if (strncmp(cmd, "MUTE ", 5) == 0) {
        g_muted = atoi(cmd + 5);
        fprintf(stderr, "Mute set to %d\n", g_muted);
    } else if (strncmp(cmd, "PACK ", 5) == 0) {
        char pack[MAX_PATH_LEN];
        strncpy(pack, cmd + 5, sizeof(pack) - 1);
        pack[sizeof(pack) - 1] = '\0';
        // trim newline
        size_t len = strlen(pack);
        while (len > 0 && (pack[len - 1] == '\r' || pack[len - 1] == '\n')) {
            pack[--len] = '\0';
        }
        load_soundpack(pack);
    } else if (strcmp(cmd, "PING") == 0) {
        printf("PONG\n");
        fflush(stdout);
    } else if (strcmp(cmd, "QUIT") == 0) {
        g_running = 0;
    }
}

int main(int argc, char **argv) {
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

    // Initialize miniaudio engine with low-latency settings
    ma_engine_config engineConfig = ma_engine_config_init();
    ma_result res = ma_engine_init(&engineConfig, &g_engine);
    if (res != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialize audio engine: error %d\n", res);
        return 1;
    }
    g_engine_inited = 1;
    g_volume = initial_volume;
    g_muted = initial_mute;
    ma_engine_set_volume(&g_engine, g_volume);

    if (strlen(pack_path) > 0) {
        load_soundpack(pack_path);
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        return 1;
    }

    // Add stdin for IPC control
    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK);
    struct epoll_event ev_stdin;
    ev_stdin.events = EPOLLIN;
    ev_stdin.data.fd = STDIN_FILENO;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev_stdin);

    // Add keyboard input devices
    if (strlen(specific_device) > 0) {
        add_device(epoll_fd, specific_device);
    } else {
        auto_detect_devices(epoll_fd);
    }

    if (g_device_count == 0) {
        fprintf(stderr, "Error: No accessible keyboard devices found. Make sure user is in 'input' group.\n");
        // We still keep running to accept IPC commands
    }

    // Ready signal to parent process
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
                // Read IPC commands
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
                    // Stdin closed (parent died)
                    g_running = 0;
                }
            } else {
                // Keyboard event
                struct input_event ev[16];
                int bytes = read(fd, ev, sizeof(ev));
                if (bytes >= (int)sizeof(struct input_event)) {
                    int count = bytes / sizeof(struct input_event);
                    for (int k = 0; k < count; k++) {
                        if (ev[k].type == EV_KEY) {
                            // value 1 = press, 2 = repeat
                            if (ev[k].value == 1 || ev[k].value == 2) {
                                play_key(ev[k].code, 1);
                            }
                        }
                    }
                }
            }
        }
    }

    // Cleanup
    for (int i = 0; i < g_device_count; i++) {
        close(g_devices[i].fd);
    }
    close(epoll_fd);
    unload_soundpack();
    ma_engine_uninit(&g_engine);

    fprintf(stderr, "mech-sound exited cleanly\n");
    return 0;
}
