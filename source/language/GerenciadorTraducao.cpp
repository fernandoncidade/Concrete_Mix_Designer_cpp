#include "GerenciadorTraducao.hpp"
#include "../utils/CaminhoPersistenteUtils.hpp"
#include "../utils/LogManager.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLibraryInfo>
#include <QLocale>
#include <QDateTime>

namespace ConcreteMix::Language {

namespace {

QString arquivoConfiguracaoIdioma() {
    const QString baseDir = source::utils::obter_caminho_persistente();
    if (baseDir.isEmpty()) {
        return QString();
    }
    return QDir(baseDir).filePath(QStringLiteral("language.json"));
}

} // namespace

GerenciadorTraducao& GerenciadorTraducao::instance() {
    static GerenciadorTraducao inst;
    return inst;
}

GerenciadorTraducao::GerenciadorTraducao(QObject* parent)
    : QObject(parent)
{
    m_idiomasDisponiveis.insert(QStringLiteral("pt_BR"), QStringLiteral("Português (Brasil)"));
    m_idiomasDisponiveis.insert(QStringLiteral("en_US"), QStringLiteral("English (United States)"));

    m_tradutorApp = new QTranslator(this);
    m_tradutorQt = new QTranslator(this);
}

GerenciadorTraducao::~GerenciadorTraducao() {
    removerTradutores();
}

void GerenciadorTraducao::inicializar() {
    carregarConfiguracaoIdioma();
    aplicarTraducao();
}

Idioma GerenciadorTraducao::idiomaAtual() const {
    if (m_idiomaAtual.startsWith(QStringLiteral("en"), Qt::CaseInsensitive)) {
        return Idioma::InglesEstadosUnidos;
    }
    return Idioma::PortuguesBrasil;
}

QString GerenciadorTraducao::simboloMoeda() const {
    if (idiomaAtual() == Idioma::InglesEstadosUnidos) {
        return QStringLiteral("$");
    }
    return QStringLiteral("R$");
}

QString GerenciadorTraducao::currencySymbol() {
    return instance().simboloMoeda();
}

void GerenciadorTraducao::mudarIdioma(Idioma novoIdioma) {
    if (novoIdioma == Idioma::InglesEstadosUnidos) {
        definirIdioma(QStringLiteral("en_US"));
    } else {
        definirIdioma(QStringLiteral("pt_BR"));
    }
}

bool GerenciadorTraducao::definirIdioma(const QString& codigoIdioma) {
    if (!m_idiomasDisponiveis.contains(codigoIdioma)) {
        source::utils::LogManager::warning(
            QStringLiteral("Tentativa de definir idioma invalido: %1").arg(codigoIdioma));
        return false;
    }

    if (m_idiomaAtual == codigoIdioma && m_tradutorApp != nullptr && !m_tradutorApp->isEmpty()) {
        return true;
    }

    m_idiomaAtual = codigoIdioma;
    salvarConfiguracaoIdioma();
    const bool res = aplicarTraducao();

    emit idiomaAlterado(m_idiomaAtual);
    emit idiomaAlteradoEnum(idiomaAtual());

    return res;
}

QString GerenciadorTraducao::trText(const char* context, const char* sourceText) {
    return QCoreApplication::translate(context, sourceText);
}

void GerenciadorTraducao::removerTradutores() {
    auto* app = QCoreApplication::instance();
    if (!app) return;

    if (m_tradutorApp) {
        app->removeTranslator(m_tradutorApp);
    }
    if (m_tradutorQt) {
        app->removeTranslator(m_tradutorQt);
    }
}

bool GerenciadorTraducao::aplicarTraducao() {
    auto* app = QCoreApplication::instance();
    if (!app) {
        source::utils::LogManager::error(QStringLiteral("QCoreApplication indisponivel ao aplicar traducao."));
        return false;
    }

    removerTradutores();

    const QString localeCode = QString(m_idiomaAtual).replace(QLatin1Char('_'), QLatin1Char('-'));
    QLocale::setDefault(QLocale(localeCode));

    // 1. Tentar carregar tradução oficial dos diálogos padrão do Qt
    const QString qtTransPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
    if (m_tradutorQt->load(QLocale(localeCode), QStringLiteral("qtbase"), QStringLiteral("_"), qtTransPath) ||
        m_tradutorQt->load(QStringLiteral("qt_") + m_idiomaAtual, qtTransPath)) {
        app->installTranslator(m_tradutorQt);
    }

    // 2. Procurar arquivo .qm do aplicativo Módulo de Dosagem de Concreto / Concrete Mix Designer (gerados pela compilacao de traducoes)
    bool appLoaded = false;
    const QString qmFileName = QStringLiteral("concrete_mix_design_%1.qm").arg(m_idiomaAtual);

    const QStringList candidatePaths = {
        QCoreApplication::applicationDirPath() + QStringLiteral("/locale/") + qmFileName,
        QCoreApplication::applicationDirPath() + QStringLiteral("/translations/") + qmFileName,
        QCoreApplication::applicationDirPath() + QStringLiteral("/") + qmFileName,
        QDir::currentPath() + QStringLiteral("/source/language/locale/") + qmFileName,
        QDir::currentPath() + QStringLiteral("/locale/") + qmFileName
    };

    for (const QString& path : candidatePaths) {
        if (QFile::exists(path) && m_tradutorApp->load(path)) {
            app->installTranslator(m_tradutorApp);
            appLoaded = true;
            break;
        }
    }

    if (!appLoaded) {
        source::utils::LogManager::warning(
            QStringLiteral("Arquivo de traducao do aplicativo nao encontrado para o idioma %1.").arg(m_idiomaAtual));
    }

    // Dispara evento de mudança de idioma global para a aplicação
    QCoreApplication::postEvent(app, new QEvent(QEvent::LanguageChange));

    return appLoaded;
}

void GerenciadorTraducao::carregarConfiguracaoIdioma() {
    const QString languageFilePath = arquivoConfiguracaoIdioma();
    if (languageFilePath.isEmpty()) {
        m_idiomaAtual = QStringLiteral("pt_BR");
        source::utils::LogManager::warning(QStringLiteral("Nao foi possivel determinar o caminho de language.json."));
        return;
    }

    QFile languageFile(languageFilePath);
    if (!languageFile.exists()) {
        m_idiomaAtual = QStringLiteral("pt_BR");
        salvarConfiguracaoIdioma();
        source::utils::LogManager::info(
            QStringLiteral("Arquivo de idioma criado com valor padrao em: %1").arg(QDir::toNativeSeparators(languageFilePath)));
        return;
    }

    if (!languageFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_idiomaAtual = QStringLiteral("pt_BR");
        source::utils::LogManager::error(
            QStringLiteral("Falha ao abrir language.json para leitura: %1").arg(QDir::toNativeSeparators(languageFilePath)));
        return;
    }

    const QByteArray fileBytes = languageFile.readAll();
    languageFile.close();

    QJsonParseError parseError;
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(fileBytes, &parseError);
    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        m_idiomaAtual = QStringLiteral("pt_BR");
        source::utils::LogManager::error(
            QStringLiteral("language.json invalido (%1). Usando idioma padrao.").arg(parseError.errorString()));
        return;
    }

