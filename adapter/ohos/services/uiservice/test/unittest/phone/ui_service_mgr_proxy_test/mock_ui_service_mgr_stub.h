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
#ifndef MOCK_UI_SERVICE_MGR_STUB_H
#define MOCK_UI_SERVICE_MGR_STUB_H
#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ui_service_mgr_stub.h"
#include "ui_service_stub.h"

namespace OHOS {
int IPCObjectStub::SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int TEST_RETVAL_ONREMOTEREQUEST = 1000;
    reply.WriteInt32(TEST_RETVAL_ONREMOTEREQUEST);
    return NO_ERROR;
}
}  // namespace OHOS

namespace OHOS {
namespace Ace {
class MockUIServiceMgrStub : public UIServiceMgrStub {
public:
    MOCK_METHOD2(RegisterCallBack, int(const AAFwk::Want& want, const sptr<IUIService>& uiService));
    MOCK_METHOD1(UnregisterCallBack, int (const AAFwk::Want& want));
    MOCK_METHOD5(Push, int(const AAFwk::Want& want, const std::string& name,
        const std::string& jsonPath, const std::string& data, const std::string& extraData));
    MOCK_METHOD3(Request, int(const AAFwk::Want& want, const std::string& name,
        const std::string& data));
    MOCK_METHOD4(ReturnRequest, int(const AAFwk::Want& want, const std::string& source,
        const std::string& data,  const std::string& extraData));
    MOCK_METHOD9(ShowDialog, int(const std::string& name,
        const std::string& params,
        uint32_t windowType,
        int x,
        int y,
        int width,
        int height,
        const sptr<OHOS::Ace::IDialogCallback>& dialogCallback,
        int* id));
    MOCK_METHOD1(CancelDialog, int(int id));
    MOCK_METHOD2(UpdateDialog, int(int id, const std::string& data));
    MOCK_METHOD2(AttachToUiService, int32_t(const sptr<IRemoteObject>& dialog, int32_t pid));
    MOCK_METHOD3(RemoteDialogCallback, int32_t(int32_t id, const std::string& event, const std::string& params));
};

class MockUIServiceStub : public Ace::UIServiceStub {
public:
    MockUIServiceStub() = default;
    virtual ~MockUIServiceStub() = default;
    MOCK_METHOD5(OnPushCallBack, void(const AAFwk::Want& want, const std::string& name,
        const std::string& jsonPath, const std::string& data, const std::string& extraData));
    MOCK_METHOD3(OnRequestCallBack, void(const AAFwk::Want& want, const std::string& name,
        const std::string& data));
    MOCK_METHOD4(OnReturnRequest, void(const AAFwk::Want& want, const std::string& source,
        const std::string& data,  const std::string& extraData));
};
}  // namespace Ace
}  // namespace OHOS
#endif /* MOCK_UI_SERVICE_MGR_STUB_H */
