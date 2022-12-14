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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_PROGRESS_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_PROGRESS_ELEMENT_H

#include "core/common/container.h"
#include "core/components/padding/padding_component.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

// Progress element implementation
class ProgressElement : public RenderElement {
    DECLARE_ACE_TYPE(ProgressElement, RenderElement);

public:
    void Update() override
    {
        customComponent_ = component_;
        RenderElement::Update();
    }

    bool CanUpdate(const RefPtr<Component>& newComponent) override
    {
        if (Container::IsCurrentUsePartialUpdate()) {
            return Element::CanUpdate(newComponent);
        }
        return (newComponent == customComponent_) && Element::CanUpdate(newComponent);
    }

private:
    WeakPtr<Component> customComponent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_PROGRESS_ELEMENT_H
