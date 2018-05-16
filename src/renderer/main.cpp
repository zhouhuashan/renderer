#include "renderitem.hpp"

#include "qmlinstance.hpp"
#include "clipboard.hpp"

#include <QtQuickControls2>

Q_DECLARE_LOGGING_CATEGORY(rendererCategory)
Q_LOGGING_CATEGORY(rendererCategory, "renderer")

int main(int argc, char * argv [])
{
    if (!qputenv("QT_FATAL_CRITICALS", QByteArrayLiteral("1"))) {
        qFatal("unable to set QT_FATAL_CRITICALS to 1");
    }
    if (qEnvironmentVariableIsEmpty("QT_IM_MODULE")) {
        if (!qputenv("QT_IM_MODULE", QByteArrayLiteral("qtvirtualkeyboard"))) {
            qFatal("unable to set QT_IM_MODULE to qtvirtualkeyboard");
        }
    }

    {
        // The pattern can also be changed at runtime by setting the QT_MESSAGE_PATTERN environment variable;
        // if both qSetMessagePattern() is called and QT_MESSAGE_PATTERN is set, the environment variable takes precedence.
        QString messagePattern;
#ifndef QT_DEBUG
        messagePattern += "[%{time yyyy/MM/dd dddd HH:mm:ss.zzz t}] ";
#endif
        messagePattern += "%{type} %{category}: %{message} (%{file}:%{line}";
#ifndef QT_DEBUG
        messagePattern += " %{function}";
#endif
        messagePattern += ")";
#if !defined(QT_DEBUG) && defined(__GLIBC__)
        messagePattern += "%{if-fatal}\n%{backtrace}%{endif}";
#endif
        qSetMessagePattern(messagePattern);
    }

    QSettings::setDefaultFormat(QSettings::IniFormat);

    QCoreApplication::setOrganizationName(ORGANIZATION_SHORTNAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(PROJECT_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication application{argc, argv};

    qmlRegisterSingletonType< Clipboard >("Clipboard", 1, 0, "Clipboard", &instance< Clipboard >);
    qmlRegisterType< RendererInterface >("Renderer", 1, 0, "RendererInterface");
    qmlRegisterType< CameraLens >("Renderer", 1, 0, "CameraLens");
    qmlRegisterType< Camera >("Renderer", 1, 0, "Camera");
    qmlRegisterType< RenderItem >("Renderer", 1, 0, "RenderItem");

    //qDebug() << QQuickStyle::availableStyles();

    QQmlApplicationEngine engine;

    //const auto rootContext = engine.rootContext();
    //rootContext->setContextProperty("localHostName", QHostInfo::localHostName());
    //rootContext->setContextProperty("platformName", QGuiApplication::platformName());

    engine.addImportPath(":/qml/imports");
    engine.addPluginPath(QCoreApplication::applicationDirPath() + "../plugins/");

    const auto onObjectCreated = [&] (QObject * const object, QUrl url)
    {
        if (object) {
            qCInfo(rendererCategory)
                    << QStringLiteral("Object from URL %1 was successfully created")
                       .arg(url.toString());
        } else {
            qCCritical(rendererCategory)
                    << QStringLiteral("Unable to create object from URL %1")
                       .arg(url.toString());
            QTimer::singleShot(0, qApp, [] { QCoreApplication::exit(EXIT_FAILURE); });
        }
    };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, qApp, onObjectCreated);
    {
        QUrl baseUrl{"qrc:///qml/"};
        engine.setBaseUrl(baseUrl);
        QUrl url{"ui.qml"};
        engine.load(url);
    }
    qCInfo(rendererCategory)
            << QStringLiteral("QML engine selectors: %1")
               .arg(QQmlFileSelector::get(&engine)->selector()->allSelectors().join(", "));
#if 0
    const auto topLevelWindows = QGuiApplication::topLevelWindows();
    if (!topLevelWindows.isEmpty()) {
        const auto root = topLevelWindows.first();
        root->setObjectName(QGuiApplication::applicationDisplayName());
        //QMetaObject::invokeMethod(renderItem->rootObject(), "performLayerBasedGrab", Q_ARG(QVariant, QStringLiteral("~/screenshot.png")));
    }
#endif
    return application.exec();
}
