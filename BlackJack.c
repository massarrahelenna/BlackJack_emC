#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    char name[50];
    char password[100];
    int balance;
} Player;

typedef struct {
    char name[50];
    int score;
} PlayerResult;

// Função para gerar um número aleatório
int random_number(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Função para calcular o valor da mão
int calculate_hand_value(int hand[], int num_cards) {
    int value = 0;
    int num_aces = 0;

    for (int i = 0; i < num_cards; i++) {
        if (hand[i] == 1) {
            num_aces++;
            value += 11;
        } else if (hand[i] > 10) {
            value += 10;
        } else {
            value += hand[i];
        }
    }
    while (value > 21 && num_aces > 0) {
        value -= 10;
        num_aces--;
    }

    return value;
}

// Função para exibir o ranking
void display_ranking(PlayerResult players[], int num_players) {
    printf("\nRanking:\n");
    printf("Posição\tNome\tPontuação\n");
    for (int i = 0; i < num_players; i++) {
        printf("%d\t%s\t%d\n", i + 1, players[i].name, players[i].score);
    }
}

// Função para comparar jogadores pelo saldo (para qsort)
int compare_players(const void *a, const void *b) {
    PlayerResult *playerA = (PlayerResult *)a;
    PlayerResult *playerB = (PlayerResult *)b;
    return playerB->score - playerA->score;
}

// Função para embaralhar o baralho
void shuffle_deck(int deck[]) {
    for (int i = 0; i < 52; i++) {
        int j = random_number(0, 51);
        int temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

// Função para inicializar as mãos
void inicializar_maos(int deck[], int player_hand[], int *num_player_cards, int dealer_hand[], int *num_dealer_cards) {
    shuffle_deck(deck);
    player_hand[(*num_player_cards)++] = deck[0];
    dealer_hand[(*num_dealer_cards)++] = deck[1];
    player_hand[(*num_player_cards)++] = deck[2];
    dealer_hand[(*num_dealer_cards)++] = deck[3];

    printf("Sua mão:\n%d\n%d\n", player_hand[0], player_hand[1]);
    printf("Mão do dealer:\n%d\n", dealer_hand[0]);
}

// Função para mostrar a mão
void mostrar_mao(int hand[], int num_cards, const char* jogador) {
    printf("%s mão:\n", jogador);
    for (int i = 0; i < num_cards; i++) {
        printf("%d ", hand[i]);
    }
    printf("\n");
}

// Função para jogar a rodada do jogador
int jogar_rodada_jogador(int deck[], int player_hand[], int *num_player_cards, int *deck_index) {
    int player_value = calculate_hand_value(player_hand, *num_player_cards);
    while (player_value < 21) {
        char choice;
        printf("Deseja pedir outra carta? (s/n): ");
        scanf(" %c", &choice);

        if (choice == 's') {
            player_hand[(*num_player_cards)++] = deck[(*deck_index)++];
            printf("Nova carta: %d\n", player_hand[*num_player_cards - 1]);
            player_value = calculate_hand_value(player_hand, *num_player_cards);
            printf("Valor total da sua mão: %d\n", player_value);
        } else {
            break;
        }
    }
    return player_value;
}

// Função para jogar a rodada do dealer
int jogar_rodada_dealer(int deck[], int dealer_hand[], int *num_dealer_cards, int *deck_index) {
    int dealer_value = calculate_hand_value(dealer_hand, *num_dealer_cards);
    while (dealer_value < 17) {
        dealer_hand[(*num_dealer_cards)++] = deck[(*deck_index)++];
        dealer_value = calculate_hand_value(dealer_hand, *num_dealer_cards);
    }
    return dealer_value;
}

// Função para mostrar o resultado da rodada
void mostrar_resultado(int player_value, int dealer_value, Player *player, int aposta) {
    if (dealer_value > 21 || player_value > dealer_value) {
        printf("Você ganhou!\n");
        player->balance += aposta;
    } else if (player_value < dealer_value) {
        printf("Você perdeu.\n");
        player->balance -= aposta;
    } else {
        printf("Empate!\n");
    }
    printf("Seu saldo atual: %d\n", player->balance);
}

// Função para reinicializar o jogo
void reinicializar_jogo(int deck[], int player_hand[], int dealer_hand[], int *num_player_cards, int *num_dealer_cards) {
    *num_player_cards = 0;
    *num_dealer_cards = 0;
}

// Função para registrar um novo jogador
void register_player(Player players[], int *num_players) {
    printf("Digite seu nome: ");
    scanf("%s", players[*num_players].name);
    printf("Digite sua senha: ");
    fgets(players[*num_players].password, 100, stdin); // Limpar buffer do teclado
    fgets(players[*num_players].password, 100, stdin); // Ler a senha com espaços
    players[*num_players].password[strcspn(players[*num_players].password, "\n")] = 0; // Remover nova linha
    players[*num_players].balance = 1000; // Saldo inicial
    (*num_players)++;
}

// Função para fazer login
Player* login(Player players[], int num_players) {
    char name[50], password[100];
    printf("Digite seu nome: ");
    scanf("%s", name);
    printf("Digite sua senha: ");
    fgets(password, 100, stdin); // Limpar buffer do teclado
    fgets(password, 100, stdin); // Ler a senha com espaços
    password[strcspn(password, "\n")] = 0; // Remover nova linha

    for (int i = 0; i < num_players; i++) {
        if (strcmp(players[i].name, name) == 0 && strcmp(players[i].password, password) == 0) {
            printf("Login bem-sucedido!\n");
            return &players[i];
        }
    }
    printf("Nome ou senha incorretos!\n");
    return NULL;
}

// Função principal do jogo
void jogar(int deck[], int player_hand[], int dealer_hand[], int *num_player_cards, int *num_dealer_cards, Player *player) {
    int aposta;
    printf("Você tem 1000 reais para apostar.\nA cada aposta, você ganha ou perde o valor apostado.\n");
    printf("Digite o valor da aposta: ");
    scanf("%d", &aposta);

    if (aposta > player->balance) {
        printf("Saldo insuficiente!\n");
        return;
    }

    reinicializar_jogo(deck, player_hand, dealer_hand, num_player_cards, num_dealer_cards);
    inicializar_maos(deck, player_hand, num_player_cards, dealer_hand, num_dealer_cards);

    int deck_index = 4; // Início do índice de distribuição após a inicialização

    int player_value = jogar_rodada_jogador(deck, player_hand, num_player_cards, &deck_index);
    if (player_value > 21) {
        printf("Você estourou 21! Você perdeu.\n");
        player->balance -= aposta;
        return;
    }

    int dealer_value = jogar_rodada_dealer(deck, dealer_hand, num_dealer_cards, &deck_index);

    mostrar_mao(player_hand, *num_player_cards, "Sua");
    printf("Valor total da sua mão: %d\n", player_value);

    mostrar_mao(dealer_hand, *num_dealer_cards, "Mão do dealer");
    printf("Valor total da mão do dealer: %d\n", dealer_value);

    mostrar_resultado(player_value, dealer_value, player, aposta);
}

int main() {
    srand(time(NULL));

    int deck[52] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10,  
                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10,
                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10,
                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

    int player_hand[12]; 
    int dealer_hand[12]; 

    int num_player_cards = 0; 
    int num_dealer_cards = 0; 

    Player players[100];
    int num_players = 0;

    // Carregar jogadores de um arquivo
    FILE *file = fopen("players.txt", "r");
    if (file != NULL) {
        while (fscanf(file, "%s %s %d", players[num_players].name, players[num_players].password, &players[num_players].balance) == 3) {
            num_players++;
        }
        fclose(file);
    }

    int choice;
    Player *current_player = NULL;

    printf("Bem-vindo ao Blackjack!\n");
    printf("1. Registrar\n");
    printf("2. Login\n");
    printf("Escolha uma opção: ");
    scanf("%d", &choice);

    if (choice == 1) {
        register_player(players, &num_players);
        current_player = &players[num_players - 1];
    } else if (choice == 2) {
        current_player = login(players, num_players);
        if (current_player == NULL) {
            return 0;
        }
    } else {
        printf("Opção inválida!\n");
        return 0;
    }

    while (1) {
        printf("1. Jogar\n");
        printf("2. Ver Ranking\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &choice);

        if (choice == 1) {
            jogar(deck, player_hand, dealer_hand, &num_player_cards, &num_dealer_cards, current_player);
        } else if (choice == 2) {
            PlayerResult results[100];
            int num_results = 0;
            for (int i = 0; i < num_players; i++) {
                strcpy(results[i].name, players[i].name);
                results[i].score = players[i].balance;
                num_results++;
            }
            qsort(results, num_results, sizeof(PlayerResult), compare_players);
            display_ranking(results, num_results);
        } else if (choice == 3) {
            break;
        } else {
            printf("Opção inválida!\n");
        }
    }

    // Salvar jogadores no arquivo
    file = fopen("players.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < num_players; i++) {
            fprintf(file, "%s %s %d\n", players[i].name, players[i].password, players[i].balance);
        }
        fclose(file);
    }

    return 0;
}
