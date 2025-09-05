// projeto.c
// gcc -std=c99 projeto.c -o projeto
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define NL putchar('\n')

typedef struct {
    int id;
    char nome[64];
    char email[64];
} Cliente;

typedef struct {
    int id;
    char nome[64];
    double preco;
    int estoque;
} Produto;

typedef struct {
    Cliente *dados;
    size_t tamanho;
    size_t capacidade;
    int prox_id;
} VClientes;

typedef struct {
    Produto *dados;
    size_t tamanho;
    size_t capacidade;
    int prox_id;
} VProdutos;

/* ---------- Util: leitura robusta ---------- */
static void limpar_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static void ler_linha(char *buf, size_t n) {
    if (fgets(buf, (int)n, stdin)) {
        size_t L = strlen(buf);
        if (L && buf[L-1] == '\n') buf[L-1] = '\0';
        else limpar_stdin(); // limpa resto da linha
    } else {
        // erro ou EOF: garante string vazia
        if (n) buf[0] = '\0';
        clearerr(stdin);
    }
}

static int ler_int(const char *prompt, int *out) {
    char buf[128];
    for (;;) {
        if (prompt && *prompt) printf("%s", prompt);
        ler_linha(buf, sizeof buf);
        if (buf[0] == '\0') { puts("Entrada vazia. Tente novamente."); continue; }
        char *end = NULL;
        errno = 0;
        long v = strtol(buf, &end, 10);
        if (errno || end == buf || *end != '\0' || v < INT_MIN || v > INT_MAX) {
            puts("Valor inválido. Digite um inteiro.");
            continue;
        }
        *out = (int)v;
        return 1;
    }
}

static int ler_double(const char *prompt, double *out) {
    char buf[128];
    for (;;) {
        if (prompt && *prompt) printf("%s", prompt);
        ler_linha(buf, sizeof buf);
        if (buf[0] == '\0') { puts("Entrada vazia. Tente novamente."); continue; }
        char *end = NULL;
        errno = 0;
        double v = strtod(buf, &end);
        if (errno || end == buf || *end != '\0') {
            puts("Valor inválido. Digite um número (use ponto como separador decimal).");
            continue;
        }
        *out = v;
        return 1;
    }
}

static void ler_texto(const char *prompt, char *dest, size_t ndest) {
    for (;;) {
        if (prompt && *prompt) printf("%s", prompt);
        ler_linha(dest, ndest);
        if (dest[0] == '\0') { puts("Campo não pode ser vazio."); continue; }
        return;
    }
}

/* ---------- Vetores dinâmicos ---------- */
static void vclientes_init(VClientes *v) {
    v->dados = NULL;
    v->tamanho = 0;
    v->capacidade = 0;
    v->prox_id = 1;
}

static void vprodutos_init(VProdutos *v) {
    v->dados = NULL;
    v->tamanho = 0;
    v->capacidade = 0;
    v->prox_id = 1;
}

static void vclientes_free(VClientes *v) {
    free(v->dados);
    v->dados = NULL;
    v->tamanho = v->capacidade = 0;
}

static void vprodutos_free(VProdutos *v) {
    free(v->dados);
    v->dados = NULL;
    v->tamanho = v->capacidade = 0;
}

static int vclientes_assegurar_cap(VClientes *v, size_t nova) {
    if (nova <= v->capacidade) return 1;
    size_t cap = v->capacidade ? v->capacidade * 2 : 8;
    while (cap < nova) cap *= 2;
    Cliente *novo = realloc(v->dados, cap * sizeof *novo);
    if (!novo) { puts("Falha de memória (clientes)."); return 0; }
    v->dados = novo;
    v->capacidade = cap;
    return 1;
}

static int vprodutos_assegurar_cap(VProdutos *v, size_t nova) {
    if (nova <= v->capacidade) return 1;
    size_t cap = v->capacidade ? v->capacidade * 2 : 8;
    while (cap < nova) cap *= 2;
    Produto *novo = realloc(v->dados, cap * sizeof *novo);
    if (!novo) { puts("Falha de memória (produtos)."); return 0; }
    v->dados = novo;
    v->capacidade = cap;
    return 1;
}

