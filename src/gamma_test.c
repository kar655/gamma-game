/* Ten plik włączamy na początku i dwa razy, aby sprawdzić, czy zawiera
 * wszystko, co jest potrzebne. */
#include "gamma.h"
#include "gamma.h"

/* CMake w wersji release wyłącza asercje. */
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/** FUNKCJE POMOCNE PRZY DEBUGOWANIU TESTÓW **/

#if 0

#include <stdio.h>
#include <inttypes.h>

static inline void print_board(gamma_t *g) {
  char *board = gamma_board(g);
  assert(board);
  printf(board);
  free(board);
}

static inline void print_players(gamma_t *g,
                                 uint32_t from_player, uint32_t to_player) {
  assert(from_player >= 1);
  for (uint32_t player = from_player - 1; player++ < to_player;)
    printf("player = %" PRIu32 ", busy = %" PRIu64 ", free = %" PRIu64
           ", golden = %d\n",
           player,
           gamma_busy_fields(g, player),
           gamma_free_fields(g, player),
           gamma_golden_possible(g, player));
}

#endif

/** KONFIGUARACJA TESTÓW **/

/* Nie było wymagania, aby ustawiać errno, więc w wersji docelowej nie
 * będziemy tego sprawdzać. */
static const bool test_errno = false;

/* Możliwe wyniki testu */
#define PASS 0
#define FAIL 1
#define WRONG_TEST 2
#define MEM_PASS 13

/* Liczba elementów tablicy x */
#define SIZE(x) (sizeof(x) / sizeof(x)[0])

/* Różne rozmiary planszy */
#define SMALL_BOARD_SIZE    10
#define MIDDLE_BOARD_SIZE  100
#define BIG_BOARD_SIZE    1000

#define MANY_GAMES 42

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t players;
    uint32_t areas;
} gamma_param_t;

/** WŁAŚCIWE TESTY **/

/* Testuje opublikowany przykład użycia. */
static int example(void) {
    static const char board[] =
            "1.........\n"
            "..........\n"
            "..........\n"
            "......2...\n"
            ".....2....\n"
            "..........\n"
            "..........\n"
            "1.........\n"
            "1221......\n"
            "1.........\n";

    gamma_t *g;

    g = gamma_new(0, 0, 0, 0);
    assert(g == NULL);

    g = gamma_new(10, 10, 2, 3);
    assert(g != NULL);

    assert(gamma_move(g, 1, 0, 0));
    assert(gamma_busy_fields(g, 1) == 1);
    assert(gamma_busy_fields(g, 2) == 0);
    assert(gamma_free_fields(g, 1) == 99);
    assert(gamma_free_fields(g, 2) == 99);
    assert(!gamma_golden_possible(g, 1));
    assert(gamma_move(g, 2, 3, 1));
    assert(gamma_busy_fields(g, 1) == 1);
    assert(gamma_busy_fields(g, 2) == 1);
    assert(gamma_free_fields(g, 1) == 98);
    assert(gamma_free_fields(g, 2) == 98);
    assert(gamma_move(g, 1, 0, 2));
    assert(gamma_move(g, 1, 0, 9));
    assert(!gamma_move(g, 1, 5, 5));
    assert(gamma_free_fields(g, 1) == 6);
    assert(gamma_move(g, 1, 0, 1));
    assert(gamma_free_fields(g, 1) == 95);
    assert(gamma_move(g, 1, 5, 5));
    assert(!gamma_move(g, 1, 6, 6));
    assert(gamma_busy_fields(g, 1) == 5);
    assert(gamma_free_fields(g, 1) == 10);
    assert(gamma_move(g, 2, 2, 1));
    assert(gamma_move(g, 2, 1, 1));
    assert(gamma_free_fields(g, 1) == 9);
    assert(gamma_free_fields(g, 2) == 92);
    assert(!gamma_move(g, 2, 0, 1));
    assert(gamma_golden_possible(g, 2));
    assert(!gamma_golden_move(g, 2, 0, 1));
    assert(gamma_golden_move(g, 2, 5, 5));
    assert(!gamma_golden_possible(g, 2));
    assert(gamma_move(g, 2, 6, 6));
    assert(gamma_busy_fields(g, 1) == 4);
    assert(gamma_free_fields(g, 1) == 91);
    assert(gamma_busy_fields(g, 2) == 5);
    assert(gamma_free_fields(g, 2) == 13);
    assert(gamma_golden_move(g, 1, 3, 1));
    assert(gamma_busy_fields(g, 1) == 5);
    assert(gamma_free_fields(g, 1) == 8);
    assert(gamma_busy_fields(g, 2) == 4);
    assert(gamma_free_fields(g, 2) == 10);

    char *p = gamma_board(g);
    assert(p);
    assert(strcmp(p, board) == 0);
    free(p);

    gamma_delete(g);
    return PASS;
}

