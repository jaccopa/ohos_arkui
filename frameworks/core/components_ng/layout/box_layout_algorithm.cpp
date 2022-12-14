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

#include "core/components_ng/layout/box_layout_algorithm.h"

#include "base/geometry/ng/size_t.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/property/measure_utils.h"
#include "core/components_ng/property/property.h"

namespace OHOS::Ace::NG {

void BoxLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto layoutConstraint = layoutWrapper->GetLayoutProperty()->CreateChildConstraint();
    for (auto&& child : layoutWrapper->GetAllChildrenWithBuild()) {
        child->Measure(layoutConstraint);
    }
    PerformMeasureSelf(layoutWrapper);
}

void BoxLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    PerformLayout(layoutWrapper);
    auto parentOffset =
        layoutWrapper->GetGeometryNode()->GetParentGlobalOffset() + layoutWrapper->GetGeometryNode()->GetFrameOffset();
    for (auto&& child : layoutWrapper->GetAllChildrenWithBuild()) {
        child->Layout(parentOffset);
    }
}

// Called to perform measure current render node.
void BoxLayoutAlgorithm::PerformMeasureSelf(LayoutWrapper* layoutWrapper)
{
    const auto& layoutConstraint = layoutWrapper->GetLayoutProperty()->GetLayoutConstraint();
    const auto& minSize = layoutConstraint->minSize;
    const auto& maxSize = layoutConstraint->maxSize;
    const auto& padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    auto measureType = layoutWrapper->GetLayoutProperty()->GetMeasureType();
    OptionalSizeF frameSize;
    do {
        // Use idea size first if it is valid.
        frameSize.UpdateSizeWithCheck(layoutConstraint->selfIdealSize);
        if (frameSize.IsValid()) {
            break;
        }

        if (measureType == MeasureType::MATCH_PARENT) {
            frameSize.UpdateIllegalSizeWithCheck(layoutConstraint->parentIdealSize);
            if (frameSize.IsValid()) {
                break;
            }
        }

        const auto& content = layoutWrapper->GetGeometryNode()->GetContent();
        if (content) {
            // use content size.
            auto contentSize = content->GetRect().GetSize();
            AddPaddingToSize(padding, contentSize);
            frameSize.UpdateIllegalSizeWithCheck(contentSize);
        } else {
            // use the max child size.
            auto childFrame = SizeF(-1, -1);
            for (const auto& child : layoutWrapper->GetAllChildrenWithBuild()) {
                auto childSize = child->GetGeometryNode()->GetFrameSize();
                childFrame = childFrame > childSize ? childFrame : childSize;
            }
            childFrame.Constrain(minSize, maxSize);
            AddPaddingToSize(padding, childFrame);
            frameSize.UpdateIllegalSizeWithCheck(childFrame);
        }
        frameSize.UpdateIllegalSizeWithCheck(SizeF { 0.0f, 0.0f });
    } while (false);

    layoutWrapper->GetGeometryNode()->SetFrameSize(frameSize.ConvertToSizeT());
}

// Called to perform layout render node and child.
void BoxLayoutAlgorithm::PerformLayout(LayoutWrapper* layoutWrapper)
{
    // update child position.
    auto size = layoutWrapper->GetGeometryNode()->GetFrameSize();
    const auto& padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    MinusPaddingToSize(padding, size);
    auto left = padding.left.value_or(0);
    auto top = padding.top.value_or(0);
    auto paddingOffset = OffsetF(left, top);
    auto align = Alignment::TOP_LEFT;
    if (layoutWrapper->GetLayoutProperty()->GetPositionProperty()) {
        align = layoutWrapper->GetLayoutProperty()->GetPositionProperty()->GetAlignment().value_or(align);
    }
    // Update child position.
    for (const auto& child : layoutWrapper->GetAllChildrenWithBuild()) {
        auto translate =
            Alignment::GetAlignPosition(size, child->GetGeometryNode()->GetFrameSize(), align) + paddingOffset;
        child->GetGeometryNode()->SetFrameOffset(translate);
    }
    // Update content position.
    const auto& content = layoutWrapper->GetGeometryNode()->GetContent();
    if (content) {
        auto translate = Alignment::GetAlignPosition(size, content->GetRect().GetSize(), align) + paddingOffset;
        content->SetOffset(translate);
    }
}
} // namespace OHOS::Ace::NG
