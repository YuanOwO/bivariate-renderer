#include "ast/statement/plot.hpp"

#include <cmath>
#include <iostream>

#include "env.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// 輔助函數：將 0.0 ~ 1.0 的比值轉為藍->綠->紅的顏色漸層 (RGB)
void getRGBGradient(double ratio, unsigned char& r, unsigned char& g, unsigned char& b) {
    ratio = std::max(0.0, std::min(1.0, ratio));

    // 經典的熱力圖（藍 -> 綠 -> 紅）漸層演算法
    r = static_cast<unsigned char>(ratio * 255);
    g = static_cast<unsigned char>((1.0 - std::abs(ratio - 0.5) * 2) * 128);  // 中間點帶點綠色
    b = static_cast<unsigned char>((1.0 - ratio) * 255);
}

//////////////////////////////////////////////////

void PlotNode::serialize(std::ostream& os) const {
    os << getClassName() << "(" << expression << ")";
}

bool PlotNode::validate(Environment& env) {
    expression->validate(env);
    return true;
}

PlotPtr PlotNode::fold(Environment& env) const {
    ExprPtr folded_expr = expression->fold(env);
    return new PlotNode(getLineno(), folded_expr);
}

Value PlotNode::evaluate(Environment& env) const {
    Environment local_env(&env);

    std::vector<std::vector<Value>> plot_data;  // 用於存儲繪圖數據的二維向量

    double x_min = -10, x_max = 10, y_min = -10, y_max = 10;  // 繪圖範圍
    int resolution = 600;
    double x_step = (x_max - x_min) / resolution;  // x 軸步長
    double y_step = (y_max - y_min) / resolution;  // y 軸步長

    double z_min = 0, z_max = 1;  // 用於自動縮放 z 軸的最小值和最大值

    for (int i = 0; i < resolution; i++) {
        double x = x_min + i * x_step;
        local_env.setVariable("x", x);
        plot_data.emplace_back();  // 為每個 x 值創建一行數據
        for (int j = 0; j < resolution; j++) {
            double y = y_min + j * y_step;
            local_env.setVariable("y", y);
            Value z = expression->evaluate(local_env);
            if (std::isfinite(z)) {  // 確保 z 是有限的數值
                z_min = std::min(z_min, z);
                z_max = std::max(z_max, z);
            }
            plot_data.back().push_back(z);  // 將 z 值添加到當前 x 行的數據中
        }
    }

    // TODO: 處理 INF 和 NaN 的情況

    for (auto& row : plot_data) {
        for (auto& z : row) {
            if (std::isfinite(z)) {
                z = (z - z_min) / (z_max - z_min);  // 將 z 值縮放到 0.0 ~ 1.0 的範圍
            } else {
                z = 0.0;  // 對於無限大或 NaN，將其設置為 0.0（或其他適當的值）
            }
        }
    }

    // std::cout << "Plot data generated for expression: " << expression << std::endl;
    std::cout << "Z range: [" << z_min << ", " << z_max << "]" << std::endl;
    // 這裡可以將 plot_data 傳遞給繪圖函數或模組進行繪製，或者將其保存到文件中以供後續使用。

    std::vector<unsigned char> image_data(resolution * resolution * 3);  // RGB 圖像數據
    for (size_t i = 0; i < plot_data.size(); ++i) {
        for (size_t j = 0; j < plot_data[i].size(); ++j) {
            unsigned char r, g, b;
            getRGBGradient(plot_data[i][j], r, g, b);
            size_t idx = (i * resolution + j) * 3;
            image_data[idx] = r;
            image_data[idx + 1] = g;
            image_data[idx + 2] = b;
        }
    }

    // 4. 呼叫 stb 函數寫出 PNG 圖片
    const char* filename = "plot_output.png";
    int channels = 3;                             // RGB 三通道
    int stride_in_bytes = resolution * channels;  // 每一行圖片佔用的位元組數

    int success =
        stbi_write_png(filename, resolution, resolution, channels, image_data.data(), stride_in_bytes);

    if (success) {
        std::cout << "Successfully saved plot window as: " << filename << std::endl;
    } else {
        std::cerr << "Failed to save plot window as: " << filename << std::endl;
    }

    return MAGICALLY_STINKY_NUMBER;
}
