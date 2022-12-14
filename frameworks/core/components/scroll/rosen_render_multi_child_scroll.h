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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_ROSEN_RENDER_MULTI_CHILD_SCROLL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_ROSEN_RENDER_MULTI_CHILD_SCROLL_H

#include "core/components/scroll/render_multi_child_scroll.h"

namespace OHOS::Ace {

class RosenRenderMultiChildScroll : public RenderMultiChildScroll {
    DECLARE_ACE_TYPE(RosenRenderMultiChildScroll, RenderMultiChildScroll);

public:
    void Paint(RenderContext& context, const Offset& offset) override;
    void Update(const RefPtr<Component>& component) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_ROSEN_RENDER_MULTI_CHILD_SCROLL_H
