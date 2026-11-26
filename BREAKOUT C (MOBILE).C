#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <math.h>
#include <stdbool.h>

/* Reduzido levemente para caber bem em telas de celular em pé */
#define LARGURA 40
#define ALTURA 18
#define LINHAS_BLOCOS 4
#define LARGURA_RAQUETE 7
#define ATRASO_US 45000

#define RESET    "\033[0m"
#define VERMELHO "\033[91m"
#define VERDE    "\033[92m"
#define AMARELO  "\033[93m"
#define CIANO    "\033[96m"
#define NEGRITO  "\033[1m"

#define VELOCIDADE_BOLA 0.40f
#define MAX_BOLAS 10  /* Limite máximo de bolas simultâneas */

typedef struct {
    float x, y;
    float dx, dy;
    int ativa;        /* 1 se a bola estiver em jogo, 0 se estiver inativa */
} Bola;

typedef struct {
    int x;
    int largura;
} Raquete;

typedef struct {
    int pontos;
    int vidas;
    int fase;
    int pausado;
    int encerrado;
    int multiplicador_ativado; /* Controla se a recompensa já foi concedida */
} EstadoJogo;

static char tela[ALTURA][LARGURA];
static int blocos[LINHAS_BLOCOS][LARGURA];
static Bola bolas[MAX_BOLAS];
static Raquete raquete;
static EstadoJogo jogo;

static struct termios terminal_original;
static int flags_originais;
static int terminal_configurado = 0;

void restaurar_terminal(void) {
    if (terminal_configurado) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal_original);
        fcntl(STDIN_FILENO, F_SETFL, flags_originais);
        printf("\033[0m\033[?25h\n");
        fflush(stdout);
        terminal_configurado = 0;
    }
}

int configurar_terminal(void) {
    if (tcgetattr(STDIN_FILENO, &terminal_original) == -1)
        return 0;

    flags_originais = fcntl(STDIN_FILENO, F_GETFL);

    struct termios raw = terminal_original;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    if (flags_originais != -1) {
        fcntl(STDIN_FILENO, F_SETFL, flags_originais | O_NONBLOCK);
    }

    terminal_configurado = 1;
    atexit(restaurar_terminal);
    printf("\033[?25l");

    return 1;
}

int ler_tecla(void) {
    unsigned char tecla;
    ssize_t n = read(STDIN_FILENO, &tecla, 1);
    return (n == 1) ? tecla : -1;
}

void definir_direcao_bola(Bola *b, float dx, float dy) {
    float comprimento = sqrtf(dx * dx + dy * dy);

    if (comprimento < 0.0001f) {
        b->dx = 0.0f;
        b->dy = -VELOCIDADE_BOLA;
        return;
    }

    b->dx = (dx / comprimento) * VELOCIDADE_BOLA;
    b->dy = (dy / comprimento) * VELOCIDADE_BOLA;
}

void reiniciar_bola(void) {
    /* Desativa todas as bolas */
    for (int i = 0; i < MAX_BOLAS; i++) {
        bolas[i].ativa = 0;
    }

    /* Ativa apenas a primeira bola principal */
    bolas[0].x = LARGURA / 2.0f;
    bolas[0].y = ALTURA - 4;
    bolas[0].ativa = 1;

    float direcao = (rand() % 2 == 0) ? -0.6f : 0.6f;
    definir_direcao_bola(&bolas[0], direcao, -1.0f);
}

void multiplicar_bolas(void) {
    int idx_origem = -1;
    for (int i = 0; i < MAX_BOLAS; i++) {
        if (bolas[i].ativa) {
            idx_origem = i;
            break;
        }
    }

    if (idx_origem == -1) return;

    /* Adiciona 2 novas bolas com trajetórias espalhadas */
    float angulos[] = {-0.8f, 0.8f};
    int ativadas = 0;

    for (int i = 0; i < MAX_BOLAS && ativadas < 2; i++) {
        if (!bolas[i].ativa) {
            bolas[i].x = bolas[idx_origem].x;
            bolas[i].y = bolas[idx_origem].y;
            definir_direcao_bola(&bolas[i], angulos[ativadas], -1.0f);
            bolas[i].ativa = 1;
            ativadas++;
        }
    }
}

