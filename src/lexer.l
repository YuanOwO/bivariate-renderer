%{
#include <cstdlib>
#include <string>

#include "ast.hpp"
#include "enums.hpp"
#include "parser.tab.hpp"
%}

%x COMMENT SINGLE_LINE_COMMENT
%option noyywrap nodefault yylineno

/* 基本的正則表達式定義 */
ALPHA       [a-zA-Z]
DIGIT       [0-9]
SPACE       [ \t\v\f\n\r]

/**
 * 數字的正則表達式定義
 * 不允許冗餘的前導零，除非數字是0本身
 * 尚不支援科學記號法（如1e10）
 *
 * 0, 123, 0.456, 123.456, 123., .456 都是合法的數字格式
 * 0123, 00.456, 01.456 等都是非法的數字格式
 */
FLOAT       (0|[1-9]{DIGIT}*)\.?{DIGIT}*|\.{DIGIT}+

/* 變量名稱由字母開頭，後面可以跟字母或數字 */
ID          {ALPHA}({ALPHA}|{DIGIT})*

%%
    /* 關鍵字 */

"plot"      { return T_PLOT; }

    /* 運算符和分隔符 */

"+"         { return T_PLUS; }
"-"         { return T_MINUS; }
"*"         { return T_MULTIPLY; }
"/"         { return T_DIVIDE; }
"^"         { return T_EXPONENT; }
"="         { return T_EQUALS; }
"("         { return T_LPAREN; }
")"         { return T_RPAREN; }
","         { return T_COMMA; }
";"         { return T_SEMICOLON; }

    /* 處理數字和識別符 */

{FLOAT}     { yylval.fval = atof(yytext); return T_NUMBER; }
{ID}        { yylval.sval = new std::string(yytext); return T_IDENTIFIER; }


    /* 處理註釋 */

<INITIAL>"/*"   { BEGIN(COMMENT); }
<COMMENT>"*/"   { BEGIN(INITIAL); }
<COMMENT>.|\n   { /* 忽略註釋內容 */ }

<INITIAL>"//"               { BEGIN(SINGLE_LINE_COMMENT); }
<SINGLE_LINE_COMMENT>\n     { BEGIN(INITIAL); }
<SINGLE_LINE_COMMENT>.      { /* 忽略單行註釋內容 */ }

    /* 其他 */

{SPACE}+        { /* 忽略空白字符 */ }
.               { return yytext[0]; /* 返回未識別的字符 */ } 

%%
