// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <algorithm>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <utils/slog.hpp>
#include "utils.hpp"

namespace {
template <typename StreamType, typename EndlType>
void SaveDetectionLogToStream(StreamType& stream, const EndlType& endl,
                              const DetectionLog& log) {
    for (const auto& entry : log) {
        std::vector<TrackedObject> objects(entry.objects.begin(),
                                           entry.objects.end());
        std::sort(objects.begin(), objects.end(),
                  [](const TrackedObject& a,
                     const TrackedObject& b)
                  { return a.object_id < b.object_id; });
        for (const auto& object : objects) {
            auto frame_idx_to_save = entry.frame_idx;
            stream << frame_idx_to_save << ',';
            stream << object.object_id << ','
                << object.rect.x << ',' << object.rect.y << ','
                << object.rect.width << ',' << object.rect.height;
            stream << endl;
        }
    }
}
}  // anonymous namespace

void DrawPolyline(const std::vector<cv::Point>& polyline,
                  const cv::Scalar& color, cv::Mat* image, int lwd) {
    PT_CHECK(image);
    PT_CHECK(!image->empty());
    PT_CHECK_EQ(image->type(), CV_8UC3);
    PT_CHECK_GT(lwd, 0);
    PT_CHECK_LT(lwd, 20);

    for (size_t i = 1; i < polyline.size(); i++) {
        cv::line(*image, polyline[i - 1], polyline[i], color, lwd);
    }
}

void SaveDetectionLogToTrajFile(const std::string& path,
                                const DetectionLog& log) {
    std::ofstream file(path.c_str());
    PT_CHECK(file.is_open());
    SaveDetectionLogToStream(file, '\n', log);
}

void PrintDetectionLog(const DetectionLog& log) {
    SaveDetectionLogToStream(slog::debug, slog::endl, log);
}
