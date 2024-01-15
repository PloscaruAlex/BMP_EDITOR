#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp_header.h"

#define LUNGIME 50
#define NR_CUV 8
#define MAXW 800
#define MAXH 800
#define BITS 24
#define BYTES 32
#define ZECE 10
#define CINCI 5
#define SASE 6
#define SAPTE 7

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} BGR;

void desenare(int x, int y, BGR **matimg, int height, int width, BGR culoare, int linew) {
    int startx = x - linew;
    int starty = y - linew;
    int stopx = x + linew;
    int stopy = y + linew;
    if (startx < 0) {
        startx = 0;
    }
    if (starty < 0) {
        starty = 0;
    }
    if (stopx > height - 1) {
        stopx = height - 1;
    }
    if (stopy > width - 1) {
        stopy = width - 1;
    }
    for (int i = startx; i <= stopx; i++) {
        for (int j = starty; j <= stopy; j++) {
            matimg[i][j] = culoare;
        }
    }
}

void drawlinex(int x1, int y1, int x2, int y2, BGR **matimg, int height, int width, BGR culoare, int linew) {
    int d = 0, x = 0, y = 0;
    int dy = y2 - y1;
    int dx = x2 - x1;
    if (dy <= dx) {
        d = dy - dx;
        x = x1;
        y = y1;
        desenare(x1, y1, matimg, height, width, culoare, linew);
        while (x < x2) {
            x++;
            if (d < 0) {
                d = d + dy;
            } else {
                d = d + dy - dx;
                y++;
            }
            desenare(x, y, matimg, height, width, culoare, linew);
        }
    } else if (dx < dy) {
        d = dx - dy;
        x = x1;
        y = y1;
        desenare(x1, y1, matimg, height, width, culoare, linew);
        while (y < y2) {
            y++;
            if (d < 0) {
                d = d + dx;
            } else {
                d = d + dx - dy;
                x++;
            }
            desenare(x, y, matimg, height, width, culoare, linew);
        }
    }
}

void drawliney(int x1, int y1, int x2, int y2, BGR **matimg, int height, int width, BGR culoare, int linew) {
    int d = 0, x = 0, y = 0;
    int dy = y2 - y1;
    int dx = x2 - x1;
    if (dy <= dx) {
        d = dy - dx;
        x = x1;
        y = y1;
        desenare(x1, y1, matimg, height, width, culoare, linew);
        while (x < x2) {
            x++;
            if (d < 0) {
                d = d + dy;
            } else {
                d = d + dy - dx;
                y++;
            }
            desenare(2 * x1 - x, y, matimg, height, width, culoare, linew);
        }
    } else if (dx < dy) {
        d = dx - dy;
        x = x1;
        y = y1;
        desenare(x1, y1, matimg, height, width, culoare, linew);
        while (y < y2) {
            y++;
            if (d < 0) {
                d = d + dx;
            } else {
                d = d + dx - dy;
                x++;
            }
            desenare(2 * x1 - x, y, matimg, height, width, culoare, linew);
        }
    }
}

void umplere(int x, int y, BGR **matimg, int height, int width, BGR culoare, BGR original) {
    if (matimg[x][y].blue == original.blue &&
        matimg[x][y].green == original.green &&
        matimg[x][y].red == original.red) {
            matimg[x][y] = culoare;

            if (x != 0) {
                umplere(x - 1, y, matimg, height, width, culoare, original);
            }
            if (x != height - 1) {
                umplere(x + 1, y, matimg, height, width, culoare, original);
            }
            if (y != 0) {
                umplere(x, y - 1, matimg, height, width, culoare, original);
            }
            if (y != width - 1) {
                umplere(x, y + 1, matimg, height, width, culoare, original);
            }
        }
}

