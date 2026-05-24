# Bivariate Renderer （雙變量函數數學渲染器）

使用者可以輸入自訂的數學表達式、變數或函數定義，系統會進行詞法、語法、語意分析與常數優化，並將二元函數（Bivariate Functions, $z = f(x, y)$）採樣並渲染為二維熱力圖（Heatmap）。

本專案使用單標頭檔開源庫 `stb_image_write.h` 進行點陣圖輸出。

## ✨ 核心功能與技術特點

### 1. 前端編譯架構 (Lexical & Syntactic Analysis)

- **詞法分析**：使用 **Flex** 將輸入文本切分為 Token。
- **語法分析**：使用 **Bison (LALR Parser)** 進行語法解析並構建抽象語法樹（Abstract Syntax Tree, AST）。
- **語句支援**：支援變數賦值、自訂多參數函數定義、以及 `plot` 繪圖語句。

### 2. 語意分析 (Semantic Validation)

在抽象語法樹執行前，系統會對各節點進行深度優先遍歷的語意檢查，以確保執行期的正確性：

- **變數安全性檢查**：防止在變數賦值時引入未知的自由變數（例如：在 `x` 未定義前，不允許 `a = x + 1`）。
- **重複定義防護**：禁止對已存在的變數或函數名稱進行二次重複定義。
- **未宣告變數偵測**：若表達式中使用了未宣告的識別碼（Identifier），會在編譯期拋出語意錯誤（Semantic Error）並回報對應的行號。

### 3. 常數摺疊優化 (Constant Folding)

為減少後端渲染時對語法樹進行重複評估的開銷，系統實現了 AST 的常數摺疊優化：

- 在驗證完語意後、執行前，系統會自動簡化抽象語法樹節點。
- **基礎摺疊**：例如輸入 `5 + 8` 的語法樹節點會在編譯階段被壓縮為單一的 `NumberNode(13)`。
- **代數規則優化**：支援特定邊界規則優化（例如 `x * 0 => 0`、`x ^ 0 => 1`），免去子樹的走訪與計算。

### 4. Colormap 圖片輸出 (Image Generation)

當解析到 `plot <expression>` 語句時，引擎會自動轉換為二維空間採樣邏輯：

- **動態範圍歸一化**：自動計算全局 $Z$ 軸的 `z_min` 與 `z_max`，將數值正規化到 `[0.0, 1.0]` 區間，並處理常數函數導致除以零或無限大（INF/NaN）等邊界狀況。
- **色彩映射**：內建 **Viridis Colormap** 顏色映射表，將歸一化後的比值轉換為平滑的多點漸層。
- **PNG 檔案輸出**：精確渲染出 $600 \times 600$ 像素（解析度可調）的點陣圖，並在生成後調用作業系統內建指令（如 Windows 的 `start` 或 macOS 的 `open`）彈出視窗展示圖片。

## 🚀 編譯與執行

### 前置需求

- cmake 3.31.0+ (建議使用 CMake 進行專案管理)
- 支援 C++11 以上
    - GCC version 15.2.0 (Rev14, Built by MSYS2 project) (Windows)
    - Apple clang version 21.0.0 (clang-2100.0.123.102) (macOS)
- Flex 2.6.4
- Bison 3.8.2

_注意：以上為開發測試環境版本，其他版本可能也能正常編譯，但建議使用相近版本或以上。_

### 編譯步驟

#### Windows (使用 MinGW)

```bash
# 1. 創建並進入 build 目錄
mkdir build && cd build
# 2. 使用 CMake 生成 Makefile
cmake -G "MinGW Makefiles" ..
# 3. 編譯專案
mingw32-make
```

#### macOS/Linux

```bash
# 1. 創建並進入 build 目錄
mkdir build && cd build
# 2. 使用 CMake 生成 Makefile
cmake ..
# 3. 編譯專案
make
```

## 📦 專案檔案結構

```text
├── include/
│   ├── ast/                # 抽象語法樹節點標頭檔 (Statement, Expression, Leaf...)
│   ├── ast.hpp             # AST 核心統整介面
│   ├── env.hpp             # 符號表與作用域鏈架構 (Environment)
│   └── stb/
│       └── stb_image_write.h # 圖片寫入外部標頭檔
├── src/
│   ├── ast/                # 各 AST 節點的具體實作 (含常數摺疊、語意驗證)
│   │   └── statement/
│   │       └── plot.cpp    # 核心：採樣、歸一化與 PNG 輸出渲染邏輯
│   ├── lexer.l             # Flex 詞法規則檔
│   ├── parser.y            # Bison 語法語意規則檔
│   ├── env.cpp             # 執行期變數/函數環境管理
│   └── main.cpp            # 程式進入點
└── CMakeLists.txt
```

## 📝 語法範例與執行說明

直譯器支援變數賦值、多參數函數定義、常數運算與繪圖陳述句。以下為合法的腳本範例：

```text
/* 1. 變數賦值：直接以 識別碼 = 數值 結尾 */
a = ++1;
b = 6+18;

/* 2. 函數定義：名稱(參數) = 運算式 */
f(x, y) = x + y^2 + 3*x*y + 5;

g(x, y) = cos(f(x, y)) + 2*sin(x) - 3*y;

/* 3. 渲染操作：用關鍵字 plot 包起來 */
plot( g(x, y) - 8 );
```
