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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_PAGE_ROUTER_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_PAGE_ROUTER_MANAGER_H

#include <cstdint>
#include <stack>
#include <string>

#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/thread/task_executor.h"
#include "base/utils/noncopyable.h"
#include "bridge/declarative_frontend/ng/entry_page_info.h"
#include "core/components_ng/base/frame_node.h"
#include "frameworks/bridge/common/manifest/manifest_parser.h"

namespace OHOS::Ace::NG {

using LoadPageCallback = std::function<bool(const std::string&)>;

enum class RouterMode {
    STANDARD = 0,
    SINGLE,
};

enum class RouterAction {
    DEFAULT = 0,
    PUSH,
    REPLACE,
    BACK,
    CLEAR,
};

struct RouterPageInfo {
    std::string url;
    RouterMode routerMode = RouterMode::STANDARD;
    std::string path;
};

struct RouterTask {
    RouterAction action = RouterAction::PUSH;
    RouterPageInfo routerPageInfo;
    std::string params;
};

class PageRouterManager : public AceType {
public:
    PageRouterManager() = default;
    ~PageRouterManager() override = default;

    void RunPage(const std::string& url, const std::string& params);

    void SetManifestParser(const RefPtr<Framework::ManifestParser>& manifestParser)
    {
        manifestParser_ = manifestParser;
    }

    void SetLoadJsCallback(LoadPageCallback&& callback)
    {
        loadJs_ = std::move(callback);
    }

    void EnableAlertBeforeBackPage(const std::string& message, std::function<void(int32_t)>&& callback);

    void DisableAlertBeforeBackPage();

    // router operation
    void Push(const RouterPageInfo& target, const std::string& params);
    bool Pop();
    void Replace(const RouterPageInfo& target, const std::string& params) {}
    void BackWithTarget(const RouterPageInfo& target, const std::string& params);
    void Clear() {}
    int32_t GetStackSize() const
    {
        return 0;
    }
    void GetState(int32_t& index, std::string& name, std::string& path) {}
    std::string GetParams() const
    {
        return "";
    }

    RefPtr<FrameNode> GetCurrentPageNode() const
    {
        if (pageRouterStack_.empty()) {
            LOGE("fail to get current page node due to page is null");
            return nullptr;
        }
        return pageRouterStack_.top().Upgrade();
    }

    std::string GetCurrentPageUrl();

    // Get the currently running JS page information in NG structure.
    RefPtr<Framework::RevSourceMap> GetCurrentPageSourceMap(const RefPtr<AssetManager>& assetManager);

private:
    // page id manage
    int32_t GenerateNextPageId();

    void StartPush(const RouterPageInfo& target, const std::string& params);
    void StartBack(const RouterPageInfo& target, const std::string& params);
    void BackCheckAlert(const RouterPageInfo& target, const std::string& params);

    // page operations
    void LoadPage(int32_t pageId, const RouterPageInfo& target, const std::string& params, bool isRestore = false);
    void PopPage();

    static bool OnPageReady(const RefPtr<FrameNode>& pageNode);
    static bool OnPopPage();

    RefPtr<Framework::ManifestParser> manifestParser_;
    LoadPageCallback loadJs_;
    int32_t pageId_ = 0;
    std::stack<WeakPtr<FrameNode>> pageRouterStack_;

    ACE_DISALLOW_COPY_AND_MOVE(PageRouterManager);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_PAGE_ROUTER_MANAGER_H
