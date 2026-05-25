#include "ast/statement/plot.hpp"

#include <cmath>
#include <iostream>
#include <limits>

#include "env.hpp"
#include "errors.hpp"
#include "utils.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

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
    return new PlotNode(getLineno(), folded_expr, filename);
}

Value PlotNode::evaluate(Environment& env) const {
    Environment local_env(&env);
    int resolution = 600;  // 繪圖解析度
    int channels = 3;      // RGB 三通道

    double x_min = -10, x_max = 10, y_min = -10, y_max = 10;  // 繪圖範圍
    double x_step = (x_max - x_min) / resolution;             // x 軸步長
    double y_step = (y_max - y_min) / resolution;             // y 軸步長

    double z_min = std::numeric_limits<double>::max();
    double z_max = std::numeric_limits<double>::lowest();

    std::vector<Value> plot_data;  // 用於存儲繪圖數據的二維向量
    plot_data.resize(resolution * resolution);

    Value x = x_min, y = y_min, z;

    for (int i = 0; i < resolution; i++) {
        y = y_max - i * y_step;  // 從上到下繪製
        local_env.setVariable("y", y);
        for (int j = 0; j < resolution; j++) {
            x = x_min + j * x_step;
            local_env.setVariable("x", x);
            z = expression->evaluate(local_env);
            if (std::isfinite(z)) {  // 確保 z 是有限的數值
                z_min = std::min(z_min, z);
                z_max = std::max(z_max, z);
            }
            plot_data[i * resolution + j] = z;  // 將 z 值添加到當前 x 行的數據中
        }
    }

    std::vector<uint8_t> image_data(resolution * resolution * channels);  // RGB 圖像數據

    // TODO: 正規化 z 值到 [0, 1] 範圍，與處理 INF 和 NaN 的情況
    for (size_t i = 0; i < resolution; ++i) {
        for (size_t j = 0; j < resolution; ++j) {
            z = plot_data[i * resolution + j];
            if (std::isfinite(z)) {
                z = normalize(z, z_min, z_max);
            } else if (std::isinf(z)) {
                z = (z > 0) ? 1.0 : 0.0;
            } else {  // NaN 或其他非數值情況
                // TODO: 我不知道怎麼處理比較好，暫時先取周圍的平均值（如果有的話）
                Value mean_z = 0.0;
                int count = 0;
                if (i > 0 && std::isfinite(plot_data[(i - 1) * resolution + j])) {
                    mean_z += plot_data[(i - 1) * resolution + j];
                    count++;
                }
                if (i < resolution - 1 && std::isfinite(plot_data[(i + 1) * resolution + j])) {
                    mean_z += plot_data[(i + 1) * resolution + j];
                    count++;
                }
                if (j > 0 && std::isfinite(plot_data[i * resolution + (j - 1)])) {
                    mean_z += plot_data[i * resolution + (j - 1)];
                    count++;
                }
                if (j < resolution - 1 && std::isfinite(plot_data[i * resolution + (j + 1)])) {
                    mean_z += plot_data[i * resolution + (j + 1)];
                    count++;
                }
                z = (count > 0) ? mean_z / count : 0.0;
            }
            // 根據 z 值獲取對應的 RGB 顏色，這裡使用 Viridis 漸層作為示例
            auto rgb = getRGBGradientViridis(z);
            size_t idx = (i * resolution + j) * channels;
            image_data[idx] = rgb.r * 255;
            image_data[idx + 1] = rgb.g * 255;
            image_data[idx + 2] = rgb.b * 255;
        }
    }

    // 4. 呼叫 stb 函數寫出 PNG 圖片
    const char* filename_str = filename.c_str();
    int stride_in_bytes = resolution * channels;  // 每一行圖片佔用的位元組數

    int success;

    switch (getImageType()) {
    case IMAGE_PNG:
        success = stbi_write_png(filename_str, resolution, resolution, channels, image_data.data(),
                                 stride_in_bytes);
        break;
    case IMAGE_JPG:
        // stbi_write_jpg 需要額外的品質參數，這裡暫時固定為 90
        success = stbi_write_jpg(filename_str, resolution, resolution, channels, image_data.data(), 90);
        break;
    case IMAGE_BMP:
        success = stbi_write_bmp(filename_str, resolution, resolution, channels, image_data.data());
        break;
    case IMAGE_TGA:
    case IMAGE_HDR:
    default:
        throw RuntimeError("Unknown image format");
    }

    if (success) {
        std::cout << "Successfully saved plot window as: " << filename << std::endl;
    } else {
        std::cerr << "Failed to save plot window as: " << filename << std::endl;
    }

    return MAGICALLY_STINKY_NUMBER;
}

IMAGE_TYPE PlotNode::getImageType() const {
    // 根據檔案副檔名判斷圖片類型
    if (filename.size() < 4) {
        return IMAGE_UNKNOWN;  // 無法判斷，副檔名太短
    }

    std::string ext = filename.substr(filename.size() - 4);

    if (ext == ".png" || ext == ".PNG") {
        return IMAGE_PNG;
    } else if (ext == ".bmp" || ext == ".BMP") {
        return IMAGE_BMP;
    } else if (ext == ".tga" || ext == ".TGA") {
        return IMAGE_TGA;
    } else if (ext == ".hdr" || ext == ".HDR") {
        return IMAGE_HDR;
    } else if (ext == ".jpg" || ext == ".JPG") {
        return IMAGE_JPG;
    } else if (ext == "jpeg" || ext == "JPEG") {
        return filename.size() >= 5 && filename[filename.size() - 5] == '.' ? IMAGE_JPG : IMAGE_UNKNOWN;
    }

    return IMAGE_UNKNOWN;  // 預設為未知類型
}
