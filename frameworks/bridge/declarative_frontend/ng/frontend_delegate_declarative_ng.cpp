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

#include "frameworks/bridge/declarative_frontend/ng/frontend_delegate_declarative_ng.h"

#include "base/i18n/localization.h"
#include "base/log/ace_trace.h"
#include "base/log/event_report.h"
#include "base/resource/ace_res_config.h"
#include "base/thread/background_task_executor.h"
#include "base/utils/utils.h"
#include "core/common/ace_application_info.h"
#include "core/common/container.h"
#include "core/common/thread_checker.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {

namespace {

const char MANIFEST_JSON[] = "manifest.json";

} // namespace

void FrontendDelegateDeclarativeNG::AttachPipelineContext(const RefPtr<PipelineBase>& context)
{
    pipelineContextHolder_.Attach(context);
}

void FrontendDelegateDeclarativeNG::RunPage(
    const std::string& url, const std::string& params, const std::string& profile)
{
    ACE_SCOPED_TRACE("FrontendDelegateDeclarativeNG::RunPage");

    LOGI("FrontendDelegateDeclarativeNG RunPage url=%{public}s", url.c_str());
    std::string jsonContent;
    if (GetAssetContent(MANIFEST_JSON, jsonContent)) {
        manifestParser_->Parse(jsonContent);
        manifestParser_->Printer();
    } else if (!profile.empty() && GetAssetContent(profile, jsonContent)) {
        LOGI("Parse profile %{public}s", profile.c_str());
        manifestParser_->Parse(jsonContent);
    } else {
        LOGE("RunPage parse manifest.json failed");
    }
    std::string mainPagePath;
    if (!url.empty()) {
        mainPagePath = manifestParser_->GetRouter()->GetPagePath(url);
    } else {
        mainPagePath = manifestParser_->GetRouter()->GetEntry();
    }
    pageRouterManager_->SetManifestParser(manifestParser_);
    pageRouterManager_->RunPage(mainPagePath, params);
    taskExecutor_->PostTask(
        [manifestParser = manifestParser_, delegate = Claim(this)]() {
            auto pipeline = delegate->GetPipelineContext();
            // TODO: get platform version from context, and should stored in AceApplicationInfo.
            if (manifestParser->GetMinPlatformVersion() > 0) {
                pipeline->SetMinPlatformVersion(manifestParser->GetMinPlatformVersion());
            }
        },
        TaskExecutor::TaskType::JS);
}

void FrontendDelegateDeclarativeNG::Push(const std::string& uri, const std::string& params)
{
    CHECK_NULL_VOID(pageRouterManager_);
    pageRouterManager_->Push(PageTarget(uri), params);
}

void FrontendDelegateDeclarativeNG::PushWithMode(const std::string& uri, const std::string& params, uint32_t routerMode)
{
    CHECK_NULL_VOID(pageRouterManager_);
    pageRouterManager_->Push(PageTarget(uri, RouterMode(routerMode)), params);
}

void FrontendDelegateDeclarativeNG::Replace(const std::string& uri, const std::string& params)
{
    CHECK_NULL_VOID(pageRouterManager_);
    pageRouterManager_->Replace(PageTarget(uri), params);
}

void FrontendDelegateDeclarativeNG::ReplaceWithMode(
    const std::string& uri, const std::string& params, uint32_t routerMode)
{
    CHECK_NULL_VOID(pageRouterManager_);
    pageRouterManager_->Replace(PageTarget(uri, RouterMode(routerMode)), params);
}

void FrontendDelegateDeclarativeNG::Back(const std::string& uri, const std::string& params)
{
    CHECK_NULL_VOID(pageRouterManager_);
    pageRouterManager_->BackWithTarget(PageTarget(uri), params);
}

void FrontendDelegateDeclarativeNG::Clear()
{
    CHECK_NULL_VOID(pageRouterManager_);
    pageRouterManager_->Clear();
}

int32_t FrontendDelegateDeclarativeNG::GetStackSize() const
{
    CHECK_NULL_RETURN(pageRouterManager_, 0);
    return pageRouterManager_->GetStackSize();
}

void FrontendDelegateDeclarativeNG::GetState(int32_t& index, std::string& name, std::string& path)
{
    CHECK_NULL_VOID(pageRouterManager_);
    pageRouterManager_->GetState(index, name, path);
}

std::string FrontendDelegateDeclarativeNG::GetParams()
{
    CHECK_NULL_RETURN(pageRouterManager_, "");
    return pageRouterManager_->GetParams();
}

