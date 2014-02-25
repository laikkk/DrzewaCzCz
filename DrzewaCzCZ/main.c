// Kamil Zieliński 215521 
// Drzewa cz-cz wstawiania + procedury ( zliczanie czerwonych wezłów, max głebokosc licia , min głębokość liścia)
//  pmp@inf.ug.edu.pl  2014
// drukowanie drzew cz-cz z wartownikiem wspolnym
// dla wszystkich wezlow:
// drukowanie na ekran tekstowy i przez dot

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED 1        /* stala oznaczajaca kolor wezla */
#define BLACK 0      /* stala oznaczajaca kolor wezla */
#define SZER_EKR 140  /* szerokosc ekranu */
#define IL_POZ   15   /* ilosc poziomow drzewa, ktore beda wydrukowane   */
/* gwiazdka bedzie sygnalizowac istnienie nizszych */
/* poziomow                                        */

/* struktury danych do reprezentowania drzewa */
typedef struct wezel *Wskwezla; /* wskaznik na wezel drzewa  */

typedef struct wezel
{
    int klucz;
    Wskwezla left, right, p;
    int kolor;
} Twezla; /* typ wezla */




/* drzewa z wartownikami: wezel wskazywany przez "nil" jest wartownikiem 
   zastepujacym NULL; dla korzenia pole "p" ma wartosc "nil"; 
   pole nil->p musi byc ustawione odpowiednio w przy usuwaniu 
 */
Wskwezla nil;

void nilInit(void)
{
    /* funkcja inicjujaca nil; musi byc wywolana przed budowaniem drzewa */
    nil = (Wskwezla)malloc(sizeof (Twezla));
    nil->p = NULL;
    nil->kolor = BLACK;
    nil->left = nil->right = NULL;
}

void drukuj(Wskwezla w);
/* funkcja drukujaca drzewo binarne na ekranie (tutaj tylko deklaracja) */
/* funkcja drukuje drzewo o korzeniu "w"                                */

void drukujDot(Wskwezla r);
// generuje w plikach drzewo0.gv,  drzewo1.gv ...
// opis drzew o korzeniu r do wydrukowania przez program dot
// zlecenie "dot -Tpdf -O drzewo1.gv" utworzy plik "drzewo1.gv.pdf" 

/* ------------  implementacja ------------------------------------- */
char wydruk[IL_POZ+1][SZER_EKR];

void drukujost(Wskwezla w, int l, int p, int poziom)
{
    int srodek = (l+p)/2;
    if (w==nil) return;
    wydruk[poziom][srodek] = '*';
}

void drukujwew(Wskwezla w, int l, int p, int poziom)
{
    int srodek = (l+p)/2;
    int i, dl;
    char s[19];
    if (w==nil) return;
    if (w->kolor==BLACK)
        dl = sprintf(s, "%d", w->klucz);
    else
        //	        dl=sprintf(s,"\e[31m%+d\e[0m",w->klucz);
        dl = sprintf(s, "%+d", w->klucz);
    for (i = 0; i<dl; i++)
        wydruk[poziom][srodek-dl/2+i] = s[i];
    if (++poziom<IL_POZ)
    {
        drukujwew(w->left, l, srodek, poziom);
        drukujwew(w->right, srodek+1, p, poziom);
    }
    else
    {
        drukujost(w->left, l, srodek, poziom);
        drukujost(w->right, srodek+1, p, poziom);
    }
}

void drukuj(Wskwezla w)
{
    int j, i;
    for (i = 0; i<=IL_POZ; i++)
        for (j = 0; j<SZER_EKR; j++)
            wydruk[i][j] = ' ';
    drukujwew(w, 0, SZER_EKR, 0);
    for (i = 0; i<=IL_POZ; i++)
    {
        for (j = 0; j<SZER_EKR; j++)
            putchar(wydruk[i][j]);
        printf("\n");
    }
}

void drukujKrawedz(FILE *plikwy, Wskwezla r, int z, Wskwezla syn, int zs)
{
    // wezel r o numerze z jest juz wydrukowany
    // teraz drukujemy jego syna "syn" o numerze zs oraz krawedz miedzy nimi
    if (syn==nil)
    {
        fprintf(plikwy, "%d [shape=circle, style=invisible, label=\"", zs);
        fprintf(plikwy, "%d ", 0);
        fprintf(plikwy, "\"]\n");
        fprintf(plikwy, "%d -- %d [style=invis];\n ", z, zs);
    }
    else
    {
        if (syn->kolor==RED)
            fprintf(plikwy, "%d [shape=circle, color=red, label=\"", zs);
        else
            fprintf(plikwy, "%d [shape=circle, color=black, label=\"", zs);
        fprintf(plikwy, "%d ", syn->klucz);
        fprintf(plikwy, "\"]\n");
        fprintf(plikwy, "%d -- %d ;\n", z, zs);
    }
}

