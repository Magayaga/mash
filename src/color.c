/*
 * Color.c (MCECS)
 * Copyright 2023-2024 Cyril John Magayaga
 *
 */

#include <stdio.h>

void colorText_red() {
        printf("\x1b[31m");
}

void colorText_orange() {
        printf("\x1b[38;5;208m");
}

void colorText_yellow() {
        printf("\x1b[33m");
}

void colorText_green() {
        printf("\x1b[32m");
}

void colorText_cyan() {
        printf("\x1b[36m");
}

void colorText_blue() {
        printf("\x1b[34m");
}

void colorText_pink() {
        printf("\x1b[38;5;218m");
}

void colorText_magenta() {
        printf("\x1b[35m");
}

void colorText_black() {
        printf("\x1b[30m");
}

void colorText_white() {
        printf("\x1b[37m");
}

void backgroundColor_black() {
        printf("\x1b[40m");
}

void backgroundColor_white() {
        printf("\x1b[47m");
}

void backgroundColor_red() {
        printf("\x1b[41m");
}

void backgroundColor_yellow() {
        printf("\x1b[43m");
}

void backgroundColor_green() {
        printf("\x1b[42m");
}

void backgroundColor_cyan() {
        printf("\x1b[46m");
}

void backgroundColor_blue() {
        printf("\x1b[44m");
}

void backgroundColor_magenta() {
        printf("\x1b[45m");
}


void reset() {
        printf("\x1b[0m");
}