/* ---------- Busca ---------- */
static Cliente* cliente_por_id(VClientes *v, int id) {
    for (size_t i = 0; i < v->tamanho; ++i)
        if (v->dados[i].id == id) return &v->dados[i];
    return NULL;
}
static Produto* produto_por_id(VProdutos *v, int id) {
    for (size_t i = 0; i < v->tamanho; ++i)
        if (v->dados[i].id == id) return &v->dados[i];
    return NULL;
}

/* ---------- CRUD Clientes ---------- */
static void cliente_inserir(VClientes *v) {
    if (!vclientes_assegurar_cap(v, v->tamanho + 1)) return;
    Cliente c;
    c.id = v->prox_id++;
    ler_texto("Nome: ", c.nome, sizeof c.nome);
    ler_texto("Email: ", c.email, sizeof c.email);
    v->dados[v->tamanho++] = c;
    printf("Cliente #%d inserido.\n", c.id);
}

static void cliente_listar(const VClientes *v) {
    if (v->tamanho == 0) { puts("(Sem clientes)"); return; }
    puts("ID | Nome                         | Email");
    puts("-----------------------------------------------");
    for (size_t i = 0; i < v->tamanho; ++i) {
        printf("%-3d| %-28s | %s\n", v->dados[i].id, v->dados[i].nome, v->dados[i].email);
    }
}

static void cliente_editar(VClientes *v) {
    int id;
    if (!ler_int("ID do cliente para editar: ", &id)) return;
    Cliente *c = cliente_por_id(v, id);
    if (!c) { puts("ID não encontrado."); return; }
    char buf[8];
    printf("Editar nome (%s)? [s/N]: ", c->nome);
    ler_linha(buf, sizeof buf);
    if (buf[0]=='s' || buf[0]=='S') ler_texto("Novo nome: ", c->nome, sizeof c->nome);

    printf("Editar email (%s)? [s/N]: ", c->email);
    ler_linha(buf, sizeof buf);
    if (buf[0]=='s' || buf[0]=='S') ler_texto("Novo email: ", c->email, sizeof c->email);

    puts("Cliente atualizado.");
}

static void cliente_remover(VClientes *v) {
    int id;
    if (!ler_int("ID do cliente para remover: ", &id)) return;
    size_t idx = v->tamanho;
    for (size_t i = 0; i < v->tamanho; ++i)
        if (v->dados[i].id == id) { idx = i; break; }
    if (idx == v->tamanho) { puts("ID não encontrado."); return; }
    // desloca à esquerda com memmove
    if (idx + 1 < v->tamanho) {
        memmove(&v->dados[idx], &v->dados[idx+1], (v->tamanho - idx - 1) * sizeof(Cliente));
    }
    v->tamanho--;
    puts("Cliente removido.");
}

/* ---------- CRUD Produtos ---------- */
static void produto_inserir(VProdutos *v) {
    if (!vprodutos_assegurar_cap(v, v->tamanho + 1)) return;
    Produto p;
    p.id = v->prox_id++;
    ler_texto("Nome: ", p.nome, sizeof p.nome);
    for (;;) {
        if (!ler_double("Preço (>= 0): ", &p.preco)) return;
        if (p.preco < 0) { puts("Preço inválido."); continue; }
        break;
    }
    for (;;) {
        if (!ler_int("Estoque inicial (>= 0): ", &p.estoque)) return;
        if (p.estoque < 0) { puts("Estoque inválido."); continue; }
        break;
    }
    v->dados[v->tamanho++] = p;
    printf("Produto #%d inserido.\n", p.id);
}

static void produto_listar(const VProdutos *v) {
    if (v->tamanho == 0) { puts("(Sem produtos)"); return; }
    puts("ID | Nome                         | Preço      | Estoque");
    puts("-------------------------------------------------------------");
    for (size_t i = 0; i < v->tamanho; ++i) {
        printf("%-3d| %-28s | %10.2f | %d\n",
               v->dados[i].id, v->dados[i].nome, v->dados[i].preco, v->dados[i].estoque);
    }
}

