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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_STAGE_PAGE_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_STAGE_PAGE_PATTERN_H

#include <stdint.h>
#include <string>

#include "base/memory/referenced.h"
#include "base/utils/noncopyable.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/stage/page_info.h"

namespace OHOS::Ace::NG {

// PagePattern is the base class for page root render node.
class ACE_EXPORT PagePattern : public Pattern {
    DECLARE_ACE_TYPE(PagePattern, Pattern);

public:
    explicit PagePattern(const RefPtr<PageInfo>& pageInfo) : pageInfo_(pageInfo) {}
    ~PagePattern() override = default;

    bool IsMeasureBoundary() const override
    {
        return true;
    }

    bool IsAtomicNode() const override
    {
        return false;
    }

    const RefPtr<PageInfo>& GetPageInfo() const
    {
        return pageInfo_;
    }

    void OnShow() {}

    void OnHide() {}

    bool OnBackPressed() const
    {
        // TODO: add page life cycle.
        return false;
    }

private:
    void OnAttachToFrameNode() override;

    RefPtr<PageInfo> pageInfo_;

    ACE_DISALLOW_COPY_AND_MOVE(PagePattern);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_STAGE_PAGE_PATTERN_H
