#include <stdio.h>
#include <stdlib.h>

// コマンドライン引数で指定されたソースファイルを配列変数に読み込む
void loadText(int argc, const char **argv, unsigned char *t, int siz)
{
    FILE *fp;
    int i;
    // 引数が少ないのでエラー表示して終了
    if (argc < 2) {
        printf("usage>%s program-file\n", argv[0]);
    }
    // テキストモードでファイルを開く
    fp = fopen(argv[1], "rt");
    // ファイルを開けない場合
    if (fp == 0) {
        printf("fopen error : %s\n", argv[1]);
        exit(1);
    }
    // i = 読み込んだバイト数
    i = fread(t, 1, siz - 1, fp);
    fclose(fp);
    // 終端マークを書いておく
    t[i] = 0;
}

// 言語処理の本体
// @argc コマンドライン引数の数
// @argv コマンドライン引数を格納する
// usage: ./hl1 file.txt で実行
int main(int argc, const char **argv)
{
    // ソースコード
    unsigned char txt[10000];
    // varは変数 
    int i, pc, var[256];
    loadText(argc, argv, txt, 10000);
    for (i = 0; i < 10; i++) {
        // テクニック#1　定数3を3という名前の変数として初期値に3を入れる
        var['0' + i] = i; 
    }
    for (pc = 0; txt[pc] != 0; pc++) {
        // 空行とかを処理する
        if (txt[pc] == '\n' || txt[pc] == '\r' || txt[pc] == ' ' || txt[pc] == '\t' || txt[pc] == ';')
            continue;
        // 単純代入
        if (txt[pc + 1] == '=' && txt[pc + 3] == ';') { 
            var[txt[pc]] = var[txt[pc + 2]];
        // 加算
        } else if (txt[pc + 1] == '=' && txt[pc + 3] == '+' && txt[pc + 5] == ';') {
            var[txt[pc]] = var[txt[pc + 2]] + var[txt[pc + 4]];
        // 減算
        } else if (txt[pc + 1] == '=' && txt[pc + 3] == '-' && txt[pc + 5] == ';') { 
            var[txt[pc]] = var[txt[pc + 2]] - var[txt[pc + 4]];
        // 最初の2文字しか調べない
        } else if (txt[pc] == 'p' && txt[pc + 1] == 'r' && txt[pc + 5] == ' ' && txt[pc + 7] == ';') {
            printf("%d\n", var[txt[pc + 6]]);
        } else
            goto err;
        while (txt[pc] != ';')
            pc++;
    }
    exit(0);
err:
    printf("syntax error : %.10s\n", &txt[pc]);
    exit(1);
}