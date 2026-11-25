#include "RegressionEngine.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ConcreteMix {

std::optional<double> Polynomial2::solveForX(double targetY, double minValid, double maxValid, double preferredX) const {
    double constTerm = c - targetY;
    if (std::abs(a) < 1e-12) {
        // Equação linear bx + constTerm = 0
        if (std::abs(b) < 1e-12) return std::nullopt;
        double x = -constTerm / b;
        if (x >= minValid && x <= maxValid) return x;
        return std::nullopt;
    }

    double delta = (b * b) - (4.0 * a * constTerm);
    if (delta < 0.0) {
        // Se delta for ligeiramente negativo devido a arredondamento, clamp para zero
        if (delta > -1e-6) delta = 0.0;
        else return std::nullopt;
    }

    double sqrtDelta = std::sqrt(delta);
    double x1 = (-b + sqrtDelta) / (2.0 * a);
    double x2 = (-b - sqrtDelta) / (2.0 * a);

    bool v1 = (x1 >= minValid && x1 <= maxValid);
    bool v2 = (x2 >= minValid && x2 <= maxValid);

    if (v1 && v2) {
        if (preferredX > 0.0) {
            return (std::abs(x1 - preferredX) < std::abs(x2 - preferredX)) ? x1 : x2;
        }
        // Fallback: seleciona a raiz com derivada negativa (ramo decrescente clássico de Abrams)
        double d1 = 2.0 * a * x1 + b;
        double d2 = 2.0 * a * x2 + b;
        if (d1 < 0.0 && d2 >= 0.0) return x1;
        if (d2 < 0.0 && d1 >= 0.0) return x2;
        return std::min(x1, x2);
    }
    if (v1) return x1;
    if (v2) return x2;

    // Se nenhuma estiver estritamente dentro, mas estiver muito próxima dos limites:
    double diff1 = std::min(std::abs(x1 - minValid), std::abs(x1 - maxValid));
    double diff2 = std::min(std::abs(x2 - minValid), std::abs(x2 - maxValid));
    if (diff1 < 0.1 && (preferredX < 0.0 || std::abs(x1 - preferredX) <= std::abs(x2 - preferredX))) 
        return std::clamp(x1, minValid, maxValid);
    if (diff2 < 0.1) 
        return std::clamp(x2, minValid, maxValid);

    return std::nullopt;
}

std::string Polynomial2::toEquationString(const std::string& yVar, const std::string& xVar) const {
    std::ostringstream ss;
    ss << std::fixed;
    ss << yVar << " = ";
    if (std::abs(a) > 1e-8) {
        int aPrec = (std::abs(a) < 0.01) ? 6 : 4;
        ss << std::setprecision(aPrec) << a << "·" << xVar << "² ";
        ss << std::setprecision(4);
        if (b >= 0.0) ss << "+ " << b << "·" << xVar << " ";
        else ss << "- " << std::abs(b) << "·" << xVar << " ";
    } else {
        ss << std::setprecision(4) << b << "·" << xVar << " ";
    }
    ss << std::setprecision(4);
    if (c >= 0.0) ss << "+ " << c;
    else ss << "- " << std::abs(c);
    ss << "  (R² = " << std::setprecision(3) << rSquared << ")";
    return ss.str();
}

std::string LinearEquation::toEquationString(const std::string& yVar, const std::string& xVar) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4);
    ss << yVar << " = " << a << "·" << xVar << " ";
    if (b >= 0.0) ss << "+ " << b;
    else ss << "- " << std::abs(b);
    ss << "  (R² = " << std::setprecision(3) << rSquared << ")";
    return ss.str();
}

double ExponentialEquation::evaluate(double x) const {
    if (k2 <= 0.0) return 0.0;
    return k1 / std::pow(k2, x);
}

std::optional<double> ExponentialEquation::solveForX(double targetY) const {
    if (targetY <= 0.0 || k1 <= 0.0 || k2 <= 0.0 || std::abs(k2 - 1.0) < 1e-9) {
        return std::nullopt;
    }
    // y = k1 / (k2^x) => k2^x = k1 / y => x * ln(k2) = ln(k1 / y)
    return std::log(k1 / targetY) / std::log(k2);
}

std::string ExponentialEquation::toEquationString(const std::string& yVar, const std::string& xVar) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << yVar << " = " << k1 << " / (" << k2 << "^(" << xVar << "))";
    ss << "  (R² = " << std::setprecision(3) << rSquared << ")";
    return ss.str();
}

