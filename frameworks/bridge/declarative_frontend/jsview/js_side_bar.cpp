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

#include "frameworks/bridge/declarative_frontend/jsview/js_side_bar.h"

#include "base/geometry/dimension.h"
#include "base/log/log.h"
#include "core/components/button/button_component.h"
#include "core/components/side_bar/render_side_bar_container.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {
enum class WidthType : uint32_t {
    SIDEBAR_WIDTH = 0,
    MIN_SIDEBAR_WIDTH,
    MAX_SIDEBAR_WIDTH,
};

void ParseAndSetWidth(const JSCallbackInfo& info, WidthType widthType)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Dimension value;
    if (!JSViewAbstract::ParseJsDimensionVp(info[0], value)) {
        return;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        LOGW("JSSideBar::ParseAndSetWidth info[0] value is less than 0, the default is set to 0.");
        value.SetValue(0.0);
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::SideBarContainerComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("side bar is null");
        return;
    }

    bool isPercentSize = value.Unit() == DimensionUnit::PERCENT;
    if (isPercentSize) {
        component->SetIsPercentSize(isPercentSize);
    }

    switch (widthType) {
        case WidthType::SIDEBAR_WIDTH:
            component->SetSideBarWidth(value);
            break;
        case WidthType::MIN_SIDEBAR_WIDTH:
            component->SetSideBarMinWidth(value);
            break;
        case WidthType::MAX_SIDEBAR_WIDTH:
            component->SetSideBarMaxWidth(value);
            break;
        default:
            break;
    }
}

} // namespace

void JSSideBar::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    SideBarContainerType style = SideBarContainerType::EMBED;
    if (!info[0]->IsNull()) {
        if (info[0]->IsBoolean()) {
            style = static_cast<SideBarContainerType>(info[0]->ToBoolean());
        } else if (info[0]->IsNumber()) {
            style = static_cast<SideBarContainerType>(info[0]->ToNumber<int>());
        } else {
            LOGE("The arg is wrong");
            return;
        }
    }

    std::list<RefPtr<Component>> children;
    auto sideBarContainer = AceType::MakeRefPtr<OHOS::Ace::SideBarContainerComponent>(children);
    ViewStackProcessor::GetInstance()->ClaimElementId(sideBarContainer);
    sideBarContainer->SetMainStackSize(MainStackSize::MAX);
    sideBarContainer->SetSideBarContainerType(style);

    auto stack = ViewStackProcessor::GetInstance();
    stack->Push(sideBarContainer);
    JSInteractableView::SetFocusable(false);
    JSInteractableView::SetFocusNode(true);
}

void JSSideBar::SetShowControlButton(bool isShow)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::SideBarContainerComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("side bar is null");
        return;
    }

    component->SetShowControlButton(isShow);
}

void JSSideBar::JSBind(BindingTarget globalObj)
{
    JSClass<JSSideBar>::Declare("SideBarContainer");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSSideBar>::StaticMethod("create", &JSSideBar::Create, opt);
    JSClass<JSSideBar>::StaticMethod("showSideBar", &JSSideBar::JsShowSideBar);
    JSClass<JSSideBar>::StaticMethod("controlButton", &JSSideBar::JsControlButton);
    JSClass<JSSideBar>::StaticMethod("showControlButton", &JSSideBar::SetShowControlButton);
    JSClass<JSSideBar>::StaticMethod("onChange", &JSSideBar::OnChange);
    JSClass<JSSideBar>::StaticMethod("sideBarWidth", &JSSideBar::JsSideBarWidth);
    JSClass<JSSideBar>::StaticMethod("minSideBarWidth", &JSSideBar::JsMinSideBarWidth);
    JSClass<JSSideBar>::StaticMethod("maxSideBarWidth", &JSSideBar::JsMaxSideBarWidth);
    JSClass<JSSideBar>::StaticMethod("autoHide", &JSSideBar::JsAutoHide);
    JSClass<JSSideBar>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSSideBar>::StaticMethod("width", SetWidth);
    JSClass<JSSideBar>::StaticMethod("height", SetHeight);
    JSClass<JSSideBar>::StaticMethod("size", SetSize);
    JSClass<JSSideBar>::StaticMethod("width", &JSStack::SetWidth);
    JSClass<JSSideBar>::StaticMethod("height", &JSStack::SetHeight);
    JSClass<JSSideBar>::StaticMethod("size", &JSStack::SetSize);
    JSClass<JSSideBar>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSSideBar>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSSideBar>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSSideBar>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSSideBar>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSSideBar>::Inherit<JSContainerBase>();
    JSClass<JSSideBar>::Inherit<JSViewAbstract>();
    JSClass<JSSideBar>::Bind(globalObj);
}

