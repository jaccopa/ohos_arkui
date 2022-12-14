/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
 *
 */
#include "frameworks/core/common/register/hdc_jdwp.h"
#include "base/log/log.h"
namespace OHOS::Ace {

HdcJdwpSimulator::HdcJdwpSimulator(uv_loop_t *loopIn, const std::string pkgName)
{
    loop_ = loopIn;
    exit_ = false;
    pkgName_ = pkgName;
    connect_ = new (std::nothrow) uv_connect_t();
    if (connect_ == nullptr) {
        LOGE("connect_ is null");
    }
    ctxPoint_ = (HCtxJdwpSimulator)MallocContext();
}

void HdcJdwpSimulator::FreeContext()
{
    if (ctxPoint_ != nullptr && loop_ != nullptr) {
        if (!uv_is_closing((uv_handle_t *)&ctxPoint_->pipe)) {
            uv_close((uv_handle_t *)&ctxPoint_->pipe, nullptr);
        }
        if (ctxPoint_->hasNewFd && !uv_is_closing((uv_handle_t *)&ctxPoint_->newFd)) {
            uv_close((uv_handle_t *)&ctxPoint_->newFd, nullptr);
        }
    }
    if (ctxPoint_ != nullptr && ctxPoint_->cfd > -1) {
        close(ctxPoint_->cfd);
        ctxPoint_->cfd = -1;
    }
}

HdcJdwpSimulator::~HdcJdwpSimulator()
{
    if (ctxPoint_ != nullptr && connect_ != nullptr) {
        delete ctxPoint_;
        ctxPoint_ = nullptr;
        delete connect_;
        connect_ = nullptr;
    }
}

void HdcJdwpSimulator::FinishWriteCallback(uv_write_t *req, int status)
{
    delete[]((uint8_t *)req->data);
    delete req;
}

RetErrCode HdcJdwpSimulator::SendToStream(uv_stream_t *handleStream, const uint8_t *buf,
                                          const int bufLen, const void *finishCallback)
{
    RetErrCode ret = RetErrCode::ERR_GENERIC;
    if (bufLen <= 0) {
        LOGE("HdcJdwpSimulator::SendToStream wrong bufLen.");
        return RetErrCode::ERR_GENERIC;
    }
    uint8_t *pDynBuf = new (std::nothrow) uint8_t[bufLen];
    if (pDynBuf == nullptr) {
        LOGE("HdcJdwpSimulator::SendToStream new pDynBuf fail.");
        return RetErrCode::ERR_GENERIC;
    }
    if (memcpy_s(pDynBuf, bufLen, buf, bufLen)) {
        LOGE("HdcJdwpSimulator::SendToStream memcpy fail.");
        delete[] pDynBuf;
        pDynBuf = nullptr;
        return RetErrCode::ERR_BUF_ALLOC;
    }

    uv_write_t *reqWrite = new (std::nothrow) uv_write_t();
    if (reqWrite == nullptr) {
        LOGE("HdcJdwpSimulator::SendToStream alloc reqWrite fail.");
        delete[] pDynBuf;
        pDynBuf = nullptr;
        return RetErrCode::ERR_GENERIC;
    }
    uv_buf_t bfr;
    while (true) {
        reqWrite->data = (void *)pDynBuf;
        bfr.base = (char *)pDynBuf;
        bfr.len = bufLen;
        if (!uv_is_writable(handleStream)) {
            delete reqWrite;
            reqWrite = nullptr;
            break;
        }
        uv_write(reqWrite, handleStream, &bfr, 1, (uv_write_cb)finishCallback);
        ret = RetErrCode::SUCCESS;
        break;
    }
    return ret;
}

void HdcJdwpSimulator::SendToJpid(int fd, const uint8_t *buf, const int bufLen)
{
    LOGI("SendToJpid: %{public}s, %{public}d", buf, bufLen);
    ssize_t rc = write(fd, buf, bufLen);
    if (rc < 0) {
        LOGE("SendToJpid failed errno:%{public}d", errno);
    }
}

void HdcJdwpSimulator::ConnectJdwp(uv_connect_t *connection, int status)
{
    uint32_t pid_curr = static_cast<uint32_t>(getpid());
    HCtxJdwpSimulator ctxJdwp = (HCtxJdwpSimulator)connection->data;
    HdcJdwpSimulator *thisClass = static_cast<HdcJdwpSimulator *>(ctxJdwp->thisClass);
#ifdef JS_JDWP_CONNECT
    string pkgName = thisClass->pkgName_;
    uint32_t pkgSize = pkgName.size() + sizeof(JsMsgHeader);
    uint8_t* info = new (std::nothrow) uint8_t[pkgSize]();
    if (!info) {
        LOGE("ConnectJdwp new info fail.");
        return;
    }
    if (memset_s(info, pkgSize, 0, pkgSize) != EOK) {
        delete[] info;
        info = nullptr;
        return;
    }
    JsMsgHeader *jsMsg = (JsMsgHeader *)info;
    jsMsg->pid = pid_curr;
    jsMsg->msgLen = pkgSize;
    bool retFail = false;
    if (memcpy_s(info + sizeof(JsMsgHeader), pkgName.size(), &pkgName[0], pkgName.size()) != EOK) {
        LOGE("ConnectJdwp memcpy_s fail :%{public}s.", pkgName.c_str());
        retFail = true;
    }
    if (!retFail) {
        thisClass->SendToStream((uv_stream_t*)connection->handle, (uint8_t*)info, pkgSize, (void*)FinishWriteCallback);
    }
    if (info) {
        delete[] info;
        info = nullptr;
    }
#endif // JS_JDWP_CONNECT
}

void HdcJdwpSimulator::ConnectJpid(void *param)
{
    uint32_t pid_curr = static_cast<uint32_t>(getpid());
    HdcJdwpSimulator *thisClass = static_cast<HdcJdwpSimulator *>(param);
#ifdef JS_JDWP_CONNECT
    string pkgName = thisClass->pkgName_;
    uint32_t pkgSize = pkgName.size() + sizeof(JsMsgHeader);
    uint8_t* info = new (std::nothrow) uint8_t[pkgSize]();
    if (!info) {
        LOGE("ConnectJpid new info fail.");
        return;
    }
    if (memset_s(info, pkgSize, 0, pkgSize) != EOK) {
        delete[] info;
        info = nullptr;
        return;
    }
    JsMsgHeader *jsMsg = reinterpret_cast<JsMsgHeader *>(info);
    jsMsg->pid = pid_curr;
    jsMsg->msgLen = pkgSize;
    LOGI("ConnectJpid send pid:%{public}d, pkgName:%{public}s, msglen:%{public}d",
        jsMsg->pid, pkgName.c_str(), jsMsg->msgLen);
    bool retFail = false;
    if (memcpy_s(info + sizeof(JsMsgHeader), pkgName.size(), &pkgName[0], pkgName.size()) != EOK) {
        LOGE("ConnectJpid memcpy_s fail :%{public}s.", pkgName.c_str());
        retFail = true;
    }
    if (!retFail) {
        LOGI("ConnectJpid send JS msg:%{public}s", info);
        SendToJpid(thisClass->ctxPoint_->cfd, (uint8_t*)info, pkgSize);
    }
    if (info) {
        delete[] info;
        info = nullptr;
    }
#endif
}

void *HdcJdwpSimulator::MallocContext()
{
    HCtxJdwpSimulator ctx = nullptr;
    if ((ctx = new (std::nothrow) ContextJdwpSimulator()) == nullptr) {
        return nullptr;
    }
    ctx->thisClass = this;
    ctx->pipe.data = ctx;
    ctx->hasNewFd = false;
    ctx->cfd = -1;
    return ctx;
}

bool HdcJdwpSimulator::Connect()
{
    const char jdwp[] = { '\0', 'o', 'h', 'j', 'p', 'i', 'd', '-', 'c', 'o', 'n', 't', 'r', 'o', 'l', 0 };
    if (!ctxPoint_) {
        LOGE("MallocContext failed");
        delete connect_;
        connect_ = nullptr;
        return false;
    }
    struct sockaddr_un caddr;
    if (memset_s(&caddr, sizeof(caddr), 0, sizeof(caddr)) != EOK) {
        LOGE("memset_s failed");
        return false;
    }
    caddr.sun_family = AF_UNIX;
    for (size_t i = 0; i < sizeof(jdwp); i++) {
        caddr.sun_path[i] = jdwp[i];
    }
    int cfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (cfd < 0) {
        LOGE("socket failed errno:%{public}d", errno);
        return false;
    }
    int rc = connect(cfd, reinterpret_cast<struct sockaddr *>(&caddr), sizeof(caddr));
    if (rc != 0) {
        LOGE("connect failed errno:%{public}d", errno);
        close(cfd);
        return false;
    }
    ctxPoint_->cfd = cfd;
    ConnectJpid(this);
    return true;
}
} // namespace OHOS::Ace
