#include "include/UiTsys/defaultTypes.h"
#include "UiRes/uires.h"
#include "Tsys/defaultTypes.h"

#include <QDialog>
#include <QTextEdit>
#include <QTextBlock>

#include <iostream>


QWidget* UiTSys::DefaultWidget::CreateTypedWidget()
{
    QFrame* frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFixedHeight(1);
    return frame;
}


void UiTSys::DefaultWidget::SetValue(std::any value)
{

}


std::any UiTSys::DefaultWidget::GetValue() const
{
    return {};
}


UiTSys::TypedWidget* UiTSys::DefaultWidgetCreator::CreateWidget() const
{
    return new DefaultWidget();
}


#define WITH_WIDGET_BLOCKED(widget, code) widget->blockSignals(true);widget->code;widget->blockSignals(false);


QWidget* UiTSys::BoolWidget::CreateTypedWidget()
{
    widget = new QCheckBox(this);
//    QObject::connect(widget, &QCheckBox::stateChanged,
//                     this, &UiTSys::BoolWidget::OnValueChanged);
    Connect(widget, &QCheckBox::stateChanged);
    return widget;
}


void UiTSys::BoolWidget::SetValue(std::any value)
{
    int castedvalue = std::any_cast<bool>(value);
    WITH_WIDGET_BLOCKED(widget, setChecked(castedvalue));
}


std::any UiTSys::BoolWidget::GetValue() const
{
    return std::make_any<bool>(widget->isChecked());
}


void UiTSys::BoolWidget::ShowConnected(bool connected)
{
    if (connected)
        widget->setStyleSheet("QCheckBox{background-color: #9A7000;}");
    else
        widget->setStyleSheet("");

    widget->setDisabled(connected);
}


void UiTSys::BoolWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}


UiTSys::TypedWidget* UiTSys::BoolWidgetCreator::CreateWidget() const
{
    return new BoolWidget();
}


QWidget* UiTSys::BoolInitWidget::CreateInitWidget()
{
    widget = new QCheckBox(this);
    return widget;
}


std::any UiTSys::BoolInitWidget::CreateValue()
{
    return std::make_any<bool>(widget->isChecked());
}



UiTSys::TypedInitWidget* UiTSys::BoolInitWidgetCreator::CreateWidget() const
{
    return new BoolInitWidget();
}









QWidget* UiTSys::IntWidget::CreateTypedWidget()
{
    widget = new QSpinBox(this);
    widget->setMinimum(-9999999);
    widget->setMaximum(9999999);
    widget->setButtonSymbols(QAbstractSpinBox::NoButtons);
    Connect(widget, &QSpinBox::editingFinished);
    return widget;
}


void UiTSys::IntWidget::SetValue(std::any value)
{
    int castedvalue = std::any_cast<int>(value);
    WITH_WIDGET_BLOCKED(widget, setValue(castedvalue));
}


std::any UiTSys::IntWidget::GetValue() const
{
    return std::make_any<int>(widget->value());
}


void UiTSys::IntWidget::ShowConnected(bool connected)
{
    if (connected)
        widget->setStyleSheet("QSpinBox{background-color: #9A7000;}");
    else
        widget->setStyleSheet("");

    widget->setReadOnly(connected);
}


void UiTSys::IntWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}


UiTSys::TypedWidget* UiTSys::IntWidgetCreator::CreateWidget() const
{
    return new IntWidget();
}




QWidget* UiTSys::IntInitWidget::CreateInitWidget()
{
    widget = new QSpinBox(this);
    widget->setMinimum(-9999999);
    widget->setMaximum(9999999);
    widget->setButtonSymbols(QAbstractSpinBox::NoButtons);
    return widget;
}


std::any UiTSys::IntInitWidget::CreateValue()
{
    return std::make_any<int>(widget->value());
}




UiTSys::TypedInitWidget* UiTSys::IntInitWidgetCreator::CreateWidget() const
{
    return new IntInitWidget();
}



QWidget* UiTSys::FloatWidget::CreateTypedWidget()
{
    widget = new QDoubleSpinBox(this);
    widget->setMinimum(-9999999.9);
    widget->setMaximum(9999999.9);
    widget->setButtonSymbols(QAbstractSpinBox::NoButtons);
//    QObject::connect(widget, &QDoubleSpinBox::valueChanged,
//                     this, &UiTSys::FloatWidget::OnValueChanged);

    Connect(widget, &QDoubleSpinBox::editingFinished);
    return widget;
}


void UiTSys::FloatWidget::SetValue(std::any value)
{
    WITH_WIDGET_BLOCKED(widget, setValue(std::any_cast<float>(value)));
}


std::any UiTSys::FloatWidget::GetValue() const
{
    return std::make_any<float>((float)widget->value());
}


void UiTSys::FloatWidget::ShowConnected(bool connected)
{
    if (connected)
        widget->setStyleSheet("QDoubleSpinBox{background-color: #9A7000;}");
    else
        widget->setStyleSheet("");

    widget->setReadOnly(connected);
}


void UiTSys::FloatWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}


