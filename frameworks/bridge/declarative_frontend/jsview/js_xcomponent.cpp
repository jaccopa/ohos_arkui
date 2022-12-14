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

#include "frameworks/bridge/declarative_frontend/jsview/js_xcomponent.h"

#include "base/memory/referenced.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_xcomponent_controller.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_view.h"

namespace OHOS::Ace::Framework {
void JSXComponent::JSBind(BindingTarget globalObj)
{
    JSClass<JSXComponent>::Declare("XComponent");
    JSClass<JSXComponent>::StaticMethod("create", &JSXComponent::Create);
    JSClass<JSXComponent>::StaticMethod("onLoad", &JSXComponent::JsOnLoad);
    JSClass<JSXComponent>::StaticMethod("onDestroy", &JSXComponent::JsOnDestroy);
    JSClass<JSXComponent>::Inherit<JSViewAbstract>();
    JSClass<JSXComponent>::Bind(globalObj);
}

void JSXComponent::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGI("xcomponent create error, info is invalid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto id = paramObject->GetProperty("id");
    if (!id->IsString()) {
        LOGI("xcomponent create error, id is invalid");
        return;
    }

    auto type = paramObject->GetProperty("type");
    auto libraryname = paramObject->GetProperty("libraryname");
    auto controllerObj = paramObject->GetProperty("controller");
    RefPtr<XComponentController> xcomponentController = nullptr;
    if (controllerObj->IsObject()) {
        auto* jsXComponentController = JSRef<JSObject>::Cast(controllerObj)->Unwrap<JSXComponentController>();
        if (jsXComponentController) {
            XComponentClient::GetInstance().AddControllerToJSXComponentControllersMap(
                id->ToString(), jsXComponentController);
            xcomponentController = jsXComponentController->GetController();
        }
    }
    if (Container::IsCurrentUseNewPipeline()) {
        NG::XComponentView::Create(id->ToString(), type->ToString(), libraryname->ToString(), xcomponentController);
        auto surfaceDestroyCallback = [xcId = id->ToString()]() {
            XComponentClient::GetInstance().DeleteFromNativeXcomponentsMapById(xcId);
            XComponentClient::GetInstance().DeleteControllerFromJSXComponentControllersMap(xcId);
        };
        NG::XComponentView::SetOnSurfaceDestroyEvent(std::move(surfaceDestroyCallback));
        return;
    }

    auto xcomponentComponent = AceType::MakeRefPtr<OHOS::Ace::XComponentComponent>("xcomponent");
    xcomponentComponent->SetId(id->ToString());
    xcomponentComponent->SetXComponentType(type->ToString());
    xcomponentComponent->SetLibraryName(libraryname->ToString());
    if (xcomponentController) {
        xcomponentComponent->SetXComponentController(xcomponentController);
    }

    XComponentClient::GetInstance().AddXComponentToXcomponentsMap(xcomponentComponent->GetId(), xcomponentComponent);
    auto deleteCallback = [xcId = id->ToString()]() {
        XComponentClient::GetInstance().DeleteFromXcomponentsMapById(xcId);
        XComponentClient::GetInstance().DeleteControllerFromJSXComponentControllersMap(xcId);
    };
    xcomponentComponent->RegisterDeleteCallback(deleteCallback);
    ViewStackProcessor::GetInstance()->Push(xcomponentComponent);
}

void JSXComponent::JsOnLoad(const JSCallbackInfo& args)
{
    if (Container::IsCurrentUseNewPipeline()) {
        auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(args[0]));
        auto onLoad = [execCtx = args.GetExecutionContext(), func = std::move(jsFunc)](
                          const std::string& xcomponentId) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("XComponent.onLoad");
            std::vector<std::string> keys = { "load", xcomponentId };
            func->ExecuteNew(keys, "");
        };
        NG::XComponentView::SetOnLoad(std::move(onLoad));
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto xcomponentComponent = AceType::DynamicCast<XComponentComponent>(stack->GetMainComponent());
    if (!xcomponentComponent) {
        LOGE("JSXComponent::JsOnLoad xcomponentComponent is null.");
        return;
    }
    auto xcomponentId = xcomponentComponent->GetId();

    std::vector<std::string> keys = { "load", xcomponentId };
    xcomponentComponent->SetXComponentInitEventId(GetEventMarker(args, keys));
}

void JSXComponent::JsOnDestroy(const JSCallbackInfo& args)
{
    if (Container::IsCurrentUseNewPipeline()) {
        auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(args[0]));
        auto onDestroy = [execCtx = args.GetExecutionContext(), func = std::move(jsFunc)]() {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("XComponent.onDestroy");
            std::vector<std::string> keys = {"destroy"};
            func->Execute(keys, "");
        };
        NG::XComponentView::SetOnDestroy(std::move(onDestroy));
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto xcomponentComponent = AceType::DynamicCast<XComponentComponent>(stack->GetMainComponent());
    if (!xcomponentComponent) {
        LOGE("JSXComponent::JsOnDestroy xcomponentComponent is null.");
        return;
    }
    std::vector<std::string> keys = {"destroy"};
    xcomponentComponent->SetXComponentDestroyEventId(GetEventMarker(args, keys));
}

EventMarker JSXComponent::GetEventMarker(const JSCallbackInfo& info, const std::vector<std::string>& keys)
{
    if (!info[0]->IsFunction()) {
        LOGE("info[0] is not a function.");
        return EventMarker();
    }

    RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
    auto eventMarker =
        EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc), keys](const std::string& param) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            std::string::size_type posLoad = param.find("load");
            // load callback method, need to return a napi instance
            if (posLoad != std::string::npos) {
                ACE_SCORING_EVENT("XComponent.onLoad");
                func->ExecuteNew(keys, param);
            } else {
                ACE_SCORING_EVENT("XComponent.onDestroy");
                func->Execute(keys, param);
            }
        });
    return eventMarker;
}
} // namespace OHOS::Ace::Framework