#pragma once

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include "FlowLog.h"
#include "FlowFile.h"
#include "FlowString.h"
#include <algorithm>

#ifdef GetObject
#undef GetObject
#endif

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/schema.h>
#include <rapidjson/filereadstream.h>

namespace FlowJson {

    struct typeName {
        typeName(const std::string &type, const std::string &name) {
            this->type = type;
            this->name = name;
        }

        std::string type;
        std::string name;

        bool operator<(const typeName &tn) const {
            return this->name < tn.name;
        }
    };

    inline const rapidjson::Value *getJValue(const rapidjson::Value &value, const std::string &path) {
        const rapidjson::Pointer pointer(path.c_str());
        return rapidjson::GetValueByPointer(value, pointer);
    }


    inline rapidjson::Document parseJson(const std::string &txt) {
        rapidjson::Document document;
        document.Parse(txt.c_str());
        return document;
    }


    inline rapidjson::Document parseJson(const std::string &txt, const std::string &schema, bool &error) {
        rapidjson::Document rtn;
        rapidjson::Document document_schema;
        document_schema.Parse(schema.c_str());

        if (document_schema.HasParseError()) {
            LOG_WARNING << "Schema is not valid.";
            error = true;
            return rtn;
        }
        rapidjson::SchemaDocument schema_document(document_schema);
        rapidjson::SchemaValidator schemaValidator(schema_document);

        rtn.Parse(txt.c_str());
        if (rtn.HasParseError()) {
            LOG_WARNING << "Invalid JSON.";
            error = true;
            return rtn;
        }
        rtn.Accept(schemaValidator);
        if (!schemaValidator.IsValid()) {
            rapidjson::StringBuffer sb;
            schemaValidator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
            LOG_WARNING << "Invalid schema: " << sb.GetString();
            LOG_WARNING << "Invalid keyword: " << schemaValidator.GetInvalidSchemaKeyword();
            sb.Clear();
            schemaValidator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
            LOG_WARNING << "Invalid document: " << sb.GetString();
            error = true;
            return rtn;
        }
        error = false;
        return rtn;
    }


    inline bool parseJson(const std::string &txt, const std::string &schema, rapidjson::Document &out) {
        rapidjson::Document document_schema;
        document_schema.Parse(schema.c_str());
        if (document_schema.HasParseError()) {
            return true;
        }
        rapidjson::SchemaDocument schema_document(document_schema);
        rapidjson::SchemaValidator schemaValidator(schema_document);
        out.Accept(schemaValidator);
        out.Parse(txt.c_str());
        return out.HasParseError() || !schemaValidator.IsValid();
    }

    inline void logJson(const rapidjson::Document &document, bool traceOnly = true) {
        rapidjson::StringBuffer buffer;

        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        if (traceOnly)
            LOG_TRACE << buffer.GetString();
        else
            LOG_INFO << buffer.GetString();
    }

