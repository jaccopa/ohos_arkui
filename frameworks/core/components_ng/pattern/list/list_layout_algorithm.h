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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_LIST_LIST_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_LIST_LIST_LAYOUT_ALGORITHM_H

#include <list>
#include <unordered_map>
#include <vector>

#include "base/memory/referenced.h"
#include "core/components_ng/layout/layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/pattern/list/list_item_builder.h"

namespace OHOS::Ace::NG {
class PipelineContext;

// TextLayoutAlgorithm acts as the underlying text layout.
class ACE_EXPORT ListLayoutAlgorithm : public LayoutAlgorithm {
    DECLARE_ACE_TYPE(ListLayoutAlgorithm, LayoutAlgorithm);

public:
    using PositionMap = std::map<int32_t, std::pair<float, float>>;

    ListLayoutAlgorithm(int32_t startIndex, int32_t endIndex) : preStartIndex_(startIndex), preEndIndex_(endIndex) {}

    ~ListLayoutAlgorithm() override = default;

    void OnReset() override
    {
        builder_.Reset();
    }

    void SetBuilder(const RefPtr<ListItemBuilder>& builder)
    {
        builder_ = builder;
    }

    void SetCurrentOffset(float offset)
    {
        currentOffset_ = offset;
    }

    float GetCurrentOffset() const
    {
        return currentOffset_;
    }

    int32_t GetStartIndex() const
    {
        return startIndex_.value_or(0);
    }

    int32_t GetEndIndex() const
    {
        return endIndex_.value_or(0);
    }

    void Measure(LayoutWrapper* layoutWrapper) override;

    void Layout(LayoutWrapper* layoutWrapper) override;

private:
    std::pair<int32_t, float> LayoutOrRecycleCachedItems(
        LayoutWrapper* layoutWrapper, const LayoutConstraintF& layoutConstraint, Axis axis);

    std::pair<int32_t, float> RequestNewItemsForward(LayoutWrapper* layoutWrapper,
        const LayoutConstraintF& layoutConstraint, int32_t startIndex, float startPos, Axis axis);

    std::pair<int32_t, float> RequestNewItemsBackward(LayoutWrapper* layoutWrapper,
        const LayoutConstraintF& layoutConstraint, int32_t startIndex, float startPos, Axis axis);

    RefPtr<ListItemBuilder> builder_;
    PositionMap itemPosition_;
    float currentOffset_ = 0.0f;
    float startMainPos_ = 0.0f;
    float endMainPos_ = 0.0f;
    int32_t preStartIndex_ = 0;
    int32_t preEndIndex_ = 0;
    std::optional<int32_t> startIndex_ = 0;
    std::optional<int32_t> endIndex_ = 0;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_LIST_LIST_LAYOUT_ALGORITHM_H