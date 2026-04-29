/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_BONE_H
#define RHEA_BONE_H

#include <vector>
#include <string>
#include <Math/Matrix4.h>

struct Bone {

    std::string name;                      // Bone name (optional)
    int index;                             // Unique bone index/ID
    int parent_index;                       // Index of the parent bone
    std::vector<int> children_indices;      // Indices of child bones

    math::Matrix4 inverse_bind_matrix;           // Inverse bind pose matrix
    math::Matrix4 local_transform;              // Local transform (relative to parent)
    math::Matrix4 global_transform;             // Global transform (optional but common)
    math::Matrix4 final_transform;              // Final transform used for skinning (optional)

    // Constructor
    Bone(int idx, int parentIdx, const math::Matrix4& invBindMat)
        : index(idx), parent_index(parentIdx), inverse_bind_matrix(invBindMat),
        local_transform(math::Matrix4()), global_transform(math::Matrix4()),
        final_transform(math::Matrix4()) {}

    // Method to add a child bone index
    void addChild(int child_index) {
        children_indices.push_back(child_index);
    }

    // Method to update the global transform (called during animation)
    void UpdateGlobalTransform(const math::Matrix4& parentGlobalTransform) {
        global_transform = parentGlobalTransform * local_transform;
        final_transform = global_transform * inverse_bind_matrix; // For skinning      
    }
};

#endif // !RHEA_BONE_H
