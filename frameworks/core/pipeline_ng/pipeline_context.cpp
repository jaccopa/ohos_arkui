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

#include "core/pipeline_ng/pipeline_context.h"

#include <cstdint>
#include <memory>

#include "base/log/ace_trace.h"
#include "base/log/ace_tracker.h"
#include "base/log/event_report.h"
#include "base/log/frame_report.h"
#include "base/memory/referenced.h"
#include "base/thread/task_executor.h"
#include "base/utils/utils.h"
#include "core/common/ace_application_info.h"
#include "core/common/container.h"
#include "core/common/thread_checker.h"
#include "core/common/window.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/custom/custom_node.h"
#include "core/components_ng/pattern/stage/stage_pattern.h"
#include "core/components_ng/property/calc_length.h"
#include "core/components_ng/property/layout_constraint.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/ui_task_scheduler.h"

namespace OHOS::Ace::NG {

PipelineContext::PipelineContext(std::unique_ptr<Window> window, RefPtr<TaskExecutor> taskExecutor,
    RefPtr<AssetManager> assetManager, RefPtr<PlatformResRegister> platformResRegister,
    const RefPtr<Frontend>& frontend, int32_t instanceId)
    : PipelineBase(std::move(window), std::move(taskExecutor), std::move(assetManager), frontend, instanceId)
{}

PipelineContext::PipelineContext(std::unique_ptr<Window> window, RefPtr<TaskExecutor> taskExecutor,
    RefPtr<AssetManager> assetManager, const RefPtr<Frontend>& frontend, int32_t instanceId)
    : PipelineBase(std::move(window), std::move(taskExecutor), std::move(assetManager), frontend, instanceId)
{}

RefPtr<PipelineContext> PipelineContext::GetCurrentContext()
{
    auto currentContainer = Container::Current();
    CHECK_NULL_RETURN(currentContainer, nullptr);
    return DynamicCast<PipelineContext>(currentContainer->GetPipelineContext());
}

float PipelineContext::GetCurrentRootWidth()
{
    auto context = GetCurrentContext();
    CHECK_NULL_RETURN(context, 0.0f);
    return static_cast<float>(context->rootWidth_);
}

float PipelineContext::GetCurrentRootHeight()
{
    auto context = GetCurrentContext();
    CHECK_NULL_RETURN(context, 0.0f);
    return static_cast<float>(context->rootHeight_);
}

void PipelineContext::AddDirtyCustomNode(const RefPtr<CustomNode>& dirtyNode)
{
    CHECK_RUN_ON(UI);
    CHECK_NULL_VOID(dirtyNode);
    dirtyNodes_.emplace(dirtyNode);
    hasIdleTasks_ = true;
    window_->RequestFrame();
}

void PipelineContext::AddDirtyLayoutNode(const RefPtr<FrameNode>& dirty)
{
    CHECK_RUN_ON(UI);
    CHECK_NULL_VOID(dirty);
    UITaskScheduler::GetInstance()->AddDirtyLayoutNode(dirty);
    hasIdleTasks_ = true;
    window_->RequestFrame();
}

void PipelineContext::AddDirtyRenderNode(const RefPtr<FrameNode>& dirty)
{
    CHECK_RUN_ON(UI);
    CHECK_NULL_VOID(dirty);
    UITaskScheduler::GetInstance()->AddDirtyRenderNode(dirty);
    hasIdleTasks_ = true;
    window_->RequestFrame();
}

void PipelineContext::FlushDirtyNodeUpdate()
{
    CHECK_RUN_ON(UI);
    ACE_FUNCTION_TRACE();
    if (FrameReport::GetInstance().GetEnable()) {
        FrameReport::GetInstance().BeginFlushBuild();
    }

    decltype(dirtyNodes_) dirtyNodes(std::move(dirtyNodes_));
    for (const auto& weakNode : dirtyNodes) {
        auto node = weakNode.Upgrade();
        if (node) {
            node->Update();
        }
    }

    if (FrameReport::GetInstance().GetEnable()) {
        FrameReport::GetInstance().EndFlushBuild();
    }
}

uint32_t PipelineContext::AddScheduleTask(const RefPtr<ScheduleTask>& task)
{
    CHECK_RUN_ON(UI);
    scheduleTasks_.try_emplace(++nextScheduleTaskId_, task);
    window_->RequestFrame();
    return nextScheduleTaskId_;
}

void PipelineContext::FlushVsync(uint64_t nanoTimestamp, uint32_t frameCount)
{
    CHECK_RUN_ON(UI);
    ACE_FUNCTION_TRACE();
    static const std::string abilityName = AceApplicationInfo::GetInstance().GetProcessName().empty()
                                               ? AceApplicationInfo::GetInstance().GetPackageName()
                                               : AceApplicationInfo::GetInstance().GetProcessName();
    window_->RecordFrameTime(nanoTimestamp, abilityName);
    FlushAnimation(GetTimeFromExternalTimer());
    FlushPipelineWithoutAnimation();
}

void PipelineContext::FlushAnimation(uint64_t nanoTimestamp)
{
    CHECK_RUN_ON(UI);
    ACE_FUNCTION_TRACE();

    if (FrameReport::GetInstance().GetEnable()) {
        FrameReport::GetInstance().BeginFlushAnimation();
    }

    decltype(scheduleTasks_) temp(std::move(scheduleTasks_));
    for (const auto& [id, weakTask] : temp) {
        auto task = weakTask.Upgrade();
        if (task) {
            task->OnFrame(nanoTimestamp);
        }
    }

    if (FrameReport::GetInstance().GetEnable()) {
        FrameReport::GetInstance().EndFlushAnimation();
    }
}

void PipelineContext::FlushMessages()
{
    ACE_FUNCTION_TRACE();
    window_->FlushTasks();
}

void PipelineContext::FlushPipelineWithoutAnimation()
{
    FlushDirtyNodeUpdate();
    FlushTouchEvents();
    UITaskScheduler::GetInstance()->FlushTask();
    FlushMessages();
}

void PipelineContext::SetupRootElement()
{
    CHECK_RUN_ON(UI);
    rootNode_ = FrameNode::CreateFrameNodeWithTree(
        V2::ROOT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), MakeRefPtr<StagePattern>());
    rootNode_->SetHostRootId(GetInstanceId());
    rootNode_->SetHostPageId(-1);
    rootNode_->GetRenderContext()->UpdateBackgroundColor(Color::WHITE);
    CalcSize idealSize { CalcLength(rootWidth_), CalcLength(rootHeight_) };
    MeasureProperty layoutConstraint;
    layoutConstraint.selfIdealSize = idealSize;
    layoutConstraint.maxSize = idealSize;
    rootNode_->UpdateLayoutConstraint(layoutConstraint);
    window_->SetRootFrameNode(rootNode_);
    stageManager_ = MakeRefPtr<StageManager>(rootNode_);
    rootNode_->AttachToMainTree();
    LOGI("SetupRootElement success!");
}