/* Testuje najmniejszą możliwą grę. */
static int minimal(void) {
    gamma_t *g = gamma_new(1, 1, 1, 1);
    assert(g != NULL);
    assert(gamma_move(g, 1, 0, 0));
    assert(gamma_busy_fields(g, 1) == 1);
    assert(gamma_free_fields(g, 1) == 0);
    gamma_delete(g);
    return PASS;
}

/* Testuje sprawdzanie poprawności parametrów poszczególnych funkcji. */
static int params(void) {
    assert(gamma_new(0, 10, 2, 2) == NULL);
    assert(gamma_new(10, 0, 2, 2) == NULL);
    assert(gamma_new(10, 10, 0, 2) == NULL);
    assert(gamma_new(10, 10, 2, 0) == NULL);

    gamma_t *g = gamma_new(10, 10, 2, 2);

    assert(!gamma_move(NULL, 1, 5, 5));
    assert(!gamma_move(g, 0, 5, 5));
    assert(!gamma_move(g, 3, 5, 5));
    assert(!gamma_move(g, UINT32_MAX, 5, 5));
    assert(!gamma_move(g, 1, 10, 5));
    assert(!gamma_move(g, 1, UINT32_MAX, 5));
    assert(!gamma_move(g, 1, 5, 10));
    assert(!gamma_move(g, 1, 5, UINT32_MAX));

    assert(!gamma_golden_move(NULL, 1, 5, 5));
    assert(!gamma_golden_move(g, 0, 5, 5));
    assert(!gamma_golden_move(g, 3, 5, 5));
    assert(!gamma_golden_move(g, UINT32_MAX, 5, 5));
    assert(!gamma_golden_move(g, 1, 10, 5));
    assert(!gamma_golden_move(g, 1, UINT32_MAX, 5));
    assert(!gamma_golden_move(g, 1, 5, 10));
    assert(!gamma_golden_move(g, 1, 5, UINT32_MAX));

    assert(gamma_busy_fields(NULL, 1) == 0);
    assert(gamma_busy_fields(g, 0) == 0);
    assert(gamma_busy_fields(g, 3) == 0);
    assert(gamma_busy_fields(g, UINT32_MAX) == 0);

    assert(gamma_free_fields(NULL, 1) == 0);
    assert(gamma_free_fields(g, 0) == 0);
    assert(gamma_free_fields(g, 3) == 0);
    assert(gamma_free_fields(g, UINT32_MAX) == 0);

    assert(gamma_golden_possible(NULL, 1) == 0);
    assert(gamma_golden_possible(g, 0) == 0);
    assert(gamma_golden_possible(g, 3) == 0);
    assert(gamma_golden_possible(g, UINT32_MAX) == 0);

    assert(gamma_board(NULL) == NULL);

    gamma_delete(g);
    return PASS;
}

/* Testuje, czy nie ma problemów przy wypisywaniu planszy w grze
 * z dużą liczbą graczy. */
static int many_players(void) {
    gamma_t *g = gamma_new(MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE,
                           MIDDLE_BOARD_SIZE * MIDDLE_BOARD_SIZE, 1);
    assert(g != NULL);

    for (uint32_t x = 0; x < MIDDLE_BOARD_SIZE; ++x)
        for (uint32_t y = 0; y < MIDDLE_BOARD_SIZE; ++y)
            assert(gamma_move(g, x * MIDDLE_BOARD_SIZE + y + 1, x, y));

    char *board = gamma_board(g);
    assert(board != NULL);
    free(board);

    gamma_delete(g);
    return PASS;
}

