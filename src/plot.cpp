#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "plot.hpp"

#include <iostream>

#include "stb/stb_image_write.h"
#include "utils.hpp"

void renderPlot(const String& filename, const std::vector<Value>& data, int resolution) {
    // 目前暫不實現具體的繪圖功能
    // 這裡可以添加對繪圖庫的調用，將計算結果渲染成圖片

    std::cout << "Plotting expression to file: " << filename << std::endl;

    std::vector<Uint8> imageData(resolution * resolution * 3);  // 預設為白色背景

    for (size_t i = 0; i < data.size(); ++i) {
        RGBColor color = getRGBGradientViridis(data[i]);
        size_t idx = i * 3;
        imageData[idx + 0] = static_cast<Uint8>(color.r * 255);
        imageData[idx + 1] = static_cast<Uint8>(color.g * 255);
        imageData[idx + 2] = static_cast<Uint8>(color.b * 255);
    }

    int success = 0;

    switch (getImageType(filename)) {
    case IMAGE_PNG:
        std::cout << "Detected image type: PNG" << std::endl;
        success =
            stbi_write_png(filename.c_str(), resolution, resolution, 3, imageData.data(), resolution * 3);
        break;
    case IMAGE_BMP:
        std::cout << "Detected image type: BMP" << std::endl;
        success = stbi_write_bmp(filename.c_str(), resolution, resolution, 3, imageData.data());
        break;
    case IMAGE_JPG:
        std::cout << "Detected image type: JPG" << std::endl;
        success = stbi_write_jpg(filename.c_str(), resolution, resolution, 3, imageData.data(), 100);
        break;
    case IMAGE_TGA:
        std::cout << "Detected image type: TGA" << std::endl;
        std::cerr << "TGA format is not supported for writing in this implementation." << std::endl;
        break;
    case IMAGE_HDR:
        std::cout << "Detected image type: HDR" << std::endl;
        std::cerr << "HDR format is not supported for writing in this implementation." << std::endl;
        break;
    default:
        std::cerr << "Unexpected error in determining image type for file: " << filename << std::endl;
        return;
    }

    if (!success) {
        std::cerr << "Failed to write image to file: " << filename << std::endl;
    } else {
        std::cout << "Successfully wrote image to file: " << filename << std::endl;
    }
}
