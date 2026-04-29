#ifndef NODELIBRARY2COMPLETE_TYPEWIDGETS_H
#define NODELIBRARY2COMPLETE_TYPEWIDGETS_H

#include "uitsys.h"

#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>


namespace UiTSys
{
    class DefaultWidget: public TypedWidget
    {
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override {};

        void ShowDisabled(bool disabled) override {};
    };

    class DefaultWidgetCreator: public TypedWidgetCreator
    {
    public:
        TypedWidget* CreateWidget() const override;
    };


    class BoolWidget: public TypedWidget
    {
        QCheckBox* widget;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    class BoolWidgetCreator: public TypedWidgetCreator
    {
    public:
        TypedWidget* CreateWidget() const override;
    };


    class BoolInitWidget: public TypedInitWidget
    {
        QCheckBox* widget;

    public:
        QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };


    class BoolInitWidgetCreator: public TypedInitWidgetCreator
    {
    public:
        TypedInitWidget* CreateWidget() const override;
    };




    class IntWidget: public TypedWidget
    {
        QSpinBox* widget;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    class IntWidgetCreator: public TypedWidgetCreator
    {
    public:
        TypedWidget* CreateWidget() const override;
    };

    class IntInitWidget: public TypedInitWidget
    {
        QSpinBox* widget;

    public:
        QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };

    class IntInitWidgetCreator: public TypedInitWidgetCreator
    {
    public:
        TypedInitWidget* CreateWidget() const override;
    };






    class FloatWidget: public TypedWidget
    {
        QDoubleSpinBox* widget;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    class FloatWidgetCreator: public TypedWidgetCreator
    {
        TypedWidget* CreateWidget() const override;
    };


    class FloatInitWidget: public TypedInitWidget
    {
        QDoubleSpinBox* widget;

    public:
        QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };


    class FloatInitWidgetCreator: public TypedInitWidgetCreator
    {
    public:
        TypedInitWidget* CreateWidget() const override;
    };







    class DoubleWidget: public TypedWidget
    {
        QDoubleSpinBox* widget;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    class DoubleWidgetCreator: public TypedWidgetCreator
    {
        TypedWidget* CreateWidget() const override;
    };


    class DoubleInitWidget: public TypedInitWidget
    {
        QDoubleSpinBox* widget;

    public:
        QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };


    class DoubleInitWidgetCreator: public TypedInitWidgetCreator
    {
    public:
        TypedInitWidget* CreateWidget() const override;
    };



    class TextEdit: public QTextEdit
    {
    public:
        using QTextEdit::QTextEdit;

        QSize sizeHint() const override
        {
            auto default_ = QTextEdit::sizeHint();

            return {default_.width(), 15};
        }

        QSize minimumSizeHint() const override
        {
            return sizeHint();
        }

    };



    class StringWidget: public TypedWidget
    {
        QWidget* mainWidget;
        TextEdit* textEdit;
        QPushButton* editButton;
        QPushButton* unfoldButton;
        QTextDocument* document;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;

        void EditText();

        void AcceptEdit(QString text);
    };

    class StringWidgetCreator: public TypedWidgetCreator
    {
        TypedWidget* CreateWidget() const override;
    };


    class StringInitWidget: public TypedInitWidget
    {
        QLineEdit* widget;

    public:
        QWidget* CreateInitWidget() override;

        std::any CreateValue() override;
    };


    class StringInitWidgetCreator: public TypedInitWidgetCreator
    {
    public:
        TypedInitWidget* CreateWidget() const override;
    };






    class EnumWidget: public TypedWidget
    {
        QComboBox* widget;
    public:
        QWidget* CreateTypedWidget() override;

        void SetValue(std::any value) override;

        std::any GetValue() const override;

        void ShowConnected(bool connected) override;

        void ShowDisabled(bool disabled) override;
    };

    class EnumWidgetCreator: public TypedWidgetCreator
    {
        TypedWidget* CreateWidget() const override;
    };


    class EnumInitWidget: public TypedInitWidget
    {
        QWidget* group;
        QComboBox* combobox;
        QLineEdit* lineedit;
        QPushButton* addbutton;
        QPushButton* rmbutton;

    public:
        QWidget* CreateInitWidget() override;

        std::any CreateValue() override;

        void AddFieldValue();

        void AddValue(QString value);

        void RemoveCurrentValue();
    };


    class EnumInitWidgetCreator: public TypedInitWidgetCreator
    {
    public:
        TypedInitWidget* CreateWidget() const override;
    };



    void RegisterDefaultTypedWidgets();
}


#endif //NODELIBRARY2COMPLETE_TYPEWIDGETS_H
