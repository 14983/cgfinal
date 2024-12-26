#ifndef __NURBS_H__
#define __NURBS_H__

#include <glm/glm.hpp>
#include <vector>

namespace nurbs {
    /**
     * @brief 计算 NURBS 基函数
     * @param i 基函数编号
     * @param p 基函数次数
     * @param u 基函数参数
     * @param knots 基函数 knot 序列
     */
    float N(int i, int p, float u, std::vector<float> knots);
    
    /**
     * @brief 计算 NURBS 曲面上的点
     * @param target_points 返回的点，大小为 (sample_density * sample_density, 3)，无需为其初始化
     * @param ctrl_points 控制点
     * @param weights 权重
     * @param knots_u 基函数 knot 序列 u 方向，保证长度为 (u 方向控制点数量 + degree + 1)
     * @param knots_v 基函数 knot 序列 v 方向，保证长度为 (v 方向控制点数量 + degree + 1)
     * @param degree 基函数次数
     * @param u 基函数参数 u 方向
     * @param v 基函数参数 v 方向
     */
    void genNURBS(
        std::vector<std::vector<glm::vec3>> &target_points,
        std::vector<std::vector<glm::vec3>> ctrl_points,
        int sample_density = 50,
        std::vector<std::vector<float>> weights = std::vector<std::vector<float>>(0),
        std::vector<float> knots_u = std::vector<float>(0),
        std::vector<float> knots_v = std::vector<float>(0),
        int degree = 2
    );
}

#endif