int rekDrukujDot(Wskwezla r, int z, FILE *plikwy)
{
    // drukuje drzewo o korzeniu r 
    // z  - numer wezla r
    // zwraca najwiekszy numer wezla w poddrzewie, ktorego jest korzeniem
    // zakladamy, ze korzen r jest juz wydrukowany
    int nz;
    if (r==nil) return z;
    else
    {
        drukujKrawedz(plikwy, r, z, r->left, z+1);
        nz = rekDrukujDot(r->left, z+1, plikwy);
        drukujKrawedz(plikwy, r, z, r->right, nz+1);
        nz = rekDrukujDot(r->right, nz+1, plikwy);
        return nz;
    }
}

void drukujDot(Wskwezla r)
{
    // generuje w plikach drzewo0.gv, dzrewo1.gv, ... 
    // opis drzewa o korzeniu r do wydrukowania przez program dot
    // zlecenie "dot -Tpdf -O drzewo1.gv" utworzy plik "drzewo1.gv.pdf" 
    static int gdzie = 0;
    char numer[10];
    char nazwap[20] = "drzewo";
    FILE *plikwy;
    snprintf(numer, 9, "%d", gdzie);
    strcat(nazwap, numer);
    strcat(nazwap, ".gv");
    plikwy = fopen(nazwap, "w");
    gdzie++;
    fprintf(plikwy, "graph drzewo{\n");
    fprintf(plikwy, "size = \"2,20\"");
    if (r!=nil)
    {
        if (r->kolor==RED)
            fprintf(plikwy, "%d [shape=circle, color=red, label=\"", 0);
        else
            fprintf(plikwy, "%d [shape=circle, color=black, label=\"", 0);
        fprintf(plikwy, "%d ", r->klucz);
        fprintf(plikwy, "\"]\n");
        rekDrukujDot(r, 0, plikwy);
    }
    fprintf(plikwy, "}\n");
    fclose(plikwy);
    printf("wydrukowane %s\n", nazwap);
}

/* ------------- miejsce na inne funkcje -------------------- */
void TreeInsert(Wskwezla* T, Wskwezla z)
{
    Wskwezla x = *T,
            y = nil; //Y bedzie zawsze ojcem x'a

    while (x!=nil)
    {
        y = x;
        if (z->klucz<(x->klucz))
            x = x->left;
        else
            x = x->right;
    }
    z->p = y;
    if (y==nil)
        *T = z;
    else if ((z->klucz)<(y->klucz))
        y->left = z;
    else y->right = z;

}

void LeftRotate(Wskwezla* T, Wskwezla x)
{
    Wskwezla y = x->right;
    x->right = y->left;
    if (y->left!=nil)
        y->left->p = x;
    y->p = x->p;
    if (x->p==nil)
        *T = y;
    else if (x==x->p->left)
        x->p->left = y;
    else x->p->right = y;
    y->left = x;
    x->p = y;
}

void RightRotate(Wskwezla* T, Wskwezla y)
{
    Wskwezla x = y->left;
    y->left = x->right;
    if (y->right!=nil)
        x->right->p = y;
    x->p = y->p;
    if (y->p==nil)
        *T = x;
    else if (y==y->p->right)
        y->p->right = x;
    else y->p->left = x;
    x->right = y;
    y->p = x;
}

void RBInsert(Wskwezla* T, Wskwezla x)
{
    TreeInsert(T, x);
    Wskwezla y;
    //kolorowanie jest juz zrobione podczas inicjowania Wezla
    while (x!=*T&&(x->p->kolor==RED))
    {
        if (x->p==x->p->p->left)
        {
            y = x->p->p->right;
            if (y->kolor==RED)
            {
                x->p->kolor = BLACK;
                y->kolor = BLACK;
                x->p->p->kolor = RED;
                x = x->p->p;
            }
            else
            {
                if (x==x->p->right)
                {
                    x = x->p;
                    LeftRotate(T, x);
                }
                x->p->kolor = BLACK;
                x->p->p->kolor = RED;
                RightRotate(T, x->p->p);
            }
        }
        else
        {
            y = x->p->p->left;
            if (y->kolor==RED)
            {
                x->p->kolor = BLACK;
                y->kolor = BLACK;
                x->p->p->kolor = RED;
                x = x->p->p;
            }
            else
            {
                if (x==x->p->left)
                {
                    x = x->p;
                    RightRotate(T, x);
                }
                x->p->kolor = BLACK;
                x->p->p->kolor = RED;
                LeftRotate(T, x->p->p);
            }
        }
    }
    (*T)->kolor = BLACK;
}
int iloscCzeronych = 0;

