/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RENDER_WEB_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RENDER_WEB_H

#include "core/components/common/layout/constants.h"
#include "core/components/text_overlay/text_overlay_component.h"
#include "core/components/web/resource/web_delegate.h"
#include "core/components/web/web_component.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {
namespace {
#ifdef OHOS_STANDARD_SYSTEM
struct TouchInfo {
    double x = -1;
    double y = -1;
    int32_t id = -1;
};

struct TouchHandleState {
    int32_t id = -1;
    int32_t x = -1;
    int32_t y = -1;
    int32_t edge_height = 0;
};

enum WebOverlayType {
    INSERT_OVERLAY,
    SELECTION_OVERLAY,
    INVALID_OVERLAY
};
#endif
}

class RenderWeb : public RenderNode {
    DECLARE_ACE_TYPE(RenderWeb, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    RenderWeb();
    ~RenderWeb() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnAttachContext() override;
    void OnMouseEvent(const MouseEvent& event);
    bool HandleMouseEvent(const MouseEvent& event) override;
    void HandleKeyEvent(const KeyEvent& keyEvent);

#ifdef OHOS_STANDARD_SYSTEM
    void OnAppShow() override;
    void OnAppHide() override;
    void OnPositionChanged() override;
    void OnSizeChanged() override;
    void HandleTouchDown(const TouchEventInfo& info, bool fromOverlay);
    void HandleTouchUp(const TouchEventInfo& info, bool fromOverlay);
    void HandleTouchMove(const TouchEventInfo& info, bool fromOverlay);
    void HandleTouchCancel(const TouchEventInfo& info);
    void HandleDoubleClick(const ClickInfo& info);
    
    // Related to text overlay
    void SetUpdateHandlePosition(
        const std::function<void(const OverlayShowOption&, float, float)>& updateHandlePosition);
    bool RunQuickMenu(
        std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> params,
        std::shared_ptr<OHOS::NWeb::NWebQuickMenuCallback> callback);
    void OnQuickMenuDismissed();
    void OnTouchSelectionChanged(
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> insertHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> startSelectionHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> endSelectionHandle);
    bool TextOverlayMenuShouldShow() const;
    bool GetShowStartTouchHandle() const;
    bool GetShowEndTouchHandle() const;
#endif

    void SetDelegate(const RefPtr<WebDelegate>& delegate)
    {
        delegate_ = delegate;
    }

    RefPtr<WebDelegate> GetDelegate() const
    {
        return delegate_;
    }

    void HandleAxisEvent(const AxisEvent& event) override;
    bool IsAxisScrollable(AxisDirection direction) override;
    WeakPtr<RenderNode> CheckAxisNode() override;

protected:
    RefPtr<WebDelegate> delegate_;
    RefPtr<WebComponent> web_;
    Size drawSize_;
    bool isUrlLoaded_ = false;

private:
#ifdef OHOS_STANDARD_SYSTEM
    void Initialize();
    bool ParseTouchInfo(const TouchEventInfo& info, std::list<TouchInfo>& touchInfos, const TouchType& touchType);
    void OnTouchTestHit(const Offset& coordinateOffset, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;
    bool IsTouchHandleValid(std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> handle);
    bool IsTouchHandleShow(std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> handle);
    WebOverlayType GetTouchHandleOverlayType(
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> insertHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> startSelectionHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> endSelectionHandle);
    RefPtr<TextOverlayComponent> CreateTextOverlay(
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> insertHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> startSelectionHandle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> endSelectionHandle);
    void PushTextOverlayToStack();
    void PopTextOverlay();
    Offset NormalizeTouchHandleOffset(float x, float y);
    void RegisterTextOverlayCallback(
        int32_t flags, std::shared_ptr<OHOS::NWeb::NWebQuickMenuCallback> callback);

    RefPtr<RawRecognizer> touchRecognizer_ = nullptr;
    RefPtr<ClickRecognizer> doubleClickRecognizer_ = nullptr;
    OnMouseCallback onMouse_;
    OnKeyEventCallback onKeyEvent_;
    RefPtr<TextOverlayComponent> textOverlay_;
    WeakPtr<StackElement> stackElement_;
    std::function<void(const OverlayShowOption&, float, float)> updateHandlePosition_ = nullptr;

    bool showTextOveralyMenu_ = false;
    bool showStartTouchHandle_ = false;
    bool showEndTouchHandle_ = false;
#endif

    Offset position_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RENDER_WEB_H
