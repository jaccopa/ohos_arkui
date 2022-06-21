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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LIST_LIST_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LIST_LIST_PATTERN_H

#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/pattern/list/list_item_builder.h"
#include "core/components_ng/pattern/list/list_layout_algorithm.h"
#include "core/components_ng/pattern/list/list_layout_property.h"
#include "core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {

class ListPattern : public Pattern {
    DECLARE_ACE_TYPE(ListPattern, Pattern);

public:
    ListPattern() = default;
    ~ListPattern() override = default;

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<ListLayoutProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        auto listLayoutAlgorithm = MakeRefPtr<ListLayoutAlgorithm>(startIndex_, endIndex_);
        listLayoutAlgorithm->SetBuilder(builder_);
        listLayoutAlgorithm->SetCurrentOffset(currentOffset_);
        currentOffset_ = 0;
        return listLayoutAlgorithm;
    }

    void AddForEachBuilder(const RefPtr<ForEachBuilder>& builder)
    {
        builder_->AddUiNodeId(builder);
    }

    void AddNonLazyItem(const RefPtr<FrameNode>& node)
    {
        builder_->AddUiNodeId(node);
    }

    void UpdateCurrentOffset(float offset);

private:
    void OnModifyDone() override;
    void OnAttachToFrameNode() override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, bool skipMeasure, bool skipLayout) override;
    void OnDetachFromFrameNode() override
    {
        builder_->Clean();
    }

    RefPtr<ScrollableEvent> scrollableEvent_;
    RefPtr<ListItemBuilder> builder_ = MakeRefPtr<ListItemBuilder>();
    int32_t startIndex_ = 0;
    int32_t endIndex_ = 0;
    float currentOffset_ = 0.0;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LIST_LIST_PATTERN_H