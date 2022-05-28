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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MODIFIERS_LAYOUT_SIZE_MODIFIER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MODIFIERS_LAYOUT_SIZE_MODIFIER_H

#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/modifier/modifier.h"
#include "core/components_ng/property/calc_length.h"

namespace OHOS::Ace::NG {

class WidthModifier : public PropertyModifier<CalcLength, LayoutProperty> {
public:
    WidthModifier(const CalcLength& value);
    ~WidthModifier() override = default;
};

class HeightModifier : public PropertyModifier<CalcLength, LayoutProperty> {
public:
    HeightModifier(const CalcLength& value);
    ~HeightModifier() override = default;
};

} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MODIFIERS_LAYOUT_SIZE_MODIFIER_H