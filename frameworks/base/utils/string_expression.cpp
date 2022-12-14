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

#include "base/utils/string_expression.h"

#include <regex>

#include "base/log/log.h"
#include "base/utils/string_utils.h"

namespace OHOS::Ace::StringExpression {
void InitMapping(std::map<std::string, int>& mapping)
{
    mapping["+"] = 0;
    mapping["-"] = 0;
    mapping["*"] = 1;
    mapping["/"] = 1;
    mapping["("] = 2;
    mapping[")"] = 2;
}

std::vector<std::string> ConvertDal2Rpn(std::string formula)
{
    std::vector<std::string> result;
    std::vector<std::string> opStack;
    std::map<std::string, int> OpMapping;
    std::string curNum, curOp;
    std::regex calc("calc");
    std::regex space(" ");
    std::string ops = "+-*/()";
    formula = regex_replace(formula, calc, "");
    formula = regex_replace(formula, space, "");
    for (char i : formula) {
        if (ops.find(i) == ops.npos) {
            curNum += i;
        } else {
            if (!curNum.empty()) {
                result.push_back(curNum);
                curNum.clear();
            }
            curOp = i;
            if (opStack.empty()) {
                opStack.push_back(curOp);
            } else if (curOp == "(") {
                opStack.push_back(curOp);
            } else if (curOp == ")") {
                while (opStack.back() != "(") {
                    result.push_back(opStack.back());
                    opStack.pop_back();
                    if (opStack.empty()) {
                        LOGE("ExpressionError, opStack is empty");
                        result.emplace_back("0");
                        return result;
                    }
                }
                opStack.pop_back();
            } else if (opStack.back() == "(") {
                opStack.push_back(curOp);
            } else if (OpMapping[curOp] > OpMapping[opStack.back()] && (!opStack.empty())) {
                opStack.push_back(curOp);
            } else {
                while ((opStack.back() != "(") && (OpMapping[opStack.back()] >= OpMapping[curOp])) {
                    result.push_back(opStack.back());
                    opStack.pop_back();
                    if (opStack.empty())
                        break;
                }
                opStack.push_back(curOp);
            }
        }
    }
    if (!opStack.empty()) {
        LOGE("opStack is not empty");
    }
    return result;
}

double CalculateExp(const std::string& expression, const std::function<double(const Dimension&)>& calcFunc)
{
    std::vector<std::string> rpnexp = ConvertDal2Rpn(expression);
    std::vector<double> result;
    double num = 0.0;
    std::string ops = "+-*/()";
    for (auto& i : rpnexp) {
        if (ops.find(i) == ops.npos) {
            std::string value = i;
            Dimension dim = StringUtils::StringToDimensionWithUnit(value);
            num = calcFunc(dim);
            result.push_back(num);
        } else {
            if (result.size() < 2) {
                LOGE("ExpressionError, size < 2");
                return 0.0;
            }
            double num1 = result.back();
            result.pop_back();
            double num2 = result.back();
            result.pop_back();
            double opRes = 0.0;
            if (i == "+") {
                opRes = num2 + num1;
            } else if (i == "-") {
                opRes = num2 - num1;
            } else if (i == "*") {
                opRes = num2 * num1;
            } else if (i == "/" && !NearZero(num1)) {
                opRes = num2 / num1;
            }
            result.push_back(opRes);
        }
    }
    if (result.size() == 1) {
        return result.back();
    } else {
        LOGE("ExpressionError");
        return 0.0;
    }
}
} // namespace OHOS::Ace::StringExpression
