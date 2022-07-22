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


#include "core/components/box/drag_drop_event.h"

#include "core/gestures/long_press_recognizer.h"
#include "core/gestures/pan_recognizer.h"
#include "core/gestures/sequenced_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t DEFAULT_FINGERS = 1;
constexpr int32_t DEFAULT_DURATION = 150;
constexpr int32_t DEFAULT_DISTANCE = 0;

}; // namespace

void DragDropEvent::CreateDragDropRecognizer(const WeakPtr<PipelineContext>& pipelineContext)
{
    if (dragDropGesture_) {
        return;
    }

    auto context = pipelineContext.Upgrade();
    if (!context) {
        return;
    }

    auto longPressRecognizer = AceType::MakeRefPtr<OHOS::Ace::LongPressRecognizer>(
        context, DEFAULT_DURATION, DEFAULT_FINGERS, false, true, false);
    longPressRecognizer->SetOnAction(std::bind(&DragDropEvent::LongPressOnAction, this, std::placeholders::_1));
    PanDirection panDirection;
    auto panRecognizer =
        AceType::MakeRefPtr<OHOS::Ace::PanRecognizer>(context, DEFAULT_FINGERS, panDirection, DEFAULT_DISTANCE);
    panRecognizer->SetOnActionStart(std::bind(&DragDropEvent::PanOnActionStart, this, std::placeholders::_1));
    panRecognizer->SetOnActionUpdate(std::bind(&DragDropEvent::PanOnActionUpdate, this, std::placeholders::_1));
    panRecognizer->SetOnActionEnd(std::bind(&DragDropEvent::PanOnActionEnd, this, std::placeholders::_1));
    panRecognizer->SetOnActionCancel(std::bind(&DragDropEvent::PanOnActionCancel, this));

    std::vector<RefPtr<GestureRecognizer>> recognizers { longPressRecognizer, panRecognizer };
    dragDropGesture_ = AceType::MakeRefPtr<OHOS::Ace::SequencedRecognizer>(pipelineContext, recognizers);
    dragDropGesture_->SetIsExternalGesture(true);
}

void DragDropEvent::LongPressOnAction(const GestureEvent& info)
{
    startPoint_ = info.GetGlobalPoint();
}

RefPtr<DragDropEvent> DragDropEvent::FindDragDropNode(const RefPtr<PipelineContext>& context, const GestureEvent& info)
{
    if (!context) {
        return nullptr;
    }

    auto pageRenderNode = context->GetLastPageRender();
    if (!pageRenderNode) {
        return nullptr;
    }
    auto offset = context->GetStageRect().GetOffset();
    auto targetRenderNode = pageRenderNode->FindDropChild(info.GetGlobalPoint(), info.GetGlobalPoint() - offset);
    if (!targetRenderNode) {
        return nullptr;
    }
    return AceType::DynamicCast<DragDropEvent>(targetRenderNode);
}

void DragDropEvent::AddDataToClipboard(const RefPtr<PipelineContext>& context, const std::string& extraInfo,
    const std::string& selectedText, const std::string& imageSrc)
{
    auto seleItemSizeStr = JsonUtil::Create(true);
    seleItemSizeStr->Put("width", selectedItemSize_.Width());
    seleItemSizeStr->Put("height", selectedItemSize_.Height());
    seleItemSizeStr->Put("customDragInfo", extraInfo.c_str());
    seleItemSizeStr->Put("selectedText", selectedText.c_str());
    seleItemSizeStr->Put("imageSrc", imageSrc.c_str());

    auto clipboard = ClipboardProxy::GetInstance()->GetClipboard(context->GetTaskExecutor());
    clipboard->SetData(seleItemSizeStr->ToString());
}

} // namespace OHOS::Ace