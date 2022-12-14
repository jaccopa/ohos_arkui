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

#include "bridge/declarative_frontend/jsview/js_scroll.h"

#include "bridge/declarative_frontend/jsview/js_scroller.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/scroll/scroll_component.h"
#include "core/components_ng/pattern/scroll/scroll_view.h"

namespace OHOS::Ace::Framework {
namespace {
    const std::vector<DisplayMode> DISPLAY_MODE = {DisplayMode::OFF, DisplayMode::AUTO, DisplayMode::ON};
    const std::vector<Axis> AXIS = { Axis::VERTICAL, Axis::HORIZONTAL, Axis::FREE, Axis::NONE };
}
void JSScroll::Create(const JSCallbackInfo& info)
{
    if (Container::IsCurrentUseNewPipeline()) {
        NG::ScrollView::Create();
        return;
    }
    
    RefPtr<Component> child;
    auto scrollComponent = AceType::MakeRefPtr<OHOS::Ace::ScrollComponent>(child);
    ViewStackProcessor::GetInstance()->ClaimElementId(scrollComponent);
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSScroller* jsScroller = JSRef<JSObject>::Cast(info[0])->Unwrap<JSScroller>();
        if (jsScroller) {
            auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
            jsScroller->SetController(positionController);
            scrollComponent->SetScrollPositionController(positionController);

            // Init scroll bar proxy.
            auto proxy = jsScroller->GetScrollBarProxy();
            if (!proxy) {
                proxy = AceType::MakeRefPtr<ScrollBarProxy>();
                jsScroller->SetScrollBarProxy(proxy);
            }
            scrollComponent->SetScrollBarProxy(proxy);
        }
    } else {
        auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
        scrollComponent->SetScrollPositionController(positionController);
    }
    // init scroll bar
    std::pair<bool, Color> barColor;
    barColor.first = false;
    std::pair<bool, Dimension> barWidth;
    barWidth.first = false;
    scrollComponent->InitScrollBar(GetTheme<ScrollBarTheme>(), barColor, barWidth, EdgeEffect::NONE);
    ViewStackProcessor::GetInstance()->Push(scrollComponent);
}

void JSScroll::SetScrollable(int32_t value)
{
    if (value < 0 || value >= static_cast<int32_t>(AXIS.size())) {
        LOGE("value is not valid: %{public}d", value);
        return;
    }

    if (Container::IsCurrentUseNewPipeline()) {
        NG::ScrollView::SetAxis(AXIS[value]);
        return;
    }

    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto scrollComponent = AceType::DynamicCast<ScrollComponent>(component);
    if (scrollComponent) {
        scrollComponent->SetAxisDirection(AXIS[value]);
    }
}

void JSScroll::OnScrollBeginCallback(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onScrollBegin =
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])]
                (const Dimension& dx, const Dimension& dy) -> ScrollInfo {
                    ScrollInfo scrollInfo { .dx = dx, .dy = dy };
                    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx, scrollInfo);
                    auto params = ConvertToJSValues(dx, dy);
                    auto result = func->Call(JSRef<JSObject>(), params.size(), params.data());
                    if (result.IsEmpty()) {
                        LOGE("Error calling onScrollBegin, result is empty.");
                        return scrollInfo;
                    }

                    if (!result->IsObject()) {
                        LOGE("Error calling onScrollBegin, result is not object.");
                        return scrollInfo;
                    }

                    auto resObj = JSRef<JSObject>::Cast(result);
                    auto dxRemainValue = resObj->GetProperty("dxRemain");
                    if (dxRemainValue->IsNumber()) {
                        scrollInfo.dx = Dimension(dxRemainValue->ToNumber<float>(), DimensionUnit::VP);
                    }
                    auto dyRemainValue = resObj->GetProperty("dyRemain");
                    if (dyRemainValue->IsNumber()) {
                        scrollInfo.dy = Dimension(dyRemainValue->ToNumber<float>(), DimensionUnit::VP);
                    }
                    return scrollInfo;
                };

        auto scrollComponent =
            AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (scrollComponent) {
            if (!scrollComponent->GetScrollPositionController()) {
                scrollComponent->SetScrollPositionController(AceType::MakeRefPtr<ScrollPositionController>());
            }
            scrollComponent->SetOnScrollBegin(onScrollBegin);
        }
    }
}

void JSScroll::OnScrollCallback(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onScroll = EventMarker(
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                auto eventInfo = TypeInfoHelper::DynamicCast<ScrollEventInfo>(info);
                if (!eventInfo) {
                    return;
                }
                auto params = ConvertToJSValues(eventInfo->GetScrollX(), eventInfo->GetScrollY());
                func->Call(JSRef<JSObject>(), params.size(), params.data());
            });
        auto scrollComponent =
            AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (scrollComponent) {
            if (!scrollComponent->GetScrollPositionController()) {
                scrollComponent->SetScrollPositionController(AceType::MakeRefPtr<ScrollPositionController>());
            }
            scrollComponent->SetOnScroll(onScroll);
        }
    }
    args.SetReturnValue(args.This());
}

