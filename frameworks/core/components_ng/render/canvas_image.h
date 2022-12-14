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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_CANVAS_IMAGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_CANVAS_IMAGE_H

#include "base/memory/ace_type.h"

namespace OHOS::Rosen::Drawing {
class Canvas;
class RectF;
}
namespace OHOS::Ace::NG {

using RSCanvas = Rosen::Drawing::Canvas;
using RSRect = Rosen::Drawing::RectF;

// CanvasImage is interface for drawing image.
class CanvasImage : public virtual AceType {
    DECLARE_ACE_TYPE(CanvasImage, AceType)

public:
    virtual void DrawToRSCanvas(RSCanvas& canvas, const RSRect& srcRect, const RSRect& dstRect) = 0;

    static RefPtr<CanvasImage> Create(void* rawImage);
    static RefPtr<CanvasImage> Create();
    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_CANVAS_IMAGE_H
