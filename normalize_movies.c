#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> // Para INT_MAX

#define MAX_LINE_LEN 1024
#define MAX_FIELD_LEN 256
#define NUM_FIELDS 3 // nome,genero,ano

// --- Estrutura para Lista Ligada (para guardar features únicas) ---
typedef struct Node {
    char* value;
    struct Node* next;
} Node;

// --- Funções de Normalização (da nossa conversa anterior) ---

float normalizar_min_max(int valor, int min, int max) {
    if (max - min == 0) return 0.5f; // Valor neutro se todos são iguais
    float resultado = (float)(valor - min) / (float)(max - min);
    
    // Garantir que está no intervalo [0,1]
    if (resultado < 0.0f) return 0.0f;
    if (resultado > 1.0f) return 1.0f;
    
    return resultado;
}

int binarizar_multivalor(const char* string_principal, const char* substring_buscada) {
    return (strstr(string_principal, substring_buscada) != NULL) ? 1 : 0;
}

int binarizar_valor_unico(const char* string_principal, const char* categoria_buscada) {
    return (strcmp(string_principal, categoria_buscada) == 0) ? 1 : 0;
}

// --- Funções Auxiliares (Parser, Lista Ligada, Sanitização) ---

/**
 * Adiciona um valor a uma lista ligada, mas apenas se ele ainda não existir.
 * strdup() aloca nova memória e copia a string.
 */
void add_unique(Node** head, const char* value) {
    Node* current = *head;
    while (current != NULL) {
        if (strcmp(current->value, value) == 0) {
            return; // Já existe
        }
        current = current->next;
    }
    // Não encontrado, adicionar ao início da lista
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = strdup(value);
    newNode->next = *head;
    *head = newNode;
}

/**
 * Libera toda a memória usada pela lista ligada.
 */
void free_list(Node* head) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp->value); // Libera a string copiada com strdup()
        free(temp);        // Libera o nó
    }
}

/**
 * Um parser de CSV simples que lida com campos entre aspas (").
 * Ele modifica a 'line' original e preenche 'fields'.
 */
void parse_csv_line(char* line, char* fields[], int num_fields) {
    int field_index = 0;
    char* current = line;
    char* field_start = line;

    while (*current && field_index < num_fields) {
        if (*current == '"') { // Início de um campo com aspas
            field_start = ++current; // Pula o " inicial
            while (*current && *current != '"') {
                current++;
            }
            if (*current == '"') {
                *current = '\0'; // Termina a string do campo
                current++; // Pula o " final
                if (*current == ',') current++; // Pula a vírgula
            }
        } else { // Campo sem aspas
            field_start = current;
            while (*current && *current != ',') {
                current++;
            }
            if (*current == ',') {
                *current = '\0'; // Termina a string
                current++; // Pula a vírgula
            }
        }
        
        // Remove quebra de linha no final
        char* end = field_start + strlen(field_start) - 1;
        if (*end == '\n' || *end == '\r') *end = '\0';

        fields[field_index++] = field_start;
    }
}

/**
 * Prepara um nome de feature para o cabeçalho do CSV
 * (ex: "Ficção Científica" -> "Ficcao_Cientifica")
 */