int main() {
    bmp_fileheader fileH;
    bmp_infoheader infoH;
    int rowsize = 0;
    unsigned int width = 0, height = 0;;

    BGR **matimg = malloc(MAXH * sizeof(BGR*));
    for (int i = MAXH - 1; i >= 0; i--) {
        matimg[i] = calloc(MAXW, sizeof(BGR));
    }

    char *instructiune = calloc(LUNGIME, sizeof(char));
    char *copie = calloc(LUNGIME, sizeof(char));

    BGR culoareLinie;
    culoareLinie.blue = 0;
    culoareLinie.green = 0;
    culoareLinie.red = 0;
    int widthLinie = 1;
    do {
        fgets(instructiune, LUNGIME, stdin);
        strncpy(copie, instructiune, strlen(instructiune));

        char **cuvToken = malloc(NR_CUV * sizeof(char *));
        for (int i = 0; i < NR_CUV; ++i) {
            cuvToken[i] = calloc(LUNGIME, sizeof(char));
        }

        char *token = calloc(LUNGIME, sizeof(char));
        token = strtok(copie, " \n");
        int contor = 0;
        while (token != NULL) {
            strncpy(cuvToken[contor], token, strlen(token));
            token = strtok(NULL, " \n");
            contor++;
        }

        if (strcmp(cuvToken[0], "edit") == 0) {
            FILE *img = fopen(cuvToken[1], "rb");

            fread(&fileH, sizeof(bmp_fileheader), 1, img);
            fread(&infoH, sizeof(bmp_infoheader), 1, img);

            rowsize = ((BITS * infoH.width + BYTES - 1) / BYTES) * 4;
            width = (unsigned int)infoH.width;
            height = (unsigned int)infoH.height;
            fseek(img, fileH.imageDataOffset, SEEK_SET);
            for (int i = infoH.height - 1; i >= 0; i--) {
                fread(matimg[i], sizeof(unsigned char), rowsize, img);
            }

            fclose(img);
        } else if (strcmp(cuvToken[0], "insert") == 0) {
            FILE *insertimg = fopen(cuvToken[1], "rb");
            bmp_fileheader insertfileH;
            bmp_infoheader insertinfoH;
            fread(&insertfileH, sizeof(bmp_fileheader), 1, insertimg);
            fread(&insertinfoH, sizeof(bmp_infoheader), 1, insertimg);

            int insertrowsize = ((BITS * insertinfoH.width + BYTES - 1) / BYTES) * 4;
            unsigned int insertwidth = (unsigned int)insertinfoH.width;
            unsigned int insertheight = (unsigned int)insertinfoH.height;
            unsigned int x = (unsigned int)strtoul(cuvToken[3], NULL, ZECE);
            unsigned int y = (unsigned int)strtoul(cuvToken[2], NULL, ZECE);
            unsigned int copiey = y;

            BGR **insertbitmap = malloc(insertinfoH.height * sizeof(BGR*));
            for (int i = 0; i < insertinfoH.height; i++) {
                insertbitmap[i] = calloc(insertrowsize, sizeof(unsigned char));
            }

            fseek(insertimg, insertfileH.imageDataOffset, SEEK_SET);
            for (int i = insertinfoH.height - 1; i >= 0; i--) {
                fread(insertbitmap[i], sizeof(unsigned char), insertrowsize, insertimg);
            }

            if (insertheight > height - x) {
                insertheight = height - x;
            }
            if (insertwidth > width - y) {
                insertwidth = width - y;
            }
            if (x < height && y < width) {
                x = height - 1 - x;
                for (int i = insertinfoH.height - 1; i >= insertinfoH.height - (int)insertheight; i--) {
                    for (int j = 0; j < (int)insertwidth; j++) {
                        matimg[x][y] = insertbitmap[i][j];
                        y++;
                    }
                    x--;
                    y = copiey;
                }
            }

            free(insertbitmap);
            fclose(insertimg);
        } else if (strcmp(cuvToken[0], "set") == 0) {
            if (strcmp(cuvToken[1], "draw_color") == 0) {
                culoareLinie.red = (unsigned char)strtoul(cuvToken[2], NULL, ZECE);
                culoareLinie.green = (unsigned char)strtoul(cuvToken[3], NULL, ZECE);
                culoareLinie.blue = (unsigned char)strtoul(cuvToken[4], NULL, ZECE);
            } else if (strcmp(cuvToken[1], "line_width") == 0) {
                widthLinie = (int)strtoul(cuvToken[2], NULL, ZECE);
                widthLinie = (widthLinie - 1) / 2;
            }
        } else if (strcmp(cuvToken[0], "draw") == 0) {
            if (strcmp(cuvToken[1], "line") == 0) {
                int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
                y1 = (int)strtoul(cuvToken[2], NULL, ZECE);
                x1 = infoH.height - 1 - (int)strtoul(cuvToken[3], NULL, ZECE);
                y2 = (int)strtoul(cuvToken[4], NULL, ZECE);
                x2 = infoH.height - 1 - (int)strtoul(cuvToken[CINCI], NULL, ZECE);

                int orizontal = 0;  // dreapta = 0 si stanga = 1
                int vertical = 0;  // jos = 0 si sus = 1
                if (x1 > x2) {
                    vertical = 1;
                }
                if (y1 > y2) {
                    orizontal = 1;
                }

                if (orizontal == 0 && vertical == 0) {
                    drawlinex(x1, y1, x2, y2, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (orizontal == 1 && vertical == 1) {
                    drawlinex(x2, y2, x1, y1, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (orizontal == 1 && vertical == 0) {
                    drawliney(x2, y2, 2 * x2 - x1, y1, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (orizontal == 0 && vertical == 1) {
                    drawliney(x1, y1, 2 * x1 - x2, y2, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                }
            } else if (strcmp(cuvToken[1], "rectangle") == 0) {
                int y1 = 0, x1 = 0, w = 0, h = 0;
                y1 = (int)strtoul(cuvToken[2], NULL, ZECE);
                x1 = infoH.height - 1 - (int)strtoul(cuvToken[3], NULL, ZECE);
                w = (int)strtoul(cuvToken[4], NULL, ZECE);
                h = (int)strtoul(cuvToken[CINCI], NULL, ZECE);

                drawlinex(x1 - h, y1, x1, y1, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                drawlinex(x1 - h, y1, x1 - h, y1 + w, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                drawlinex(x1 - h, y1 + w, x1, y1 + w, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                drawlinex(x1, y1, x1, y1 + w, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
            } else if (strcmp(cuvToken[1], "triangle") == 0) {
                int y1 = 0, x1 = 0, y2 = 0, x2 = 0, y3 = 0, x3 = 0;
                y1 = (int)strtoul(cuvToken[2], NULL, ZECE);
                x1 = infoH.height - 1 - (int)strtoul(cuvToken[3], NULL, ZECE);
                y2 = (int)strtoul(cuvToken[4], NULL, ZECE);
                x2 = infoH.height - 1 - (int)strtoul(cuvToken[CINCI], NULL, ZECE);
                y3 = (int)strtoul(cuvToken[SASE], NULL, ZECE);
                x3 = infoH.height - 1 - (int)strtoul(cuvToken[SAPTE], NULL, ZECE);

                int oriz1 = 0, oriz2 = 0, oriz3 = 0;  // dreapta = 0 si stanga = 1
                int vert1 = 0, vert2 = 0, vert3 = 0;  // jos = 0 si sus = 1

                if (x1 > x2) {
                    vert1 = 1;
                }
                if (y1 > y2) {
                    oriz1 = 1;
                }
                if (x2 > x3) {
                    vert2 = 1;
                }
                if (y2 > y3) {
                    oriz2 = 1;
                }
                if (x3 > x1) {
                    vert3 = 1;
                }
                if (y3 > y1) {
                    oriz3 = 1;
                }

                if (oriz1 == 0 && vert1 == 0) {
                    drawlinex(x1, y1, x2, y2, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (oriz1 == 1 && vert1 == 1) {
                    drawlinex(x2, y2, x1, y1, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (oriz1 == 1 && vert1 == 0) {
                    drawliney(x2, y2, 2 * x2 - x1, y1, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (oriz1 == 0 && vert1 == 1) {
                    drawliney(x1, y1, 2 * x1 - x2, y2, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                }

                if (oriz2 == 0 && vert2 == 0) {
                    drawlinex(x2, y2, x3, y3, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (oriz2 == 1 && vert2 == 1) {
                    drawlinex(x3, y3, x2, y2, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (oriz2 == 1 && vert2 == 0) {
                    drawliney(x3, y3, 2 * x3 - x2, y2, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (oriz2 == 0 && vert2 == 1) {
                    drawliney(x2, y2, 2 * x2 - x3, y3, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                }

                if (oriz3 == 0 && vert3 == 0) {
                    drawlinex(x3, y3, x1, y1, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (oriz3 == 1 && vert3 == 1) {
                    drawlinex(x1, y1, x3, y3, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (oriz3 == 1 && vert3 == 0) {
                    drawliney(x1, y1, 2 * x1 - x3, y3, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                } else if (oriz3 == 0 && vert3 == 1) {
                    drawliney(x3, y3, 2 * x3 - x1, y1, matimg, infoH.height, infoH.width, culoareLinie, widthLinie);
                }
            }
        } else if (strcmp(cuvToken[0], "fill") == 0) {
            int y = (int)strtoul(cuvToken[1], NULL, ZECE);
            int x = infoH.height - 1 - (int)strtoul(cuvToken[2], NULL, ZECE);

            BGR original = matimg[x][y];
            umplere(x, y, matimg, infoH.height, infoH.width, culoareLinie, original);
        } else if (strcmp(cuvToken[0], "save") == 0) {
            FILE *myimg = fopen(cuvToken[1], "wb");

            fwrite(&fileH, sizeof(bmp_fileheader), 1, myimg);
            fwrite(&infoH, sizeof(bmp_infoheader), 1, myimg);

            fseek(myimg, fileH.imageDataOffset, SEEK_SET);
            for (int i = infoH.height - 1; i >= 0; i--) {
                fwrite(matimg[i], sizeof(unsigned char), rowsize, myimg);
            }

            fclose(myimg);
        }

        free(token);
        free(cuvToken);
    } while (strcmp(instructiune, "quit\n") != 0);
    free(instructiune);
    free(copie);
    free(matimg);

    return 0;
}