Polynomial2 RegressionEngine::fitPolynomial2(const std::vector<Point2D>& points) {
    Polynomial2 poly;
    const size_t n = points.size();
    if (n < 3) {
        // Fallback para menos de 3 pontos
        if (n == 2) {
            auto lin = fitLinear(points);
            poly.a = 0.0;
            poly.b = lin.a;
            poly.c = lin.b;
            poly.rSquared = lin.rSquared;
            poly.minX = lin.minX;
            poly.maxX = lin.maxX;
            poly.isValid = lin.isValid;
        }
        return poly;
    }

    // Calcula limites do domínio experimental [minX, maxX] e estatísticas básicas
    double minX = points[0].x;
    double maxX = points[0].x;
    double sumX = 0.0;
    double sumY = 0.0;
    for (const auto& pt : points) {
        if (pt.x < minX) minX = pt.x;
        if (pt.x > maxX) maxX = pt.x;
        sumX += pt.x;
        sumY += pt.y;
    }
    poly.minX = minX;
    poly.maxX = maxX;

    double meanX = sumX / static_cast<double>(n);
    double meanY = sumY / static_cast<double>(n);

    // Variância e desvio padrão de x para centralização e escalonamento
    double varX = 0.0;
    for (const auto& pt : points) {
        double dx = pt.x - meanX;
        varX += dx * dx;
    }
    varX /= static_cast<double>(n);

    // Se todos os pontos x forem idênticos (varX < 1e-12), sistema é degenerado
    if (varX < 1e-12) {
        poly.a = 0.0;
        poly.b = 0.0;
        poly.c = meanY;
        poly.rSquared = 0.0;
        poly.isValid = false;
        return poly;
    }

    double scaleX = std::sqrt(varX); // escala = sigma_x

    // Monta o sistema normal nas variáveis padronizadas u_i = (x_i - meanX) / scaleX
    // Em u, mean(u) = 0 e var(u) = 1.0, garantindo condicionamento numérico excelente
    double s_u0 = static_cast<double>(n);
    double s_u1 = 0.0, s_u2 = 0.0, s_u3 = 0.0, s_u4 = 0.0;
    double s_yu0 = 0.0, s_yu1 = 0.0, s_yu2 = 0.0;

    for (const auto& pt : points) {
        double u = (pt.x - meanX) / scaleX;
        double u2 = u * u;
        double u3 = u2 * u;
        double u4 = u3 * u;

        s_u1 += u;
        s_u2 += u2;
        s_u3 += u3;
        s_u4 += u4;

        s_yu0 += pt.y;
        s_yu1 += u * pt.y;
        s_yu2 += u2 * pt.y;
    }

    // Matriz 3x3 para variáveis u
    double M[3][4] = {
        {s_u4, s_u3, s_u2, s_yu2},
        {s_u3, s_u2, s_u1, s_yu1},
        {s_u2, s_u1, s_u0, s_yu0}
    };

    // Eliminação de Gauss com pivoteamento
    for (int i = 0; i < 3; ++i) {
        int maxRow = i;
        for (int k = i + 1; k < 3; ++k) {
            if (std::abs(M[k][i]) > std::abs(M[maxRow][i])) {
                maxRow = k;
            }
        }
        for (int k = 0; k < 4; ++k) {
            std::swap(M[i][k], M[maxRow][k]);
        }

        if (std::abs(M[i][i]) < 1e-12) continue;

        for (int k = i + 1; k < 3; ++k) {
            double factor = M[k][i] / M[i][i];
            for (int j = i; j < 4; ++j) {
                M[k][j] -= factor * M[i][j];
            }
        }
    }

    // Substituição retroativa
    double solU[3] = {0.0, 0.0, 0.0};
    bool singular = false;
    for (int i = 2; i >= 0; --i) {
        solU[i] = M[i][3];
        for (int j = i + 1; j < 3; ++j) {
            solU[i] -= M[i][j] * solU[j];
        }
        if (std::abs(M[i][i]) > 1e-12) {
            solU[i] /= M[i][i];
        } else {
            singular = true;
        }
    }

    if (singular) {
        // Fallback para linear se polinômio for singular
        auto lin = fitLinear(points);
        poly.a = 0.0;
        poly.b = lin.a;
        poly.c = lin.b;
        poly.rSquared = lin.rSquared;
        poly.minX = minX;
        poly.maxX = maxX;
        poly.isValid = lin.isValid;
        return poly;
    }

    double a_u = solU[0];
    double b_u = solU[1];
    double c_u = solU[2];

    // Converte analiticamente os coeficientes de volta para a variável original x:
    // y = a_u * ((x - meanX)/scaleX)^2 + b_u * ((x - meanX)/scaleX) + c_u
    //   = a * x^2 + b * x + c
    double invScale = 1.0 / scaleX;
    double invScale2 = invScale * invScale;

    poly.a = a_u * invScale2;
    poly.b = (b_u * invScale) - (2.0 * a_u * meanX * invScale2);
    poly.c = c_u - (b_u * meanX * invScale) + (a_u * meanX * meanX * invScale2);
    poly.isValid = true;

    // Cálculo do coeficiente de determinação R²
    double ssTot = 0.0;
    double ssRes = 0.0;
    for (const auto& pt : points) {
        double yPred = poly.evaluate(pt.x);
        ssTot += (pt.y - meanY) * (pt.y - meanY);
        ssRes += (pt.y - yPred) * (pt.y - yPred);
    }

    if (ssTot > 1e-9) {
        poly.rSquared = std::max(0.0, std::min(1.0, 1.0 - (ssRes / ssTot)));
    } else {
        poly.rSquared = 1.0;
    }

    return poly;
}

