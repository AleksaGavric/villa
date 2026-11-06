#include "AppTheme.hpp"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QPalette>
#include <QStyleFactory>

bool vc3d::applyAppTheme(QApplication& app)
{
    QFile theme(QStringLiteral(":/themes/vc3d-modern.qss"));
    if (!theme.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot load VC3D theme:" << theme.errorString();
        return false;
    }

    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#0d1117"));
    palette.setColor(QPalette::WindowText, QColor("#e6edf3"));
    palette.setColor(QPalette::Base, QColor("#0d1117"));
    palette.setColor(QPalette::AlternateBase, QColor("#161b22"));
    palette.setColor(QPalette::Text, QColor("#e6edf3"));
    palette.setColor(QPalette::Button, QColor("#21262d"));
    palette.setColor(QPalette::ButtonText, QColor("#e6edf3"));
    palette.setColor(QPalette::Mid, QColor("#30363d"));
    palette.setColor(QPalette::Light, QColor("#484f58"));
    palette.setColor(QPalette::Dark, QColor("#010409"));
    palette.setColor(QPalette::Highlight, QColor("#1f6feb"));
    palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    palette.setColor(QPalette::Link, QColor("#58a6ff"));
    palette.setColor(QPalette::LinkVisited, QColor("#bc8cff"));
    palette.setColor(QPalette::PlaceholderText, QColor("#8b949e"));
    palette.setColor(QPalette::ToolTipBase, QColor("#161b22"));
    palette.setColor(QPalette::ToolTipText, QColor("#e6edf3"));
    for (const auto role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText}) {
        palette.setColor(QPalette::Disabled, role, QColor("#8b949e"));
    }
    app.setPalette(palette);
    app.setStyleSheet(QString::fromUtf8(theme.readAll()));
    return true;
}