void montar_blocos(void) {
    for (int i = 0; i < LINHAS_BLOCOS; i++) {
        for (int j = 1; j < LARGURA - 1; j++) {
            blocos[i][j] = ((j + i * 2) % 4 != 0);
        }
    }
}

int contar_blocos(void) {
    int total = 0;
    for (int i = 0; i < LINHAS_BLOCOS; i++) {
        for (int j = 1; j < LARGURA - 1; j++) {
            total += blocos[i][j];
        }
    }
    return total;
}

void inicializar_jogo(void) {
    jogo.pontos = 0;
    jogo.vidas = 3;
    jogo.fase = 1;
    jogo.pausado = 0;
    jogo.encerrado = 0;
    jogo.multiplicador_ativado = 0;

    raquete.largura = LARGURA_RAQUETE;
    raquete.x = (LARGURA - raquete.largura) / 2;

    montar_blocos();
    reiniciar_bola();
}

void processar_tecla(int tecla) {
    switch (tecla) {
        case 'a': case 'A':
            raquete.x -= 3;
            break;

        case 'd': case 'D':
            raquete.x += 3;
            break;

        case 'p': case 'P':
            jogo.pausado = !jogo.pausado;
            break;

        case 'm': case 'M': case 's': case 'S':
            jogo.encerrado = 1;
            break;
        default: 
            break;
    }
        
    if (raquete.x < 1)
        raquete.x = 1;

    if (raquete.x + raquete.largura > LARGURA - 1)
        raquete.x = LARGURA - 1 - raquete.largura;
}

int atualizar_bola_individual(Bola *b) {
    if (!b->ativa) return 0;

    float nx = b->x + b->dx;
    float ny = b->y + b->dy;

    int px = (int)(nx + 0.5f);
    int py = (int)(ny + 0.5f);

    /* Paredes */
    if (px <= 0 || px >= LARGURA - 1) {
        b->dx = -b->dx;
        nx = b->x + b->dx;
        px = (int)(nx + 0.5f);
    }

    /* Teto */
    if (py <= 0) {
        b->dy = fabsf(b->dy);
        ny = b->y + b->dy;
        py = (int)(ny + 0.5f);
    }

    /* Blocos */
    if (py >= 2 && py < LINHAS_BLOCOS + 2 && px > 0 && px < LARGURA - 1) {
        int bi = py - 2;
        int bj = px;

        if (blocos[bi][bj]) {
            blocos[bi][bj] = 0;
            jogo.pontos += 10;
            b->dy = -b->dy;
            ny = b->y + b->dy;
            py = (int)(ny + 0.5f);
        }
    }

    /* Raquete */
    int linha_raquete = ALTURA - 2;

    if (b->dy > 0 && py >= linha_raquete && (int)(b->y + 0.5f) < linha_raquete) {
        if (px >= raquete.x && px < raquete.x + raquete.largura) {
            float centro = raquete.x + raquete.largura / 2.0f;
            float distancia = (px - centro) / (raquete.largura / 2.0f);
            definir_direcao_bola(b, distancia * 1.5f, -1.0f);
            ny = linha_raquete - 0.6f;
        }
    }

    b->x = nx;
    b->y = ny;

    /* Bola caiu no fundo da tela */
    if (b->y >= ALTURA - 1) {
        b->ativa = 0;
        return 1;
    }

    return 0;
}

