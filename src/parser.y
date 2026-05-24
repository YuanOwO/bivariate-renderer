%{
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "ast.hpp"
#include "env.hpp"
#include "enums.hpp"

extern int yylex();
extern int yylineno;
void yyerror(const char *s);

extern std::vector<ASTNodePtr> statements;
extern Environment global_env;
%}

/* 加上這行讓 Bison 告訴你是什麼 Token 出了錯 */
%define parse.error verbose

%union {
    double fval;
    std::string* sval;
    int func_id;
    StmtPtr stmt_val;
    ExprPtr expr_val;
    std::vector<ASTNodePtr>* node_list;
    std::vector<ExprPtr>* expr_list;
    std::vector<IdPtr>* id_list;
}

/* 宣告所有在 lexer.l 裡面的 Tokens */
%token<fval> T_NUMBER
%token<sval> T_IDENTIFIER
%token<func_id> T_BUILTIN_FUNC
%token T_PLOT
%token T_PLUS T_MINUS T_MULTIPLY T_DIVIDE T_EXPONENT
%token T_EQUALS T_LPAREN T_RPAREN T_COMMA T_SEMICOLON

%type<stmt_val> statement
%type<expr_val> expression
%type<expr_list> argument_list
%type<id_list> param_list

%destructor { delete $$; }<sval>

/* 定義運算子的結合律與優先級 */
%left T_PLUS T_MINUS
%left T_MULTIPLY T_DIVIDE
%right T_EXPONENT

/* 宣告一個代表"一元運算子"的高優先級虛擬 Token */
%nonassoc UNARY_OP

%%

program:
    program statement {
        // 將每個 statement 的 AST 節點加入全局的 statements 向量
        statements.push_back($2);
    }
    | /* empty */ {
        // 允許程式碼為空
    }
    ;

statement:
    T_IDENTIFIER T_EQUALS expression T_SEMICOLON {
        // 處理變數賦值
        IdPtr var_id = new IdentifierNode(yylineno, *$1);
        $$ = new AssignmentNode(yylineno, var_id, $3);
        delete $1; // 釋放從 lexer 傳來的字串
    }
    | T_IDENTIFIER T_LPAREN param_list T_RPAREN T_EQUALS expression T_SEMICOLON {
        // 處理函數定義
        IdPtr func_id = new IdentifierNode(yylineno, *$1);
        $$ = new FuncDefNode(yylineno, func_id, $3, $6);
        delete $1; // 釋放從 lexer 傳來的字串
    }
    | T_PLOT T_LPAREN expression T_RPAREN T_SEMICOLON {
        // 處理繪圖命令
        $$ = new PlotNode(yylineno, $3);
    }
    ;

expression:
    // ======================================
    // 1. 葉節點：數字與變數
    // ======================================
    T_NUMBER {
        // 例如: 3.14
        $$ = new NumberNode(yylineno, $1);
    }
    | T_IDENTIFIER {
        // 例如: my_var (使用者自訂變數)
        $$ = new IdentifierNode(yylineno, *$1);
        delete $1; // 釋放從 lexer 傳來的字串
    }
    // ======================================
    // 2. 括號與函數呼叫
    // ======================================
    | T_LPAREN expression T_RPAREN {
        // 例如: (a + b)
        $$ = $2; // 括號不會產生新的 AST 節點，只是改變運算順序
    }
    /* | T_BUILTIN_FUNC T_LPAREN expression T_RPAREN {
        // 例如: sin(x)
    } */
    | T_IDENTIFIER T_LPAREN argument_list T_RPAREN {
        // 例如: my_func(a, b) (呼叫自訂函數)
        IdPtr func_id = new IdentifierNode(yylineno, *$1);
        $$ = new FuncCallNode(yylineno, func_id, $3);
        delete $1; // 釋放從 lexer 傳來的字串
    }
    // ======================================
    // 3. 二元運算
    // ======================================
    | expression T_PLUS expression { 
        // 加法
        $$ = new BinaryOpNode(yylineno, OP_PLUS, $1, $3);
    }
    | expression T_MINUS expression { 
        // 減法
        $$ = new BinaryOpNode(yylineno, OP_MINUS, $1, $3);
    }
    | expression T_MULTIPLY expression { 
        // 乘法
        $$ = new BinaryOpNode(yylineno, OP_MULTIPLY, $1, $3);
    }
    | expression T_DIVIDE expression { 
        // 除法
        $$ = new BinaryOpNode(yylineno, OP_DIVIDE, $1, $3);
    }
    | expression T_EXPONENT expression { 
        // 指數
        $$ = new BinaryOpNode(yylineno, OP_EXPONENT, $1, $3);
    }
    // ======================================
    // 4. 一元運算
    // ======================================
    | T_PLUS expression %prec UNARY_OP { 
        // 一元正號
        $$ = new UnaryOpNode(yylineno, OP_PLUS, $2);
    }
    | T_MINUS expression %prec UNARY_OP { 
        // 一元負號
        $$ = new UnaryOpNode(yylineno, OP_MINUS, $2);
    }
    ;

param_list:
    param_list T_COMMA T_IDENTIFIER {
        // 處理函數參數列表
        $$->push_back(new IdentifierNode(yylineno, *$3)); // 將新的參數加入向量
        delete $3; // 釋放從 lexer 傳來的字串
    }
    | T_IDENTIFIER {
        // 處理單個參數
        $$ = new std::vector<IdPtr>(); // 初始化向量
        $$->push_back(new IdentifierNode(yylineno, *$1)); // 將第一個參數
        delete $1; // 釋放從 lexer 傳來的字串
    }
    | /* empty */ {
        // 處理空參數列表
        $$ = new std::vector<IdPtr>(); // 初始化為空向量
    }
    ;

argument_list:
    argument_list T_COMMA expression {
        // 處理函數參數列表
        $$->push_back($3); // 將新的參數加入向量
    }
    | expression {
        // 處理單個參數
        $$ = new std::vector<ExprPtr>(); // 初始化向量
        $$->push_back($1); // 將第一個參數加入向量
    }
    | /* empty */ {
        // 處理空參數列表
        $$ = new std::vector<ExprPtr>(); // 初始化為空向量
    }
    ;

%%

void yyerror(const char *s) {
    std::cerr << "語法解析錯誤 (第 " << yylineno << " 行): " << s << std::endl;
}
