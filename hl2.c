#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Stringをunsigned char *の代用にする
typedef unsigned char *String;

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

#define MAX_TC  1000 // トークンコードの最大値.
String ts[MAX_TC + 1]; // トークンの内容(文字列)を記憶.
int tl[MAX_TC + 1]; // トークンの長さ.
unsigned char tcBuf[(MAX_TC + 1) * 10]; // トークン1つ当たり平均10バイトを想定.
int tcs = 0, tcb = 0;

//　変数
int var[MAX_TC + 1];

int getTc(String s, int len) // トークン番号を得るための関数.
{
    int i;
    for (i = 0; i < tcs; i++) { // 登録済みの中から探す.
        if (len == tl[i] && strncmp(s, ts[i], len) == 0)
            break; // 発見
    }
    if (i == tcs) {
        if (tcs >= MAX_TC) {
            printf("too many tokens\n");
            exit(1);
        }
        strncpy(&tcBuf[tcb], s, len); // 見つからなかったので新規登録.
        tcBuf[tcb + len] = 0; // 終端文字コード.
        ts[i] = &tcBuf[tcb];
        tl[i] = len;
        tcb += len + 1;
        tcs++;
        var[i] = strtol(ts[i], 0, 0);	// 定数だった場合に初期値を設定（定数ではないときは0になる）.
    }
    return i;
}

int isAlphabetOrNumber(unsigned char c)		// 変数名に使用できる文字かどうか.
{
    if ('0' <= c && c <= '9') return 1;
    if ('a' <= c && c <= 'z') return 1;
    if ('A' <= c && c <= 'Z') return 1;
    if (c == '_') return 1;
    return 0;
}

int lexer(String s, int tc[])		// プログラムをトークンコード列に変換する.
{
    int i = 0, j = 0, len; // i:今s[]のどこを読んでいるか、j:これまでに変換したトークン列の長さ.
    for (;;) {
        if (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r') {	// スペース、タブ、改行.
            i++;
            continue;
        }
        if (s[i] == 0)	// ファイル終端.
            return j;
        len = 0;
        if (strchr("(){}[];,", s[i]) != 0) {	// 1文字記号.
            len = 1;
        } else if (isAlphabetOrNumber(s[i])) {  // 1文字目が英数字.
            while (isAlphabetOrNumber(s[i + len]))
                len++;
        } else if (strchr("=+-*/!%&~|<>?:.#", s[i]) != 0) {  // 1文字目が普通の記号.
            while (strchr("=+-*/!%&~|<>?:.#", s[i + len]) != 0 && s[i + len] != 0)
                len++;
        } else {
            printf("syntax error : %.10s\n", &s[i]);
            exit(1);
        }
        tc[j] = getTc(&s[i], len);
        i += len;
        j++;
    }
}


//　トークンコード
int tc[10000];

int main(int argc, const char **argv)
{
    int pc, pc1;
    unsigned char txt[10000]; // ソースコード用のバッファ.
    loadText(argc, argv, txt, 10000);
    pc1 = lexer(txt, tc);
    tc[pc1] = tc[pc1 + 1] = tc[pc1 + 2] = tc[pc1 + 3] = getTc(".", 1);	// エラー表示用のために末尾にピリオドを登録しておく.
    int semi = getTc(";", 1);
    for (pc = 0; pc < pc1; pc++) { // プログラム実行開始.
        if (tc[pc + 1] == getTc("=", 1) && tc[pc + 3] == semi) { // 単純代入.
            var[tc[pc]] = var[tc[pc + 2]];
        } else if (tc[pc + 1] == getTc("=", 1) && tc[pc + 3] == getTc("+", 1) && tc[pc + 5] == semi) {  // 加算.
            var[tc[pc]] = var[tc[pc + 2]] + var[tc[pc + 4]];
        } else if (tc[pc + 1] == getTc("=", 1) && tc[pc + 3] == getTc("-", 1) && tc[pc + 5] == semi) {  // 減算.
            var[tc[pc]] = var[tc[pc + 2]] - var[tc[pc + 4]];
        } else if (tc[pc] == getTc("print", 5) && tc[pc + 2] == semi) { // print.
            printf("%d\n", var[tc[pc + 1]]);
        } else
            goto err;
        while (tc[pc] != semi)
            pc++;
    }
    exit(0);
err:
    printf("syntax error : %s %s %s %s\n", ts[tc[pc]], ts[tc[pc + 1]], ts[tc[pc + 2]], ts[tc[pc + 3]]);
    exit(1);
}