void JSSideBar::OnChange(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&SideBarContainerComponent::SetOnChange, info)) {
        LOGE("Failed to bind event");
    }
    info.ReturnSelf();
}



void JSSideBar::JsSideBarWidth(const JSCallbackInfo& info)
{
    ParseAndSetWidth(info, WidthType::SIDEBAR_WIDTH);
}

void JSSideBar::JsMaxSideBarWidth(const JSCallbackInfo& info)
{
    ParseAndSetWidth(info, WidthType::MAX_SIDEBAR_WIDTH);
}

void JSSideBar::JsMinSideBarWidth(const JSCallbackInfo& info)
{
    ParseAndSetWidth(info, WidthType::MIN_SIDEBAR_WIDTH);
}

void JSSideBar::JsShowSideBar(bool isShow)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::SideBarContainerComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("side bar is null");
        return;
    }
    component->SetShowSideBar(isShow);
}

void JSSideBar::JsControlButton(const JSCallbackInfo& info)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::SideBarContainerComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("side bar is null");
        return;
    }

    if (!info[0]->IsNull() && info[0]->IsObject()) {
        JSRef<JSObject> value = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> width = value->GetProperty("width");
        JSRef<JSVal> height = value->GetProperty("height");
        JSRef<JSVal> left = value->GetProperty("left");
        JSRef<JSVal> top = value->GetProperty("top");
        JSRef<JSVal> icons = value->GetProperty("icons");

        if (!width->IsNull() && width->IsNumber()) {
            component->SetButtonWidth(width->ToNumber<double>());
        }

        if (!height->IsNull() && height->IsNumber()) {
            component->SetButtonHeight(height->ToNumber<double>());
        }

        if (!left->IsNull() && left->IsNumber()) {
            component->SetButtonLeft(left->ToNumber<double>());
        }

        if (!top->IsNull() && top->IsNumber()) {
            component->SetButtonTop(top->ToNumber<double>());
        }

        if (!icons->IsNull() && icons->IsObject()) {
            JSRef<JSObject> iconsVal = JSRef<JSObject>::Cast(icons);
            JSRef<JSVal> showIcon = iconsVal->GetProperty("shown");
            JSRef<JSVal> switchingIcon = iconsVal->GetProperty("switching");
            JSRef<JSVal> hiddenIcon = iconsVal->GetProperty("hidden");
            std::string showIconStr;
            if (!showIcon->IsNull() && ParseJsMedia(showIcon, showIconStr)) {
                component->SetShowIcon(showIconStr);
            }
            std::string hiddenIconStr;
            if (!hiddenIcon->IsNull() && ParseJsMedia(hiddenIcon, hiddenIconStr)) {
                component->SetHiddenIcon(hiddenIconStr);
            }
            std::string switchingIconStr;
            if (!switchingIcon->IsNull() && ParseJsMedia(switchingIcon, switchingIconStr)) {
                component->SetSwitchIcon(switchingIconStr);
            }
        }
    }
}

void JSSideBar::JsAutoHide(bool autoHide)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::SideBarContainerComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("side bar is null");
        return;
    }
    component->SetAutoHide(autoHide);
}

} // namespace OHOS::Ace::Framework
