// Copyright (c) 2016, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: Armin Töpfer

#pragma once

#include <pbcopper/json/JSON.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace PacBio {
namespace Juliet {
class DMutation
{
public:
    DMutation(char refAA, int pos, char curAA);

public:
    // operator int() const { return pos; }
    operator std::string() const
    {
        std::string out;
        out += refAA;
        out += std::to_string(pos);
        out += curAA;
        return out;
    }

    bool operator==(const DMutation& a) const
    {
        if (a.pos != pos) return false;
        if (a.curAA != '*' && curAA != '*' && a.curAA != curAA) return false;
        if (a.refAA != '*' && refAA != '*' && a.refAA != refAA) return false;
        return true;
    }

public:
    char refAA;
    int pos;
    char curAA;
};

class DRM
{
public:
    std::string name;
    std::vector<DMutation> positions;
    std::vector<std::string> PositionStrings() const
    {
        std::vector<std::string> out;
        for (const auto& p : positions)
            out.emplace_back(p);
        return out;
    }

public:
    JSON::Json ToJson() const;
};

class ExpectedMinor
{
public:
    int position;
    std::string aminoacid;
    std::string codon;

public:
    JSON::Json ToJson() const;
};

class TargetGene
{
public:
    TargetGene(const int begin, const int end, const std::string& name,
               const std::vector<DRM>& drms,
               const std::vector<ExpectedMinor>& minors = std::vector<ExpectedMinor>());
    TargetGene() = default;

public:
    int begin;
    int end;
    std::string name;
    std::vector<DRM> drms;
    std::vector<ExpectedMinor> minors;

public:
    JSON::Json ToJson() const;
    static JSON::Json ToJson(const std::vector<TargetGene>& genes);
};

class TargetConfig
{
public:
    TargetConfig() = default;
    TargetConfig(const std::string& input);

public:
    std::vector<TargetGene> targetGenes;
    std::string referenceName;
    std::string referenceSequence;
    bool HasExpectedMinors() const;
    size_t NumExpectedMinors() const;

private:
    static std::string DetermineConfigInput(const std::string& input);
    static std::string ReferenceNameFromJson(const JSON::Json& root);
    static std::string ReferenceSequenceFromJson(const JSON::Json& root);
    static std::vector<TargetGene> TargetGenesFromJson(const JSON::Json& root);

private:
    static std::unordered_map<std::string, std::string> predefinedConfigs_;
};
}
}  //::PacBio::Juliet