void atualizar_jogo(void) {
    if (jogo.pausado || jogo.encerrado) return;

    /* Ativa o multiplicador quando o jogador atinge 100 pontos */
    if (jogo.pontos >= 100 && !jogo.multiplicador_ativado) {
        jogo.multiplicador_ativado = 1;
        multiplicar_bolas();
    }
    
    int bolas_ativas = 0;

    for (int i = 0; i < MAX_BOLAS; i++) {
        if (bolas[i].ativa) {
            atualizar_bola_individual(&bolas[i]);
            if (bolas[i].ativa) {
                bolas_ativas++;
            }
        }
    }

    /* Perda de vida somente se TODAS as bolas caírem */
    if (bolas_ativas == 0) {
        jogo.vidas--;

        if (jogo.vidas <= 0) {
            jogo.vidas = 0;
            jogo.encerrado = 1;
        } else {
            jogo.multiplicador_ativado = 0;
            reiniciar_bola();
            usleep(350000);
        }
    }

    /* Próxima fase */
    if (contar_blocos() == 0) {
        jogo.fase++;
        jogo.vidas++;
        jogo.multiplicador_ativado = 0;
        montar_blocos();
        reiniciar_bola();
        usleep(500000);
    }
}

void preparar_tela(void) {
    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            tela[i][j] = ' ';
        }
    }

    for (int j = 0; j < LARGURA; j++) {
        tela[0][j] = '-';
        tela[ALTURA - 1][j] = '-';
    }

    for (int i = 0; i < ALTURA; i++) {
        tela[i][0] = '|';
        tela[i][LARGURA - 1] = '|';
    }

    for (int i = 0; i < LINHAS_BLOCOS; i++) {
        for (int j = 1; j < LARGURA - 1; j++) {
            if (blocos[i][j])
                tela[i + 2][j] = '#';
        }
    }

    for (int j = 0; j < raquete.largura; j++)
        tela[ALTURA - 2][raquete.x + j] = '=';

    /* Renderiza todas as bolas ativas */
    for (int i = 0; i < MAX_BOLAS; i++) {
        if (bolas[i].ativa) {
            int bx = (int)(bolas[i].x + 0.5f);
            int by = (int)(bolas[i].y + 0.5f);

            if (bx > 0 && bx < LARGURA - 1 && by > 0 && by < ALTURA - 1) {
                tela[by][bx] = 'O';
            }
        }
    }
}

void desenhar(void) {
    preparar_tela();

    printf("\033[H");
    printf(NEGRITO CIANO "==== BREAKOUT C (MOBILE) ====\n" RESET);
    printf(" F:%d | Pts:%d | Vidas:%d\n", jogo.fase, jogo.pontos, jogo.vidas);
    printf(" A/D: Mover | P: Pausar | Q: Sair\n\n");

    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            char c = tela[i][j];

            if (c == '#')
                printf("%s%c%s", AMARELO, c, RESET);
            else if (c == '=')
                printf("%s%c%s", VERDE, c, RESET);
            else if (c == 'O')
                printf("%s%c%s", VERMELHO, c, RESET);
            else
                putchar(c);
        }
        putchar('\n');
    }

    if (jogo.pausado)
        printf("\n" AMARELO NEGRITO " PAUSADO (P para continuar)" RESET "\n");

    fflush(stdout);
}

void mostrar_resultado(void) {
    printf("\033[2J\033[H");

    if (jogo.vidas == 0) {
        printf(VERMELHO NEGRITO "\n    FIM DE JOGO!\n" RESET);
    } else {
        printf(CIANO NEGRITO "\n    JOGO ENCERRADO\n" RESET);
    }

    printf("\nPontos: %d\nFase: %d\n\n", jogo.pontos, jogo.fase);
}

int main(void) {
    srand((unsigned int)time(NULL));

    inicializar_jogo();
    configurar_terminal();

    printf("\033[2J\033[H");

    while (!jogo.encerrado) {
        int tecla;

        while ((tecla = ler_tecla()) != -1)
            processar_tecla(tecla);

        atualizar_jogo();
        desenhar();
        usleep(ATRASO_US);
    }

    restaurar_terminal();
    mostrar_resultado();

    return EXIT_SUCCESS;
}
    
