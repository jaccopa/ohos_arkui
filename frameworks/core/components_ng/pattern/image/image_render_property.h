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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_IMAGE_IMAGE_RENDER_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_IMAGE_IMAGE_RENDER_PROPERTY_H

#include "core/components_ng/render/render_property.h"

namespace OHOS::Ace::NG {
// RenderProperty are used to set render properties.
class ImageRenderProperty : public RenderProperty {
    DECLARE_ACE_TYPE(ImageRenderProperty, RenderProperty)

public:
    ImageRenderProperty() = default;
    ~ImageRenderProperty() override = default;

    RefPtr<RenderProperty> Clone() const override
    {
        auto renderProperty = MakeRefPtr<ImageRenderProperty>();
        renderProperty->UpdateRenderProperty(this);
        renderProperty->propAutoResize_ = CloneAutoResize();
        return renderProperty;
    }

    void Reset() override
    {
        propAutoResize_.reset();
    }

    ACE_DEFINE_CLASS_PROPERTY_WITHOUT_GROUP(AutoResize, bool, PROPERTY_UPDATE_RENDER);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_IMAGE_IMAGE_RENDER_PROPERTY_H