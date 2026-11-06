#include "AppTheme.hpp"
#include "CommandPalette.hpp"
#include "Keybinds.hpp"
#include "elements/ActivityIcon.hpp"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

class CopilotUiTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QVERIFY(vc3d::applyAppTheme(*qobject_cast<QApplication*>(qApp)));
    }

    void init()
    {
        QTest::failOnWarning(QRegularExpression(
            QStringLiteral(".*(Unknown property|Could not parse|Could not create pixmap).*")));
    }

    void collectsMenuCommands()
    {
        QMenu file(QStringLiteral("&File"));
        auto* open = file.addAction(QStringLiteral("&Open Project..."));
        open->setShortcut(QKeySequence::Open);
        file.addSeparator();
        file.addAction(QStringLiteral("Disabled"))->setEnabled(false);
        file.addAction(QStringLiteral("Hidden"))->setVisible(false);
        auto* submenu = file.addMenu(QStringLiteral("&Export"));
        submenu->addAction(QStringLiteral("Save && Close"));
        auto* unavailable = file.addMenu(QStringLiteral("Unavailable"));
        unavailable->addAction(QStringLiteral("Must not appear"));
        unavailable->setEnabled(false);

        CommandPalette palette;
        palette.setActions({file.menuAction(), open});
        auto* results = palette.findChild<QListWidget*>("commandResults");
        QCOMPARE(results->count(), 2);
        QVERIFY(results->item(0)->text().startsWith(QStringLiteral("File / Open Project...")));
        QVERIFY(results->item(0)->text().contains(open->shortcut().toString(QKeySequence::NativeText)));
        QCOMPARE(results->item(1)->text(), QStringLiteral("File / Export / Save & Close"));
    }

    void launchesFromButtonAndShortcuts_data()
    {
        QTest::addColumn<int>("method");
        QTest::newRow("button") << 0;
        QTest::newRow("Ctrl+K") << 1;
        QTest::newRow("Ctrl+Shift+P") << 2;
    }

    void launchesFromButtonAndShortcuts()
    {
        QFETCH(int, method);
        QMainWindow window;
        auto* panel = new QWidget(&window);
        auto* layout = new QVBoxLayout(panel);
        auto* launcher = CommandPalette::createLauncher(&window, panel);
        layout->addWidget(launcher);
        window.setCentralWidget(panel);
        window.menuBar()->addMenu(QStringLiteral("File"))->addAction(QStringLiteral("Open Project"));
        auto* sidebar = new QToolBar(&window);
        sidebar->setObjectName(QStringLiteral("panelSidebar"));
        sidebar->addAction(QStringLiteral("Segment"));
        window.addToolBar(Qt::LeftToolBarArea, sidebar);
        window.show();
        window.activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(&window));
        bool opened = false;
        int commandCount = 0;
        QTimer::singleShot(0, &window, [&]() {
            if (auto* palette = window.findChild<QDialog*>("commandPalette")) {
                opened = palette->isVisible();
                commandCount = palette->findChild<QListWidget*>("commandResults")->count();
                palette->reject();
            }
        });
        if (method == 0) {
            QTest::mouseClick(launcher, Qt::LeftButton);
        } else if (method == 1) {
            QTest::keySequence(panel, vc3d::keybinds::sequenceFor(
                vc3d::keybinds::shortcuts::SearchCommands));
        } else {
            QTest::keySequence(panel, vc3d::keybinds::sequenceFor(
                vc3d::keybinds::shortcuts::SearchCommandsAlternate));
        }
        QCoreApplication::processEvents();
        QVERIFY(opened);
        QCOMPARE(commandCount, 2);
        QVERIFY(vc3d::keybinds::buildKeybindsHelpText().contains(QStringLiteral("Search commands")));
    }

    void filtersAllSearchWords()
    {
        QAction open(QStringLiteral("Open Project"));
        QAction save(QStringLiteral("Save Project"));
        CommandPalette palette;
        palette.setActions({&open, &save});
        auto* search = palette.findChild<QLineEdit*>("commandSearch");
        auto* results = palette.findChild<QListWidget*>("commandResults");
        search->setText(QStringLiteral("  PROJECT    oPeN "));
        QCOMPARE(results->count(), 1);
        QCOMPARE(results->item(0)->text(), QStringLiteral("Open Project"));
        search->clear();
        QCOMPARE(results->count(), 2);
    }

    void navigatesAndTriggersFromSearch()
    {
        QAction first(QStringLiteral("First"));
        QAction second(QStringLiteral("Second"));
        QSignalSpy firstTriggered(&first, &QAction::triggered);
        QSignalSpy secondTriggered(&second, &QAction::triggered);
        CommandPalette palette;
        palette.setActions({&first, &second});
        palette.show();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));
        auto* search = palette.findChild<QLineEdit*>("commandSearch");
        auto* results = palette.findChild<QListWidget*>("commandResults");
        QVERIFY(search->hasFocus());
        QCOMPARE(results->currentRow(), 0);
        QTest::keyClick(search, Qt::Key_Up);
        QCOMPARE(results->currentRow(), 0);
        QTest::keyClick(search, Qt::Key_Down);
        QCOMPARE(results->currentRow(), 1);
        QTest::keyClick(search, Qt::Key_Down);
        QCOMPARE(results->currentRow(), 1);
        QVERIFY(search->hasFocus());
        QTest::keyClick(search, Qt::Key_Return);
        QCOMPARE(firstTriggered.count(), 0);
        QCOMPARE(secondTriggered.count(), 1);
        QCOMPARE(palette.result(), int(QDialog::Accepted));
        QVERIFY(!palette.isVisible());
    }

    void togglesCheckableCommandOnClick()
    {
        QAction toggle(QStringLiteral("Project panel"));
        toggle.setCheckable(true);
        CommandPalette palette;
        palette.setActions({&toggle});
        palette.show();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));
        auto* results = palette.findChild<QListWidget*>("commandResults");
        QSignalSpy triggered(&toggle, &QAction::triggered);
        QTest::mouseClick(results->viewport(), Qt::LeftButton, Qt::NoModifier,
                          results->visualItemRect(results->item(0)).center());
        QCOMPARE(triggered.count(), 1);
        QVERIFY(toggle.isChecked());
        QCOMPARE(palette.result(), int(QDialog::Accepted));
    }

    void emptySearchDoesNotRunCommand()
    {
        QAction action(QStringLiteral("Open"));
        QSignalSpy triggered(&action, &QAction::triggered);
        CommandPalette palette;
        palette.setActions({&action});
        palette.show();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));
        auto* search = palette.findChild<QLineEdit*>("commandSearch");
        search->setText(QStringLiteral("no such command"));
        QCOMPARE(palette.findChild<QListWidget*>("commandResults")->count(), 0);
        QVERIFY(palette.findChild<QLabel*>("commandPaletteEmpty")->isVisible());
        QTest::keyClick(search, Qt::Key_Return);
        QCOMPARE(triggered.count(), 0);
        QVERIFY(palette.isVisible());
        QTest::keyClick(search, Qt::Key_Escape);
        QCOMPARE(palette.result(), int(QDialog::Rejected));
        QVERIFY(!palette.isVisible());
    }

    void staleCommandsCannotRun_data()
    {
        QTest::addColumn<int>("change");
        QTest::newRow("deleted") << 0;
        QTest::newRow("disabled") << 1;
        QTest::newRow("hidden") << 2;
    }

    void staleCommandsCannotRun()
    {
        QFETCH(int, change);
        auto* action = new QAction(QStringLiteral("Open"), this);
        QSignalSpy triggered(action, &QAction::triggered);
        CommandPalette palette;
        palette.setActions({action});
        palette.show();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));
        if (change == 0) {
            delete action;
        } else if (change == 1) {
            action->setEnabled(false);
        } else {
            action->setVisible(false);
        }
        QTest::keyClick(palette.findChild<QLineEdit*>("commandSearch"), Qt::Key_Return);
        QCOMPARE(triggered.count(), 0);
        QCOMPARE(palette.findChild<QListWidget*>("commandResults")->count(), 0);
        QVERIFY(palette.isVisible());
        if (change != 0) {
            delete action;
        }
    }

    void themeUsesDarkPaletteAndRenders()
    {
        QCOMPARE(qApp->palette().color(QPalette::Window), QColor("#0d1117"));
        QCOMPARE(qApp->palette().color(QPalette::Text), QColor("#e6edf3"));
        QCOMPARE(qApp->palette().color(QPalette::Highlight), QColor("#1f6feb"));
        QWidget panel;
        auto* layout = new QVBoxLayout(&panel);
        layout->addWidget(new QLineEdit(QStringLiteral("Project"), &panel));
        auto* button = new QPushButton(QStringLiteral("New Project"), &panel);
        button->setProperty("accent", true);
        layout->addWidget(button);
        layout->addWidget(new QCheckBox(QStringLiteral("Show surface"), &panel));
        panel.resize(420, 200);
        panel.show();
        QVERIFY(QTest::qWaitForWindowExposed(&panel));
        const QImage image = panel.grab().toImage();
        QVERIFY(!image.isNull());
        QCOMPARE(image.pixelColor(0, 0), QColor("#0d1117"));
        const QPoint buttonPixel = button->mapTo(&panel, QPoint(10, button->height() / 2));
        QCOMPARE(image.pixelColor(buttonPixel), QColor("#238636"));
    }

    void activityIconsHaveDistinctStates()
    {
        for (const auto kind : {vc3d::ActivityIcon::Project, vc3d::ActivityIcon::Segment,
                                vc3d::ActivityIcon::View, vc3d::ActivityIcon::Display,
                                vc3d::ActivityIcon::Ink, vc3d::ActivityIcon::Unwrap}) {
            const QIcon icon = vc3d::activityIcon(kind);
            QVERIFY(!icon.isNull());
            const QImage normal = icon.pixmap(24, 24, QIcon::Normal, QIcon::Off).toImage();
            const QImage active = icon.pixmap(24, 24, QIcon::Normal, QIcon::On).toImage();
            const QImage disabled = icon.pixmap(24, 24, QIcon::Disabled, QIcon::Off).toImage();
            QVERIFY(normal != active);
            QVERIFY(normal != disabled);
            QCOMPARE(icon.pixmap(48, 48).size(), QSize(48, 48));
        }
    }
};

QTEST_MAIN(CopilotUiTest)
#include "test_copilot_ui.moc"