/* Testuje, czy można rozgrywać równocześnie więcej niż jedną grę. */
static int many_games(void) {
    static const gamma_param_t game[] = {
            {7,  9,  2, 4},
            {11, 21, 2, 4},
            {47, 3,  2, 4},
            {2,  99, 2, 2},
    };
    static const uint64_t free1[SIZE(game)] = { 8, 8, 6, 4 };

    static gamma_t *g[MANY_GAMES][SIZE(game)];

    for (size_t i = 0; i < MANY_GAMES; ++i) {
        for (size_t j = 0; j < SIZE(game);
        ++j) {
            g[i][j] = gamma_new(game[j].width, game[j].height,
                                game[j].players, game[j].areas);
            assert(g[i][j]);
        }
    }

    for (size_t i = 0; i < MANY_GAMES; ++i) {
        for (size_t j = 0; j < SIZE(game);
        ++j) {
            assert(gamma_move(g[i][j], 1, 0, 0));
            assert(gamma_move(g[i][j], 1, 0, game[j].height - 1));
            assert(gamma_move(g[i][j], 1, game[j].width - 1, 0));
            assert(gamma_move(g[i][j], 1, game[j].width - 1, game[j].height - 1));
            assert(!gamma_move(g[i][j], 1, game[j].width, game[j].height - 1));
            assert(!gamma_move(g[i][j], 1, game[j].width - 1, game[j].height));
        }
    }

    for (size_t i = 0; i < MANY_GAMES; ++i) {
        for (size_t j = 0; j < SIZE(game);
        ++j) {
            assert(gamma_busy_fields(g[i][j], 1) == 4);
            assert(gamma_free_fields(g[i][j], 1) == free1[j]);
            assert(gamma_busy_fields(g[i][j], 2) == 0);
            uint64_t size = (uint64_t) game[j].width * (uint64_t) game[j].height;
            assert(gamma_free_fields(g[i][j], 2) == size - 4);
        }
    }

    for (size_t i = 0; i < MANY_GAMES; ++i)
        for (size_t j = 0; j < SIZE(game);
    ++j)
    gamma_delete(g[i][j]);

    return PASS;
}

/* Testuje, czy gamma_delete wywołane z parametrem NULL nic nie robi. */
static int delete_null(void) {
    gamma_delete(NULL);
    return PASS;
}

/* Uruchamia kilka krótkich testów poprawności wykonywania zwykłych ruchów oraz
 * obliczania liczby zajętych i wolnych pól po wykonaniu zwykłego ruchu. */
static int normal_move(void) {
    gamma_t *g = gamma_new(2, 2, 2, 2);
    assert(g != NULL);

    assert(gamma_move(g, 1, 0, 0));
    assert(gamma_move(g, 2, 1, 0));
    assert(gamma_move(g, 1, 1, 1));
    assert(gamma_move(g, 2, 0, 1));

    assert(gamma_busy_fields(g, 1) == 2);
    assert(gamma_free_fields(g, 1) == 0);
    assert(gamma_busy_fields(g, 2) == 2);
    assert(gamma_free_fields(g, 2) == 0);

    gamma_delete(g);
    g = gamma_new(5, 5, 2, 4);
    assert(g != NULL);

    assert(gamma_move(g, 1, 2, 1));
    assert(gamma_move(g, 1, 2, 3));
    assert(gamma_move(g, 1, 1, 2));
    assert(gamma_move(g, 1, 3, 2));
    assert(gamma_move(g, 2, 2, 2));

    assert(gamma_busy_fields(g, 1) == 4);
    assert(gamma_free_fields(g, 1) == 8);
    assert(gamma_busy_fields(g, 2) == 1);
    assert(gamma_free_fields(g, 2) == 20);

    assert(gamma_move(g, 2, 1, 1));
    assert(gamma_move(g, 2, 3, 3));
    assert(gamma_move(g, 2, 1, 3));

    assert(gamma_busy_fields(g, 1) == 4);
    assert(gamma_free_fields(g, 1) == 5);
    assert(gamma_busy_fields(g, 2) == 4);
    assert(gamma_free_fields(g, 2) == 6);

    gamma_delete(g);
    g = gamma_new(5, 5, 5, 1);
    assert(g != NULL);

    assert(gamma_move(g, 2, 1, 2));
    assert(gamma_move(g, 4, 2, 1));
    assert(gamma_move(g, 5, 3, 2));
    assert(gamma_move(g, 3, 2, 2));
    assert(gamma_move(g, 1, 2, 3));

    assert(gamma_busy_fields(g, 1) == 1);
    assert(gamma_free_fields(g, 1) == 3);
    assert(gamma_busy_fields(g, 2) == 1);
    assert(gamma_free_fields(g, 2) == 3);
    assert(gamma_busy_fields(g, 3) == 1);
    assert(gamma_free_fields(g, 3) == 0);
    assert(gamma_busy_fields(g, 4) == 1);
    assert(gamma_free_fields(g, 4) == 3);
    assert(gamma_busy_fields(g, 5) == 1);
    assert(gamma_free_fields(g, 5) == 3);

    gamma_delete(g);
    return PASS;
}