static void produto_editar(VProdutos *v) {
    int id;
    if (!ler_int("ID do produto para editar: ", &id)) return;
    Produto *p = produto_por_id(v, id);
    if (!p) { puts("ID não encontrado."); return; }
    char buf[8];
    printf("Editar nome (%s)? [s/N]: ", p->nome);
    ler_linha(buf, sizeof buf);
    if (buf[0]=='s' || buf[0]=='S') ler_texto("Novo nome: ", p->nome, sizeof p->nome);

    printf("Editar preço (%.2f)? [s/N]: ", p->preco);
    ler_linha(buf, sizeof buf);
    if (buf[0]=='s' || buf[0]=='S') {
        double novo;
        for (;;) {
            if (!ler_double("Novo preço (>= 0): ", &novo)) return;
            if (novo < 0) { puts("Preço inválido."); continue; }
            p->preco = novo; break;
        }
    }

    printf("Editar estoque (%d)? [s/N]: ", p->estoque);
    ler_linha(buf, sizeof buf);
    if (buf[0]=='s' || buf[0]=='S') {
        int novo;
        for (;;) {
            if (!ler_int("Novo estoque (>= 0): ", &novo)) return;
            if (novo < 0) { puts("Estoque inválido."); continue; }
            p->estoque = novo; break;
        }
    }

    puts("Produto atualizado.");
}

static void produto_ajustar_estoque(VProdutos *v) {
    int id;
    if (!ler_int("ID do produto: ", &id)) return;
    Produto *p = produto_por_id(v, id);
    if (!p) { puts("ID não encontrado."); return; }
    int delta;
    if (!ler_int("Ajuste de estoque (ex.: +5 ou -3): ", &delta)) return;
    if (p->estoque + delta < 0) { puts("Operação resultaria em estoque negativo."); return; }
    p->estoque += delta;
    printf("Novo estoque de '%s': %d\n", p->nome, p->estoque);
}

static void produto_remover(VProdutos *v) {
    int id;
    if (!ler_int("ID do produto para remover: ", &id)) return;
    size_t idx = v->tamanho;
    for (size_t i = 0; i < v->tamanho; ++i)
        if (v->dados[i].id == id) { idx = i; break; }
    if (idx == v->tamanho) { puts("ID não encontrado."); return; }
    if (idx + 1 < v->tamanho) {
        memmove(&v->dados[idx], &v->dados[idx+1], (v->tamanho - idx - 1) * sizeof(Produto));
    }
    v->tamanho--;
    puts("Produto removido.");
}

/* ---------- Persistência Binária ---------- */
typedef struct {
    int prox_id;
    size_t tamanho;
} HeaderPersist;

static int salvar_clientes(const VClientes *v, const char *caminho) {
    FILE *f = fopen(caminho, "wb");
    if (!f) { perror("fopen"); return 0; }
    HeaderPersist h = { v->prox_id, v->tamanho };
    if (fwrite(&h, sizeof h, 1, f) != 1) { perror("fwrite"); fclose(f); return 0; }
    if (v->tamanho) {
        if (fwrite(v->dados, sizeof(Cliente), v->tamanho, f) != v->tamanho) { perror("fwrite"); fclose(f); return 0; }
    }
    fclose(f);
    return 1;
}

static int carregar_clientes(VClientes *v, const char *caminho) {
    FILE *f = fopen(caminho, "rb");
    if (!f) { perror("fopen"); return 0; }
    HeaderPersist h;
    if (fread(&h, sizeof h, 1, f) != 1) { perror("fread"); fclose(f); return 0; }
    Cliente *buf = NULL;
    if (h.tamanho) {
        buf = malloc(h.tamanho * sizeof *buf);
        if (!buf) { puts("Falha de memória ao carregar clientes."); fclose(f); return 0; }
        if (fread(buf, sizeof(Cliente), h.tamanho, f) != h.tamanho) { perror("fread"); free(buf); fclose(f); return 0; }
    }
    fclose(f);
    vclients_free:
    free(v->dados);
    v->dados = buf;
    v->tamanho = h.tamanho;
    v->capacidade = h.tamanho;
    v->prox_id = h.prox_id;
    return 1;
}