void JSScroll::OnScrollEdgeCallback(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onScroll = EventMarker(
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                auto eventInfo = TypeInfoHelper::DynamicCast<ScrollEventInfo>(info);
                if (!eventInfo) {
                    return;
                }
                int32_t eventType = -1;
                if (eventInfo->GetType() == ScrollEvent::SCROLL_TOP) {
                    eventType = 0; // 0 means Edge.Top
                } else if (eventInfo->GetType() == ScrollEvent::SCROLL_BOTTOM) {
                    eventType = 2; // 2 means Edge.Bottom
                } else {
                    LOGE("EventType is not support: %{public}d", static_cast<int32_t>(eventInfo->GetType()));
                    return;
                }
                auto param = ConvertToJSValue(eventType);
                func->Call(JSRef<JSObject>(), 1, &param);
            });
        auto scrollComponent =
            AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (scrollComponent) {
            if (!scrollComponent->GetScrollPositionController()) {
                scrollComponent->SetScrollPositionController(AceType::MakeRefPtr<ScrollPositionController>());
            }
            scrollComponent->SetOnScrollEdge(onScroll);
        }
    }
    args.SetReturnValue(args.This());
}

void JSScroll::OnScrollEndCallback(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onScrollStop = EventMarker(
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                func->Call(JSRef<JSObject>(), 0, nullptr);
            });
        auto scrollComponent =
            AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (scrollComponent) {
            if (!scrollComponent->GetScrollPositionController()) {
                scrollComponent->SetScrollPositionController(AceType::MakeRefPtr<ScrollPositionController>());
            }
            scrollComponent->SetOnScrollEnd(onScrollStop);
        }
    }
    args.SetReturnValue(args.This());
}

void JSScroll::JSBind(BindingTarget globalObj)
{
    JSClass<JSScroll>::Declare("Scroll");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSScroll>::StaticMethod("create", &JSScroll::Create, opt);
    JSClass<JSScroll>::StaticMethod("scrollable", &JSScroll::SetScrollable, opt);
    JSClass<JSScroll>::StaticMethod("onScrollBegin", &JSScroll::OnScrollBeginCallback, opt);
    JSClass<JSScroll>::StaticMethod("onScroll", &JSScroll::OnScrollCallback, opt);
    JSClass<JSScroll>::StaticMethod("onScrollEdge", &JSScroll::OnScrollEdgeCallback, opt);
    JSClass<JSScroll>::StaticMethod("onScrollEnd", &JSScroll::OnScrollEndCallback, opt);
    JSClass<JSScroll>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSScroll>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSScroll>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSScroll>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSScroll>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSScroll>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSScroll>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSScroll>::StaticMethod("edgeEffect", &JSScroll::SetEdgeEffect, opt);
    JSClass<JSScroll>::StaticMethod("scrollBar", &JSScroll::SetScrollBar, opt);
    JSClass<JSScroll>::StaticMethod("scrollBarColor", &JSScroll::SetScrollBarColor, opt);
    JSClass<JSScroll>::StaticMethod("scrollBarWidth", &JSScroll::SetScrollBarWidth, opt);
    JSClass<JSScroll>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSScroll>::Inherit<JSContainerBase>();
    JSClass<JSScroll>::Inherit<JSViewAbstract>();
    JSClass<JSScroll>::Bind<>(globalObj);
}

void JSScroll::SetScrollBar(int displayMode)
{
    auto scrollComponent = AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!scrollComponent) {
        return;
    }
    if (displayMode >= 0 && displayMode < static_cast<int32_t>(DISPLAY_MODE.size())) {
        scrollComponent->SetDisplayMode(DISPLAY_MODE[displayMode]);
    }
}

void JSScroll::SetScrollBarWidth(const std::string& scrollBarWidth)
{
    auto scrollComponent = AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!scrollComponent || scrollBarWidth.empty()) {
        return;
    }
    scrollComponent->SetScrollBarWidth(StringUtils::StringToDimension(scrollBarWidth));
}

void JSScroll::SetScrollBarColor(const  std::string& scrollBarColor)
{
    auto scrollComponent = AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!scrollComponent || scrollBarColor.empty()) {
        return;
    }
    scrollComponent->SetScrollBarColor(Color::FromString(scrollBarColor));
}

void JSScroll::SetEdgeEffect(int edgeEffect)
{
    auto scrollComponent = AceType::DynamicCast<ScrollComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!scrollComponent) {
        return;
    }
    RefPtr<ScrollEdgeEffect> scrollEdgeEffect;
    if (edgeEffect == 0) {
        scrollEdgeEffect = AceType::MakeRefPtr<ScrollSpringEffect>();
    } else if (edgeEffect == 1) {
        scrollEdgeEffect = AceType::MakeRefPtr<ScrollFadeEffect>(Color::GRAY);
    } else {
        scrollEdgeEffect = AceType::MakeRefPtr<ScrollEdgeEffect>(EdgeEffect::NONE);
    }
    scrollComponent->SetScrollEffect(scrollEdgeEffect);
}

} // namespace OHOS::Ace::Framework