void FrontendDelegateDeclarativeNG::NavigatePage(uint8_t type, const PageTarget& target, const std::string& params)
{
    CHECK_NULL_VOID(pageRouterManager_);
    switch (static_cast<RouterAction>(type)) {
        case RouterAction::PUSH:
            pageRouterManager_->Push(target, params);
            break;
        case RouterAction::REPLACE:
            pageRouterManager_->Replace(target, params);
            break;
        case RouterAction::BACK:
            pageRouterManager_->BackWithTarget(target, params);
            break;
        default:
            LOGE("Navigator type is invalid!");
    }
}

RefPtr<JsAcePage> FrontendDelegateDeclarativeNG::GetPage(int32_t pageId) const
{
    CHECK_NULL_RETURN(pageRouterManager_, nullptr);
    return pageRouterManager_->GetPage(pageId);
}

int32_t FrontendDelegateDeclarativeNG::GetRunningPageId() const
{
    CHECK_NULL_RETURN(pageRouterManager_, -1);
    return pageRouterManager_->GetRunningPageId();
}

void FrontendDelegateDeclarativeNG::PostJsTask(std::function<void()>&& task)
{
    taskExecutor_->PostTask(task, TaskExecutor::TaskType::JS);
}

const std::string& FrontendDelegateDeclarativeNG::GetAppID() const
{
    return manifestParser_->GetAppInfo()->GetAppID();
}

const std::string& FrontendDelegateDeclarativeNG::GetAppName() const
{
    return manifestParser_->GetAppInfo()->GetAppName();
}

const std::string& FrontendDelegateDeclarativeNG::GetVersionName() const
{
    return manifestParser_->GetAppInfo()->GetVersionName();
}

int32_t FrontendDelegateDeclarativeNG::GetVersionCode() const
{
    return manifestParser_->GetAppInfo()->GetVersionCode();
}

void FrontendDelegateDeclarativeNG::PostSyncTaskToPage(std::function<void()>&& task)
{
    pipelineContextHolder_.Get(); // Wait until Pipeline Context is attached.
    taskExecutor_->PostSyncTask(task, TaskExecutor::TaskType::UI);
}

bool FrontendDelegateDeclarativeNG::GetAssetContent(const std::string& url, std::string& content)
{
    return GetAssetContentImpl(assetManager_, url, content);
}

bool FrontendDelegateDeclarativeNG::GetAssetContent(const std::string& url, std::vector<uint8_t>& content)
{
    return GetAssetContentImpl(assetManager_, url, content);
}

std::string FrontendDelegateDeclarativeNG::GetAssetPath(const std::string& url)
{
    return GetAssetPathImpl(assetManager_, url);
}

void FrontendDelegateDeclarativeNG::ChangeLocale(const std::string& language, const std::string& countryOrRegion)
{
    LOGD("JSFrontend ChangeLocale");
    taskExecutor_->PostTask(
        [language, countryOrRegion]() { AceApplicationInfo::GetInstance().ChangeLocale(language, countryOrRegion); },
        TaskExecutor::TaskType::PLATFORM);
}

void FrontendDelegateDeclarativeNG::RegisterFont(const std::string& familyName, const std::string& familySrc)
{
    pipelineContextHolder_.Get()->RegisterFont(familyName, familySrc);
}

SingleTaskExecutor FrontendDelegateDeclarativeNG::GetAnimationJsTask()
{
    return SingleTaskExecutor::Make(taskExecutor_, TaskExecutor::TaskType::JS);
}

SingleTaskExecutor FrontendDelegateDeclarativeNG::GetUiTask()
{
    return SingleTaskExecutor::Make(taskExecutor_, TaskExecutor::TaskType::UI);
}

RefPtr<PipelineBase> FrontendDelegateDeclarativeNG::GetPipelineContext()
{
    return pipelineContextHolder_.Get();
}

bool FrontendDelegateDeclarativeNG::OnPageBackPress()
{
    auto result = false;
    taskExecutor_->PostSyncTask(
        [weak = AceType::WeakClaim(this), &result] {
            auto delegate = weak.Upgrade();
            if (!delegate) {
                return;
            }
            auto pageId = delegate->GetRunningPageId();
            auto page = delegate->GetPage(pageId);
            if (page) {
                result = page->FireDeclarativeOnBackPressCallback();
            }
        },
        TaskExecutor::TaskType::JS);
    return result;
}

} // namespace OHOS::Ace::Framework
