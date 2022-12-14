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

#include "core/components_ng/pattern/image/image_layout_algorithm.h"

#ifdef NG_BUILD
#include "ace_shell/shell/common/window_manager.h"
#endif

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/image/image_layout_property.h"

namespace OHOS::Ace::NG {

std::optional<SizeF> ImageLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* /*layoutWrapper*/)
{
    // case 1: image component is set with valid size, return contentConstraint.selfIdealSize as component size
    if (contentConstraint.selfIdealSize.IsValid()) {
        return contentConstraint.selfIdealSize.ConvertToSizeT();
    }

    // case 2: image component is not set with size, use image source size to determine component size
    // if image data is not ready, can not decide content size,
    // return std::nullopt and wait for next layout task triggered by [OnImageDataReady]
    if (!loadingCtx_->GetImageSize().IsPositive()) {
        return std::nullopt;
    }

    auto rawImageSize = loadingCtx_->GetImageSize();
    SizeF componentSize(rawImageSize);
    do {
        // case 2.1: image component is not set with size, use image source size as image component size
        if (contentConstraint.selfIdealSize.IsNull()) {
            break;
        }

        // case 2.2 image data is ready, use image source size to determine image component size
        //          keep the principle of making the component aspect ratio and the image source aspect ratio the same
        auto sizeSet = contentConstraint.selfIdealSize.ConvertToSizeT();
        uint8_t sizeSetStatus = Negative(sizeSet.Width()) << 1 | Negative(sizeSet.Height());
        double aspectRatio = Size::CalcRatio(rawImageSize);
        switch (sizeSetStatus) {
            case 0b01: // width is positive and height is negative
                componentSize.SetHeight(static_cast<float>(rawImageSize.Width() / aspectRatio));
                break;
            case 0b10: // width is negative and height is positive
                componentSize.SetWidth(static_cast<float>(rawImageSize.Height() * aspectRatio));
                break;
            case 0b11: // both width and height are negative
            default:
                break;
        }
    } while (false);
    return contentConstraint.Constrain(componentSize);
}

void ImageLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    BoxLayoutAlgorithm::Layout(layoutWrapper);
    // if layout size has not decided yet, resize target can not be calculated
    if (!layoutWrapper->GetGeometryNode()->GetContent()) {
        return;
    }
    const auto& imageLayoutProperty = DynamicCast<ImageLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(imageLayoutProperty);
    const auto& dstSize = layoutWrapper->GetGeometryNode()->GetContentSize();
    bool incomingNeedResize = imageLayoutProperty->GetAutoResize().value_or(true);
    ImageFit incomingImageFit = imageLayoutProperty->GetImageFit().value_or(ImageFit::COVER);
    bool needMakeCanvasImage = incomingNeedResize != loadingCtx_->GetNeedResize() ||
                               dstSize != loadingCtx_->GetDstSize() || incomingImageFit != loadingCtx_->GetImageFit();
    // do [MakeCanvasImage] only when:
    // 1. [autoResize] changes
    // 2. component size (aka [dstSize] here) changes.
    // 3. [ImageFit] changes
    if (needMakeCanvasImage) {
        loadingCtx_->MakeCanvasImage(dstSize, incomingNeedResize, incomingImageFit);
    }
}

} // namespace OHOS::Ace::NG