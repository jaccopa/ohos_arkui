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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REMOTE_WINDOW_ROSEN_RENDER_REMOTE_WINDOW_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REMOTE_WINDOW_ROSEN_RENDER_REMOTE_WINDOW_H

#include "core/components/remote_window/render_remote_window.h"

namespace OHOS::Ace {
class RosenRenderRemoteWindow final : public RenderRemoteWindow {
    DECLARE_ACE_TYPE(RosenRenderRemoteWindow, RenderRemoteWindow);

public:
    RosenRenderRemoteWindow() = default;
    ~RosenRenderRemoteWindow() override = default;

    void Update(const RefPtr<Component>& component) override;
    static std::shared_ptr<Rosen::RSNode> ExtractRSNode(const RefPtr<Component>& component);
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REMOTE_WINDOW_ROSEN_RENDER_REMOTE_WINDOW_H
