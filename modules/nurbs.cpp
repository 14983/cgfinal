#include "../include/nurbs.h"
#include <cassert>
#include <iostream>

namespace nurbs {
    float N(int i, int p, float u, std::vector<float> knots) {
        if (p == 0) {
            if (u >= knots[i] && u < knots[i + 1])
                return 1.0;
            else
                return 0.0;
        } else {
            float left = ((knots[i + p] - knots[i]) == 0) ? 0.0 :
                (u - knots[i]) / (knots[i + p] - knots[i]) * N(i, p - 1, u, knots);
            float right = ((knots[i + p + 1] - knots[i + 1]) == 0) ? 0.0 :
                (knots[i + p + 1] - u) / (knots[i + p + 1] - knots[i + 1]) * N(i + 1, p - 1, u, knots);
            return left + right;
        }
    }

    void genNURBS(
        std::vector<std::vector<glm::vec3>> &target_points,
        std::vector<std::vector<glm::vec3>> ctrl_points,
        int sample_density,
        std::vector<std::vector<float>> weights,
        std::vector<float> knots_u,
        std::vector<float> knots_v,
        int degree
    ) {
        if (weights.size() == 0) {
            weights.resize(ctrl_points.size());
            for (auto &i : weights)
                i.resize(ctrl_points[0].size(), 1.0);
        }
        if (knots_u.size() == 0) {
            knots_u.resize(ctrl_points.size() + 3);
            for (size_t i = 0; i < 3; i++)
                knots_u[i] = 0.0;
            for (size_t i = 3; i < knots_u.size() - 3; i++)
                knots_u[i] = i - 2;
            for (size_t i = knots_u.size() - 3; i < knots_u.size(); i++)
                knots_u[i] = knots_u.size() - 5;
        }
        if (knots_v.size() == 0) {
            knots_v.resize(ctrl_points[0].size() + 3);
            for (size_t i = 0; i < 3; i++)
                knots_v[i] = 0.0;
            for (size_t i = 3; i < knots_v.size() - 3; i++)
                knots_v[i] = i - 2;
            for (size_t i = knots_v.size() - 3; i < knots_v.size(); i++)
                knots_v[i] = knots_v.size() - 5;
        }
        assert(knots_u.size() == ctrl_points.size() + degree + 1);
        assert(knots_v.size() == ctrl_points[0].size() + degree + 1);
        target_points.resize(sample_density);
        for (auto &i : target_points)
            i.resize(sample_density);
        for (int i = 0; i < sample_density; i++) {
            float u = knots_u[0] + (knots_u[knots_u.size()-1]) * ((float)i / sample_density);
            for (int j = 0; j < sample_density; j++) {
                float v = knots_v[0] + (knots_v[knots_v.size()-1]) * ((float)j / sample_density);
                glm::vec3 tmp = {0.0, 0.0, 0.0};
                float frac = 0.0;
                for (size_t i1 = 0; i1 < ctrl_points.size(); i1++)
                    for (size_t j1 = 0; j1 < ctrl_points[0].size(); j1++)
                        frac += weights[i1][j1] * N(i1, degree, u, knots_u) * N(j1, degree, v, knots_v);
                for (size_t i1 = 0; i1 < ctrl_points.size(); i1++)
                    for (size_t j1 = 0; j1 < ctrl_points[0].size(); j1++)
                        tmp += ctrl_points[i1][j1] * weights[i1][j1] * N(i1, degree, u, knots_u) * N(j1, degree, v, knots_v);
                target_points[i][j] = tmp / frac;
            }
        }
    }
}