UiTSys::TypedWidget* UiTSys::FloatWidgetCreator::CreateWidget() const
{
    return new FloatWidget();
}


QWidget* UiTSys::FloatInitWidget::CreateInitWidget()
{
    widget = new QDoubleSpinBox(this);
    widget->setMinimum(-9999999.9);
    widget->setMaximum(9999999.9);
    widget->setButtonSymbols(QAbstractSpinBox::NoButtons);
    return widget;
}


std::any UiTSys::FloatInitWidget::CreateValue()
{
    return std::make_any<float>(widget->value());
}




UiTSys::TypedInitWidget* UiTSys::FloatInitWidgetCreator::CreateWidget() const
{
    return new FloatInitWidget();
}










QWidget* UiTSys::DoubleWidget::CreateTypedWidget()
{
    widget = new QDoubleSpinBox(this);
    widget->setMinimum(-9999999.9);
    widget->setMaximum(9999999.9);
    widget->setButtonSymbols(QAbstractSpinBox::NoButtons);
//    QObject::connect(widget, &QDoubleSpinBox::valueChanged,
//                     this, &UiTSys::DoubleWidget::OnValueChanged);
    Connect(widget, &QDoubleSpinBox::editingFinished);
    return widget;
}


void UiTSys::DoubleWidget::SetValue(std::any value)
{
    WITH_WIDGET_BLOCKED(widget, setValue(std::any_cast<double>(value)));
}


std::any UiTSys::DoubleWidget::GetValue() const
{
    return std::make_any<double>(widget->value());
}


void UiTSys::DoubleWidget::ShowConnected(bool connected)
{
    if (connected)
        widget->setStyleSheet("QDoubleSpinBox{background-color: #9A7000;}");
    else
        widget->setStyleSheet("");

    widget->setReadOnly(connected);
}


void UiTSys::DoubleWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}


UiTSys::TypedWidget* UiTSys::DoubleWidgetCreator::CreateWidget() const
{
    return new DoubleWidget();
}


QWidget* UiTSys::DoubleInitWidget::CreateInitWidget()
{
    widget = new QDoubleSpinBox(this);
    widget->setMinimum(-9999999.9);
    widget->setMaximum(9999999.9);
    widget->setButtonSymbols(QAbstractSpinBox::NoButtons);
    return widget;
}


std::any UiTSys::DoubleInitWidget::CreateValue()
{
    return std::make_any<double>((double)widget->value());
}




UiTSys::TypedInitWidget* UiTSys::DoubleInitWidgetCreator::CreateWidget() const
{
    return new DoubleInitWidget();
}




QWidget* UiTSys::StringWidget::CreateTypedWidget()
{
    setFixedHeight(15);

    document = new QTextDocument(this);

    mainWidget = new QWidget(this);
    mainWidget->setFixedHeight(15);

    auto* mainLayout = new QHBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setAlignment(Qt::AlignRight);

    textEdit = new TextEdit(this);
    textEdit->setDocument(document);
    textEdit->setFixedHeight(15);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setSizePolicy(
        QSizePolicy(QSizePolicy::Ignored,
                    QSizePolicy::Fixed)
    );
    mainLayout->addWidget(textEdit);

    Connect(document, &QTextDocument::contentsChanged);

    editButton = new QPushButton(this);
    editButton->setFixedSize(QSize(15, 15));
    editButton->setIcon(
            Res::UiRes::GetRes()->GetQtAwesome()->icon(
                fa::fa_solid, fa::fa_external_link
            )
    );
    mainLayout->addWidget(editButton, Qt::AlignRight);

    QObject::connect(editButton, &QPushButton::clicked, this,
                     &StringWidget::EditText);

    return mainWidget;
}


void UiTSys::StringWidget::SetValue(std::any value)
{
    WITH_WIDGET_BLOCKED(document, setPlainText(std::any_cast<std::string>(value).c_str()));
}


std::any UiTSys::StringWidget::GetValue() const
{
    return std::make_any<std::string>(document->toPlainText().toStdString());
}


void UiTSys::StringWidget::ShowConnected(bool connected)
{
    editButton->setDisabled(connected);
}


void UiTSys::StringWidget::ShowDisabled(bool disabled)
{
    mainWidget->setDisabled(disabled);
}


QString ToMultiLineText(QTextEdit* edit)
{
    return edit->document()->toPlainText();
}


void UiTSys::StringWidget::EditText()
{
    QDialog* editDialog = new QDialog(this);
    editDialog->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    editDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    QVBoxLayout* layout = new QVBoxLayout(editDialog);

    QTextEdit* textEdit = new QTextEdit(editDialog);
    textEdit->setDocument(document);
    layout->addWidget(textEdit);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(buttonsLayout);

    QPushButton* acceptButton = new QPushButton(editDialog);
    acceptButton->setText("Save");
    buttonsLayout->addWidget(acceptButton);

    QPushButton* rejectButton = new QPushButton(editDialog);
    rejectButton->setText("Cancel");
    buttonsLayout->addWidget(rejectButton);

    editDialog->show();

    QObject::connect(acceptButton, &QPushButton::clicked,
                     this, [this, textEdit]()
                     {this->AcceptEdit(ToMultiLineText(textEdit));});

    QObject::connect(acceptButton, &QPushButton::clicked,
                     editDialog, &QDialog::accept);

    QObject::connect(rejectButton, &QPushButton::clicked,
                     editDialog, &QDialog::reject);
}


