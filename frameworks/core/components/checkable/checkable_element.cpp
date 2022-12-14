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

#include "core/components/checkable/checkable_element.h"

#include "core/components/checkable/render_checkable.h"

namespace OHOS::Ace {

void CheckableElement::Update()
{
    RenderElement::Update();
    customComponent_ = component_;
    auto labelTarget = AceType::DynamicCast<LabelTarget>(component_);
    if (!labelTarget) {
        // switch is not label target, radio and checkbox are label target
        LOGD("not find label target");
        return;
    }
    auto trigger = labelTarget->GetTrigger();
    if (!trigger) {
        // component not set label trigger
        LOGE("get label trigger failed");
        return;
    }
    auto weak = AceType::WeakClaim(this);
    trigger->clickHandler_ = [weak]() {
        auto checkable = weak.Upgrade();
        if (checkable) {
            checkable->OnClick();
        }
    };
}

void CheckableElement::OnClick()
{
    auto renderCheckable = AceType::DynamicCast<RenderCheckable>(renderNode_);
    if (!renderCheckable || renderCheckable->IsDisable()) {
        return;
    }
    renderCheckable->HandleClick();
}

void CheckableElement::OnFocus()
{
    auto context = context_.Upgrade();
    if (context && context->GetIsTabKeyPressed() && renderNode_) {
        renderNode_->ChangeStatus(RenderStatus::FOCUS);
    }
}

void CheckableElement::OnBlur()
{
    auto context = context_.Upgrade();
    if (context && context->GetIsTabKeyPressed() && renderNode_) {
        renderNode_->ChangeStatus(RenderStatus::BLUR);
    }
}

bool CheckableElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    auto renderCheckable = AceType::DynamicCast<RenderCheckable>(renderNode_);
    if (!renderCheckable || renderCheckable->IsDisable()) {
        return false;
    }

    if (keyEvent.action != KeyAction::DOWN) {
        return false;
    }

    switch (keyEvent.code) {
        case KeyCode::KEY_ENTER:
        case KeyCode::KEY_NUMPAD_ENTER:
        case KeyCode::KEY_DPAD_CENTER:
        case KeyCode::KEY_SPACE:
            renderCheckable->HandleClick();
            return true;
        default:
            return false;
    }
}

} // namespace OHOS::Ace
