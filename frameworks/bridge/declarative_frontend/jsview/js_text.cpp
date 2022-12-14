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

#include "frameworks/bridge/declarative_frontend/jsview/js_text.h"

#include <sstream>
#include <string>
#include <vector>

#include "base/geometry/dimension.h"
#include "base/log/ace_trace.h"
#include "core/components/declaration/text/text_declaration.h"
#include "core/components/text/text_theme.h"
#include "core/components_ng/pattern/text/text_view.h"
#include "core/event/ace_event_handler.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_click_function.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_drag_function.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr Dimension DEFAULT_FONT_SIZE = 30.0_px;
const std::vector<TextCase> TEXT_CASES = { TextCase::NORMAL, TextCase::LOWERCASE, TextCase::UPPERCASE };
const std::vector<TextOverflow> TEXT_OVERFLOWS = { TextOverflow::CLIP, TextOverflow::ELLIPSIS, TextOverflow::NONE };
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const std::vector<TextAlign> TEXT_ALIGNS = { TextAlign::START, TextAlign::CENTER, TextAlign::END, TextAlign::LEFT,
    TextAlign::RIGHT, TextAlign::JUSTIFY };

}; // namespace

void JSText::SetWidth(const JSCallbackInfo& info)
{
    JSViewAbstract::JsWidth(info);
    if (Container::IsCurrentUseNewPipeline()) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (!box) {
        LOGE("box is not valid");
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    component->SetMaxWidthLayout(box->GetWidthDimension().IsValid());
}

void JSText::SetHeight(const JSCallbackInfo& info)
{
    JSViewAbstract::JsHeight(info);
    if (Container::IsCurrentUseNewPipeline()) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (!box) {
        LOGE("box is not valid");
        return;
    }
    box->SetBoxClipFlag(true);
}

void JSText::SetFontSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension fontSize;
    if (!ParseJsDimensionFp(info[0], fontSize)) {
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetFontSize(fontSize);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetFontSize(fontSize);
    component->SetTextStyle(textStyle);
}

void JSText::SetFontWeight(const std::string& value)
{
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetFontWeight(ConvertStrToFontWeight(value));
        return;
    }

    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetFontWeight(ConvertStrToFontWeight(value));
    component->SetTextStyle(textStyle);
}

void JSText::SetTextColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color textColor;
    if (!ParseJsColor(info[0], textColor)) {
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetTextColor(textColor);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetTextColor(textColor);
    component->SetTextStyle(textStyle);
}

void JSText::SetTextOverflow(const JSCallbackInfo& info)
{
    do {
        if (!info[0]->IsObject()) {
            LOGE("info[0] not is Object");
            break;
        }
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> overflowValue = obj->GetProperty("overflow");
        if (!overflowValue->IsNumber()) {
            LOGE("overflow value is not a number");
            break;
        }
        auto overflow = overflowValue->ToNumber<int32_t>();
        if (overflow < 0 || overflow >= static_cast<int32_t>(TEXT_OVERFLOWS.size())) {
            LOGE("Text: textOverflow(%{public}d) illegal value", overflow);
            break;
        }
        if (Container::IsCurrentUseNewPipeline()) {
            NG::TextView::SetTextOverflow(TEXT_OVERFLOWS[overflow]);
            break;
        }
        auto component = GetComponent();
        if (!component) {
            LOGE("component is not valid");
            break;
        }
        auto textStyle = component->GetTextStyle();
        textStyle.SetTextOverflow(TEXT_OVERFLOWS[overflow]);
        component->SetTextStyle(textStyle);
    } while (false);

    info.SetReturnValue(info.This());
}

void JSText::SetMaxLines(int32_t value)
{
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetMaxLines(value);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetMaxLines(value);
    component->SetTextStyle(textStyle);
}

void JSText::SetFontStyle(int32_t value)
{
    if (value < 0 || value >= static_cast<int32_t>(FONT_STYLES.size())) {
        LOGE("Text fontStyle(%{public}d) illegal value", value);
        return;
    }

    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetItalicFontStyle(FONT_STYLES[value]);
        return;
    }

    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetFontStyle(FONT_STYLES[value]);
    component->SetTextStyle(textStyle);
}

void JSText::SetTextAlign(int32_t value)
{
    if (value < 0 || value >= static_cast<int32_t>(TEXT_ALIGNS.size())) {
        LOGE("Text: TextAlign(%d) expected positive number", value);
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetTextAlign(TEXT_ALIGNS[value]);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetTextAlign(TEXT_ALIGNS[value]);
    component->SetTextStyle(textStyle);
}

void JSText::SetAlign(int32_t value)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    Alignment alignment = ParseAlignment(value);
    if (NearEqual(alignment.GetHorizontal(), -1.0)) {
        component->SetAlignment(TextAlign::LEFT);
    } else if (NearEqual(alignment.GetHorizontal(), 0.0)) {
        component->SetAlignment(TextAlign::CENTER);
    } else if (NearEqual(alignment.GetHorizontal(), 1.0)) {
        component->SetAlignment(TextAlign::RIGHT);
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetAlignment(alignment);
}

void JSText::SetLineHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionFp(info[0], value)) {
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetLineHeight(value);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetLineHeight(value);
    component->SetTextStyle(textStyle);
}