    inline std::string valueToString(const rapidjson::Value &value) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        value.Accept(writer);
        return std::string(buffer.GetString());
    }

    inline std::vector<std::string> findAllJsonValues(const rapidjson::Value &doc, const std::string &path) {

        std::vector<std::string> paths;
        paths.emplace_back(path);
        std::vector<std::string> rtn;

        toWhile:
        while (!paths.empty()) {
            auto &path = paths.front();
            std::vector<std::string> token = FlowString::splitNotEmpty(path, "/");
            for (size_t i = 0; i < token.size(); ++i) {
                std::string t = token.at(i);
                if (t != "-")
                    continue;
                std::string cpath;
                for (size_t j = 0; j < i; ++j) cpath += "/" + token.at(j);
                const rapidjson::Pointer pointer(cpath.c_str());
                auto value = GetValueByPointer(doc, pointer);
                if (value == nullptr) {
                    LOG_TRACE << "Could not find value: " << cpath;
                    continue;
                }
                if (value->GetType() == rapidjson::kArrayType) {
                    size_t count = 0;
                    for (auto itr = value->Begin(); itr != value->End(); ++itr) {
                        std::string newPath = cpath + "/" + std::to_string(count++);
                        for (size_t j = i + 1; j < token.size(); ++j) newPath += "/" + token.at(j);
                        paths.emplace_back(newPath);
                    }
                    paths.erase(paths.begin());
                    i = token.size();
                    goto toWhile;
                }
            }
            rapidjson::Pointer existsCheck(path.c_str());
            if (existsCheck.Get(doc) != nullptr)
                rtn.emplace_back(path);
            paths.erase(paths.begin());
        }

        return rtn;
    }

    // Create String
    inline rapidjson::Value cS(const std::string &txt, rapidjson::Document &document) {
        rapidjson::Value rtn(rapidjson::kStringType);
        rtn.SetString(txt.c_str(), txt.size(), document.GetAllocator());
        return rtn;
    }

    // Create String
    inline rapidjson::Value cS(const std::string &txt, rapidjson::MemoryPoolAllocator<> &allocator) {
        rapidjson::Value rtn(rapidjson::kStringType);
        rtn.SetString(txt.c_str(), txt.size(), allocator);
        return rtn;
    }

    inline void logJson(const rapidjson::Value &value, bool traceOnly = true) {

        rapidjson::StringBuffer buffer;

        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        value.Accept(writer);
        if (traceOnly)
            LOG_TRACE << buffer.GetString();
        else
            LOG_INFO << buffer.GetString();
    }

    inline std::string prettyJSON(const rapidjson::Value &value) {
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> prettyWriter(buffer);
        value.Accept(prettyWriter);
        return buffer.GetString();
    }

    inline std::string prettyJSON(const std::string &value) {
        auto doc = parseJson(value);
        return prettyJSON(doc);
    }

    inline std::string docToString(rapidjson::Document &doc) {
        rapidjson::StringBuffer buffer;

        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);

        return buffer.GetString();
    }

    inline std::string docToPrettyString(rapidjson::Document &doc) {
        rapidjson::StringBuffer buffer;

        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);

        return buffer.GetString();
    }

    inline std::string normelizePath(const std::string &from, const std::string &to) {

        std::string np;
        std::vector<std::string> toToken = FlowString::splitNotEmpty(to, "/");
        for (size_t i = 0; i < toToken.size(); ++i) {
            if (toToken.at(i) != "-") {
                np += "/" + toToken.at(i);
                continue;
            }

            std::vector<std::string> token = FlowString::splitNotEmpty(from, "/");
            if (token.size() > i && FlowString::isInteger(token.at(i))) {
                np += "/" + token.at(i);
            } else {
                np += "/" + toToken.at(i);
            }
        }

        return np;
    }

    inline std::unordered_map<std::string, std::set<typeName>> generlizeJson(const std::string &file) {
        std::string value = FlowFile::fileToString(file);
        auto newJson = parseJson(value);

        std::vector<std::string> mis;

        mis.emplace_back("");
        std::unordered_map<std::string, std::vector<typeName>> rtn;

        while (!mis.empty()) {
            auto cPos = mis.front();
            std::string cname = FlowString::findFromLast(cPos, "/", 1);
            auto tokens = FlowString::splitNotEmpty(cPos, "/");
            std::string cObject = "";
            size_t pos = 1;
            while (tokens.size() >= ++pos) {
                cObject = tokens.at(tokens.size() - pos);
                if (!FlowString::isInteger(cObject)) {
                    break;
                }
            }


            FlowString::replaceAll(cname, "/", "~1");
            rapidjson::Pointer p(cPos.c_str());
            auto *njcVal = GetValueByPointer(newJson, p);

            switch (njcVal->GetType()) {
                case rapidjson::kObjectType: {
                    LOG_TRACE << "kObjectType";
                    auto njcValEnd = njcVal->MemberEnd();
                    for (auto &mem : njcVal->GetObject()) {
                        std::string test = mem.name.GetString();
                        FlowString::replaceAll(test, "/", "~1");
                        mis.emplace_back(cPos + "/" + test);
                    }
                    rtn[cname];
                    break;
                }
                case rapidjson::kStringType: {
                    LOG_TRACE << "rapidjson::kStringType";
                    auto &v = rtn[cObject];
                    v.emplace_back("String", cname);
                    LOG_TRACE << "rapidjson::kStringType";
                    break;
                }
                case rapidjson::kNullType: {
                    LOG_TRACE << "kNullType";
                    break;
                }
                case rapidjson::kFalseType: {
                    LOG_TRACE << "kFalseType";
                    rtn[cObject].emplace_back("Boolean", cname);
                    break;
                }
                case rapidjson::kTrueType: {
                    LOG_TRACE << "kTrueType";
                    rtn[cObject].emplace_back("Boolean", cname);
                    break;
                }
                case rapidjson::kArrayType: {
                    LOG_TRACE << "rapidjson::kArrayType";
                    rapidjson::Value *cVal = GetValueByPointer(newJson, p);
                    auto cEnd = cVal->End();
                    size_t count = 0;
                    for (auto itr = cVal->Begin(); itr != cEnd; ++itr) {
                        mis.emplace_back(cPos + "/" + std::to_string(count++));
                    }
                    rtn[cObject].emplace_back("Array", cname);
                    break;
                }
                case rapidjson::kNumberType:
                    LOG_TRACE << "kNumberType";
                    rtn[cObject].emplace_back("Number", cname);
                    break;
            }
            mis.erase(mis.begin());
        }
        LOG_INFO << rtn.size();
        return std::unordered_map<std::string, std::set<typeName >>();
    }

    //CamelCase rapidjson::Value
    inline std::string CCValue(const std::string &value, const std::string &path, const bool firstLetter,
                               const bool firstLetterLower = false) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &path : paths) {
            rapidjson::Pointer pointer(path.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kStringType) {
                std::string sval = val->GetString();
                FlowString::underscoreToCamelCase(sval, firstLetter, firstLetterLower);
                rapidjson::Value newVal(sval.c_str(), doc.GetAllocator());
                SetValueByPointer(doc, pointer, newVal);
            }
        }

        return docToString(doc);
    }

    //CamelCase rapidjson::Value
    inline std::string underScoreValue(const std::string &value, const std::string &path, const bool firstLetter,
                                       const bool firstLetterLower = false) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &path : paths) {
            rapidjson::Pointer pointer(path.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kStringType) {
                std::string sval = val->GetString();
                FlowString::toUnderScore(sval, false);
                rapidjson::Value newVal(sval.c_str(), doc.GetAllocator());
                SetValueByPointer(doc, pointer, newVal);
            }
        }

        return docToString(doc);
    }

    inline std::vector<std::string>
    getValueIfValueExist(std::string &value, const std::string &valuepath, const std::string &toCheck,
                         const std::string &pathToGet) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, valuepath);

        std::vector<std::string> rtn;

        for (auto &path : paths) {
            rapidjson::Pointer pointer(path.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kStringType && strcmp(val->GetString(), toCheck.c_str()) == 0) {
                std::string newPath = normelizePath(path, pathToGet);
                rapidjson::Pointer getPointer(newPath.c_str());
                auto toGet = GetValueByPointer(doc, getPointer);
                if (toGet->GetType() == rapidjson::kStringType)
                    rtn.emplace_back(toGet->GetString());
                else if (toGet->GetType() == rapidjson::kNumberType)
                    rtn.emplace_back(std::to_string(toGet->GetFloat()));
                else
                    rtn.emplace_back(valueToString(*toGet));
            }
        }

        return rtn;
    }


    inline std::string orElse(const rapidjson::Value &value, const std::string &var, const std::string &elseString) {
        if (value.HasMember(var.c_str()) && !value[var.c_str()].IsNull())
            return value[var.c_str()].GetString();
        return elseString;
    }

    inline bool exists(const rapidjson::Value &value, const std::string &path) {
        rapidjson::Pointer pointer(path.c_str());
        auto val = GetValueByPointer(value, pointer);
        return val != nullptr && val->GetType() != rapidjson::kNullType;
    }

    inline std::string getValue(const rapidjson::Value &value, const std::string &path) {
        rapidjson::Pointer pointer(path.c_str());
        auto val = GetValueByPointer(value, pointer);
        if (val == nullptr)
            return "";

        switch (val->GetType()) {
            case rapidjson::kFalseType:
                return "false";
            case rapidjson::kTrueType:
                return "true";
            case rapidjson::kStringType:
                return std::string(val->GetString());
            case rapidjson::kNumberType:
                if (val->IsInt())
                    return std::to_string(val->GetInt());
                if (val->IsFloat())
                    return std::to_string(val->GetFloat());
                if (val->IsDouble())
                    return std::to_string(val->GetDouble());
                if (val->IsInt64())
                    return std::to_string(val->GetInt64());
            default:
                break;
        }

        return "";
    }

    inline std::string getValue(const std::string &value, const std::string &path) {
        rapidjson::Document doc = parseJson(value);
        rapidjson::Pointer pointer(path.c_str());
        return getValue(doc, path);
    }

    inline std::string getValueOr(const rapidjson::Value &value, const std::string &path, const std::string &orValue) {
        rapidjson::Pointer pointer(path.c_str());
        auto val = GetValueByPointer(value, pointer);
        if (val != nullptr) {
            switch (val->GetType()) {
                case rapidjson::kStringType:
                    return std::string(val->GetString());
                case rapidjson::kNumberType:
                    return std::to_string(val->GetFloat());
                default:
                    break;
            }
        }

        return orValue;
    }


    inline std::vector<std::string> getValues(const rapidjson::Value &doc, const std::string &path) {
        std::vector<std::string> rtn;
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &valLoc :paths) {
            rapidjson::Pointer pointer(valLoc.c_str());
            auto val = GetValueByPointer(doc, pointer);
            switch (val->GetType()) {
                case rapidjson::kStringType:
                    rtn.emplace_back(val->GetString());
                    break;
                case rapidjson::kNumberType:
                    rtn.emplace_back(std::to_string(val->GetFloat()));
                    break;
                default:
                    break;
            }
        }

        return rtn;
    }

    inline std::vector<std::string> getValues(const std::string &value, const std::string &path) {
        const rapidjson::Document doc = parseJson(value);
        return getValues(doc, path);
    }

    inline void modifyDocumentByMap(rapidjson::Document &doc, const std::string &path,
                                    const std::unordered_map<std::string, std::string> &modifier) {
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        bool hasDefault = modifier.find("default") != modifier.end();
        for (auto &path : paths) {
            rapidjson::Pointer pointer(path.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kStringType) {
                std::string sval = val->GetString();
                auto itr = modifier.find(sval);
                if (itr != modifier.end()) {
                    rapidjson::Value newVal(itr->second.c_str(), doc.GetAllocator());
                    SetValueByPointer(doc, pointer, newVal);
                    continue;
                }
                if (hasDefault) {
                    rapidjson::Value newVal(modifier.at("default").c_str(), doc.GetAllocator());
                    SetValueByPointer(doc, pointer, newVal);
                }
            }
        }
    }

    inline std::string modifyValueByMap(const std::string &value, const std::string &path,
                                        const std::unordered_map<std::string, std::string> &modifier) {
        rapidjson::Document doc = parseJson(value);
        modifyDocumentByMap(doc, path, modifier);
        return docToString(doc);
    }

    inline std::string copyValue(const std::string &value, const std::string &path, const std::string &copyPath) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &path : paths) {
            const std::string npath = normelizePath(path, copyPath);
            const std::vector<std::string> pathSplit = FlowString::splitOnLast(npath, "/");
            const std::string &prefixPath = pathSplit.at(0);
            std::vector<std::string> nPaths = findAllJsonValues(doc, prefixPath);
            for (const auto &np : nPaths) {
                rapidjson::Pointer pointer(path.c_str());
                const auto n_np = normelizePath(np, npath);
                rapidjson::Pointer copypointer(n_np.c_str());
                rapidjson::Value *val = GetValueByPointer(doc, pointer);
                rapidjson::Value nv;
                nv.CopyFrom(*val, doc.GetAllocator());
                SetValueByPointer(doc, copypointer, nv);
            }
        }

        return docToString(doc);
    }

    inline std::string addValue(const std::string &value, const std::string &path, const std::string &txt) {
        rapidjson::Document doc = parseJson(value);
        rapidjson::Pointer pointer(path.c_str());
        rapidjson::Value nv;

        if (FlowString::isNumber(txt)) {
            if (FlowString::isInteger(txt)) {
                nv.SetInt(std::stoi(txt));
            } else {
                nv.SetFloat(std::stof(txt));
            }
        } else if (FlowString::isBool(txt)) {
            nv.SetBool(FlowString::isTrue(txt));
        } else {
            nv.SetString(txt.c_str(), doc.GetAllocator());
        }


        SetValueByPointer(doc, pointer, nv);
        return docToString(doc);
    }

    inline std::string addStringValue(const std::string &value, const std::string &path, const std::string &txt) {
        rapidjson::Document doc = parseJson(value);
        rapidjson::Pointer pointer(path.c_str());
        rapidjson::Value nv;
        nv.SetString(txt.c_str(), doc.GetAllocator());
        SetValueByPointer(doc, pointer, nv);
        logJson(doc, false);
        return docToString(doc);
    }

    inline std::string addBoolValueIfOnce(const std::string &value, const std::string &path, const std::string &toCheck,
                                          const std::string &newPath) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &path : paths) {
            const auto val = getValue(doc, path);
            if (val == toCheck) {
                rapidjson::Pointer newPointer(newPath.c_str());
                SetValueByPointer(doc, newPointer, true);
                return docToString(doc);
            }
        }
        return value;
    }

    inline std::string removeValueIf(const std::string &value, const std::string &path, const std::string &toCheck,
                                     const std::string &removePath) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        std::reverse(paths.begin(), paths.end());
        for (auto &cpath : paths) {
            const auto val = getValue(doc, cpath);
            if (val == toCheck) {

                const auto crp = normelizePath(cpath, removePath);
                rapidjson::Pointer newPointer(crp.c_str());
                EraseValueByPointer(doc, newPointer);
            }
        }
        return docToString(doc);
    }

    inline void copyValueIf(rapidjson::Document &doc, const std::string &path, const std::string &toCheck,
                            const std::string &copyPath,
                            const std::string &newPath) {
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &path : paths) {
            rapidjson::Pointer pointer(path.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kStringType && strcmp(val->GetString(), toCheck.c_str()) == 0) {
                std::string npath = normelizePath(path, newPath);
                std::string cpath = normelizePath(path, copyPath);
                rapidjson::Pointer newPointer(npath.c_str());
                rapidjson::Pointer copyPointer(cpath.c_str());
                rapidjson::Value *copyValue = GetValueByPointer(doc, copyPointer);
                SetValueByPointer(doc, newPointer, cS(std::string(copyValue->GetString()), doc));
            }
        }
    }

    inline void removeNonAlphaNum(rapidjson::Document &doc, const std::string &path) {
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &path : paths) {
            rapidjson::Pointer pointer(path.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kStringType) {
                std::string modString(val->GetString());
                FlowString::removeNonAlphaNum(modString);
                SetValueByPointer(doc, pointer, cS(modString, doc));
            }
        }
    }

    inline std::vector<std::string>
    getIf(const rapidjson::Document &doc, const std::string &path, const std::string &get) {

        std::vector<std::string> rtn;
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (std::string p : paths) {
            rapidjson::Pointer pointer(p.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kTrueType) {
                std::string nget = normelizePath(p, get);
                rapidjson::Pointer innerPointer(nget.c_str());
                auto toGet = GetValueByPointer(doc, innerPointer);
                if (toGet->GetType() == rapidjson::kStringType)
                    rtn.emplace_back(toGet->GetString());
            }
        }
        return rtn;
    }

    inline void addBoolIf(rapidjson::Document &doc, const std::string &path, const std::string &toCheck,
                          const std::string &newPath) {
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &current_path : paths) {
            if (getValue(doc, current_path) == toCheck) {
                std::string npath = normelizePath(current_path, newPath);
                rapidjson::Pointer newPointer(npath.c_str());
                SetValueByPointer(doc, newPointer, true);
            }
        }
    }

    inline std::string addBoolIf(const std::string &json, const std::string &path, const std::string &toCheck,
                                 const std::string &newPath) {
        rapidjson::Document json_doc = FlowJson::parseJson(json);
        addBoolIf(json_doc, path, toCheck, newPath);
        return docToString(json_doc);
    }

    inline std::string cStS(const rapidjson::Value &value) {
        return std::string(value.GetString(), value.GetStringLength());
    }

    inline std::string toString(const rapidjson::Value &value) {
        switch (value.GetType()) {
            case rapidjson::kStringType:
                return cStS(value);
            case rapidjson::kNumberType:
                if (value.IsInt())
                    return std::to_string(value.GetInt());
                if (value.IsUint())
                    return std::to_string(value.GetUint());
                if (value.IsUint64())
                    return std::to_string(value.GetUint64());
                if (value.IsInt64())
                    return std::to_string(value.GetInt64());
            case rapidjson::kTrueType:
                return "true";
            case rapidjson::kFalseType:
                return "false";
            default:
                return valueToString(value);
        }
    }

    inline rapidjson::Value toArray(const std::vector<std::string> &values, rapidjson::Document &doc) {
        rapidjson::Value rtn(rapidjson::kArrayType);

        for (auto &value : values) {
            auto item = cS(value, doc);
            rtn.PushBack(item.Move(), doc.GetAllocator());
        }
        return rtn;
    }

    inline std::string valuesToArray(const std::vector<std::string> &values) {
        rapidjson::StringBuffer buffer;

        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.StartArray();
        for (auto &value : values)
            writer.String(value.c_str());
        writer.EndArray();

        return buffer.GetString();
    }

    inline void valuesToObject(const std::vector<std::string> &values) {
        rapidjson::Document document;
        document.SetObject();
        auto &allocator = document.GetAllocator();

        bool firstRound = true;
        for (const std::string &value : values) {
            auto newJson = parseJson(value);

            if (firstRound) {
                firstRound = false;
                document.Swap(newJson);
                continue;
            }

            auto end = newJson.MemberEnd();

            std::vector<std::string> mis;

            mis.emplace_back("");

            while (!mis.empty()) {
                auto cPos = mis.front();
                rapidjson::Pointer p(cPos.c_str());
                auto *njcVal = GetValueByPointer(newJson, p);

                switch (njcVal->GetType()) {
                    case rapidjson::kObjectType: {
                        LOG_INFO << "kObjectType";
                        auto njcValEnd = njcVal->MemberEnd();
                        for (auto &mem : njcVal->GetObject()) {
                            mis.emplace_back(cPos + "/" + mem.name.GetString());
                        }
                        break;
                    }
                    case rapidjson::kStringType: {
                        LOG_TRACE << "rapidjson::kStringType";
                        rapidjson::Value val(njcVal->GetString(), allocator);
                        SetValueByPointer(document, p, val);
                        break;
                    }
                    case rapidjson::kNullType: {
                        LOG_TRACE << "kNullType";
                        break;
                    }
                    case rapidjson::kFalseType: {
                        LOG_TRACE << "kFalseType";
                        rapidjson::Value val(njcVal->GetBool());
                        SetValueByPointer(document, p, val);
                        break;
                    }
                    case rapidjson::kTrueType: {
                        LOG_TRACE << "kTrueType";
                        rapidjson::Value val(njcVal->GetBool());
                        SetValueByPointer(document, p, val);
                        break;
                    }
                    case rapidjson::kArrayType: {
                        LOG_TRACE << "rapidjson::kArrayType";
                        rapidjson::Value *cVal = GetValueByPointer(newJson, p);
                        auto cEnd = cVal->End();
                        size_t count = 0;
                        for (auto itr = cVal->Begin(); itr != cEnd; ++itr) {
                            mis.emplace_back(cPos + "/" + std::to_string(count++));
                        }
                        break;
                    }
                    case rapidjson::kNumberType:
                        LOG_TRACE << "kNumberType";
                        rapidjson::Value val(njcVal->GetInt());
                        SetValueByPointer(document, p, val);
                        break;
                }
                mis.erase(mis.begin());
            }
        }
        logJson(document);
        LOG_TRACE << "End of valuesToArray!";
    }


    inline std::string
    addBoolValueIfTrueOnce(const std::string &value, const std::string &path, const std::string &newPath) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &path : paths) {
            rapidjson::Pointer pointer(path.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kTrueType) {
                rapidjson::Pointer newPointer(newPath.c_str());
                SetValueByPointer(doc, newPointer, true);
                logJson(doc, false);
                return docToString(doc);
            }
        }
        return value;
    }

    inline bool isTrueOnce(const std::string &value, const std::string &path) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &path : paths) {
            rapidjson::Pointer pointer(path.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kTrueType) {
                return true;
            }
        }
        return false;
    }


    inline bool isTrueOnce(const std::string &value, const std::string &path, const std::string &match) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        for (auto &path : paths) {
            rapidjson::Pointer pointer(path.c_str());
            auto val = GetValueByPointer(doc, pointer);
            if (val->GetType() == rapidjson::kStringType && strcmp(val->GetString(), match.c_str()) == 0) {
                return true;
            }
        }
        return false;
    }

    inline bool isTrue(const std::string &value, const std::string &path) {
        rapidjson::Document doc = parseJson(value);
        std::vector<std::string> paths = findAllJsonValues(doc, path);
        rapidjson::Pointer pointer(path.c_str());
        auto val = GetValueByPointer(doc, pointer);
        return val != nullptr && val->GetType() == rapidjson::kTrueType;
    }

    inline bool isTrue(const rapidjson::Value &value, const std::string &path) {
        rapidjson::Pointer pointer(path.c_str());
        auto val = GetValueByPointer(value, pointer);
        return val != nullptr && val->GetType() == rapidjson::kTrueType;
    }

    inline bool isFalse(const rapidjson::Value &value, const std::string &path) {
        rapidjson::Pointer pointer(path.c_str());
        auto val = GetValueByPointer(value, pointer);
        return val != nullptr && val->GetType() == rapidjson::kFalseType;
    }

    inline bool isFalse(const std::string &value, const std::string &path) {
        rapidjson::Document doc = parseJson(value);
        return isFalse(doc, path);
    }

    inline void addMemberTo(rapidjson::Value &value, const std::string &memberName, const std::string &memberValue,
                            rapidjson::Document &document) {
        rapidjson::Value memberNameValue(rapidjson::kStringType);
        memberNameValue.SetString(memberName.c_str(), static_cast<rapidjson::SizeType>(memberName.size()),
                                  document.GetAllocator());
        rapidjson::Value memberValueValue(rapidjson::kStringType);
        memberValueValue.SetString(memberValue.c_str(), static_cast<rapidjson::SizeType>(memberValue.size()),
                                   document.GetAllocator());
        value.AddMember(memberNameValue.Move(), memberValueValue.Move(), document.GetAllocator());
    }

    inline void markLast(rapidjson::Document &document, const std::string &path) {
        std::vector<std::string> paths = findAllJsonValues(document, path);
        for (std::string path : paths) {
            auto array = GetValueByPointer(document, rapidjson::Pointer(path.c_str()));
            if (array->GetType() == rapidjson::kArrayType) {
                if (array->Size() == 0)
                    continue;
                std::string lastValue = path + "/" + std::to_string(array->Size() - 1);
                auto last = GetValueByPointer(document, rapidjson::Pointer(lastValue.c_str()));
                if (last->GetType() == rapidjson::kObjectType) {
                    std::string isLast = lastValue + "/isLast";
                    SetValueByPointer(document, rapidjson::Pointer(isLast.c_str()),
                                      rapidjson::Value(rapidjson::kTrueType));
                }
            }
        }
    }

    inline rapidjson::Document
    getFirstWhereIs(const rapidjson::Document &document, const std::string &path, const std::string &get) {
        std::vector<std::string> paths = findAllJsonValues(document, path);
        for (std::string path : paths) {
            auto value = GetValueByPointer(document, rapidjson::Pointer(path.c_str()));
            if (value->GetType() == rapidjson::kStringType && strcmp(value->GetString(), get.c_str()) == 0) {
                std::string copypath = path.substr(0, path.find_last_of('/'));

                auto toCopy = GetValueByPointer(document, rapidjson::Pointer(copypath.c_str()));
                rapidjson::Document copy;
                copy.CopyFrom(*toCopy, copy.GetAllocator());
                return copy;

            }
        }

        return rapidjson::Document();
    }

    inline std::vector<std::string> getKeys(rapidjson::Document &document, const std::string &path) {
        std::vector<std::string> rtn;
        auto obj = GetValueByPointer(document, rapidjson::Pointer(path.c_str()));
        if (obj != nullptr && !obj->IsNull() && obj->GetType() == rapidjson::kObjectType) {
            for (auto &value : obj->GetObject()) {
                rtn.emplace_back(value.name.GetString());
            }
        }

        return rtn;
    }

    inline std::vector<std::string> getKeys(const std::string &values, const std::string &path) {
        rapidjson::Document document = parseJson(values);
        return getKeys(document, path);
    }

    inline std::vector<std::string> splitValue(const std::string &values, const std::string &path) {
        std::vector<std::string> rtn;
        rapidjson::Document document = parseJson(values);
        const rapidjson::Pointer pointer(path.c_str());
        const auto &toSplit = rapidjson::GetValueByPointer(document, pointer);
        if (toSplit != nullptr && toSplit->GetType() == rapidjson::kArrayType) {
            for (const auto &value : toSplit->GetArray()) {
                rtn.emplace_back(toString(value));
            }
        } else {
            LOG_WARNING << "Split Value is not an array";
        }
        return rtn;
    }
};
