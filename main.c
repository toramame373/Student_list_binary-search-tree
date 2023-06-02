#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define FALSE -1
#define FALSEUSINGMEMORY -2
#define TRUE 1

typedef struct school_record SRec; /*typedefによる構造体の定義*/

struct school_record {
    float       gpa; /*累積GPA(Grade Point Average)*/
    int      credit; /*習得単位数*/
    char  name[200]; /*氏のみ、英数字で199文字以内*/
    SRec      *next; /*連結リストにおいて次の要素を指すポインタ*/
    SRec *left, *right; /*二分探索木用のポインタ*/
};

int compGpa(const void *a, const void *b);    /*sort用のgpaによる比較*/
int compCredit(const void *a, const void *b); /*sort用のcreditによる比較*/
int compName(const void *a, const void *b);   /*sort用のnameによる比較*/

SRec *input(const char *fname);
void output(const char *fname, SRec *list);
SRec *list_sort_by_tree(SRec *list, int (*comp)(const void *, const void *)); /*二分探索木によって連結リストを昇順にソートする関数*/
SRec *scan(SRec *tree, SRec *next); /*部分木を線形リストに変換する関数*/

void free_list(SRec *list); /*malloc関数で確保した連結リストの領域を開放する関数*/

void dump_list(SRec *list); /*デバック用出力関数*/

int main(int argc, char *argv[]) {
    clock_t clk_start, clk_end;
    SRec *linkedlist; /*構造体の連結リストの宣言*/

    if(argc != 4) {
        fprintf(stderr,"Usage: %s sorting_rule infile outfile\n","program name");
        return FALSE;
    }

    linkedlist = input(argv[2]); /*連結リストの先頭へのポインタを受け取る*/
    if(linkedlist == NULL) {
        free_list(linkedlist); /*開放した分の領域を解放して実行を終了。メモリを確保していない場合に実行しても関数内の条件分岐で処理が実行されないので、ヌルポインタにfree()が実行されることはない*/
        return FALSE;
    }

    if(!strcmp(argv[1], "gpa")) {           /*コマンドラインの第一引数がgpaと一致しているか判定*/
        linkedlist = list_sort_by_tree(linkedlist,compGpa);
    } else if(!strcmp(argv[1], "credit")) { /*コマンドラインの第一引数がcreditと一致しているか判定*/
        linkedlist = list_sort_by_tree(linkedlist,compCredit);
    } else if(!strcmp(argv[1], "name")) {   /*コマンドラインの第一引数がnameと一致しているか判定*/
        clk_start = clock();
        linkedlist = list_sort_by_tree(linkedlist,compName);
        clk_end = clock();
    } else {
        fprintf(stderr,"Sorting was not done due to inappropriate sorting rules.\n");  /*ソートは行われず入力ファイルがそのまま出力される*/
    }

    printf("calculating time: %fsec\n",(double)(clk_end - clk_start) / CLOCKS_PER_SEC);

//    dump_list(linkedlist);
    output(argv[3],linkedlist);

    free_list(linkedlist); /*使用したメモリを解放して実行を終了*/
    return 0;
}

SRec *input(const char *fname) {
    FILE *fp;

    SRec  *head = NULL;  /*連結リストの先頭を指すポインタ*/
    SRec **tail = &head; /*連結リストの末尾を表すポインタのポインタ*/
    SRec temp; /*入力データを一時的に保管*/
    int count = 0; /*入力データの状態を判断するための変数*/

    if((fp = fopen(fname,"r")) == NULL) { /*ファイルが開けない場合はエラー*/
        fprintf(stderr,"Error: Cannot open infile.\n");
        return NULL; /*エラー処理のためにヌルポインタを返す*/
    }

    while(fscanf(fp, "%f %d %s",&temp.gpa,&temp.credit,temp.name) == 3) { /*入力データの終わりがくれば入力終了*/

        SRec  *p; /*連結リストの一要素*/
        if((p = (SRec *)malloc(sizeof(SRec))) == NULL) { /*必要なメモリの領域を要素ごとに確保*/
            fprintf(stderr,"Error: Memory cannot be allocated.\n");
            return NULL; /*エラー処理のためにヌルポインタを返す*/
        }

        p->gpa    =      temp.gpa; /*要素を代入*/
        p->credit =   temp.credit;
        strcpy(p->name,temp.name);

        p->next = NULL;
        *tail = p;                 /*ループが1回目のときはheadがpが指すセルを指すようになる。2回目以降では末尾の次がpになる*/
        tail = &(p->next);
        count++;
    }

    if(count == 0) {
        fprintf(stderr,"The order of the data may be incorrect, or the data in the input file may be empty.\nusage:gpa credit name\n"); /*データの並び順が間違っている、もしくはデータの中身が空である*/
        return NULL;
    }

    return head; /*連結リストの先頭へのポインタを返す*/
}

