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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_PAINT_METHOD_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_PAINT_METHOD_H

#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/components_ng/render/node_paint_method.h"
#include "core/components_ng/render/paragraph.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT TextPaintMethod : public NodePaintMethod {
    DECLARE_ACE_TYPE(TextPaintMethod, NodePaintMethod)
public:
    explicit TextPaintMethod(const RefPtr<Paragraph>& paragraph) : paragraph_(paragraph) {}
    ~TextPaintMethod() override = default;

    CanvasDrawFunction GetContentDrawFunction(PaintWrapper* paintWrapper) override
    {
        CHECK_NULL_RETURN(paragraph_, nullptr);
        auto offset = paintWrapper->GetContentOffset();
        return [paragraph = paragraph_, offset](
                   const RefPtr<Canvas>& canvas) { paragraph->Paint(canvas, offset.GetX(), offset.GetY()); };
    }

private:
    RefPtr<Paragraph> paragraph_;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TEXT_TEXT_PAINT_METHOD_H