/* Uruchamia kilka krótkich testów poprawności wykonywania zwykłych ruchów
 * i złotych ruchów oraz obliczania liczby zajętych i wolnych pól po wykonaniu
 * tych ruchów. */
static int golden_move(void) {
    gamma_t *g = gamma_new(10, 10, 3, 2);
    assert(g != NULL);

    assert(gamma_move(g, 1, 4, 5));
    assert(gamma_move(g, 2, 5, 5));
    assert(gamma_move(g, 3, 4, 6));
    assert(gamma_move(g, 1, 5, 6));
    assert(gamma_move(g, 2, 3, 5));
    assert(gamma_move(g, 3, 4, 4));
    assert(gamma_golden_move(g, 1, 3, 5));

    assert(gamma_busy_fields(g, 1) == 3);
    assert(gamma_free_fields(g, 1) == 5);
    assert(gamma_busy_fields(g, 2) == 1);
    assert(gamma_free_fields(g, 2) == 94);
    assert(gamma_busy_fields(g, 3) == 2);
    assert(gamma_free_fields(g, 3) == 5);

    gamma_delete(g);
    g = gamma_new(7, 5, 3, 2);
    assert(g != NULL);

    assert(gamma_move(g, 1, 2, 2));
    assert(gamma_move(g, 2, 4, 2));
    assert(gamma_move(g, 3, 3, 2));
    assert(gamma_move(g, 1, 6, 4));
    assert(gamma_move(g, 2, 0, 0));
    assert(gamma_move(g, 1, 1, 2));
    assert(gamma_golden_move(g, 1, 3, 2));

    assert(gamma_busy_fields(g, 1) == 4);
    assert(gamma_free_fields(g, 1) == 9);
    assert(gamma_busy_fields(g, 2) == 2);
    assert(gamma_free_fields(g, 2) == 5);
    assert(gamma_busy_fields(g, 3) == 0);
    assert(gamma_free_fields(g, 3) == 29);

    gamma_delete(g);
    g = gamma_new(6, 3, 2, 2);
    assert(g != NULL);

    assert(gamma_move(g, 1, 0, 1));
    assert(gamma_move(g, 1, 2, 1));
    assert(gamma_move(g, 2, 3, 0));
    assert(gamma_move(g, 2, 3, 1));
    assert(gamma_move(g, 2, 3, 2));
    assert(gamma_golden_move(g, 1, 3, 1));

    assert(gamma_busy_fields(g, 1) == 3);
    assert(gamma_free_fields(g, 1) == 6);
    assert(gamma_busy_fields(g, 2) == 2);
    assert(gamma_free_fields(g, 2) == 4);

    gamma_delete(g);
    g = gamma_new(6, 3, 2, 2);
    assert(g != NULL);

    assert(gamma_move(g, 1, 0, 1));
    assert(gamma_move(g, 1, 2, 1));
    assert(gamma_move(g, 2, 3, 0));
    assert(gamma_move(g, 2, 3, 1));
    assert(gamma_move(g, 2, 3, 2));
    assert(gamma_move(g, 2, 5, 0));
    assert(!gamma_golden_move(g, 1, 3, 1));

    assert(gamma_busy_fields(g, 1) == 2);
    assert(gamma_free_fields(g, 1) == 5);
    assert(gamma_busy_fields(g, 2) == 4);
    assert(gamma_free_fields(g, 2) == 6);

    gamma_delete(g);
    g = gamma_new(6, 3, 2, 2);
    assert(g != NULL);

    assert(gamma_move(g, 1, 0, 0));
    assert(gamma_move(g, 1, 0, 2));
    assert(gamma_move(g, 2, 3, 0));
    assert(gamma_move(g, 2, 3, 1));
    assert(gamma_move(g, 2, 3, 2));
    assert(!gamma_golden_move(g, 1, 3, 1));

    assert(gamma_busy_fields(g, 1) == 2);
    assert(gamma_free_fields(g, 1) == 3);
    assert(gamma_busy_fields(g, 2) == 3);
    assert(gamma_free_fields(g, 2) == 13);

    gamma_delete(g);
    return PASS;
}