LinearEquation RegressionEngine::fitLinear(const std::vector<Point2D>& points) {
    LinearEquation lin;
    const size_t n = points.size();
    if (n < 2) return lin;

    double minX = points[0].x;
    double maxX = points[0].x;
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    for (const auto& pt : points) {
        if (pt.x < minX) minX = pt.x;
        if (pt.x > maxX) maxX = pt.x;
        sumX += pt.x;
        sumY += pt.y;
        sumXY += pt.x * pt.y;
        sumX2 += pt.x * pt.x;
    }
    lin.minX = minX;
    lin.maxX = maxX;

    double denom = (n * sumX2) - (sumX * sumX);
    if (std::abs(denom) > 1e-12) {
        lin.a = ((n * sumXY) - (sumX * sumY)) / denom;
        lin.b = (sumY - (lin.a * sumX)) / static_cast<double>(n);
        lin.isValid = true;
    } else {
        lin.a = 0.0;
        lin.b = sumY / static_cast<double>(n);
        lin.isValid = false;
        lin.rSquared = 0.0;
        return lin;
    }

    double yMean = sumY / static_cast<double>(n);
    double ssTot = 0.0, ssRes = 0.0;
    for (const auto& pt : points) {
        double yPred = lin.evaluate(pt.x);
        ssTot += (pt.y - yMean) * (pt.y - yMean);
        ssRes += (pt.y - yPred) * (pt.y - yPred);
    }
    if (ssTot > 1e-9) {
        lin.rSquared = std::max(0.0, std::min(1.0, 1.0 - (ssRes / ssTot)));
    } else {
        lin.rSquared = 1.0;
    }

    return lin;
}

ExponentialEquation RegressionEngine::fitAbramsExponential(const std::vector<Point2D>& points) {
    ExponentialEquation eq;
    std::vector<Point2D> logPoints;
    logPoints.reserve(points.size());

    double minX = 1e9;
    double maxX = -1e9;
    for (const auto& pt : points) {
        if (pt.y > 0.0) {
            logPoints.push_back({pt.x, std::log(pt.y)});
            if (pt.x < minX) minX = pt.x;
            if (pt.x > maxX) maxX = pt.x;
        }
    }

    if (logPoints.size() >= 2) {
        eq.minX = minX;
        eq.maxX = maxX;
        // ln(y) = ln(k1) - x * ln(k2)
        // Regressão linear: Y = A*x + B onde B = ln(k1) e A = -ln(k2)
        auto lin = fitLinear(logPoints);
        if (lin.isValid) {
            eq.k1 = std::exp(lin.b);
            eq.k2 = std::exp(-lin.a);
            eq.isValid = (eq.k1 > 0.0 && eq.k2 > 0.0 && std::abs(eq.k2 - 1.0) > 1e-9);

            double yMean = 0.0;
            for (const auto& pt : points) yMean += pt.y;
            yMean /= static_cast<double>(points.size());

            double ssTot = 0.0, ssRes = 0.0;
            for (const auto& pt : points) {
                double yPred = eq.evaluate(pt.x);
                ssTot += (pt.y - yMean) * (pt.y - yMean);
                ssRes += (pt.y - yPred) * (pt.y - yPred);
            }
            if (ssTot > 1e-9) {
                eq.rSquared = std::max(0.0, std::min(1.0, 1.0 - (ssRes / ssTot)));
            }
        }
    }

    return eq;
}

double RegressionEngine::calculateTheoreticalConsumption(
    double sandRatio,
    double gravelRatio,
    double waterCementRatio,
    double cementDensity,
    double sandDensity,
    double gravelDensity,
    double waterDensity
) {
    // Proteção rigorosa contra densidades nulas ou negativas (divisão por zero)
    if (cementDensity <= 0.0 || sandDensity <= 0.0 || gravelDensity <= 0.0 || waterDensity <= 0.0) {
        return 0.0;
    }

    // Equação dos volumes absolutos por metro cúbico (1000 litros):
    // V_concreto = V_cimento + V_areia + V_brita + V_agua + V_ar_incorporado
    // C * (1/rho_c + a/rho_a + p/rho_b + (a/c)/rho_w) = 1000
    double specificSum = (1.0 / cementDensity) +
                         (sandRatio / sandDensity) +
                         (gravelRatio / gravelDensity) +
                         (waterCementRatio / waterDensity);

    if (specificSum <= 0.0) return 350.0;
    return 1000.0 / specificSum;
}

} // namespace ConcreteMix
