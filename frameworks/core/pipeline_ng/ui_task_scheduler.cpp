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

#include "core/pipeline_ng/ui_task_scheduler.h"

#include "base/thread/background_task_executor.h"
#include "base/thread/cancelable_callback.h"
#include "core/common/thread_checker.h"
#include "core/components_ng/base/frame_node.h"

namespace OHOS::Ace::NG {

std::unique_ptr<UITaskScheduler> UITaskScheduler::instance_ = nullptr;

std::mutex UITaskScheduler::mutex_ = std::mutex();

UITaskScheduler* UITaskScheduler::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        if (!instance_) {
            instance_.reset(new UITaskScheduler);
        }
    }
    return instance_.get();
}

void UITaskScheduler::AddDirtyLayoutNode(const RefPtr<FrameNode>& dirty)
{
    CHECK_RUN_ON(UI);
    if (!dirty) {
        LOGW("dirty is null");
        return;
    }
    dirtyLayoutNodes_[dirty->GetRootId()][dirty->GetPageId()].emplace(dirty);
}

void UITaskScheduler::AddDirtyRenderNode(const RefPtr<FrameNode>& dirty)
{
    CHECK_RUN_ON(UI);
    if (!dirty) {
        LOGW("dirty is null");
        return;
    }
    dirtyRenderNodes_[dirty->GetRootId()][dirty->GetPageId()].emplace(dirty);
}

void UITaskScheduler::FlushLayoutTask(bool onCreate, bool forceUseMainThread)
{
    CHECK_RUN_ON(UI);
    ACE_FUNCTION_TRACE();
    auto dirtyLayoutNodes = std::move(dirtyLayoutNodes_);
    // Priority task creation
    for (auto&& rootNodes : dirtyLayoutNodes) {
        for (auto&& pageNodes : rootNodes.second) {
            for (auto&& node : pageNodes.second) {
                auto task = node->CreateLayoutTask(onCreate, forceUseMainThread);
                if (task) {
                    if (forceUseMainThread || (task->GetTaskThreadType() == MAIN_TASK)) {
                        (*task)();
                    } else {
                        LOGW("need to use multithread feature");
                    }
                }
            }
        }
    }
}

void UITaskScheduler::FlushRenderTask(bool forceUseMainThread)
{
    CHECK_RUN_ON(UI);
    ACE_FUNCTION_TRACE();
    auto dirtyRenderNodes = std::move(dirtyRenderNodes_);
    // Priority task creation
    for (auto&& rootNodes : dirtyRenderNodes) {
        for (auto&& pageNodes : rootNodes.second) {
            for (auto&& node : pageNodes.second) {
                auto task = node->CreateRenderTask(forceUseMainThread);
                if (task) {
                    if (forceUseMainThread || (task->GetTaskThreadType() == MAIN_TASK)) {
                        (*task)();
                    } else {
                        LOGW("need to use multithread feature");
                    }
                }
            }
        }
    }
}

void UITaskScheduler::FlushTask()
{
    CHECK_RUN_ON(UI);
    ACE_SCOPED_TRACE("UITaskScheduler::FlushTask");
    FlushLayoutTask();
    FlushRenderTask();
}

} // namespace OHOS::Ace::NG