/* Testuje zgodność implementacji funkcji golden_possible z jej opisem. */
static int golden_possible(void) {
    gamma_t *g = gamma_new(10, 10, 3, 1);
    assert(g != NULL);

    assert(gamma_move(g, 2, 1, 1));
    assert(gamma_move(g, 2, 1, 2));
    assert(gamma_move(g, 2, 1, 3));

    assert(gamma_golden_possible(g, 1));
    assert(!gamma_golden_move(g, 1, 1, 2));
    assert(!gamma_golden_move(g, 1, 2, 2));
    assert(gamma_golden_move(g, 1, 1, 1));

    gamma_delete(g);
    g = gamma_new(9, 9, 2, 2);
    assert(g != NULL);

    assert(gamma_move(g, 2, 1, 1));
    assert(gamma_move(g, 2, 1, 2));
    assert(gamma_move(g, 2, 1, 3));
    assert(gamma_move(g, 1, 5, 4));
    assert(gamma_move(g, 1, 4, 5));
    assert(gamma_golden_possible(g, 1));
    assert(!gamma_golden_move(g, 1, 1, 1));
    assert(!gamma_golden_move(g, 1, 1, 2));
    assert(!gamma_golden_move(g, 1, 1, 3));

    gamma_delete(g);
    return PASS;
}

/* Testuje liczenie obszarów jednego gracza. */
static int areas(void) {
    gamma_t *g = gamma_new(31, 37, 1, 42);
    assert(g != NULL);

    for (uint32_t i = 0; i < 21; ++i) {
        assert(gamma_move(g, 1, i, i));
        assert(gamma_move(g, 1, i + 2, i));
    }

    assert(!gamma_move(g, 1, 0, 2));
    assert(!gamma_move(g, 1, 0, 4));

    for (uint32_t i = 0; i < 9; ++i)
        assert(gamma_move(g, 1, i + 1, i));

    for (uint32_t i = 2; i <= 36; i += 2)
        assert(gamma_move(g, 1, 0, i));

    assert(!gamma_move(g, 1, 4, 0));
    assert(!gamma_move(g, 1, 6, 0));

    gamma_delete(g);
    return PASS;
}

