#include <stdio.h>
#include <string.h>
#define PADDING 2
#define MAX(a,b) (((a)>(b))?(a):(b))
void empty_line(size_t box_len) {
    printf("|");
    for (int i = 1; i < box_len-1; i++) {
        printf(" ");
    }
    printf("|\n");
}

void make_button(size_t box_len) {
    size_t btn_padding = (box_len - 12) / 2 - 1;
    printf("|");
    for (int i = 0; i < btn_padding+1; i++) {
        printf(" ");
    }
    for (int i = 0; i < 10; i++) {
        printf("_");
    }
    for (int i = 0; i < btn_padding+1; i++) {
        printf(" ");
    }
    printf("|\n");

    printf("|");
    for (int i = 0; i < btn_padding; i++) {
        printf(" ");
    }
    printf("|____OK____|");
    for (int i = 0; i < btn_padding; i++) {
        printf(" ");
    }
    printf("|\n");

}

void string_line(const char *str, size_t spacing, int odd) {
    printf("|");
    for (int i = 0; i < spacing-1; i++) {
        printf(" ");
    }

    printf("%s", str); 
    for (int i = 0; i < spacing-1; i++) {
        printf(" ");
    }
    if (!odd) {
        printf("|\n");
    }
    else {
        printf(" |\n");
    }
}

void MyMessageBox(const char *msg, const char *title) {
    size_t msg_len = strlen(msg);
    size_t title_len = strlen(title);
    size_t box_len = MAX(msg_len, title_len) + 2 + PADDING;
    if (box_len % 2 != 0) {
        box_len++;
    }
	if (box_len <= 12) {
		box_len = 14;
	}
    size_t title_spacing = (box_len - title_len)/2;
    size_t msg_spacing = (box_len - msg_len)/2;
    //printf("msg_len: %u\n", msg_len);
    //printf("title_len: %u\n", title_len);
    //printf("box_len: %u\n", box_len);
    //printf("title_spacing: %u\n", title_spacing);
    //printf("msg_spacing: %u\n", msg_spacing);
    if (msg_len == 0) {
        printf("Please enter a message\n");
        return;
    }
    if (title_len == 0) {
        printf("Please enter a title\n");
        return;
    }
    
    printf(" ");
    for (int i = 1; i < box_len-1; i++) {
        printf("_");
    }
    printf("\n");

    empty_line(box_len);
    string_line(title, title_spacing, (title_len % 2) != 0);

    printf("|");
    for (int i = 1; i < box_len-1; i++) {
        printf("_");
    }
    printf("|\n");

    empty_line(box_len);
    string_line(msg, msg_spacing, (msg_len % 2) != 0);
    empty_line(box_len);
    make_button(box_len);
    empty_line(box_len);
    printf("|");
    for (int i = 1; i < box_len-1; i++) {
        printf("_");
    }
    printf("|\n");

}