void JSText::SetFontFamily(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    std::vector<std::string> fontFamilies;
    if (!ParseJsFontFamilies(info[0], fontFamilies)) {
        LOGE("Parse FontFamilies failed");
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetFontFamily(fontFamilies);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetFontFamilies(fontFamilies);
    component->SetTextStyle(textStyle);
}

void JSText::SetMinFontSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension fontSize;
    if (!ParseJsDimensionFp(info[0], fontSize)) {
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetAdaptMinFontSize(fontSize);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetAdaptMinFontSize(fontSize);
    component->SetTextStyle(textStyle);
}

void JSText::SetMaxFontSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension fontSize;
    if (!ParseJsDimensionFp(info[0], fontSize)) {
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetAdaptMaxFontSize(fontSize);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetAdaptMaxFontSize(fontSize);
    component->SetTextStyle(textStyle);
}

void JSText::SetLetterSpacing(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionFp(info[0], value)) {
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetLetterSpacing(value);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetLetterSpacing(value);
    component->SetTextStyle(textStyle);
}

void JSText::SetTextCase(int32_t value)
{
    if (value < 0 || value >= static_cast<int32_t>(TEXT_CASES.size())) {
        LOGE("Text textCase(%d) illegal value", value);
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetTextCase(TEXT_CASES[value]);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetTextCase(TEXT_CASES[value]);
    component->SetTextStyle(textStyle);
}

void JSText::SetBaselineOffset(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionFp(info[0], value)) {
        return;
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::SetBaselineOffset(value);
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetBaselineOffset(value);
    component->SetTextStyle(textStyle);
}

void JSText::SetDecoration(const JSCallbackInfo& info)
{
    do {
        if (!info[0]->IsObject()) {
            LOGE("info[0] not is Object");
            break;
        }
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> typeValue = obj->GetProperty("type");
        JSRef<JSVal> colorValue = obj->GetProperty("color");

        std::optional<TextDecoration> textDecoration;
        if (typeValue->IsNumber()) {
            textDecoration = static_cast<TextDecoration>(typeValue->ToNumber<int32_t>());
        }
        std::optional<Color> colorVal;
        Color result;
        if (ParseJsColor(colorValue, result)) {
            colorVal = result;
        }

        if (Container::IsCurrentUseNewPipeline()) {
            if (textDecoration) {
                NG::TextView::SetTextDecoration(textDecoration.value());
            }
            if (colorVal) {
                NG::TextView::SetTextDecorationColor(colorVal.value());
            }
            break;
        }

        auto component = GetComponent();
        if (!component) {
            LOGE("component is not valid");
            break;
        }
        auto textStyle = component->GetTextStyle();
        if (textDecoration) {
            textStyle.SetTextDecoration(textDecoration.value());
        }
        if (colorVal) {
            textStyle.SetTextDecorationColor(colorVal.value());
        }
        component->SetTextStyle(textStyle);
    } while (false);
    info.SetReturnValue(info.This());
}

void JSText::JsOnClick(const JSCallbackInfo& info)
{
    if (info[0]->IsFunction()) {
        auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
        if (!inspector) {
            LOGE("fail to get inspector for on click event");
            return;
        }
        auto impl = inspector->GetInspectorFunctionImpl();
        RefPtr<JsClickFunction> jsOnClickFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(info[0]));
        auto onClickId = EventMarker(
            [execCtx = info.GetExecutionContext(), func = std::move(jsOnClickFunc), impl](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                LOGD("About to call onclick method on js");
                const auto* clickInfo = TypeInfoHelper::DynamicCast<ClickInfo>(info);
                auto newInfo = *clickInfo;
                if (impl) {
                    impl->UpdateEventInfo(newInfo);
                }
                ACE_SCORING_EVENT("Text.onClick");
                func->Execute(newInfo);
            });
        auto click = ViewStackProcessor::GetInstance()->GetClickGestureListenerComponent();
        if (click) {
            click->SetOnClickId(onClickId);
        }
        auto component = GetComponent();
        if (component) {
            component->SetOnClick(onClickId);
        }

        auto focusableComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent(false);
        if (focusableComponent) {
            focusableComponent->SetOnClickId(onClickId);
        }
    }
}

void JSText::JsRemoteMessage(const JSCallbackInfo& info)
{
    EventMarker remoteMessageEventId;
    JSInteractableView::JsRemoteMessage(info, remoteMessageEventId);
    auto click = ViewStackProcessor::GetInstance()->GetClickGestureListenerComponent();
    if (click) {
        click->SetRemoteMessageId(remoteMessageEventId);
    }
    auto textComponent = GetComponent();
    if (textComponent) {
        textComponent->SetRemoteMessageEvent(remoteMessageEventId);
    }
}

void JSText::Create(const JSCallbackInfo& info)
{
    std::string data;
    if (info.Length() > 0) {
        ParseJsString(info[0], data);
    }

    if (Container::IsCurrentUseNewPipeline()) {
        NG::TextView::Create(data);
        return;
    }

    auto textComponent = AceType::MakeRefPtr<TextComponentV2>(data);
    ViewStackProcessor::GetInstance()->ClaimElementId(textComponent);
    ViewStackProcessor::GetInstance()->Push(textComponent);
    JSInteractableView::SetFocusable(false);
    JSInteractableView::SetFocusNode(false);

    // Init text style, allowScale is not supported in declarative.
    auto textStyle = textComponent->GetTextStyle();
    textStyle.SetAllowScale(false);
    textStyle.SetFontSize(DEFAULT_FONT_SIZE);
    textComponent->SetTextStyle(textStyle);
}

RefPtr<TextComponentV2> JSText::GetComponent()
{
    auto* stack = ViewStackProcessor::GetInstance();
    if (!stack) {
        return nullptr;
    }
    auto component = AceType::DynamicCast<TextComponentV2>(stack->GetMainComponent());
    return component;
}

void JSText::SetCopyOption(const JSCallbackInfo& info)
{
    if (info.Length() == 0) {
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    auto copyOptions = CopyOptions::None;
    if (info[0]->IsNumber()) {
        auto emunNumber = info[0]->ToNumber<int>();
        copyOptions = static_cast<CopyOptions>(emunNumber);
    }
    LOGI("copy option: %{public}d", copyOptions);
    component->SetCopyOption(copyOptions);
}

void JSText::JsOnDragStart(const JSCallbackInfo& info)
{
    RefPtr<JsDragFunction> jsOnDragStartFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDragStartId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragStartFunc)](
                             const RefPtr<DragEvent>& info, const std::string& extraParams) -> DragItemInfo {
        DragItemInfo itemInfo;
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx, itemInfo);

        auto ret = func->Execute(info, extraParams);
        if (!ret->IsObject()) {
            LOGE("builder param is not an object.");
            return itemInfo;
        }
        auto component = ParseDragItemComponent(ret);
        if (component) {
            LOGI("use custom builder param.");
            itemInfo.customComponent = component;
            return itemInfo;
        }

        auto builderObj = JSRef<JSObject>::Cast(ret);
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        auto pixmap = builderObj->GetProperty("pixelMap");
        itemInfo.pixelMap = CreatePixelMapFromNapiValue(pixmap);
#endif
        auto extraInfo = builderObj->GetProperty("extraInfo");
        ParseJsString(extraInfo, itemInfo.extraInfo);
        component = ParseDragItemComponent(builderObj->GetProperty("builder"));
        itemInfo.customComponent = component;
        return itemInfo;
    };

    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    component->SetOnDragStartId(onDragStartId);
}

