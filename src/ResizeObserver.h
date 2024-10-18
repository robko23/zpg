//
// Created by robko on 10/18/24.
//

#pragma once

class ResizeObserver {
public:
    virtual void onWindowResize(int width, int height) = 0;

    virtual ~ResizeObserver() = default;
};
