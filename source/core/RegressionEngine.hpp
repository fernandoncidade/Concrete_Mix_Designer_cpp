#pragma once

#include <vector>
#include <string>
#include <optional>
#include <utility>

namespace ConcreteMix {

struct Point2D {
    double x = 0.0;
    double y = 0.0;
};

struct Polynomial2 {
    double a = 0.0; // coeficiente de x²
    double b = 0.0; // coeficiente de x
    double c = 0.0; // termo constante
    double rSquared = 1.0;
    double minX = 0.0; // limite inferior do domínio experimental
    double maxX = 0.0; // limite superior do domínio experimental
    bool isValid = false;

    double evaluate(double x) const {
        return (a * x * x) + (b * x) + c;
    }

    bool isInsideDomain(double x) const {
        return isValid && (x >= minX - 1e-6) && (x <= maxX + 1e-6);
    }

    // Resolve ax² + bx + (c - targetY) = 0
    // Retorna a raiz que cai dentro do intervalo de plausibilidade física [minValid, maxValid]
    // preferredX: se informado (> 0), desempatará raízes favorecendo a mais próxima de preferredX
    std::optional<double> solveForX(double targetY, double minValid, double maxValid, double preferredX = -1.0) const;

    std::string toEquationString(const std::string& yVar = "y", const std::string& xVar = "x") const;
};

struct LinearEquation {
    double a = 0.0; // inclinação (coeficiente angular)
    double b = 0.0; // intercepto (coeficiente linear)
    double rSquared = 1.0;
    double minX = 0.0;
    double maxX = 0.0;
    bool isValid = false;

    double evaluate(double x) const {
        return (a * x) + b;
    }

    bool isInsideDomain(double x) const {
        return isValid && (x >= minX - 1e-6) && (x <= maxX + 1e-6);
    }

    double solveForX(double targetY) const {
        if (std::abs(a) < 1e-9) return 0.0;
        return (targetY - b) / a;
    }

    std::string toEquationString(const std::string& yVar = "y", const std::string& xVar = "x") const;
};

struct ExponentialEquation {
    double k1 = 0.0;
    double k2 = 0.0;
    double rSquared = 1.0;
    double minX = 0.0;
    double maxX = 0.0;
    bool isValid = false;

    bool isInsideDomain(double x) const {
        return isValid && (x >= minX - 1e-6) && (x <= maxX + 1e-6);
    }

    // y = k1 / (k2^x)
    double evaluate(double x) const;
    std::optional<double> solveForX(double targetY) const;
    std::string toEquationString(const std::string& yVar = "fcj", const std::string& xVar = "a/c") const;
};

class RegressionEngine {
public:
    // Ajuste de Polinômio de 2º Grau por Mínimos Quadrados: y = ax² + bx + c
    static Polynomial2 fitPolynomial2(const std::vector<Point2D>& points);

    // Ajuste Linear por Mínimos Quadrados: y = ax + b
    static LinearEquation fitLinear(const std::vector<Point2D>& points);

    // Ajuste da Lei de Abrams Exponencial clássica: y = k1 / (k2^x) <=> ln(y) = ln(k1) - x*ln(k2)
    static ExponentialEquation fitAbramsExponential(const std::vector<Point2D>& points);

    // Determina o consumo de cimento teórico pela lei dos volumes absolutos
    // C = 1000 / (1/rho_c + a/rho_a + b/rho_b + (a/c)/rho_w)
    static double calculateTheoreticalConsumption(
        double sandRatio,
        double gravelRatio,
        double waterCementRatio,
        double cementDensity,
        double sandDensity,
        double gravelDensity,
        double waterDensity = 1.0
    );
};

} // namespace ConcreteMix
