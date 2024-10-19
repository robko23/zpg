//
// Created by robko on 10/19/24.
//

#pragma once

class Scene {
public:
    virtual void render() = 0;

    virtual bool shouldExit() const = 0;
};
