#include <QApplication>
#include <QIcon>
#include <QStyleFactory>
#include "source/gui/MainWindow.hpp"
#include "source/language/GerenciadorTraducao.hpp"
#include "source/utils/LogManager.hpp"

#if defined(Q_OS_WIN)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#endif

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    // Garante que o ícone na barra de tarefas do Windows seja exibido de forma independente e destacada
    SetCurrentProcessExplicitAppUserModelID(L"ConcreteMixDesign.CivilEngineering.IPTMethod.2026.10");
#endif

    QApplication app(argc, argv);

    // Configurações institucionais do aplicativo
    QApplication::setApplicationName("ConcreteMixDesigner");
    QApplication::setOrganizationName("EngenhariaCivil");
QApplication::setApplicationVersion("2026.11.6.0");

    source::utils::LogManager::configure_logging();
    source::utils::LogManager::info(QStringLiteral("Inicializando ConcreteMixDesigner."));

    // Vincula o ícone do aplicativo (recurso embarcado ou arquivo local)
    QIcon appIcon(":/icons/concrete_mix_design.ico");
    if (appIcon.isNull()) {
        appIcon = QIcon("source/assets/icons/concrete_mix_design.ico");
    }
    app.setWindowIcon(appIcon);

    // Utiliza o estilo nativo padrão do Qt6 no Windows 11 ("windows11")
    // com suporte nativo e automático a modo claro e escuro em tempo de execução.
    if (QStyleFactory::keys().contains("windows11", Qt::CaseInsensitive)) {
        app.setStyle(QStyleFactory::create("windows11"));
    }

    ConcreteMix::Language::GerenciadorTraducao::instance().inicializar();

    ConcreteMix::MainWindow mainWindow;
    mainWindow.setWindowIcon(appIcon);
    mainWindow.show();

    return app.exec();
}
