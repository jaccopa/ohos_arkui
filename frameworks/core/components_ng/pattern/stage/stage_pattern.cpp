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

#include "core/components_ng/pattern/stage/stage_pattern.h"

#include "base/log/log.h"

namespace OHOS::Ace::NG {
void StagePattern::OnAttachToFrameNode()
{
    auto host = frameNode_.Upgrade();
    if (!host) {
        LOGE("fail to update measure type due to host is null");
        return;
    }
    host->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_PARENT);
}
} // namespace OHOS::Ace::NG