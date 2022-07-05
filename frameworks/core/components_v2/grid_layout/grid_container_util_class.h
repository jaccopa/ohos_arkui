/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_LAYOUT_GRID_CONTAINER_UTIL_CLASS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_LAYOUT_GRID_CONTAINER_UTIL_CLASS_H

#include <utility>

#include "base/geometry/dimension.h"
#include "base/memory/ace_type.h"

namespace OHOS::Ace::V2 {

struct GridContainerSize : public AceType {
    GridContainerSize() = default;
    explicit GridContainerSize(int32_t column)
    {
        xs = column;
        sm = column;
        md = column;
        lg = column;
        xl = column;
        xxl = column;
    };
    int32_t xs = 12;
    int32_t sm = 12;
    int32_t md = 12;
    int32_t lg = 12;
    int32_t xl = 12;
    int32_t xxl = 12;
};

enum class BreakPointsReference {
    WindowSize,
    ComponentSize,
};

enum class GridRowDirection {
    Row,
    RowReverse,
};

enum class GridSizeType {
    XS = 0,
    SM = 1,
    MD = 2,
    LG = 3,
    XL = 4,
    XXL = 5,
};

struct GridSizeInfo : public AceType {
    std::vector<Dimension> sizeInfo {
        Dimension(320, DimensionUnit::VP),
        Dimension(520, DimensionUnit::VP),
        Dimension(840, DimensionUnit::VP),
    };

    void Reset()
    {
        sizeInfo.clear();
    }
};

class Getter : public AceType {
    DECLARE_ACE_TYPE(Getter, AceType);

public:
    Getter() = default;
    explicit Getter(Dimension dimension)
        : xXs(dimension), yXs(dimension), xSm(dimension), ySm(dimension), xMd(dimension), yMd(dimension),
          xLg(dimension), yLg(dimension), xXl(dimension), yXl(dimension), xXXl(dimension), yXXl(dimension) {};
    Getter(Dimension xDimension, Dimension yDimension)
        : xXs(xDimension), yXs(yDimension), xSm(xDimension), ySm(yDimension), xMd(xDimension), yMd(yDimension),
          xLg(xDimension), yLg(yDimension), xXl(xDimension), yXl(yDimension), xXXl(xDimension), yXXl(yDimension) {};

    void SetYGutter(Dimension yDimension)
    {
        yXs = yDimension;
        ySm = yDimension;
        yMd = yDimension;
        yLg = yDimension;
        yXl = yDimension;
        yXXl = yDimension;
    }

    void SetXGutter(Dimension xDimension)
    {
        xXs = xDimension;
        xSm = xDimension;
        xMd = xDimension;
        xLg = xDimension;
        xXl = xDimension;
        xXXl = xDimension;
    }
    Dimension xXs;
    Dimension yXs;
    Dimension xSm;
    Dimension ySm;
    Dimension xMd;
    Dimension yMd;
    Dimension xLg;
    Dimension yLg;
    Dimension xXl;
    Dimension yXl;
    Dimension xXXl;
    Dimension yXXl;
};

class BreakPoints : public AceType {
    DECLARE_ACE_TYPE(BreakPoints, AceType);

public:
    BreakPointsReference reference = BreakPointsReference::WindowSize;
    std::vector<std::string> breakpoints { "320vp", "600vp", "840vp" };
};

} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_LAYOUT_GRID_CONTAINER_UTIL_CLASS_H