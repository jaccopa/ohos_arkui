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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SOLE_CHILD_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SOLE_CHILD_ELEMENT_H

#include "base/utils/macros.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class ACE_EXPORT SoleChildElement : public RenderElement {
    DECLARE_ACE_TYPE(SoleChildElement, RenderElement);

public:
    SoleChildElement() = default;
    ~SoleChildElement() override = default;

    static RefPtr<Element> Create();

    void PerformBuild() override;

    // specialhandling for noGridItem, ListItem
    // whose 'wrapping' copmponent are descendants, not ancestors
    void LocalizedUpdateWithItemComponent(
        const RefPtr<Component>& innerMostWrappingComponent, const RefPtr<Component>& mainComponent);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SOLE_CHILD_ELEMENT_H