RefPtr<StageManager> PipelineContext::GetStageManager()
{
    return stageManager_;
}

void PipelineContext::SetRootRect(double width, double height, double offset)
{
    CHECK_RUN_ON(UI);
    UpdateRootSizeAndScale(width, height);
    if (!rootNode_) {
        LOGE("rootNode_ is nullptr");
        return;
    }
    LOGI("SetRootRect %{public}f %{public}f", width, height);
    SizeF sizeF { static_cast<float>(width), static_cast<float>(height) };
    if (rootNode_->GetGeometryNode()->GetFrameSize() != sizeF) {
        CalcSize idealSize { CalcLength(width), CalcLength(height) };
        MeasureProperty layoutConstraint;
        layoutConstraint.selfIdealSize = idealSize;
        layoutConstraint.maxSize = idealSize;
        rootNode_->UpdateLayoutConstraint(layoutConstraint);
        rootNode_->MarkDirtyNode();
    }
}

bool PipelineContext::OnBackPressed()
{
    LOGD("OnBackPressed");
    CHECK_RUN_ON(PLATFORM);
    auto frontend = weakFrontend_.Upgrade();
    if (!frontend) {
        // return back.
        return false;
    }

    auto result = false;
    taskExecutor_->PostSyncTask(
        [weakFrontend = weakFrontend_, weakPipelineContext = WeakClaim(this), &result]() {
            auto frontend = weakFrontend.Upgrade();
            if (!frontend) {
                LOGW("frontend is nullptr");
                result = false;
                return;
            }
            result = frontend->OnBackPressed();
        },
        TaskExecutor::TaskType::JS);

    if (result) {
        // user accept
        LOGI("CallRouterBackToPopPage(): frontend accept");
        return true;
    }
    LOGI("CallRouterBackToPopPage(): return platform consumed");
    return false;
}

void PipelineContext::OnTouchEvent(const TouchEvent& point, bool isSubPipe)
{
    CHECK_RUN_ON(UI);
    touchEvents_.emplace_back(point);
    hasIdleTasks_ = true;
    window_->RequestFrame();
}

void PipelineContext::OnSurfaceDensityChanged(double density)
{
    CHECK_RUN_ON(UI);
    LOGI("OnSurfaceDensityChanged density_(%{public}lf)", density_);
    LOGI("OnSurfaceDensityChanged dipScale_(%{public}lf)", dipScale_);
    density_ = density;
    if (!NearZero(viewScale_)) {
        LOGI("OnSurfaceDensityChanged viewScale_(%{public}lf)", viewScale_);
        dipScale_ = density_ / viewScale_;
    }
}

void PipelineContext::FlushTouchEvents()
{
    CHECK_RUN_ON(UI);
    if (!rootNode_) {
        LOGE("root node is nullptr");
        return;
    }
    {
        ACE_SCOPED_TRACE("PipelineContext::DispatchTouchEvent");
        decltype(touchEvents_) touchEvents(std::move(touchEvents_));
        for (const auto& touchEvent : touchEvents) {
            auto scalePoint = touchEvent.CreateScalePoint(GetViewScale());
            LOGD("AceTouchEvent: x = %{public}f, y = %{public}f, type = %{public}zu", scalePoint.x, scalePoint.y,
                scalePoint.type);
            if (scalePoint.type == TouchType::DOWN) {
                LOGD("receive touch down event, first use touch test to collect touch event target");
                TouchRestrict touchRestrict { TouchRestrict::NONE };
                touchRestrict.sourceType = touchEvent.sourceType;
                eventManager_->TouchTest(scalePoint, rootNode_, touchRestrict, false);
            }
            eventManager_->DispatchTouchEvent(scalePoint);
        }
    }
}

} // namespace OHOS::Ace::NG