/* Testuje rozgałęzione obszary. */
static int tree(void) {
    gamma_t *g = gamma_new(16, 15, 3, 2);
    assert(g != NULL);

    assert(gamma_move(g, 1, 1, 1));
    assert(gamma_move(g, 1, 12, 3));
    assert(gamma_move(g, 1, 2, 1));
    assert(gamma_move(g, 1, 12, 4));
    assert(gamma_move(g, 1, 1, 2));
    assert(gamma_move(g, 1, 12, 5));
    assert(gamma_move(g, 1, 3, 1));
    assert(gamma_move(g, 1, 12, 2));
    assert(gamma_move(g, 1, 1, 3));
    assert(gamma_move(g, 1, 12, 1));
    assert(gamma_move(g, 1, 4, 1));
    assert(gamma_move(g, 1, 11, 3));
    assert(gamma_move(g, 1, 2, 3));
    assert(gamma_move(g, 1, 10, 3));
    assert(gamma_move(g, 1, 5, 1));
    assert(gamma_move(g, 1, 13, 3));
    assert(gamma_move(g, 1, 3, 3));
    assert(gamma_move(g, 1, 14, 3));
    assert(gamma_move(g, 1, 6, 1));
    assert(gamma_move(g, 1, 10, 2));
    assert(gamma_move(g, 1, 4, 3));
    assert(gamma_move(g, 1, 10, 1));
    assert(gamma_move(g, 1, 7, 1));
    assert(gamma_move(g, 1, 11, 1));
    assert(gamma_move(g, 1, 5, 3));
    assert(gamma_move(g, 1, 13, 1));
    assert(gamma_move(g, 1, 8, 1));
    assert(gamma_move(g, 1, 14, 1));
    assert(gamma_move(g, 1, 6, 3));
    assert(gamma_move(g, 1, 14, 2));
    assert(gamma_move(g, 1, 6, 4));
    assert(gamma_move(g, 1, 14, 4));
    assert(gamma_move(g, 1, 7, 4));
    assert(gamma_move(g, 1, 14, 5));
    assert(gamma_move(g, 1, 7, 5));
    assert(gamma_move(g, 1, 13, 5));
    assert(gamma_move(g, 1, 11, 4));
    assert(gamma_move(g, 1, 11, 5));
    assert(gamma_move(g, 1, 10, 5));
    assert(gamma_move(g, 1, 13, 2));
    assert(gamma_move(g, 1, 13, 4));
    assert(gamma_move(g, 1, 9, 1));
    assert(gamma_move(g, 1, 4, 7));
    assert(gamma_move(g, 1, 4, 8));
    assert(gamma_move(g, 1, 4, 9));
    assert(gamma_move(g, 1, 4, 10));
    assert(gamma_move(g, 1, 4, 11));
    assert(gamma_move(g, 1, 4, 12));
    assert(gamma_move(g, 1, 4, 13));
    assert(gamma_move(g, 1, 3, 10));
    assert(gamma_move(g, 1, 2, 10));
    assert(gamma_move(g, 1, 1, 10));
    assert(gamma_move(g, 1, 5, 10));
    assert(gamma_move(g, 1, 6, 10));
    assert(gamma_move(g, 1, 7, 10));
    assert(gamma_move(g, 1, 8, 10));
    assert(gamma_move(g, 1, 3, 8));
    assert(gamma_move(g, 1, 5, 8));
    assert(gamma_move(g, 1, 2, 9));
    assert(gamma_move(g, 1, 2, 11));
    assert(gamma_move(g, 1, 3, 12));
    assert(gamma_move(g, 1, 5, 12));
    assert(gamma_move(g, 1, 7, 9));
    assert(gamma_move(g, 1, 7, 8));
    assert(gamma_move(g, 1, 7, 7));
    assert(gamma_move(g, 1, 7, 11));
    assert(gamma_move(g, 1, 7, 12));
    assert(gamma_move(g, 1, 8, 12));
    assert(gamma_move(g, 1, 8, 13));
    assert(gamma_move(g, 1, 9, 10));
    assert(gamma_move(g, 1, 10, 10));
    assert(gamma_move(g, 1, 11, 10));
    assert(gamma_move(g, 1, 11, 9));
    assert(gamma_move(g, 1, 11, 8));
    assert(gamma_move(g, 1, 12, 8));
    assert(gamma_move(g, 1, 12, 7));
    assert(gamma_move(g, 1, 11, 11));
    assert(gamma_move(g, 1, 11, 12));
    assert(gamma_move(g, 1, 10, 12));
    assert(gamma_move(g, 1, 12, 10));
    assert(gamma_move(g, 1, 13, 10));
    assert(gamma_move(g, 1, 13, 11));
    assert(gamma_move(g, 1, 14, 10));
    assert(gamma_move(g, 1, 13, 9));

    assert(!gamma_move(g, 1, 2, 5));
    assert(!gamma_golden_move(g, 3, 9, 10));
    assert(gamma_move(g, 1, 9, 12));
    assert(gamma_golden_move(g, 3, 9, 10));
    assert(gamma_move(g, 2, 7, 6));
    assert(gamma_golden_move(g, 1, 7, 6));
    assert(gamma_golden_move(g, 2, 9, 12));
    assert(!gamma_move(g, 1, 9, 7));

    gamma_delete(g);
    return PASS;
}