    const QJsonObject jsonObject = jsonDoc.object();
    const QString codigo = jsonObject.value(QStringLiteral("language")).toString(QStringLiteral("pt_BR"));
    m_idiomaAtual = codigo;

    if (!m_idiomasDisponiveis.contains(m_idiomaAtual)) {
        source::utils::LogManager::warning(
            QStringLiteral("Idioma salvo '%1' nao suportado. Revertendo para pt_BR.").arg(m_idiomaAtual));
        m_idiomaAtual = QStringLiteral("pt_BR");
        salvarConfiguracaoIdioma();
    }
}

void GerenciadorTraducao::salvarConfiguracaoIdioma() const {
    const QString languageFilePath = arquivoConfiguracaoIdioma();
    if (languageFilePath.isEmpty()) {
        source::utils::LogManager::error(QStringLiteral("Nao foi possivel determinar o caminho para salvar language.json."));
        return;
    }

    QFile languageFile(languageFilePath);
    if (!languageFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        source::utils::LogManager::error(
            QStringLiteral("Falha ao abrir language.json para escrita: %1").arg(QDir::toNativeSeparators(languageFilePath)));
        return;
    }

    QJsonObject jsonObject;
    jsonObject.insert(QStringLiteral("language"), m_idiomaAtual);
    jsonObject.insert(QStringLiteral("updated_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

    const QJsonDocument jsonDoc(jsonObject);
    const QByteArray jsonBytes = jsonDoc.toJson(QJsonDocument::Indented);
    if (languageFile.write(jsonBytes) == -1) {
        source::utils::LogManager::error(
            QStringLiteral("Falha ao gravar conteudo de language.json: %1").arg(QDir::toNativeSeparators(languageFilePath)));
    }

    languageFile.close();
}

} // namespace ConcreteMix::Language