void output(const char *fname, SRec *list) {
    FILE *fp;
    SRec *p;

    fp = fopen(fname,"w");

    if(list != NULL) { /*listが空でなければ出力する*/
        for(p = list; p != NULL; p = p->next) {
            fprintf(fp, "%5.3f %3d %10s\n", p->gpa,p->credit,p->name); /*GPAは小数点以下２位まで、習得単位数は３桁表示*/
        }

        printf("Completed writing to the file.\n"); /*ファイルへの書き込みが完了*/
        fclose(fp);
    } else {
        fprintf(stderr,"The list is empty and writing to the file was not done.\n");
        fclose(fp);
    }
}

void free_list(SRec *list) { /*malloc関数で確保した連結リストの領域を開放する関数*/
    if(list == NULL) {
        return;
    } else {
        free_list(list->next);
        free(list);
    }
}

int compGpa(const void *a, const void *b) { /*gpaを比較して大小関係を返す*/
    if (((SRec *) a)->gpa > ((SRec *) b)->gpa) {
        return TRUE;
    } else if (((SRec *) a)->gpa < ((SRec *) b)->gpa) {
        return FALSE;
    } else {
        return 0;
    }
}

int compCredit(const void *a, const void *b) { /*creditを比較して大小関係を返す*/
    if (((SRec *) a)->credit > ((SRec *) b)->credit) {
        return TRUE;
    } else if (((SRec *) a)->credit < ((SRec *) b)->credit) {
        return FALSE;
    } else {
        return 0;
    }
}

int compName(const void *a, const void *b) { /*nameを比較して前後関係を返す*/
    return strcmp(((SRec *)a)->name,((SRec *)b)->name); /*a > b なら正の値、a < b なら負の値、a = b なら０を返す（大小関係は文字コードに依存する）*/
}

SRec *scan(SRec *tree, SRec *next) { /*部分木を線形リストに変換する関数*/
    if(tree == NULL) {
        return next;
    }
    tree->next = scan(tree->right,next); /*右部分木をtree->nextにつなぐ*/

    return scan(tree->left,tree);        /*左部分木をtreeにつなぐ。関数の返り値は線形リストの先頭を指すポインタ*/
}

SRec *list_sort_by_tree(SRec *list, int (*comp)(const void *, const void *)) { /*二分探索木によって連結リストを昇順にソートする関数*/
    SRec *root = NULL; /*作成する二分探索木の根*/
    SRec *node;        /*挿入するノードへのポインタ*/
    SRec *sorted;      /*ソート後リストの先頭へのポインタ*/
    SRec **p;
    int order; /*比較結果を保存する変数*/

    if(list == NULL) { /*受け取ったポインタが空ならそのまま返して終了*/
        return list;
    }

    root = list;       /*根＝線形リストの先頭のセル*/
    node = root->next; /*挿入するノードは２番目のセルから末尾のセルまで*/

    while(node != NULL) {
        p = &root;

        while(*p != NULL) {
            order = comp(node,*p);
            if(order < 0) { /*比較対象よりも小さい場合*/
                p = &((*p)->left);
            } else {        /*比較対象よりも大きい、または同じ場合*/
                p = &((*p)->right);
            }
        }

        node->left = NULL;  /*二分木に登録*/
        node->right = NULL;
        *p = node;
        node = node->next;
    }
    sorted = scan(root,NULL);

    return sorted; /*ソート後リストの先頭へのポインタを返す*/
}

void dump_list(SRec *list) { /*デバック用出力関数*/
    printf("Dump information of binary search tree\n");
    puts("-------------------------------------------------------------------------");
    while (list != NULL) {
        printf("(%5.3f %3d %10s) n:%p l:%14p r:%14p \n",
               list->gpa, list->credit, list->name, list, list->left, list->right);
        list = list->next;
    }
    puts("");
}