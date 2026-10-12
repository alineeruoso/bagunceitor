/*

Para compilar de dentro do VS Code:

Windows/Linux: CTRL+SHIFT+B (Terminal -> Run Build Task)
macOS: COMMAND+SHIFT+B

Para executar de dentro do VS Code:

Executar normalmente: CTRL+F5 (Run -> Run Without Debugging)
Debugar: F5 (Run -> Start Debugging)

Pelo terminal:

Windows: mingw32-make
Linux/macOS: make

Para executar:

./bagunceitor [arquivo com a imagem de entrada]

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Para usar strings
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>
#include <stb_image_write.h>

#define CONSTANTE_XOR_CONFUSAO 215

// Um pixel Pixel (24 bits)
typedef struct {
  unsigned char r, g, b;
} Pixel;

// Uma imagem Pixel
typedef struct {
  int width, height;  // largura, altura
  int channels;       // qtd de canais (geralmente 3, RGB)
  Pixel* pixels;
} Img;

// As 2 imagens
Img in, out;

// Protótipos
void load(char* name, Img* pic);
void confusao(Img* pic_in, Img* pic_out, Pixel(*pin)[in.width], Pixel(*pout)[in.width]);
void inverte_confusao(Img* pic_in, Img* pic_out, Pixel(*pin)[in.width], Pixel(*pout)[in.width]);
void difusao(Img* pic_in, Img* pic_out, Pixel(*pin)[in.width], Pixel(*pout)[in.width], int *horizontal, int *vertical);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("bagunceitor [origem]\n");
    exit(1);
  }

  srand((unsigned)time(NULL));

  // Carrega a imagem original
  load(argv[1], &in);

  // Exibe as dimensões na tela, para conferência
  printf("Origem   : %s %d x %d\n", argv[1], in.width, in.height);

  printf("Processando...\n");

  // Cria imagem de saída e "zera" ela
  int tam = in.width * in.height;
  out = in;
  out.pixels = malloc(tam * sizeof(Pixel));
  memset(out.pixels, 0, tam * sizeof(Pixel));

  // Converte para interpretar como matrizes
  Pixel(*pin)[in.width] = (Pixel(*)[in.height])in.pixels;
  Pixel(*pout)[in.width] = (Pixel(*)[in.height])out.pixels;

  int *horizontal = malloc(in.height * sizeof *horizontal);
  int *vertical = malloc(in.width * sizeof *vertical);

  for (int i = 0; i < in.height; i++) {
    horizontal[i] = rand() % in.width;
  } 

  for (int i = 0; i < in.width; i++) {
    vertical[i] = rand() % in.height;
  } 

  confusao(&in, &out, pin, pout);
  inverte_confusao(&out, &out, pin, pout);

  difusao(&in, &out, pin, pout, horizontal, vertical);
  
  size_t total_bytes = (size_t)in.width * in.height * sizeof(Pixel);

  if (memcmp(in.pixels, out.pixels, total_bytes) == 0) {
    printf("OK: original and reverted output are exactly identical.\n");
  } else {
    printf("ERROR: original and reverted output differ.\n");
  }

  free(in.pixels);
  free(out.pixels);
  free(horizontal);
  free(vertical);
}

void confusao(Img* pic_in, Img* pic_out, Pixel(*pin)[in.width], Pixel(*pout)[in.width]) {
  for (int i = 0; i < pic_in->height; i++) {
    for (int j = 0; j < pic_in->width; j++) {
      pout[i][j].r = pin[i][j].g ^ CONSTANTE_XOR_CONFUSAO;
      pout[i][j].g = pin[i][j].b ^ CONSTANTE_XOR_CONFUSAO;
      pout[i][j].b = pin[i][j].r ^ CONSTANTE_XOR_CONFUSAO;
    }
  }

  // Grava a imagem como PNG para registro
  stbi_write_png("saida_confusao.png", pic_out->width, pic_out->height, 3, pout, 0);
}

void inverte_confusao(Img* pic_in, Img* pic_out, Pixel(*pin)[in.width], Pixel(*pout)[in.width]) {
  for (int i = 0; i < pic_out->height; i++) {
    for (int j = 0; j < pic_out->width; j++) {
      unsigned char r = pout[i][j].r;
      unsigned char g = pout[i][j].g;
      unsigned char b = pout[i][j].b;

      pout[i][j].r = b ^ CONSTANTE_XOR_CONFUSAO;
      pout[i][j].g = r ^ CONSTANTE_XOR_CONFUSAO;
      pout[i][j].b = g ^ CONSTANTE_XOR_CONFUSAO;
    }
  }

  // Grava a imagem como PNG para registro
  stbi_write_png("saida_confusao_revert.png", pic_out->width, pic_out->height, 3, pout, 0);
}

void difusao(Img* pic_in, Img* pic_out, Pixel(*pin)[in.width], Pixel(*pout)[in.width], int *horizontal, int *vertical) {
  Pixel (*temp)[in.width] = malloc(in.height * sizeof *temp);
  
  for (int y = 0; y < in.height; y++) {
    for (int x = 0; x < in.width; x++) {
      temp[y][(x + horizontal[y]) % in.width] = pin[y][x];
    }
  }

  for (int x = 0; x < in.width; x++) {
    for (int y = 0; y < in.height; y++) {
      pout[(y + vertical[x]) % in.height][x] = temp[y][x];
    }
  }

  free(temp);
  stbi_write_png("saida_difusao.png", out.width, out.height, 3, pout, 0);
}

void load(char* name, Img* pic) {
  pic->pixels =
      (Pixel*)stbi_load(name, &pic->width, &pic->height, &pic->channels, 0);
  if (!pic->pixels) {
    printf("Erro de leitura: %s\n", stbi_failure_reason());
    exit(1);
  }
  printf("Load: %d x %d x %d\n", pic->width, pic->height, pic->channels);
  // Exibe um bloco de 8 x 8 pixels em hexadecimal (teste)
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      printf("[%02X %02X %02X] ", pic->pixels[i].r, pic->pixels[i].g,
             pic->pixels[i].b);
    }
    printf("\n");
  }
}