void IleCzerwonychWezlow(Wskwezla Korzen)
{
    if (Korzen!=NULL)
    {
        IleCzerwonychWezlow(Korzen->left);
        if (Korzen->kolor==RED)iloscCzeronych++;
        IleCzerwonychWezlow(Korzen->right);
    }

}

int glebokoscMax(Wskwezla t, int poziom)
{
    //Zlicza poziom nagłebszego liscia
    int lewy = 0, prawy = 0;
    if (t->left==nil&&t->right==nil) return poziom;
    if (t->left!=nil) lewy = glebokoscMax(t->left, poziom+1);
    if (t->right!=nil) prawy = glebokoscMax(t->right, poziom+1);
    if (lewy>prawy) return lewy;
    else return prawy;
}

int glebokoscMin(Wskwezla t, int poziom)
{
    //zlicza poziom najpłytszego liścia
    int lewy = 0, prawy = 0;
    if (t->left==nil&&t->right==nil) return poziom;
    if (t->left!=nil) lewy = glebokoscMax(t->left, poziom+1);
    if (t->right!=nil) prawy = glebokoscMax(t->right, poziom+1);
    if (lewy<prawy) return lewy;
    else return prawy;
}

/* ----------------- program testujacy -----------------------*/

Wskwezla nowyWezel(int klucz, int kolor)
{
    // tworzy nowy wezel. Przydatne do testowania wydruku
    Wskwezla w = (Wskwezla)malloc(sizeof (Twezla));
    w->p = nil;
    w->klucz = klucz;
    w->kolor = kolor;
    w->left = w->right = nil;
    return w;
}

int main()
{
    /* zbudujemy drzewo   
              5
             / \
            3   8       
               / \
              6   9
       z czerwonym wezlem 8.
       Drukowanie po dolaczeniu kolejnych wezlow. 
       Trzeba naciskac ENTER zeby isc dalej
     */
    /*
        nilInit();
        Wskwezla korzen;
        korzen=nil;
        printf("klucz = %d",korzen->klucz);
        test(korzen);
        printf("klucz = %d",korzen->klucz);
     */

    nilInit();
    Wskwezla korzen, w5, w3, w8, w6, w9;
    korzen = nil;

    drukuj(korzen);

    w5 = nowyWezel(5, BLACK);
    RBInsert(&korzen, w5);
    drukujDot(korzen);
    drukuj(korzen);
    // getchar();

    w3 = nowyWezel(3, RED);
    RBInsert(&korzen, w3);
    drukujDot(korzen);
    drukuj(korzen);
    //getchar();

    w8 = nowyWezel(8, RED);
    RBInsert(&korzen, w8);
    drukujDot(korzen);
    drukuj(korzen);
    //getchar();

    w6 = nowyWezel(6, RED);
    RBInsert(&korzen, w6);
    drukujDot(korzen);
    drukuj(korzen);
    //getchar();

    w9 = nowyWezel(9, RED);
    RBInsert(&korzen, w9);
    drukujDot(korzen);
    drukuj(korzen);
    //getchar();

    Wskwezla w899;
    w899 = nowyWezel(899, RED);
    RBInsert(&korzen, w899);
    drukujDot(korzen);
    drukuj(korzen);
    //getchar();

    Wskwezla w900;
    w900 = nowyWezel(900, RED);
    RBInsert(&korzen, w900);
    drukujDot(korzen);
    drukuj(korzen);
    //getchar();

    Wskwezla w903;
    w903 = nowyWezel(903, RED);
    RBInsert(&korzen, w903);
    drukujDot(korzen);
    drukuj(korzen);
    //getchar();

    Wskwezla w902;
    w902 = nowyWezel(902, RED);
    RBInsert(&korzen, w902);
    drukujDot(korzen);
    drukuj(korzen);
    //getchar();

    Wskwezla w901;
    w901 = nowyWezel(901, RED);
    RBInsert(&korzen, w901);
    drukujDot(korzen);
    drukuj(korzen);
    //getchar();

    IleCzerwonychWezlow(korzen);
    printf("Ilosc Czerwonych Wezlow = %d \n", iloscCzeronych);
    printf("glebokosc max = %d \n", glebokoscMax(korzen, 0));
    printf("glebokosc min = %d \n", glebokoscMin(korzen, 0));
    return 0;
}





