#include <iostream>
#include <stdio.h>     
#include <stdlib.h> 
#include "Profiler.h"


int timp = 0;
int nrc = 0;
int nrop = 0;

//pentru parcurgerea dfs

enum {
    WHITE, GRAY, BLACK
};

// structura pentru noduri (linked list)

typedef struct nod
{
    int comp;
    int val;
    struct nod* next;
} node;

// structura pentru arbore 

typedef struct
{
    int n;
    node** t;
    int* d;
    int* f;
    int* color;
} Graf;


//functii pentru lista simplu inlantuita

void insert_last(node** first, node** last, int key)
{
    node* p = (node*)malloc(sizeof(node));
    p->val = key;
    p->next = NULL;
    p->comp = -1;
    if ((*first) == NULL)
    {
        (*first) = p;
        (*last) = p;
    }
    else
    {
        (*last)->next = p;
        (*last) = p;
    }
}

void insert_first(node** first, int key) {
    node* p = (node*)malloc(sizeof(node));
    p->val = key;
    p->next = *first;
    *first = p;
}

int delete_first(node** first) {
    node* q = *first;
    if (q != NULL)
    {
        *first = (*first)->next;
        int x = q->val;
        free(q);
        return x;
    }
    return 0;
}

// afisare arbore

void print(Graf G)
{
    for (int i = 0; i < G.n; i++)
    {
        node* g = G.t[i]->next;
        printf("[%d]: ", i);
        while (g != NULL)
        {
            printf("%d, ", g->val);
            g = g->next;
        }
        printf("\n");
    }
    printf("\n");
}

// afisare componenta

void print_comp(Graf* G) {
    for (int i = 1; i <= nrc;i++) {
        printf("\nComponenta nr %d: ", i);
        for (int j = 0;j < G->n;j++) {
            if (G->t[j]->comp == i)
                printf("%d, ", j);
        }
    }
}

// returneaza minim

int minim(int a, int b) {
    if (a < b)
        return a;
    else
        return b;
}

// initializeaza un graf cu valorile 0 si il coloreaza cu alb

void init(Graf* G)
{
    for (int i = 0; i < G->n; i++)
    {
        G->color[i] = WHITE;
        G->d[i] = G->f[i] = 0;
    }
}

// DFS

int dfs_visit(int v, Graf* G, int topo, node** tsort, Operation* op)
{
    timp++;
    op->count(2);
    G->color[v] = GRAY;
    G->d[v] = timp;
    node* w = G->t[v]->next;
    op->count();
    while (w != NULL)
    {
        op->count(2);
        if (G->color[w->val] == WHITE) {
            if (dfs_visit(w->val, G, topo, tsort, op) == 0)
                return 0;
        }
        else if (G->color[w->val] == GRAY && topo == 1) {
            printf("\n!NU SE POATE REALIZA SORTAREA!\nMuchia (%d , %d) formeaza un ciclu in graf.\n", v, w->val);
            return 0;
        }
        op->count();
        w = w->next;
    }
    op->count();
    G->color[v] = BLACK;
    if (topo)
        insert_first(tsort, v);
    timp++;
    op->count();
    G->f[v] = timp;
    return 1;
}


node* dfs(Graf* G, int topo, Operation* op)
{
    node* tsort = NULL;
    init(G);
    timp = 0;
    for (int i = 0; i < G->n; i++)
    {
        op->count();
        if (G->color[i] == WHITE)
            if (dfs_visit(i, G, topo, &tsort, op) == 0)
                return NULL;
    }
    return tsort;
}


//Tarjan

void connect(int v, Graf* G, node** s, int* lowlink, int* onStack) {
    G->color[v] = GRAY;
    G->d[v] = lowlink[v] = timp;
    timp++;
    insert_first(s, v);
    onStack[v] = 1;
    node* p = G->t[v]->next;
    while (p != NULL)
    {
        int w = p->val;
        if (G->color[w] == WHITE) {
            connect(w, G, s, lowlink, onStack);
            lowlink[v] = minim(lowlink[v], lowlink[w]);
        }
        else if (onStack[w])
            lowlink[v] = minim(lowlink[v], lowlink[w]);
        p = p->next;
    }

    if (lowlink[v] == G->d[v]) {
        nrc++;
        printf("\nComponenta nr %d: ", nrc);
        int w;
        do {
            w = delete_first(s);
            printf("%d, ", w);
            onStack[w] = 0;
            G->t[w]->comp = nrc;
        } while (w != v);
    }
    G->color[v] = BLACK;
}


void tarjan(Graf* G) {
    timp = 0;
    int* lowlink = (int*)malloc(G->n * sizeof(int));
    int* onStack = (int*)calloc(G->n, sizeof(int));
    for (int i = 0; i < G->n; i++) {
        lowlink[i] = -1;
    }
    init(G);
    node* first = NULL;
    for (int i = 0; i < G->n; i++)
    {
        if (G->color[i] == WHITE)
            connect(i, G, &first, lowlink, onStack);
    }
    printf("\n");
    free(lowlink);
    free(onStack);
}

// sortareTopologica arbore

void sortareTopologica(Graf* G) {
    Profiler g("x");
    Operation x = g.createOperation("x", 0);

    node* p = dfs(G, 1, &x);
    if (p)
    {
        printf("Sortare topologica:");
        while (p != NULL)
            printf("%d ---> ", delete_first(&p));
        printf("\n");
    }
}


//Graf

Graf* graf(int n, int m) {
    Graf* G = (Graf*)malloc(sizeof(Graf));
    G->t = (node**)calloc(n, sizeof(node*));
    G->d = (int*)calloc(n, sizeof(int));
    G->f = (int*)calloc(n, sizeof(int));
    G->color = (int*)calloc(n, sizeof(int));
    node** last = (node**)malloc((n) * sizeof(node*));
    G->n = n;
    init(G);
    for (int i = 0; i < n; i++)
        insert_last(&(G->t[i]), &last[i], i);
    for (int i = 0; i < m; i++) {
        int u, v;
        int gasit = 1;
        //  srand(i);
        while (gasit == 1)
        {
            u = rand() % n;
            v = rand() % n;

            if (u != v) {
                gasit = 0;
                node* w = G->t[u]->next;
                while (w != NULL) {
                    if (w->val == v)
                    {
                        gasit = 1;
                        break;
                    }
                    w = w->next;
                }
            }
        }
        insert_last(&(G->t[u]), &last[u], v);
    }
    free(last);
    return G;
}

// masuratori

void perf() {
    Profiler p("DFS");

    for (int n = 1000; n <= 4500; n += 100) {
        Operation op = p.createOperation("DFS_E", n);
        Graf* G;
        G = graf(100, n);
        dfs(G, 0, &op);
        //free_graph(&graph);
    }

    for (int n = 100; n <= 200; n += 10) {
        Operation op = p.createOperation("DFS_V", n);
        Graf* G;
        G = graf(n, 4500);
        dfs(G, 0, &op);
        //free_graph(&graph);
    }

    p.showReport();
}

int main()
{
    perf();
    Graf* G;
    G = graf(4, 3);
    print(*G);
    sortareTopologica(G);
    tarjan(G);
    return 0;
}