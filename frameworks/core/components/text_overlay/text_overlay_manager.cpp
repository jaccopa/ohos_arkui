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

#include <cmath>
#include <string>

#include "core/components/text_overlay/text_overlay_manager.h"

#include "core/components/font/constants_converter.h"
#include "core/components/stack/stack_element.h"
#include "core/components/text_overlay/text_overlay_component.h"
#include "flutter/third_party/txt/src/txt/paragraph_txt.h"
#include "third_party/skia/include/core/SkCanvas.h"

namespace OHOS::Ace {

namespace {
constexpr char16_t NEWLINE_CODE = u'\n';
constexpr double FIFTY_PERCENT = 0.5;
constexpr int32_t SHOW_HANDLE_DURATION = 250;
} // namespace

TextOverlayBase::~TextOverlayBase() = default;

Offset TextOverlayBase::MakeEmptyOffset() const
{
    if (realTextDirection_ == TextDirection::RTL) {
        return Offset(textOverlayPaintRect_.Width(), 0.0);
    }

    switch (textAlign_) {
        case TextAlign::LEFT: {
            return Offset::Zero();
        }
        case TextAlign::RIGHT: {
            return Offset(textOverlayPaintRect_.Width(), 0.0);
        }
        case TextAlign::JUSTIFY:
        case TextAlign::CENTER: {
            return Offset(textOverlayPaintRect_.Width() / 2.0, 0.0);
        }
        case TextAlign::END: {
            switch (defaultTextDirection_) {
                case TextDirection::RTL: {
                    return Offset::Zero();
                }
                case TextDirection::LTR:
                default:{
                    return Offset(textOverlayPaintRect_.Width(), 0.0);
                }
            }
        }
        case TextAlign::START:
        default: {
            // Default to start.
            switch (defaultTextDirection_) {
                case TextDirection::RTL: {
                    return Offset(textOverlayPaintRect_.Width(), 0.0);
                }
                case TextDirection::LTR:
                default:{
                    return Offset::Zero();
                }
            }
        }
    }
}

double TextOverlayBase::GetBoundaryOfParagraph(bool isLeftBoundary) const
{
    if (!paragraph_ || textValue_.text.empty()) {
        return 0.0;
    }
    auto boxes = paragraph_->GetRectsForRange(0, textValue_.GetWideText().length(),
        txt::Paragraph::RectHeightStyle::kMax, txt::Paragraph::RectWidthStyle::kTight);
    if (boxes.empty()) {
        return 0.0;
    }
    double leftBoundaryOfParagraph = boxes.front().rect.fLeft;
    double rightBoundaryOfParagraph = boxes.front().rect.fLeft;
    double bottomBoundaryOfParagraph = boxes.front().rect.fBottom;
    for (const auto& box : boxes) {
        if (cursorPositionType_ == CursorPositionType::END && !NearEqual(box.rect.fBottom, bottomBoundaryOfParagraph)) {
            bottomBoundaryOfParagraph = box.rect.fBottom;
            leftBoundaryOfParagraph = box.rect.fLeft;
            rightBoundaryOfParagraph = box.rect.fRight;
            continue;
        }
        leftBoundaryOfParagraph = std::min(static_cast<double>(box.rect.fLeft), leftBoundaryOfParagraph);
        rightBoundaryOfParagraph = std::max(static_cast<double>(box.rect.fRight), rightBoundaryOfParagraph);
    }
    return isLeftBoundary ? leftBoundaryOfParagraph : rightBoundaryOfParagraph;
}

bool TextOverlayBase::ComputeOffsetForCaretUpstream(int32_t extent, CaretMetrics& result) const
{
    auto text = StringUtils::Str8ToStr16(textForDisplay_);
    if (!paragraph_ || text.empty()) {
        return false;
    }

    char16_t prevChar = 0;
    if (static_cast<size_t>(extent) <= text.length()) {
        prevChar = text[std::max(0, extent - 1)];
    }

    result.Reset();
    int32_t graphemeClusterLength = StringUtils::NotInUtf16Bmp(prevChar) ? 2 : 1;
    int32_t prev = extent - graphemeClusterLength;
    auto boxes = paragraph_->GetRectsForRange(
        prev, extent, txt::Paragraph::RectHeightStyle::kMax, txt::Paragraph::RectWidthStyle::kTight);
    while (boxes.empty() && !textValue_.text.empty()) {
        graphemeClusterLength *= 2;
        prev = extent - graphemeClusterLength;
        if (prev < 0) {
            boxes = paragraph_->GetRectsForRange(
                0, extent, txt::Paragraph::RectHeightStyle::kMax, txt::Paragraph::RectWidthStyle::kTight);
            break;
        }
        boxes = paragraph_->GetRectsForRange(
            prev, extent, txt::Paragraph::RectHeightStyle::kMax, txt::Paragraph::RectWidthStyle::kTight);
    }
    if (boxes.empty()) {
        return false;
    }

    const auto& textBox = *boxes.begin();

    if (prevChar == NEWLINE_CODE) {
        // Return the start of next line.
        auto emptyOffset = MakeEmptyOffset();
        result.offset.SetX(emptyOffset.GetX());
        result.offset.SetY(textBox.rect.fBottom);
        result.height = caretProto_.Height();
        return true;
    }

    bool isLtr = textBox.direction == txt::TextDirection::ltr;
    // Caret is within width of the upstream glyphs.
    double caretEnd = isLtr ? textBox.rect.fRight : textBox.rect.fLeft;
    if (cursorPositionType_ == CursorPositionType::END) {
        caretEnd = GetBoundaryOfParagraph(realTextDirection_ != TextDirection::LTR);
    }
    double dx = isLtr ? caretEnd : caretEnd - caretProto_.Width();
    double offsetX = std::min(dx, paragraph_->GetMaxWidth());
    result.offset.SetX(offsetX);
    result.offset.SetY(textBox.rect.fTop);
    result.height = textBox.rect.fBottom - textBox.rect.fTop;

    return true;
}

bool TextOverlayBase::ComputeOffsetForCaretDownstream(int32_t extent, CaretMetrics& result) const
{
    if (!paragraph_ || static_cast<size_t>(extent) >= textValue_.GetWideText().length()) {
        return false;
    }

    result.Reset();
    const int32_t graphemeClusterLength = 1;
    const int32_t next = extent + graphemeClusterLength;
    auto boxes = paragraph_->GetRectsForRange(
        extent, next, txt::Paragraph::RectHeightStyle::kMax, txt::Paragraph::RectWidthStyle::kTight);
    if (boxes.empty()) {
        return false;
    }

    const auto& textBox = *boxes.begin();
    bool isLtr = textBox.direction == txt::TextDirection::ltr;
    // Caret is within width of the downstream glyphs.
    double caretStart = isLtr ? textBox.rect.fLeft : textBox.rect.fRight;
    if (cursorPositionType_ == CursorPositionType::END) {
        caretStart = GetBoundaryOfParagraph(realTextDirection_ != TextDirection::LTR);
    }
    double dx = isLtr ? caretStart : caretStart - caretProto_.Width();
    double offsetX = std::min(dx, paragraph_->GetMaxWidth());
    result.offset.SetX(offsetX);
    result.offset.SetY(textBox.rect.fTop);
    result.height = textBox.rect.fBottom - textBox.rect.fTop;

    return true;
}

bool TextOverlayBase::ComputeOffsetForCaretCloserToClick(int32_t extent, CaretMetrics& result) const
{
    CaretMetrics upStreamMetrics;
    bool upStreamSuccess = ComputeOffsetForCaretUpstream(extent, upStreamMetrics);
    CaretMetrics downStreamMetrics;
    bool downStreamSuccess = ComputeOffsetForCaretDownstream(extent, downStreamMetrics);
    bool nearToUpStream = LessOrEqual(std::abs(upStreamMetrics.offset.GetX() - clickOffset_.GetX()),
        std::abs(downStreamMetrics.offset.GetX() - clickOffset_.GetX()));
    result = nearToUpStream ? upStreamMetrics : downStreamMetrics;
    return upStreamSuccess || downStreamSuccess;
}

DirectionStatus TextOverlayBase::GetDirectionStatusOfPosition(int32_t position) const
{
    const char mark = ' ';
    std::string tempBefore = textValue_.GetSelectedText(TextSelection(0, position));
    StringUtils::DeleteAllMark(tempBefore, mark);
    const auto& textBeforeCursor = StringUtils::ToWstring(tempBefore);

    std::string tempAfter =
        textValue_.GetSelectedText(TextSelection(position, textValue_.GetWideText().length()));
    StringUtils::DeleteAllMark(tempAfter, mark);
    const auto& textAfterCursor = StringUtils::ToWstring(tempAfter);

    bool isBeforeCharRtl = false;
    if (!textBeforeCursor.empty()) {
        const auto& charBefore = textBeforeCursor.back();
        isBeforeCharRtl = (u_charDirection(charBefore) == UCharDirection::U_RIGHT_TO_LEFT ||
                           u_charDirection(charBefore) == UCharDirection::U_RIGHT_TO_LEFT_ARABIC);
    }

    bool isAfterCharRtl = false;
    if (!textAfterCursor.empty()) {
        const auto& charAfter = textAfterCursor.front();
        isAfterCharRtl = (u_charDirection(charAfter) == UCharDirection::U_RIGHT_TO_LEFT ||
                          u_charDirection(charAfter) == UCharDirection::U_RIGHT_TO_LEFT_ARABIC);
    }
    return static_cast<DirectionStatus>(
        (static_cast<uint8_t>(isBeforeCharRtl) << 1) | static_cast<uint8_t>(isAfterCharRtl));
}

bool TextOverlayBase::GetCaretRect(int32_t extent, Rect& caretRect, double caretHeightOffset) const
{
    CaretMetrics metrics;
    bool computeSuccess = false;
    DirectionStatus directionStatus = GetDirectionStatusOfPosition(extent);
    if (extent != 0 && extent != static_cast<int32_t>(textValue_.GetWideText().length()) &&
        (directionStatus == DirectionStatus::LEFT_RIGHT || directionStatus == DirectionStatus::RIGHT_LEFT) &&
        cursorPositionType_ != CursorPositionType::NONE &&
        LessOrEqual(clickOffset_.GetX(), textOverlayPaintRect_.Width())) {
        computeSuccess = ComputeOffsetForCaretCloserToClick(cursorPositionForShow_, metrics);
    } else {
        if (textAffinity_ == TextAffinity::DOWNSTREAM) {
            computeSuccess =
                ComputeOffsetForCaretDownstream(extent, metrics) || ComputeOffsetForCaretUpstream(extent, metrics);
        } else {
            computeSuccess =
                ComputeOffsetForCaretUpstream(extent, metrics) || ComputeOffsetForCaretDownstream(extent, metrics);
        }
    }
    if (computeSuccess && !textValue_.text.empty()) {
        if (metrics.height <= 0 || std::isnan(metrics.height)) {
            // The reason may be text lines is exceed the paragraph maxline.
            LOGD("Illegal caret height. Consider release restriction of paragraph max_line.");
            return false;
        }
        caretRect.SetRect(metrics.offset.GetX(), metrics.offset.GetY() + caretHeightOffset, cursorWidth_,
            metrics.height - caretHeightOffset * 2.0);
    } else {
        LOGD("Caret is at the begin of text. Make caret offset according to alignment.");
        // Use proto caret.
        caretRect = caretProto_ + MakeEmptyOffset();
    }

    return true;
}

int32_t TextOverlayBase::GetCursorPositionForClick(const Offset& offset, const Offset& globalOffset)
{
    if (!paragraph_) {
        return 0;
    }
    cursorPositionType_ = CursorPositionType::NORMAL;
    clickOffset_ = offset - globalOffset - textOverlayPaintRect_.GetOffset() - textOffsetForShowCaret_;
    // Solve can't select right boundary of RTL language.
    double rightBoundary = GetBoundaryOfParagraph(false);
    if (GreatOrEqual(clickOffset_.GetX(), rightBoundary)) {
        int32_t rightBoundaryPosition =
            static_cast<int32_t>(paragraph_->GetGlyphPositionAtCoordinateWithCluster(
                rightBoundary - cursorWidth_, clickOffset_.GetY()).position);
        return realTextDirection_ == TextDirection::RTL ? 0 : rightBoundaryPosition;
    }

    return static_cast<int32_t>(
        paragraph_->GetGlyphPositionAtCoordinate(clickOffset_.GetX(), clickOffset_.GetY()).position);
}

int32_t TextOverlayBase::GetGraphemeClusterLength(int32_t extend, bool isPrefix) const
{
    auto text = textForDisplay_;
    char16_t aroundChar = 0;
    if (isPrefix) {
        if (static_cast<size_t>(extend) <= text.length()) {
            aroundChar = text[std::max(0, extend - 1)];
        }
    } else {
        if (static_cast<size_t>(extend) < (text.length())) {
            aroundChar = text[std::min(static_cast<int32_t>(text.length() - 1), extend)];
        }
    }
    return StringUtils::NotInUtf16Bmp(aroundChar) ? 2 : 1;
}

void TextOverlayBase::InitAnimation(const WeakPtr<PipelineContext>& pipelineContext)
{
    auto context = pipelineContext.Upgrade();
    if (!context) {
        LOGE("Context is null.");
        return;
    }

    if (!textOverlay_) {
        LOGE("InitAnimation error, textOverlay is nullptr");
        return;
    }

    // Get the handleDiameter in theme, textoverlay is not nullptr
    double initHandleDiameter = textOverlay_->GetHandleDiameter().Value();
    double initHandleDiameterInner = textOverlay_->GetHandleDiameterInner().Value();

    // Add the animation for handleDiameter
    auto diameterAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(
        initHandleDiameter * FIFTY_PERCENT, initHandleDiameter, Curves::ELASTICS);
    diameterAnimation->AddListener([weak = AceType::WeakClaim(this)](double value) {
        auto renderNode = weak.Upgrade();
        if (renderNode && renderNode->updateHandleDiameter_) {
            renderNode->updateHandleDiameter_(value);
        }
    });

    // Add the animation for handleDiameterinner
    auto diameterInnerAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(
        initHandleDiameterInner * FIFTY_PERCENT, initHandleDiameterInner, Curves::ELASTICS);
    diameterInnerAnimation->AddListener([weak = AceType::WeakClaim(this)](double value) {
        auto renderNode = weak.Upgrade();
        if (renderNode && renderNode->updateHandleDiameterInner_) {
            renderNode->updateHandleDiameterInner_(value);
        }
    });

    // Add the animation
    LOGD("Add animation to animator");
    animator_ = AceType::MakeRefPtr<Animator>(context);
    animator_->AddInterpolator(diameterAnimation);
    animator_->AddInterpolator(diameterInnerAnimation);
    animator_->SetDuration(SHOW_HANDLE_DURATION);
    animator_->Play();
}

void TextOverlayBase::PaintSelection(SkCanvas* canvas) const
{
    if (!IsSelectiveDevice()) {
        return;
    }
    using namespace Constants;

    if (!paragraph_ || (canvas == nullptr)) {
        return;
    }
    const auto& selection = textValue_.selection;
    if (textValue_.text.empty() || selection.GetStart() == selection.GetEnd()) {
        return;
    }
    const auto& boxes = paragraph_->GetRectsForRange(selection.GetStart(), selection.GetEnd(),
        txt::Paragraph::RectHeightStyle::kMax, txt::Paragraph::RectWidthStyle::kTight);
    if (boxes.empty()) {
        return;
    }
    canvas->save();
    SkPaint paint;
    paint.setColor(selectedColor_.GetValue());
    Offset effectiveOffset = textOverlayPaintRect_.GetOffset() + textOffsetForShowCaret_;
    for (const auto& box : boxes) {
        const auto& selectionRect = ConvertSkRect(box.rect) + effectiveOffset;
        if (box.direction == txt::TextDirection::ltr) {
            canvas->drawRect(SkRect::MakeLTRB(selectionRect.Left(), selectionRect.Top(), selectionRect.Right(),
                                 selectionRect.Bottom()),
                paint);
        } else {
            canvas->drawRect(SkRect::MakeLTRB(selectionRect.Right(), selectionRect.Top(), selectionRect.Left(),
                                 selectionRect.Bottom()),
                paint);
        }
    }
    canvas->restore();
}

void TextOverlayBase::InitSelection(const Offset& pos, const Offset& globalOffset)
{
    int32_t extend = GetCursorPositionForClick(pos, globalOffset);
    int32_t extendEnd = extend + GetGraphemeClusterLength(extend, false); 
    textValue_.UpdateSelection(extend, extendEnd);
}

void TextOverlayBase::UpdateStartSelection(int32_t end, const Offset& pos, const Offset& globalOffset)
{
    int32_t extend = GetCursorPositionForClick(pos, globalOffset);
    textValue_.UpdateSelection(extend, end);
}

void TextOverlayBase::UpdateEndSelection(int32_t start, const Offset& pos, const Offset& globalOffset)
{
    int32_t extend = GetCursorPositionForClick(pos, globalOffset);
    textValue_.UpdateSelection(start, extend);
}

void TextOverlayBase::ChangeSelection(int32_t start, int32_t end)
{
    textValue_.UpdateSelection(start, end);
}

RefPtr<TextOverlayManager> TextOverlayBase::GetTextOverlayManager(const WeakPtr<PipelineContext>& pipelineContext)
{
    auto context = pipelineContext.Upgrade();
    if (!context) {
        return nullptr;
    }

    return context->GetTextOverlayManager();
}

TextOverlayManager::TextOverlayManager(const WeakPtr<PipelineContext>& context)
{
    context_ = context;
}

TextOverlayManager::~TextOverlayManager() = default;

const RefPtr<RenderNode> TextOverlayManager::GetTargetNode() const
{
    auto textOverlayBase = textOverlayBase_.Upgrade();
    if (!textOverlayBase) {
        return nullptr;
    }

    auto targetNode = AceType::DynamicCast<RenderNode>(textOverlayBase);
    if (!targetNode) {
        return nullptr;
    }
    return targetNode;
}

void TextOverlayManager::PopTextOverlay()
{
    const auto& stackElement = stackElement_.Upgrade();
    if (stackElement) {
        stackElement->PopTextOverlay();
    }
}

void TextOverlayManager::PushTextOverlayToStack(const RefPtr<TextOverlayComponent>& textOverlay,
    const WeakPtr<PipelineContext>& pipelineContext)
{
    if (!textOverlay) {
        LOGE("TextOverlay is null");
        return;
    }

    auto context = pipelineContext.Upgrade();
    if (!context) {
        LOGE("Context is null");
        return;
    }

    auto lastStack = context->GetLastStack();
    if (!lastStack) {
        LOGE("LastStack is null");
        return;
    }

    lastStack->PushComponent(textOverlay, false);
    stackElement_ = WeakClaim(RawPtr(lastStack));
}

void TextOverlayManager::HandleCtrlC() const
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("get context fail");
        return;
    }
    auto clipboard = ClipboardProxy::GetInstance()->GetClipboard(context->GetTaskExecutor());
    if (!clipboard) {
        LOGE("get clipboard fail");
        return;
    }
    auto textOverlayBase = textOverlayBase_.Upgrade();
    if (!textOverlayBase) {
        LOGE("get textOverlayBase fail");
        return;
    }
    clipboard->SetData(textOverlayBase->GetSelectedContent());
}

} // namespace OHOS::Ace