void sanitize_header(const char* input, char* output) {
    int j = 0;
    for (int i = 0; input[i]; i++) {
        if (input[i] == ' ' || input[i] == '/' || input[i] == ',' || input[i] == '-') {
            output[j++] = '_';
        } else if (input[i] == '(' || input[i] == ')' || input[i] == '.') {
            // Ignorar caracteres problemáticos
            continue;
        } else if ((input[i] >= 'A' && input[i] <= 'Z') || 
                   (input[i] >= 'a' && input[i] <= 'z') || 
                   (input[i] >= '0' && input[i] <= '9') || 
                   input[i] == '_') {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

// --- Função Principal ---

int main() {
    // ---- ATENÇÃO: Altere os nomes dos arquivos aqui se necessário ----
    const char* input_filename = "not_normalized_movies.data"; // Arquivo com dados de filmes
    const char* output_filename = "movies_normalized.csv";
    // ----------------------------------------------------------------

    FILE* file;
    char line[MAX_LINE_LEN];
    char* fields[NUM_FIELDS];

    // Listas para features únicas
    Node* generos_head = NULL;

    // Valores para normalização do ano
    int min_ano = INT_MAX;
    int max_ano = 0;
    
    // ================================================================
    // PASSO 1: Descoberta (Ler o arquivo pela primeira vez)
    // ================================================================

    file = fopen(input_filename, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    // Ler e ignorar a linha de cabeçalho
    if (fgets(line, MAX_LINE_LEN, file) == NULL) {
        printf("Arquivo de entrada vazio ou ilegível.\n");
        fclose(file);
        return 1;
    }

    printf("--- PASSO 1: Lendo %s para descobrir features ---\n", input_filename);

    while (fgets(line, MAX_LINE_LEN, file)) {
        char line_copy[MAX_LINE_LEN];
        strcpy(line_copy, line); // parse_csv_line modifica a string, então usamos uma cópia
        
        parse_csv_line(line_copy, fields, NUM_FIELDS);

        // fields[0] = nome
        // fields[1] = genero
        // fields[2] = ano
        
        // 1. Gêneros (são multivalorados, separados por '/')
        char genero_str[MAX_FIELD_LEN];
        strcpy(genero_str, fields[1]); // Copiar para não modificar original
        char* token = strtok(genero_str, "/");
        while (token != NULL) {
            // Remover espaços no início e fim
            while (*token == ' ') token++;
            char* end = token + strlen(token) - 1;
            while (end > token && *end == ' ') {
                *end = '\0';
                end--;
            }
            if (strlen(token) > 0) {
                add_unique(&generos_head, token);
            }
            token = strtok(NULL, "/");
        }

        // 2. Ano
        int ano = atoi(fields[2]);
        if (ano > 1800 && ano < 2030) { // Validação básica de ano
            if (ano < min_ano) min_ano = ano;
            if (ano > max_ano) max_ano = ano;
        }
    }
    fclose(file);

    // Contar quantos itens únicos foram encontrados
    int generos_count = 0;
    for (Node* current = generos_head; current != NULL; current = current->next) {
        generos_count++;
    }

    printf("Descoberta concluída:\n");
    printf("  Ano (Min/Max): %d / %d\n", min_ano, max_ano);
    printf("  Gêneros únicos encontrados: %d\n", generos_count);
    
    // Mostrar alguns exemplos
    printf("  Exemplos de gêneros: ");
    int count = 0;
    for (Node* current = generos_head; current != NULL && count < 5; current = current->next, count++) {
        printf("'%s' ", current->value);
    }
    printf("\n\n");

    // ================================================================
    // PASSO 2: Transformação (Ler e Escrever)
    // ================================================================

    FILE* infile = fopen(input_filename, "r");
    FILE* outfile = fopen(output_filename, "w");

    if (infile == NULL || outfile == NULL) {
        perror("Erro ao reabrir arquivos para o Passo 2");
        return 1;
    }

    printf("--- PASSO 2: Lendo %s e escrevendo %s ---\n", input_filename, output_filename);

    // --- 2a. Escrever o Cabeçalho do novo CSV ---
    fprintf(outfile, "nome,ano_norm");
    
    char sanitized_name[MAX_FIELD_LEN];

    for (Node* current = generos_head; current != NULL; current = current->next) {
        sanitize_header(current->value, sanitized_name);
        fprintf(outfile, ",genero_%s", sanitized_name);
    }
    fprintf(outfile, "\n");

    // Ignorar o cabeçalho do arquivo de entrada novamente
    fgets(line, MAX_LINE_LEN, infile);

    // --- 2b. Ler e Processar cada linha ---
    while (fgets(line, MAX_LINE_LEN, infile)) {
        char line_copy[MAX_LINE_LEN];
        strcpy(line_copy, line);
        
        parse_csv_line(line_copy, fields, NUM_FIELDS);

        char* nome = fields[0];
        char* genero_bruto = fields[1];
        int ano = atoi(fields[2]);

        // Escrever Nome (o rótulo)
        fprintf(outfile, "\"%s\"", nome);

        // Escrever Ano Normalizado
        float ano_norm = 0.0f;
        if (ano > 1800 && ano < 2030) {
            ano_norm = normalizar_min_max(ano, min_ano, max_ano);
        }
        fprintf(outfile, ",%.6f", ano_norm); // Mais precisão na saída

        // Escrever Gêneros Binarizados
        for (Node* current = generos_head; current != NULL; current = current->next) {
            int val = binarizar_multivalor(genero_bruto, current->value);
            fprintf(outfile, ",%d", val);
        }
        
        fprintf(outfile, "\n");
    }

    fclose(infile);
    fclose(outfile);

    // ================================================================
    // PASSO 3: Limpeza
    // ================================================================
    free_list(generos_head);

    printf("\nConcluído! Dados normalizados salvos em %s\n", output_filename);

    return 0;
}