static int salvar_produtos(const VProdutos *v, const char *caminho) {
    FILE *f = fopen(caminho, "wb");
    if (!f) { perror("fopen"); return 0; }
    HeaderPersist h = { v->prox_id, v->tamanho };
    if (fwrite(&h, sizeof h, 1, f) != 1) { perror("fwrite"); fclose(f); return 0; }
    if (v->tamanho) {
        if (fwrite(v->dados, sizeof(Produto), v->tamanho, f) != v->tamanho) { perror("fwrite"); fclose(f); return 0; }
    }
    fclose(f);
    return 1;
}

static int carregar_produtos(VProdutos *v, const char *caminho) {
    FILE *f = fopen(caminho, "rb");
    if (!f) { perror("fopen"); return 0; }
    HeaderPersist h;
    if (fread(&h, sizeof h, 1, f) != 1) { perror("fread"); fclose(f); return 0; }
    Produto *buf = NULL;
    if (h.tamanho) {
        buf = malloc(h.tamanho * sizeof *buf);
        if (!buf) { puts("Falha de memória ao carregar produtos."); fclose(f); return 0; }
        if (fread(buf, sizeof(Produto), h.tamanho, f) != h.tamanho) { perror("fread"); free(buf); fclose(f); return 0; }
    }
    fclose(f);
    free(v->dados);
    v->dados = buf;
    v->tamanho = h.tamanho;
    v->capacidade = h.tamanho;
    v->prox_id = h.prox_id;
    return 1;
}

/* ---------- Menus ---------- */
static void menu_clientes(VClientes *vc) {
    for (;;) {
        NL;
        puts("=== MENU CLIENTES ===");
        puts("1 - Inserir");
        puts("2 - Listar");
        puts("3 - Editar");
        puts("4 - Remover");
        puts("0 - Voltar");
        int op; ler_int("Escolha: ", &op);
        switch (op) {
            case 1: cliente_inserir(vc); break;
            case 2: cliente_listar(vc); break;
            case 3: cliente_editar(vc); break;
            case 4: cliente_remover(vc); break;
            case 0: return;
            default: puts("Opção inválida.");
        }
    }
}

static void menu_produtos(VProdutos *vp) {
    for (;;) {
        NL;
        puts("=== MENU PRODUTOS ===");
        puts("1 - Inserir");
        puts("2 - Listar");
        puts("3 - Editar");
        puts("4 - Remover");
        puts("5 - Ajustar estoque (+/-)");
        puts("0 - Voltar");
        int op; ler_int("Escolha: ", &op);
        switch (op) {
            case 1: produto_inserir(vp); break;
            case 2: produto_listar(vp); break;
            case 3: produto_editar(vp); break;
            case 4: produto_remover(vp); break;
            case 5: produto_ajustar_estoque(vp); break;
            case 0: return;
            default: puts("Opção inválida.");
        }
    }
}

int main(void) {
    VClientes vc; vclientes_init(&vc);
    VProdutos vp; vprodutos_init(&vp);

    const char *ARQ_CLIENTES = "clientes.bin";
    const char *ARQ_PRODUTOS = "produtos.bin";

    for (;;) {
        NL;
        puts("==== SISTEMA SIMPLES (CLIENTES/PRODUTOS) ====");
        puts("1 - Clientes");
        puts("2 - Produtos");
        puts("3 - Salvar tudo");
        puts("4 - Carregar tudo");
        puts("0 - Sair");
        int op; ler_int("Escolha: ", &op);

        switch (op) {
            case 1: menu_clientes(&vc); break;
            case 2: menu_produtos(&vp); break;
            case 3: {
                int ok1 = salvar_clientes(&vc, ARQ_CLIENTES);
                int ok2 = salvar_produtos(&vp, ARQ_PRODUTOS);
                puts((ok1 && ok2) ? "Dados salvos." : "Falha ao salvar.");
            } break;
            case 4: {
                int ok1 = carregar_clientes(&vc, ARQ_CLIENTES);
                int ok2 = carregar_produtos(&vp, ARQ_PRODUTOS);
                puts((ok1 && ok2) ? "Dados carregados." : "Falha ao carregar.");
            } break;
            case 0:
                puts("Saindo...");
                vclientes_free(&vc);
                vprodutos_free(&vp);
                return 0;
            default:
                puts("Opção inválida.");
        }
    }
    return 0;
}
