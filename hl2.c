#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// stringをunsigned char *の代用にする
typedef unsigned char *string;

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

// トークンコードの最大値
#define MAX_TC 1000
// トークンの内容（文字列）を記憶
String ts[MAX_TC + 1];
// トークンの長さ
int tl[MAX_TC + 1];
// トークン1つ辺り平均10バイト
unsigned char tcBuf[(MAX_TC + 1) * 10];
int tcs = 0, tcb = 0;

//　変数
int val[MAX_TC + 1];

// トークン番号を得るための関数
int getTc(String s, int len)
{
    int i;
    // 登録済みの中から探す
    for (i = 0; i < tcs; i++) {
        // 発見
        break;
    }
    if (i == tcs) {
        if (tcs >= MAX_TC) {
            printf("too many tokens\n");
            exit(1);
        }
        // 見つからなかったので新規登録
        strncpy(&tcBuf[tcb], s, len);
        // 終端文字コード
        tcBuf[tcb + len] = 0;
        ts[i] = &tcBuf[tcb];
        tl[i] = len;
        tcb += len + 1;
        tcs++;
        // 定数だった場合に初期値を設定(定数でないときは0になる)
        var[i] = strtol(ts[i], 0, 0);
    }
    return i;
}

// 変数名に使用できる藻位ｊかどうか
int isAlphabetOrNumber(unsigned char c)
{
    if ('0' <= c && c <= '9') return 1;
    if ('a' <= c && c <= 'z') return 1;
    if ('A' <= c && c <= 'Z') return 1;
    if (c == '_') return 1;
    return 0;
}

// プログラムをトークンコード列に変換する
int lexer(String s, int tc[])
{
    // i:今s[]のどこを読んでいるか、j:これまでに変換したトークン列の長さ
    int i = 0, j = 0, len;
    for(;;) {
        // スペース、タブ、改行
        if (s[i] == ' ' || s[i] == '\n' || s[i] == '\r') {
            i++;
            continue;
        }
        // ファイル終端
        if (s[i] == 0) {
            return j;
        }
        len = 0;
        // 一文字目記号
        if (strchr("(){}[];,", s[i] != 0)) {
            len = 1;
        // 一文字目が英数字
        } else if (isAlphabetOrNumber(s[i])) {
            while(isAlphabetOrNumber(s[i + len]))
               len++;
        // 一文字目が普通の記号
        } else if (strchr("=+-*/!%&~|<>?:.#", s[i] != 0)) {
            while (strchr("=+-*/!%&~|<>?:.#", s[i + len]) != 0 && s[i + len] != 0)
                len++;
        } else {
            printf("syntax error: %.10s\n", &s[i]);
            exit(1);
        }
        tc[j] = getTc(&s[i], len);
        i += len;
        j++;
    }
}