
#include "TightAtlasPacker.h"

#include <vector>
#include "Rectangle.h"
#include "rectangle-packing.h"
#include "size-selectors.h"

namespace msdf_atlas {

TightAtlasPacker::TightAtlasPacker() :
    width(-1), height(-1),
    padding(0),
    dimensionsConstraint(DimensionsConstraint::POWER_OF_TWO_SQUARE),
    scale(-1),
    minScale(1),
    unitRange(0),
    pxRange(0),
    miterLimit(0),
    alignOriginX(false), alignOriginY(false),
    scaleMaximizationTolerance(.001)
{ }

int TightAtlasPacker::tryPack(GlyphGeometry *glyphs, int count, DimensionsConstraint dimensionsConstraint, int &width, int &height, double scale) const {
    double range = unitRange+pxRange/scale;
    // Wrap glyphs into boxes
    std::vector<Rectangle> rectangles;
    std::vector<GlyphGeometry *> rectangleGlyphs;
    rectangles.reserve(count);
    rectangleGlyphs.reserve(count);
    for (GlyphGeometry *glyph = glyphs, *end = glyphs+count; glyph < end; ++glyph) {
        if (!glyph->isWhitespace()) {
            Rectangle rect = { };
            glyph->wrapBox(scale, range, miterLimit, alignOriginX, alignOriginY);
            glyph->getBoxSize(rect.w, rect.h);
            if (rect.w > 0 && rect.h > 0) {
                rectangles.push_back(rect);
                rectangleGlyphs.push_back(glyph);
            }
        }
    }
    // No non-zero size boxes?
    if (rectangles.empty()) {
        if (width < 0 || height < 0)
            width = 0, height = 0;
        return 0;
    }
    // Box rectangle packing
    if (width < 0 || height < 0) {
        std::pair<int, int> dimensions = std::make_pair(width, height);
        switch (dimensionsConstraint) {
            case DimensionsConstraint::POWER_OF_TWO_SQUARE:
                dimensions = packRectangles<SquarePowerOfTwoSizeSelector>(rectangles.data(), rectangles.size(), padding);
                break;
            case DimensionsConstraint::POWER_OF_TWO_RECTANGLE:
                dimensions = packRectangles<PowerOfTwoSizeSelector>(rectangles.data(), rectangles.size(), padding);
                break;
            case DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE:
                dimensions = packRectangles<SquareSizeSelector<4> >(rectangles.data(), rectangles.size(), padding);
                break;
            case DimensionsConstraint::EVEN_SQUARE:
                dimensions = packRectangles<SquareSizeSelector<2> >(rectangles.data(), rectangles.size(), padding);
                break;
            case DimensionsConstraint::SQUARE:
                dimensions = packRectangles<SquareSizeSelector<> >(rectangles.data(), rectangles.size(), padding);
                break;
        }
        if (!(dimensions.first > 0 && dimensions.second > 0))
            return -1;
        width = dimensions.first, height = dimensions.second;
    } else {
        if (int result = packRectangles(rectangles.data(), rectangles.size(), width, height, padding))
            return result;
    }
    // Set glyph box placement
    for (size_t i = 0; i < rectangles.size(); ++i)
        rectangleGlyphs[i]->placeBox(rectangles[i].x, height-(rectangles[i].y+rectangles[i].h));
    return 0;
}

double TightAtlasPacker::packAndScale(GlyphGeometry *glyphs, int count) const {
    bool lastResult = false;
    int w = width, h = height;
    #define TRY_PACK(scale) (lastResult = !tryPack(glyphs, count, DimensionsConstraint(), w, h, (scale)))
    double minScale = 1, maxScale = 1;
    if (TRY_PACK(1)) {
        while (maxScale < 1e+32 && ((maxScale = 2*minScale), TRY_PACK(maxScale)))
            minScale = maxScale;
    } else {
        while (minScale > 1e-32 && ((minScale = .5*maxScale), !TRY_PACK(minScale)))
            maxScale = minScale;
    }
    if (minScale == maxScale)
        return 0;
    while (minScale/maxScale < 1-scaleMaximizationTolerance) {
        double midScale = .5*(minScale+maxScale);
        if (TRY_PACK(midScale))
            minScale = midScale;
        else
            maxScale = midScale;
    }
    if (!lastResult)
        TRY_PACK(minScale);
    return minScale;
}

int TightAtlasPacker::pack(GlyphGeometry *glyphs, int count) {
    double initialScale = scale > 0 ? scale : minScale;
    if (initialScale > 0) {
        if (int remaining = tryPack(glyphs, count, dimensionsConstraint, width, height, initialScale))
            return remaining;
    } else if (width < 0 || height < 0)
        return -1;
    if (scale <= 0)
        scale = packAndScale(glyphs, count);
    if (scale <= 0)
        return -1;
    pxRange += scale*unitRange;
    unitRange = 0;
    return 0;
}

void TightAtlasPacker::setDimensions(int width, int height) {
    this->width = width, this->height = height;
}

void TightAtlasPacker::unsetDimensions() {
    width = -1, height = -1;
}

void TightAtlasPacker::setDimensionsConstraint(DimensionsConstraint dimensionsConstraint) {
    this->dimensionsConstraint = dimensionsConstraint;
}

void TightAtlasPacker::setPadding(int padding) {
    this->padding = padding;
}

void TightAtlasPacker::setScale(double scale) {
    this->scale = scale;
}

void TightAtlasPacker::setMinimumScale(double minScale) {
    this->minScale = minScale;
}

void TightAtlasPacker::setUnitRange(double unitRange) {
    this->unitRange = unitRange;
}

void TightAtlasPacker::setPixelRange(double pxRange) {
    this->pxRange = pxRange;
}

void TightAtlasPacker::setMiterLimit(double miterLimit) {
    this->miterLimit = miterLimit;
}

void TightAtlasPacker::setOriginAlignment(bool align) {
    alignOriginX = align, alignOriginY = align;
}

void TightAtlasPacker::setOriginAlignment(bool alignX, bool alignY) {
    alignOriginX = alignX, alignOriginY = alignY;
}

void TightAtlasPacker::getDimensions(int &width, int &height) const {
    width = this->width, height = this->height;
}

double TightAtlasPacker::getScale() const {
    return scale;
}

double TightAtlasPacker::getPixelRange() const {
    return pxRange;
}

}
