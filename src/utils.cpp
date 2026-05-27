#include "utils.hpp"

#include <cmath>

//////////////////////////////////////////////////

IMAGE_TYPE getImageType(String filename) {
    // 根據檔案副檔名判斷圖片類型
    if (filename.size() < 4) {
        return IMAGE_UNKNOWN;  // 無法判斷，副檔名太短
    }

    std::string ext = filename.substr(filename.size() - 4);

    // 將副檔名轉為小寫以進行不區分大小寫的比較
    for (auto&& c : ext) {
        c = std::tolower(c);
    }

    if (ext == ".png") {
        return IMAGE_PNG;
    } else if (ext == ".bmp") {
        return IMAGE_BMP;
    } else if (ext == ".tga") {
        return IMAGE_TGA;
    } else if (ext == ".hdr") {
        return IMAGE_HDR;
    } else if (ext == ".jpg") {
        return IMAGE_JPG;
    } else if (ext == "jpeg") {
        // 有些人可能會使用 .jpeg 作為 JPG 的副檔名，這裡也做一個簡單的判斷
        return filename.size() >= 5 && filename[filename.size() - 5] == '.' ? IMAGE_JPG : IMAGE_UNKNOWN;
    }

    return IMAGE_UNKNOWN;  // 預設為未知類型
}

//////////////////////////////////////////////////

RGBColor getRGBGradientJet(double ratio) {
    ratio = clamp(ratio, 0.0, 1.0);
    // 透過 4 個控制點切分紅綠藍
    double v = ratio * 4.0;

    double r = clamp(std::min(v - 1.5, 4.5 - v), 0.0, 1.0);
    double g = clamp(std::min(v - 0.5, 3.5 - v), 0.0, 1.0);
    double b = clamp(std::min(v + 0.5, 2.5 - v), 0.0, 1.0);

    return {r, g, b};
}

RGBColor getRGBGradientViridis(double ratio) {
    // 1. 範圍防護
    ratio = clamp(ratio, 0.0, 1.0);

    // 2. 定義 Viridis 的 5 個核心控制點 (從深紫藍到鮮黃)
    const int NUM_POINTS = 5;
    static const RGBColor viridisPoints[NUM_POINTS] = {
        {0.267004, 0.004874, 0.329415}, // 0.00: 深紫藍 (Low)
        {0.226588, 0.281145, 0.542875}, // 0.25: 藍綠
        {0.127568, 0.563234, 0.551226}, // 0.50: 綠
        {0.369214, 0.788888, 0.382914}, // 0.75: 黃綠
        {0.993248, 0.906157, 0.143936}  // 1.00: 鮮黃 (High)
    };

    // 3. 計算目前 ratio 落在第幾個區間
    double v = ratio * (NUM_POINTS - 1);  // 轉成 0.0 ~ 4.0
    int index = static_cast<int>(std::floor(v));
    if (index >= NUM_POINTS - 1) index = NUM_POINTS - 2;  // 防護邊界

    double t = v - index;  // 區間內的局部比例 (0.0 ~ 1.0)

    // 4. 線性插值 (Linear Interpolation)
    double r = clamp((1.0 - t) * viridisPoints[index].r + t * viridisPoints[index + 1].r, 0.0, 1.0);
    double g = clamp((1.0 - t) * viridisPoints[index].g + t * viridisPoints[index + 1].g, 0.0, 1.0);
    double b = clamp((1.0 - t) * viridisPoints[index].b + t * viridisPoints[index + 1].b, 0.0, 1.0);

    return {r, g, b};
}
