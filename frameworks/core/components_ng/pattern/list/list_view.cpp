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

#include "core/components_ng/pattern/list/list_view.h"

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/list/list_modifier.h"
#include "core/components_ng/pattern/list/list_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"

namespace OHOS::Ace::NG {

void ListView::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    // TODO: Add unique id.
    auto frameNode = FrameNode::CreateFrameNode(V2::LIST_ETS_TAG, V2::LIST_ETS_TAG, AceType::MakeRefPtr<ListPattern>());
    stack->Push(frameNode);
}

void ListView::SetListDirection(Axis axis)
{
    ViewStackProcessor::GetInstance()->PushLayoutTask(ListDirectionModifier(axis));
}

void ListView::SetEdgeEffect(EdgeEffect edgeEffect)
{
    // TODO: add impl.
}

} // namespace OHOS::Ace::NG