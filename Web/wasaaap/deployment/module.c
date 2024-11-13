// emcc -s WASM=1 -g -s EXPORTED_RUNTIME_METHODS='["cwrap", "addFunction"]' -s ALLOW_TABLE_GROWTH module.c --no-entry -o module.js


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten/emscripten.h>

typedef struct msg {
    char *msg_data;  
    size_t msg_data_len;      
    int msg_time;
    int msg_status;
} msg;

typedef struct stuff {
    char array[5];
    msg *mess;
    int size;
    int capacity;
} stuff;

#define INITIAL_CAPACITY 10

stuff s;

void init_stuff(stuff *s) {
    s->size = 0;
    s->capacity = INITIAL_CAPACITY;
    s->mess = (msg *)malloc(s->capacity * sizeof(msg));
    if (s->mess == NULL) {
        exit(1);
    }
}


char *sanitize(char *data, size_t len, size_t *out_len) {
    char *safe = malloc(len * 6);
    size_t safe_len = 0;
    
    for (size_t i = 0; i < len; i++) {
        switch (data[i]) {
            case '<':
                memcpy(safe + safe_len, "&lt;", 4);
                safe_len += 4;
                break;
            case '>':
                memcpy(safe + safe_len, "&gt;", 4);
                safe_len += 4;
                break;
            case '&':
                memcpy(safe + safe_len, "&amp;", 5);
                safe_len += 5;
                break;
            case '"':
                memcpy(safe + safe_len, "&quot;", 6);
                safe_len += 6;
                break;
            case '\'':
                memcpy(safe + safe_len, "&#x27;", 6);
                safe_len += 6;
                break;
            default:
                safe[safe_len++] = data[i];
                break;
        }
    }
    
    *out_len = safe_len;
    return safe;
}

char *toSafeHTML(char *content, size_t content_len) {

    size_t html_len = content_len + 100;
    char *safeHTML = malloc(html_len);
    
    snprintf((char *)safeHTML, html_len, "<article><p>%.*s</p></article>", (int)content_len, content);
    
    return safeHTML;
}

int add_msg_to_stuff(stuff *s, msg new_msg) {
    if (s->size >= s->capacity) {
        s->capacity *= 2;
        s->mess = (msg *)realloc(s->mess, s->capacity * sizeof(msg));
        if (s->mess == NULL) {
            exit(1);
        }
    }
    s->mess[s->size++] = new_msg;
    return s->size-1;
}

  
void free_stuff(stuff *s) {
    for (int i = 0; i < s->size; i++) {
        free(s->mess[i].msg_data);
    }
    free(s->mess);
}

EMSCRIPTEN_KEEPALIVE
void initialize() {
    init_stuff(&s);
};

EMSCRIPTEN_KEEPALIVE
void populateMsgHTML(int (*callback)( char *, int, int,int)) {
    int i = 0;
    for (i = 0; i < s.size; i++) {
        char *safeHTML = toSafeHTML(s.mess[i].msg_data, s.mess[i].msg_data_len);
        callback(safeHTML, s.mess[i].msg_status , s.mess[i].msg_time, i);
        free(safeHTML);
    }
};

EMSCRIPTEN_KEEPALIVE
int addMsg(char *content, size_t content_len, int time, int status) {
    if (content_len > 100) {
        return -1;
    }

    size_t safe_content_len;
    char *safe_content = sanitize(content, content_len, &safe_content_len);
    memset(content, '\0', content_len);

    msg new_msg;
    new_msg.msg_data = safe_content;
    new_msg.msg_data_len = safe_content_len;
    new_msg.msg_time = time;
    new_msg.msg_status = status;

    return add_msg_to_stuff(&s, new_msg);
};

EMSCRIPTEN_KEEPALIVE
int editMsg(int idx, char *newContent, size_t newContent_len, int newTime) {
    if (idx < 0 || idx >= s.size) {
        return -1;
    }

    if (newContent_len > 100) {
        return -1;
    }

    size_t safe_newContent_len;
    char *safe_newContent = sanitize(newContent, newContent_len, &safe_newContent_len);
    memset(newContent, '\0', newContent_len);

    memcpy(s.mess[idx].msg_data, safe_newContent, safe_newContent_len);
    free(safe_newContent);

    s.mess[idx].msg_data_len = safe_newContent_len;
    s.mess[idx].msg_time = newTime;
    s.mess[idx].msg_status = 1;

    return 0;
};


EMSCRIPTEN_KEEPALIVE
int deleteMsg(int idx, void (*callback)(int)) {
    if (idx < 0 || idx >= s.size) {
        return -1;
    }

    free(s.mess[idx].msg_data);

    for (int i = idx; i < s.size - 1; i++) {
        s.mess[i] = s.mess[i + 1];
    }

    s.size--;
    callback(idx);
    return 0;
}


