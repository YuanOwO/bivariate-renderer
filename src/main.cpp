#include <exception>
#include <iostream>
#include <vector>

#include "ast.hpp"
#include "env.hpp"
#include "errors.hpp"

#define VERSION_MAJOR 0
#define VERSION_MINOR 6
#define VERSION_PATCH 1

extern int yyparse();  // Bison 生成的函數
extern FILE* yyin;     // Flex 使用的檔案指針

std::vector<StmtPtr> statements;  // 由 parser.y 中的 program 規則填充

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

int main(int argc, char** argv) {
    if (argc > 1) {
        // 處理命令行參數，支援選項和輸入檔案
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

    for (size_t i = 0; i < statements.size(); i++) {
        auto stmt = statements[i];
        // std::cout << stmt << std::endl << std::endl;

        try {
            stmt->validate(global_env);  // 驗證語法和語義
            // WARNING: 這裡直接修改了 statements 中的指針，請確保 fold 返回的新節點是動態分配的，
            // 並且原始節點在 fold 內部被適當處理（例如刪除或不再使用），以避免記憶體洩漏或使用已刪除的物件。
            statements[i] = stmt->fold(global_env);  // 常量折疊
            statements[i]->evaluate(global_env);     // 評估陳述句，這裡的返回值可以根據需要進行處理
            delete stmt;                             // 釋放原始陳述句的記憶體

            std::cout << "折疊後的陳述句 " << i + 1 << ": " << statements[i] << std::endl;
            std::cout << "\n==================================================\n" << std::endl;

            // stmt->evaluate(global_env);  // 評估陳述句，這裡的返回值可以根據需要進行處理
        } catch (const SemanticError& e) {
            std::cerr << "語意錯誤 (第 " << stmt->getLineno() << " 行): " << e.what() << std::endl;
            return 1;
        } catch (const std::exception& e) {
            std::cerr << "錯誤 (第 " << stmt->getLineno() << " 行): " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}