/* Testuje ruchy wykonywane wyłącznie na brzegu planszy. */
static int border(void) {
    static const char board[] =
            "321442\n"
            "4....1\n"
            "4....4\n"
            "1....3\n"
            "123412\n";

    gamma_t *g = gamma_new(6, 5, 4, 4);
    assert(g != NULL);

    assert(gamma_move(g, 1, 0, 0));
    assert(gamma_move(g, 2, 1, 0));
    assert(gamma_move(g, 3, 2, 0));
    assert(gamma_move(g, 4, 3, 0));
    assert(gamma_move(g, 1, 4, 0));
    assert(gamma_move(g, 2, 5, 0));
    assert(gamma_move(g, 3, 5, 1));
    assert(gamma_move(g, 4, 5, 2));
    assert(gamma_move(g, 1, 5, 3));
    assert(gamma_move(g, 2, 5, 4));
    assert(gamma_move(g, 3, 4, 4));
    assert(gamma_move(g, 4, 3, 4));
    assert(gamma_move(g, 1, 2, 4));
    assert(gamma_move(g, 2, 1, 4));
    assert(gamma_move(g, 3, 0, 4));
    assert(gamma_move(g, 4, 0, 3));

    assert(!gamma_move(g, 1, 0, 2));
    assert(!gamma_move(g, 2, 0, 1));
    assert(!gamma_move(g, 3, 0, 2));
    assert(!gamma_move(g, 4, 0, 1));
    assert(gamma_move(g, 4, 0, 2));
    assert(gamma_move(g, 1, 0, 1));
    assert(!gamma_golden_move(g, 3, 2, 4));
    assert(gamma_golden_move(g, 4, 4, 4));

    char *p = gamma_board(g);
    assert(p);
    assert(strcmp(p, board) == 0);
    free(p);

    assert(gamma_busy_fields(g, 1) == 5);
    assert(gamma_free_fields(g, 1) == 4);
    assert(gamma_busy_fields(g, 2) == 4);
    assert(gamma_free_fields(g, 2) == 2);
    assert(gamma_busy_fields(g, 3) == 3);
    assert(gamma_free_fields(g, 3) == 12);
    assert(gamma_busy_fields(g, 4) == 6);
    assert(gamma_free_fields(g, 4) == 6);

    gamma_delete(g);
    return PASS;
}

/* Wykonanie niektórych funkcji w tym teście może się nie udać z powodu braku
 * pamięci, ale nie powinno to skutkować załamaniem wykonywania programu. */
static int memory_alloc(void) {
    gamma_t *g = gamma_new(SMALL_BOARD_SIZE, SMALL_BOARD_SIZE, 2, 2);
    assert(g != NULL);

    /* Alokujemy całą dostępną pamięć. */
    for (size_t s = 1024 * 1024 * 1024; s >= 1; s /= 2) {
        void *p;
        do {
            p = malloc(s);
        } while (p);
    }

    while (gamma_new(MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE, 10, 3) != NULL);

    for (uint32_t x = 0; x < SMALL_BOARD_SIZE; ++x)
        for (uint32_t y = 0; y < SMALL_BOARD_SIZE; ++y)
            gamma_move(g, 1, x, y);

    /* To jest test alokacji pamięci – nie zwalniamy jej. */
    return MEM_PASS;
}

