#include "ast/statement/plot.hpp"

PlotPtr PlotNode::clone() const {
    ExprPtr cloned_expr = expression->clone();
    return new PlotNode(getLineno(), cloned_expr, filename);
}

IMAGE_TYPE PlotNode::getImageType() const {
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
