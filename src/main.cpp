#include <cstdlib>
#include <exception>
#include <iostream>
#include <vector>

#include "ast.hpp"
#include "env.hpp"
#include "errors.hpp"
#include "visitor.hpp"

#define VERSION_MAJOR 0
#define VERSION_MINOR 7
#define VERSION_PATCH 0

extern int yyparse();  // Bison 生成的函數
extern FILE* yyin;     // Flex 使用的檔案指針

Vector<StmtPtr> statements;  // 由 parser.y 中的 program 規則填充

void printHelp(const char* program_name) {
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Bivariate Renderer - A simple mathematical expression parser and plotter" << std::endl;
    std::cout << "Usage: " << program_name << "[options] [input_file]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message and exit" << std::endl;
    std::cout << "  -v, --version           Show version information and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Example usage:" << std::endl;
    std::cout << "  " << program_name << " test.math" << std::endl;
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
}

void printVersion() {
    std::cout << "Bivariate Renderer version " << VERSION_MAJOR << "." << VERSION_MINOR << "."
              << VERSION_PATCH << std::endl;
}

void haruhikage() {
#ifdef _WIN32  // Windows
    system("start https://www.youtube.com/watch?v=ZsvJUh03MwI");
#elif __APPLE__  // macOS
    system("open https://www.youtube.com/watch?v=ZsvJUh03MwI");
#elif __linux__  // Linux
    system("xdg-open https://www.youtube.com/watch?v=ZsvJUh03MwI");
#else
    printf("Please visit: https://www.youtube.com/watch?v=ZsvJUh03MwI\n");
#endif
}

int main(int argc, char** argv) {
    if (rand() % 100 < 5) haruhikage();

    // 處理命令行參數，支援選項和輸入檔案
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg[0] == '-') {
                // 這裡可以處理選項，例如 -h, --help, -v, --version 等
                if (arg == "-h" || arg == "--help") {
                    printHelp(argv[0]);
                    return 0;
                } else if (arg == "-v" || arg == "--version") {
                    printVersion();
                    return 0;
                } else {
                    std::cerr << "Error: Unknown option " << arg << std::endl;
                    return 1;
                }
            } else {
                // 第一個非選項參數當作輸入檔案
                if (!yyin) {
                    FILE* file = fopen(arg.c_str(), "r");
                    if (!file) {
                        std::cerr << "Error: Cannot open input file " << arg << std::endl;
                        return 1;
                    }
                    yyin = file;
                }
            }
        }
    }

    // 如果沒有指定輸入檔案，則提示錯誤並顯示幫助訊息
    if (!yyin) {
        std::cerr << "Error: No input file specified." << std::endl;
        printHelp(argv[0]);
        return 1;
    }

    std::cout << "開始解析語法..." << std::endl;

    if (yyparse() == 0) {
        std::cout << "語法解析成功！" << std::endl;
    } else {
        std::cerr << "語法解析失敗！請檢查輸入檔案的格式。" << std::endl;
        return 1;
    }

    fclose(yyin);  // 記得關閉檔案

    Environment global_env = initGlobalEnvironment();  // 全域環境
    std::cout << statements.size() << " 條陳述句已解析。" << std::endl;

    std::cout << "--------------------" << std::endl;
    for (int i = 0; i < statements.size(); i++) {
        auto lineno = statements[i]->getLineno();

        Validator validator(&global_env);
        Folder folder(&global_env);
        Evaluator evaluator(&global_env);
        try {
            std::cout << "原始陳述句 " << i + 1 << ": " << statements[i] << std::endl;
            std::cout << std::endl;

            statements[i]->accept(validator);                                    // 驗證語法和語義
            StmtPtr stmt = static_cast<StmtPtr>(statements[i]->accept(folder));  // 常量折疊
            delete statements[i];                                                // 釋放記憶體

            std::cout << "折疊後的陳述句 " << i + 1 << ": " << stmt << std::endl;
            std::cout << std::endl;

            stmt->accept(evaluator);  // 評估陳述句
            delete stmt;
        } catch (const SemanticError& e) {
            std::cerr << "語意錯誤 (第 " << lineno << " 行): " << e.what() << std::endl;
            return 1;
        } catch (const std::exception& e) {
            std::cerr << "錯誤 (第 " << lineno << " 行): " << e.what() << std::endl;
            return 1;
        }

        std::cout << "--------------------" << std::endl;
    }

    std::cout << "語意驗證成功！" << std::endl;

    return 0;
}
