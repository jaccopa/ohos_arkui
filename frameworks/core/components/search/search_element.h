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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEARCH_SEARCH_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEARCH_SEARCH_ELEMENT_H

#include "core/components/focus_animation/render_focus_animation.h"
#include "core/components/search/render_search.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class SearchElement : public RenderElement, public FocusGroup {
    DECLARE_ACE_TYPE(SearchElement, RenderElement, FocusGroup);

public:
    void PerformBuild() override;
    void LocalizedUpdate() override;

protected:
    bool OnKeyEvent(const KeyEvent& keyEvent) override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEARCH_SEARCH_ELEMENT_H
