// Welcome screen: recent projects + New / Open actions.

#include "WelcomeDialog.hpp"

#include <QEasingCurve>
#include <QFileInfo>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include "CWindow.hpp"
#include "VCSettings.hpp"

WelcomeDialog::WelcomeDialog(CWindow* window, QWidget* parent)
    : QDialog(parent)
    , _window(window)
{
    setWindowTitle(tr("Welcome to VC3D"));
    setMinimumSize(460, 520);
    setModal(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(36, 32, 36, 32);
    layout->setSpacing(12);

    auto* logoLabel = new QLabel(this);
    const QIcon logo(QStringLiteral(":/images/logo.png"));
    logoLabel->setPixmap(logo.pixmap(72, 72));
    logoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(logoLabel);

    auto* title = new QLabel(tr("VC3D"), this);
    title->setAlignment(Qt::AlignCenter);
    QFont titleFont = title->font();
    titleFont.setPointSize(28);
    titleFont.setWeight(QFont::DemiBold);
    title->setFont(titleFont);
    layout->addWidget(title);

    auto* subtitle = new QLabel(tr("Volumetric segmentation for the Vesuvius Challenge"), this);
    subtitle->setProperty("secondary", true);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);

    layout->addSpacing(8);

    auto* newBtn = new QPushButton(tr("New Project"), this);
    newBtn->setProperty("accent", true);
    newBtn->setMinimumHeight(40);
    connect(newBtn, &QPushButton::clicked, this, [this]() {
        if (_window->newProjectDialog()) {
            accept();
        }
    });
    layout->addWidget(newBtn);

    auto* openBtn = new QPushButton(tr("Open Project…"), this);
    openBtn->setMinimumHeight(40);
    connect(openBtn, &QPushButton::clicked, this, [this]() {
        if (_window->openProjectDialog()) {
            accept();
        }
    });
    layout->addWidget(openBtn);

    auto* catalogBtn = new QPushButton(tr("Browse Open Data…"), this);
    catalogBtn->setMinimumHeight(40);
    connect(catalogBtn, &QPushButton::clicked, this, [this]() {
        _window->showOpenDataCatalog();
        accept();
    });
    layout->addWidget(catalogBtn);

    layout->addSpacing(8);

    auto* recentLabel = new QLabel(tr("Recent Projects"), this);
    recentLabel->setProperty("secondary", true);
    layout->addWidget(recentLabel);

    auto* recentList = new QListWidget(this);
    recentList->setMinimumHeight(160);
    QSettings settings(vc3d::settingsFilePath(), QSettings::IniFormat);
    const QStringList recent =
        settings.value(vc3d::settings::project::RECENT).toStringList();
    for (const QString& path : recent) {
        if (path.isEmpty()) {
            continue;
        }
        QFileInfo info(path);
        auto* item = new QListWidgetItem(recentList);
        item->setText(info.fileName() + QStringLiteral("\n") + info.absolutePath());
        item->setData(Qt::UserRole, path);
    }
    if (recentList->count() == 0) {
        auto* item = new QListWidgetItem(tr("No recent projects"), recentList);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
    }
    connect(recentList, &QListWidget::itemActivated, this,
            [this](QListWidgetItem* item) { openRecent(item->data(Qt::UserRole).toString()); });
    connect(recentList, &QListWidget::itemClicked, this,
            [this](QListWidgetItem* item) { openRecent(item->data(Qt::UserRole).toString()); });
    layout->addWidget(recentList, 1);

    // Both click and double-click/Enter open immediately; a single click is
    // the common case on a welcome screen.

    // Subtle Apple-like entrance: fade + rise.
    setWindowOpacity(0.0);
    auto* fade = new QPropertyAnimation(this, "windowOpacity", this);
    fade->setDuration(320);
    fade->setStartValue(0.0);
    fade->setEndValue(1.0);
    fade->setEasingCurve(QEasingCurve::OutCubic);
    fade->start(QPropertyAnimation::DeleteWhenStopped);
}

void WelcomeDialog::openRecent(const QString& path)
{
    if (path.isEmpty() || !_window) {
        return;
    }
    if (_window->openVolumePackage(path)) {
        accept();
    }
}
