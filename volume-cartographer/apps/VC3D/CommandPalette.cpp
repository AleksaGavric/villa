#include "CommandPalette.hpp"
#include "Keybinds.hpp"

#include <QAction>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QSet>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <algorithm>
#include <functional>

namespace {
QString actionLabel(const QAction* action)
{
    const QString text = action->text().section('\t', 0, 0);
    QString label;
    for (int i = 0; i < text.size(); ++i) {
        if (text[i] == QLatin1Char('&')) {
            if (i + 1 < text.size() && text[i + 1] == QLatin1Char('&')) {
                label += text[++i];
            }
        } else {
            label += text[i];
        }
    }
    return label.trimmed();
}
}

QToolButton* CommandPalette::createLauncher(QMainWindow* window, QWidget* parent)
{
    auto* commands = new QAction(tr("Search commands..."), window);
    commands->setShortcuts({
        vc3d::keybinds::sequenceFor(vc3d::keybinds::shortcuts::SearchCommands),
        vc3d::keybinds::sequenceFor(vc3d::keybinds::shortcuts::SearchCommandsAlternate)});
    window->addAction(commands);
    auto* button = new QToolButton(parent);
    button->setObjectName(QStringLiteral("commandLauncher"));
    button->setText(tr("Search commands...  %1").arg(
        commands->shortcut().toString(QKeySequence::NativeText)));
    button->setToolTip(tr("Find and run a VC3D command"));
    button->setAccessibleName(tr("Search commands"));
    connect(button, &QToolButton::clicked, commands, &QAction::trigger);
    connect(commands, &QAction::triggered, window, [window]() {
        CommandPalette palette(window);
        auto actions = window->menuBar()->actions();
        if (auto* sidebar = window->findChild<QToolBar*>(QStringLiteral("panelSidebar"))) {
            actions.append(sidebar->actions());
        }
        palette.setActions(actions);
        palette.exec();
    });
    return button;
}

CommandPalette::CommandPalette(QWidget* parent)
    : QDialog(parent)
{
    setObjectName(QStringLiteral("commandPalette"));
    setWindowTitle(tr("Search commands"));
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setModal(true);
    resize(640, 420);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    _search = new QLineEdit(this);
    _search->setObjectName(QStringLiteral("commandSearch"));
    _search->setPlaceholderText(tr("Search commands..."));
    _search->setAccessibleName(tr("Search commands"));
    _search->setClearButtonEnabled(true);
    _search->installEventFilter(this);
    layout->addWidget(_search);

    _results = new QListWidget(this);
    _results->setObjectName(QStringLiteral("commandResults"));
    _results->setAccessibleName(tr("Available commands"));
    _results->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    layout->addWidget(_results, 1);

    _empty = new QLabel(tr("No matching commands. Try a different search."), this);
    _empty->setObjectName(QStringLiteral("commandPaletteEmpty"));
    _empty->setProperty("secondary", true);
    _empty->setWordWrap(true);
    _empty->hide();
    layout->addWidget(_empty);

    auto* hint = new QLabel(tr("Up/Down to navigate   Enter to run   Esc to close"), this);
    hint->setProperty("secondary", true);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    connect(_search, &QLineEdit::textChanged, this, &CommandPalette::updateResults);
    connect(_search, &QLineEdit::returnPressed, this, &CommandPalette::activateCurrent);
    connect(_results, &QListWidget::itemActivated, this, &CommandPalette::activateCurrent);
    connect(_results, &QListWidget::itemClicked, this, &CommandPalette::activateCurrent);
    _search->setFocus();
}

void CommandPalette::setActions(const QList<QAction*>& actions)
{
    _commands.clear();
    QSet<QAction*> visited;
    std::function<void(const QList<QAction*>&, const QString&)> append =
        [&](const QList<QAction*>& entries, const QString& prefix) {
            for (auto* action : entries) {
                if (!action || visited.contains(action) || action->isSeparator() ||
                    !action->isVisible() || !action->isEnabled()) {
                    continue;
                }
                visited.insert(action);
                const QString label = actionLabel(action);
                if (label.isEmpty()) {
                    continue;
                }
                const QString title = prefix.isEmpty() ? label : prefix + QStringLiteral(" / ") + label;
                if (auto* menu = action->menu()) {
                    append(menu->actions(), title);
                } else {
                    _commands.append({action, title});
                }
            }
        };
    append(actions, {});
    updateResults();
}

void CommandPalette::updateResults()
{
    _results->clear();
    const QStringList words = _search->text().simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);
    for (int i = 0; i < _commands.size(); ++i) {
        const auto& command = _commands[i];
        if (!command.action || !command.action->isEnabled() || !command.action->isVisible()) {
            continue;
        }
        if (!std::all_of(words.cbegin(), words.cend(), [&](const QString& word) {
                return command.title.contains(word, Qt::CaseInsensitive);
            })) {
            continue;
        }
        QString text = command.title;
        const QString shortcut = command.action->shortcut().toString(QKeySequence::NativeText);
        if (!shortcut.isEmpty()) {
            text += QStringLiteral("    ") + shortcut;
        }
        if (command.action->isCheckable() && command.action->isChecked()) {
            text += tr(" (on)");
        }
        auto* item = new QListWidgetItem(command.action->icon(), text, _results);
        item->setData(Qt::UserRole, i);
        item->setToolTip(command.title);
    }
    _empty->setVisible(_results->count() == 0);
    if (_results->count() > 0) {
        _results->setCurrentRow(0);
    }
}

void CommandPalette::activateCurrent()
{
    const auto* item = _results->currentItem();
    if (!item) {
        return;
    }
    const QPointer<QAction> action = _commands[item->data(Qt::UserRole).toInt()].action;
    if (!action || !action->isEnabled() || !action->isVisible()) {
        updateResults();
        return;
    }
    accept();
    if (action) {
        action->trigger();
    }
}

bool CommandPalette::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == _search && event->type() == QEvent::KeyPress) {
        const auto* key = static_cast<QKeyEvent*>(event);
        if (key->key() == Qt::Key_Down || key->key() == Qt::Key_Up) {
            if (_results->count() > 0) {
                const int step = key->key() == Qt::Key_Down ? 1 : -1;
                _results->setCurrentRow(
                    qBound(0, _results->currentRow() + step, _results->count() - 1));
            }
            return true;
        }
    }
    return QDialog::eventFilter(watched, event);
}
