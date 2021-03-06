// Copyright (c) 2011-2017, Pacific Biosciences of California, Inc.
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

#include <pbbam/DataSet.h>

#include <pacbio/io/BamParser.h>

namespace PacBio {
namespace IO {
std::unique_ptr<BAM::internal::IQuery> BamQuery(const std::string& filePath)
{
    BAM::DataSet ds(filePath);
    const auto filter = BAM::PbiFilter::FromDataSet(ds);
    std::unique_ptr<BAM::internal::IQuery> query(nullptr);
    if (filter.IsEmpty())
        query.reset(new BAM::EntireFileQuery(ds));
    else
        query.reset(new BAM::PbiFilterQuery(filter, ds));
    return query;
}

std::vector<std::shared_ptr<Data::ArrayRead>> BamToArrayReads(const std::string& filePath,
                                                              int regionStart, int regionEnd)
{
    std::vector<std::shared_ptr<Data::ArrayRead>> returnList;
    regionStart = std::max(regionStart - 1, 0);
    regionEnd = std::max(regionEnd - 1, 0);

    auto query = BamQuery(filePath);

    int idx = 0;
    // Iterate over all records and convert online
    for (auto& record : *query) {
        if (record.Impl().IsSupplementaryAlignment()) continue;
        if (!record.Impl().IsPrimaryAlignment()) continue;
        if (record.ReferenceStart() < regionEnd && record.ReferenceEnd() > regionStart) {
            record.Clip(BAM::ClipType::CLIP_TO_REFERENCE, regionStart, regionEnd);
            returnList.emplace_back(
                std::make_shared<Data::BAMArrayRead>(Data::BAMArrayRead(record, idx++)));
        }
    }
    return returnList;
}
}
}  // ::PacBio::IO