/* Testuje odporność implementacji na duże wartości parametrów w gamma_new. */
static int big_board(void) {
    static const gamma_param_t game[] = {
            {UINT32_MAX,        UINT32_MAX,        UINT32_MAX,        UINT32_MAX},
            {2,                 UINT32_MAX,        UINT32_MAX,        UINT32_MAX},
            {UINT32_MAX,    2,                     UINT32_MAX,        UINT32_MAX},
            {UINT32_MAX,        UINT32_MAX, 2,                        UINT32_MAX},
            {UINT32_MAX,        UINT32_MAX,        UINT32_MAX, 5},
            {UINT32_MAX,    2,              2,                 5},
            {2,                 UINT32_MAX, 2,                 5},
            {2,             2,                     UINT32_MAX, 5},
            {2,             2,              2,                        UINT32_MAX},
            {1 << 16,       1 << 16,        2,                 5},
            {(1 << 24) + 1, 1 << 8,         2,                 5},
            {(1 << 15) + 1, (1 << 14) + 1,  2,                 5},
            {(1 << 14) + 1, (1 << 13) + 1,  2,                 5},
            {(1 << 13) + 1, (1 << 12) + 1,  2,                 5},
            {(1 << 12) + 1, (1 << 11) + 1,  2,                 5},
            {(1 << 11) + 1, (1 << 10) + 1,  2,                 5},
            {BIG_BOARD_SIZE,    BIG_BOARD_SIZE,    BIG_BOARD_SIZE,    BIG_BOARD_SIZE},
            {MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE, MIDDLE_BOARD_SIZE},
            {SMALL_BOARD_SIZE,  SMALL_BOARD_SIZE,  SMALL_BOARD_SIZE,  SMALL_BOARD_SIZE},
    };

    unsigned success = 0;
    for (size_t i = 0; i < SIZE(game);
    ++i) {
        errno = 0;
        gamma_t *g = gamma_new(game[i].width, game[i].height,
                               game[i].players, game[i].areas);
        if (g != NULL) {
            uint64_t size = (uint64_t) game[i].width * (uint64_t) game[i].height;
            assert(gamma_free_fields(g, game[i].players) == size);
            // Przynajmniej cztery ruchy powinny się udać.
            assert(gamma_move(g, game[i].players, 0, 0));
            assert(gamma_move(g, game[i].players, 0, game[i].height - 1));
            assert(gamma_move(g, game[i].players, game[i].width - 1, 0));
            assert(gamma_move(g, game[i].players, game[i].width - 1,
                              game[i].height - 1));
            assert(gamma_free_fields(g, game[i].players) == size - 4);
            assert(gamma_busy_fields(g, game[i].players) == 4);
            gamma_delete(g);
            ++success;
        }
        else {
            assert(!test_errno || errno == ENOMEM);
        }
    }
    // Przynajmniej jedną z tych gier powinno się udać utworzyć.
    assert(success > 0);

    return PASS;
}

/* Testuje ogranicznenia na rozmiar planszy w gamma_new. */
static int middle_board(void) {
    unsigned success = 0;
    for (uint32_t size = 8000; size >= 125; size /= 2) {
        gamma_t *g1 = gamma_new(size, size, 2, 2);
        if (g1 != NULL) {
            gamma_delete(g1);
            // Jeśli udało się zaalokować planszę o rozmiarze size * size, to powinno
            // się też udać zaalokować inną planszę o nieco mniejszej powierzchni.
            gamma_t *g2;
            g2 = gamma_new((size - 1) * (size - 1), 1, 2, 2);
            assert(g2 != NULL);
            gamma_delete(g2);
            g2 = gamma_new(1, (size - 1) * (size - 1), 2, 2);
            assert(g2 != NULL);
            gamma_delete(g2);
            ++success;
        }
        else {
            assert(!test_errno || errno == ENOMEM);
        }
    }
    // Przynajmniej jedną z tych gier powinno się udać utworzyć.
    assert(success > 0);

    return PASS;
}

/** URUCHAMIANIE TESTÓW **/

typedef struct {
    char const *name;

    int (*function)(void);
} test_list_t;

#define TEST(t) {#t, t}

static const test_list_t test_list[] = {
        TEST(example),
        TEST(minimal),
        TEST(params),
        TEST(many_players),
        TEST(many_games),
        TEST(delete_null),
        TEST(normal_move),
        TEST(golden_move),
        TEST(golden_possible),
        TEST(areas),
        TEST(tree),
        TEST(border),
        TEST(memory_alloc),
        TEST(big_board),
        TEST(middle_board),
};

int main(int argc, char *argv[]) {
    if (argc != 2)
        return WRONG_TEST;

    for (size_t i = 0; i < SIZE(test_list);
    ++i)
    if (strcmp(argv[1], test_list[i].name) == 0)
        return test_list[i].function();

    return WRONG_TEST;
}