void JSText::JsOnDragEnter(const JSCallbackInfo& info)
{
    RefPtr<JsDragFunction> jsOnDragEnterFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDragEnterId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragEnterFunc)](
                             const RefPtr<DragEvent>& info, const std::string& extraParams) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onDragEnter");
        func->Execute(info, extraParams);
    };
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    component->SetOnDragEnterId(onDragEnterId);
}

void JSText::JsOnDragMove(const JSCallbackInfo& info)
{
    RefPtr<JsDragFunction> jsOnDragMoveFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDragMoveId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragMoveFunc)](
                            const RefPtr<DragEvent>& info, const std::string& extraParams) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onDragMove");
        func->Execute(info, extraParams);
    };
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    component->SetOnDragMoveId(onDragMoveId);
}

void JSText::JsOnDragLeave(const JSCallbackInfo& info)
{
    RefPtr<JsDragFunction> jsOnDragLeaveFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDragLeaveId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragLeaveFunc)](
                             const RefPtr<DragEvent>& info, const std::string& extraParams) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onDragLeave");
        func->Execute(info, extraParams);
    };
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    component->SetOnDragLeaveId(onDragLeaveId);
}

void JSText::JsOnDrop(const JSCallbackInfo& info)
{
    RefPtr<JsDragFunction> jsOnDropFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDropId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDropFunc)](
                        const RefPtr<DragEvent>& info, const std::string& extraParams) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onDrop");
        func->Execute(info, extraParams);
    };
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    component->SetOnDropId(onDropId);
}

void JSText::JsFocusable(const JSCallbackInfo& info)
{
    if (!info[0]->IsBoolean()) {
        LOGE("The info is wrong, it is supposed to be an boolean");
        return;
    }
    JSInteractableView::SetFocusable(info[0]->IsBoolean());
    JSInteractableView::SetFocusNode(false);
}

} // namespace OHOS::Ace::Framework
