/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_AREACOMPONENT_H
#define RHEA_AREACOMPONENT_H

#include <Rhea/Component.h>
#include <Rhea/Ref.h>
#include <string>

namespace rhea
{
    class Area;

    /** Manages logical area asset - both reference and resolved instance */
    class AreaComponent : public Component
    {
        RECLASS(AreaComponent, Component, "//meworla/areacomponent")
    public:
        AreaComponent(std::string name, std::string type_name) :
            Component(name, type_name)
        {}
        void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

        void onActivate() override;
		void onDeactivate() override;
    };
}

#endif // !RHEA_AREACOMPONENT_H
