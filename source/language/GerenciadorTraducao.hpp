#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QTranslator>
#include <QCoreApplication>

namespace ConcreteMix::Language {

enum class Idioma {
    PortuguesBrasil,
    InglesEstadosUnidos
};

/**
 * @brief Gerenciador central de internacionalização e tradução dinâmica em tempo de execução.
 * Permite alternar instantaneamente entre pt_BR (Português do Brasil) e en_US (Inglês dos EUA)
 * sem a necessidade de reiniciar a aplicação.
 */
class GerenciadorTraducao : public QObject {
    Q_OBJECT

public:
    static GerenciadorTraducao& instance();

    void inicializar();
    bool definirIdioma(const QString& codigoIdioma);
    void mudarIdioma(Idioma novoIdioma);

    QString codigoIdiomaAtual() const { return m_idiomaAtual; }
    Idioma idiomaAtual() const;
    QString simboloMoeda() const;
    static QString currencySymbol();

    const QMap<QString, QString>& idiomasDisponiveis() const { return m_idiomasDisponiveis; }

    static QString trText(const char* context, const char* sourceText);

signals:
    void idiomaAlterado(const QString& codigoIdioma);
    void idiomaAlteradoEnum(Idioma novoIdioma);

private:
    explicit GerenciadorTraducao(QObject* parent = nullptr);
    ~GerenciadorTraducao() override;

    bool aplicarTraducao();
    void carregarConfiguracaoIdioma();
    void salvarConfiguracaoIdioma() const;
    void removerTradutores();

    QString m_idiomaAtual{QStringLiteral("pt_BR")};
    QMap<QString, QString> m_idiomasDisponiveis;

    QTranslator* m_tradutorApp{nullptr};
    QTranslator* m_tradutorQt{nullptr};
};

} // namespace ConcreteMix::Language
