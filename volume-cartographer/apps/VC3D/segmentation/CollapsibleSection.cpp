// Simple disclosure section for workflow grouping.

#include "CollapsibleSection.hpp"

#include <QToolButton>
#include <QVBoxLayout>

CollapsibleSection::CollapsibleSection(const QString& title,
                                       QWidget* parent,
                                       bool expanded)
    : QWidget(parent)
    , _title(title)
    , _expanded(expanded)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    _toggle = new QToolButton(this);
    _toggle->setProperty("sectionHeader", true);
    _toggle->setCheckable(true);
    _toggle->setChecked(expanded);
    _toggle->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _toggle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    updateToggleText();
    connect(_toggle, &QToolButton::toggled, this, &CollapsibleSection::setExpanded);
    layout->addWidget(_toggle);

    _content = new QWidget(this);
    _contentLayout = new QVBoxLayout(_content);
    _contentLayout->setContentsMargins(0, 4, 0, 0);
    _contentLayout->setSpacing(12);
    _content->setVisible(expanded);
    layout->addWidget(_content);
}

void CollapsibleSection::setExpanded(bool expanded)
{
    _expanded = expanded;
    _content->setVisible(expanded);
    if (_toggle->isChecked() != expanded) {
        _toggle->setChecked(expanded);
    }
    updateToggleText();
}

void CollapsibleSection::updateToggleText()
{
    // Chevron indicates state; no icon files needed.
    _toggle->setText((_expanded ? QStringLiteral("▼ ") : QStringLiteral("▶ ")) + _title);
    _toggle->setArrowType(_expanded ? Qt::DownArrow : Qt::RightArrow);
}
