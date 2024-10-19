//
// Created by robko on 10/19/24.
//

#pragma once

namespace math {
    template<typename T>
    T interpolate(T a, T in_min, T in_max, T out_min, T out_max) {
        return (a - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
}