void UiTSys::StringWidget::AcceptEdit(QString text)
{
    document->setPlainText(text);
    OnValueChanged();  // Force property edition.
}


UiTSys::TypedWidget* UiTSys::StringWidgetCreator::CreateWidget() const
{
    return new StringWidget();
}



QWidget* UiTSys::StringInitWidget::CreateInitWidget()
{
    widget = new QLineEdit(this);
    return widget;
}


std::any UiTSys::StringInitWidget::CreateValue()
{
    return std::make_any<std::string>(widget->text().toStdString());
}




UiTSys::TypedInitWidget* UiTSys::StringInitWidgetCreator::CreateWidget() const
{
    return new StringInitWidget();
}







QWidget* UiTSys::EnumWidget::CreateTypedWidget()
{
    widget = new QComboBox(this);

//    QObject::connect(widget, &QComboBox::currentIndexChanged,
//                     this, &UiTSys::EnumWidget::OnValueChanged);
    Connect(widget, &QComboBox::currentIndexChanged);
    return widget;
}


void UiTSys::EnumWidget::SetValue(std::any value)
{
    widget->blockSignals(true);
    widget->clear();
    TSys::Enum enum_ = std::any_cast<TSys::Enum>(value);

    for (int idx : enum_.Indices())
    {
        std::string value_ = enum_.ValueAtIndex(idx);
        widget->addItem(value_.c_str());
    }

    widget->setCurrentIndex(enum_.CurrentIndex());

    widget->blockSignals(false);
}


std::any UiTSys::EnumWidget::GetValue() const
{
    TSys::Enum enum_;
    for (int i = 0; i < widget->count(); i++)
    {
        std::string val = widget->itemText(i).toStdString();
        enum_.AddValue(i, val);
    }

    enum_.SetCurrentIndex(widget->currentIndex());

    return std::make_any<TSys::Enum>(enum_);
}


void UiTSys::EnumWidget::ShowConnected(bool connected)
{
    if (connected)
        widget->setStyleSheet("QComboBox{background-color: #9A7000;}");
    else
        widget->setStyleSheet("");
}


void UiTSys::EnumWidget::ShowDisabled(bool disabled)
{
    widget->setDisabled(disabled);
}


UiTSys::TypedWidget* UiTSys::EnumWidgetCreator::CreateWidget() const
{
    return new EnumWidget();
}


std::any UiTSys::EnumInitWidget::CreateValue()
{
    TSys::Enum enum_;
    for (unsigned int i = 0; i < combobox->count(); i++)
    {
        enum_.AddValue(i, combobox->itemText(i).toStdString());
    }

    enum_.SetCurrentIndex(combobox->currentIndex());

    return std::make_any<TSys::Enum>(enum_);
}


QWidget* UiTSys::EnumInitWidget::CreateInitWidget()
{
    group = new QWidget();
    auto* layout = new QVBoxLayout();
    group->setLayout(layout);

    QHBoxLayout* valueLayout = new QHBoxLayout();
    valueLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(valueLayout);

    combobox = new QComboBox(group);
    valueLayout->addWidget(combobox);

    rmbutton = new QPushButton(group);
    rmbutton->setFixedSize(15, 15);
    rmbutton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_minus));
    valueLayout->addWidget(rmbutton);

    QHBoxLayout* newValueLayout = new QHBoxLayout();
    newValueLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(newValueLayout);

    lineedit = new QLineEdit(group);
    lineedit->setPlaceholderText("New value");
    newValueLayout->addWidget(lineedit);

    addbutton = new QPushButton(group);
    addbutton->setFixedSize(15, 15);
    addbutton->setIcon(Res::UiRes::GetRes()->GetQtAwesome()->icon(fa::fa_solid, fa::fa_plus));
    newValueLayout->addWidget(addbutton);

    QObject::connect(addbutton, &QPushButton::clicked,
                     this, &EnumInitWidget::AddFieldValue);
    QObject::connect(rmbutton, &QPushButton::clicked,
                     this, &EnumInitWidget::RemoveCurrentValue);

    return group;
}


void UiTSys::EnumInitWidget::AddFieldValue()
{
    QString text = lineedit->text();
    AddValue(text);
    lineedit->clear();
}


void UiTSys::EnumInitWidget::AddValue(QString text)
{
    combobox->addItem(text);
}


void UiTSys::EnumInitWidget::RemoveCurrentValue()
{
    if (!combobox->count())
        return;

    combobox->removeItem(combobox->currentIndex());
}




UiTSys::TypedInitWidget* UiTSys::EnumInitWidgetCreator::CreateWidget() const
{
    return new EnumInitWidget();
}




void UiTSys::RegisterDefaultTypedWidgets()
{

}