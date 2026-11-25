#pragma once

#include <QMap>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace concrete_tests {

struct TestCase {
    std::string name;
    std::function<void()> fn;
};

class TestRunner {
public:
    explicit TestRunner(std::string suiteName)
        : m_suiteName(std::move(suiteName)) {}

    void add(const std::string& name, std::function<void()> fn) {
        m_cases.push_back({name, std::move(fn)});
    }

    int runAll() const {
        std::cout << "====================================================================\n";
        std::cout << "  SUITE: " << m_suiteName << "\n";
        std::cout << "====================================================================\n";

        int passed = 0;
        int failed = 0;

        for (const auto& tc : m_cases) {
            try {
                tc.fn();
                ++passed;
                std::cout << "  [PASS] " << tc.name << "\n";
            } catch (const std::exception& ex) {
                ++failed;
                std::cerr << "  [FAIL] " << tc.name << " -> " << ex.what() << "\n";
            } catch (...) {
                ++failed;
                std::cerr << "  [FAIL] " << tc.name << " -> unknown exception\n";
            }
        }

        std::cout << "--------------------------------------------------------------------\n";
        std::cout << "  Total: " << m_cases.size()
                  << " | Passed: " << passed
                  << " | Failed: " << failed << "\n";
        std::cout << "====================================================================\n";
        return (failed == 0 && passed > 0) ? 0 : 1;
    }

private:
    std::string m_suiteName;
    std::vector<TestCase> m_cases;
};

/// Guarda e restaura automaticamente as chaves de QSettings("AppsCpp", "ConcreteMixDesign")
/// para garantir que os testes não alterem preferências de idioma ou projeto do usuário.
class SettingsBackupGuard {
public:
    SettingsBackupGuard() {
        QSettings settings(QStringLiteral("AppsCpp"), QStringLiteral("ConcreteMixDesign"));
        const QStringList keys = settings.allKeys();
        for (const QString& key : keys) {
            m_snapshot.insert(key, settings.value(key));
        }
    }

    ~SettingsBackupGuard() {
        QSettings settings(QStringLiteral("AppsCpp"), QStringLiteral("ConcreteMixDesign"));
        settings.clear();
        for (auto it = m_snapshot.cbegin(); it != m_snapshot.cend(); ++it) {
            settings.setValue(it.key(), it.value());
        }
        settings.sync();
    }

    SettingsBackupGuard(const SettingsBackupGuard&) = delete;
    SettingsBackupGuard& operator=(const SettingsBackupGuard&) = delete;

private:
    QMap<QString, QVariant> m_snapshot;
};

inline void fail(const std::string& expr, const std::string& msg, const char* file, int line) {
    std::ostringstream oss;
    oss << file << ":" << line << ": assertion failed (" << expr << ")";
    if (!msg.empty()) {
        oss << " — " << msg;
    }
    throw std::runtime_error(oss.str());
}

}  // namespace concrete_tests

#define CMD_ASSERT(cond)                                                              \
    do {                                                                              \
        if (!(cond)) {                                                                \
            ::concrete_tests::fail(#cond, "", __FILE__, __LINE__);                    \
        }                                                                             \
    } while (false)

#define CMD_ASSERT_MSG(cond, msg)                                                     \
    do {                                                                              \
        if (!(cond)) {                                                                \
            ::concrete_tests::fail(#cond, (msg), __FILE__, __LINE__);                 \
        }                                                                             \
    } while (false)

#define CMD_ASSERT_NEAR(actual, expected, tol)                                        \
    do {                                                                              \
        const double _a = static_cast<double>(actual);                                \
        const double _e = static_cast<double>(expected);                              \
        const double _t = static_cast<double>(tol);                                   \
        if (std::isnan(_a) || std::abs(_a - _e) > _t) {                               \
            std::ostringstream _oss;                                                  \
            _oss << "actual=" << _a << ", expected=" << _e << ", tol=" << _t;         \
            ::concrete_tests::fail(#actual " ~= " #expected, _oss.str(), __FILE__, __LINE__); \
        }                                                                             \
    } while (false)

#define CMD_ASSERT_CONTAINS(haystack, needle)                                         \
    do {                                                                              \
        const std::string _h = (haystack);                                            \
        const std::string _n = (needle);                                              \
        if (_h.find(_n) == std::string::npos) {                                       \
            ::concrete_tests::fail(#haystack " contains " #needle,                    \
                                   "missing substring: " + _n, __FILE__, __LINE__);   \
        }                                                                             \
    } while (false)
