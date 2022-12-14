/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_PAGE_TARGET_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_PAGE_TARGET_H

#include "core/components/stage/stage_element.h"

namespace OHOS::Ace {

enum class RouterMode {
    STANDARD = 0,
    SINGLE,
};

struct PageTarget {
    PageTarget() = delete;
    PageTarget(const std::string& url, const WeakPtr<StageElement>& container) : url(url), container(container) {}
    PageTarget(const std::string& url, const WeakPtr<StageElement>& container, bool useSubStage)
        : url(url), container(container), useSubStage(useSubStage)
    {}
    PageTarget(const std::string& url, RouterMode routerMode) : url(url), routerMode(routerMode) {}
    PageTarget(const PageTarget& pageTarget, const std::string& url) : url(url), routerMode(pageTarget.routerMode),
        container(pageTarget.container) {}
    explicit PageTarget(const std::string& url) : url(url) {}
    ~PageTarget() = default;

    std::string url;
    RouterMode routerMode = RouterMode::STANDARD;
    WeakPtr<StageElement> container;
    bool useSubStage = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_PAGE_TARGET_H
