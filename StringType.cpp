/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "StringType.h"

#include <hidl-util/Formatter.h>

namespace android {

StringType::StringType() {}

void StringType::addNamedTypesToSet(std::set<const FQName> &) const {
    // do nothing
}

std::string StringType::getCppType(StorageMode mode,
                                   bool specifyNamespaces) const {
    const std::string base =
          std::string(specifyNamespaces ? "::android::hardware::" : "")
        + "hidl_string";

    switch (mode) {
        case StorageMode_Stack:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";

        case StorageMode_Result:
            return "const " + base + "*";
    }
}

std::string StringType::getJavaType(bool /* forInitializer */) const {
    return "String";
}

std::string StringType::getJavaSuffix() const {
    return "String";
}

std::string StringType::getVtsType() const {
    return "TYPE_STRING";
}

void StringType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    const std::string parentName = "_hidl_" + name + "_parent";
    out << "size_t " << parentName << ";\n\n";

    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        out << name
            << " = (const ::android::hardware::hidl_string *)"
            << parcelObjDeref
            << "readBuffer("
            << "&"
            << parentName
            << ");\n";

        out << "if ("
            << name
            << " == nullptr) {\n";

        out.indent();

        out << "_hidl_err = ::android::UNKNOWN_ERROR;\n";
        handleError2(out, mode);

        out.unindent();
        out << "}\n\n";
    } else {
        out << "_hidl_err = "
            << parcelObjDeref
            << "writeBuffer(&"
            << name
            << ", sizeof("
            << name
            << "), &"
            << parentName
            << ");\n";

        handleError(out, mode);
    }

    emitReaderWriterEmbedded(
            out,
            0 /* depth */,
            name,
            name /* sanitizedName */,
            isReader /* nameIsPointer */,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            parentName,
            "0 /* parentOffset */");
}

void StringType::emitReaderWriterEmbedded(
        Formatter &out,
        size_t /* depth */,
        const std::string &name,
        const std::string & /*sanitizedName*/,
        bool nameIsPointer,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode,
        const std::string &parentName,
        const std::string &offsetText) const {
    emitReaderWriterEmbeddedForTypeName(
            out,
            name,
            nameIsPointer,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            parentName,
            offsetText,
            "::android::hardware::hidl_string",
            "" /* childName */);
}

void StringType::emitJavaFieldInitializer(
        Formatter &out, const std::string &fieldName) const {
    out << "String "
        << fieldName
        << " = new String();\n";
}

void StringType::emitJavaFieldReaderWriter(
        Formatter &out,
        size_t /* depth */,
        const std::string &parcelName,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) const {
    if (isReader) {
        out << "\n"
            << parcelName
            << ".readEmbeddedBuffer(\n";

        out.indent();
        out.indent();

        out << blobName
            << ".handle(),\n"
            << offset
            << " + 0 /* offsetof(hidl_string, mBuffer) */);\n\n";

        out.unindent();
        out.unindent();

        out << fieldName
            << " = "
            << blobName
            << ".getString("
            << offset
            << ");\n";

        return;
    }

    out << blobName
        << ".putString("
        << offset
        << ", "
        << fieldName
        << ");\n";
}

bool StringType::needsEmbeddedReadWrite() const {
    return true;
}

bool StringType::resultNeedsDeref() const {
    return true;
}

status_t StringType::emitVtsTypeDeclarations(Formatter &out) const {
    out << "type: TYPE_STRING\n";
    return OK;
}

void StringType::getAlignmentAndSize(size_t *align, size_t *size) const {
    *align = 8;  // hidl_string
    *size = 24;
}

}  // namespace android

