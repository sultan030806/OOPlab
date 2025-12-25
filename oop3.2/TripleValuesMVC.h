#ifndef TRIPLEVALUESMVC_H
#define TRIPLEVALUESMVC_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>
#include <QSignalBlocker>

// Модель данных
class TripleValueModel : public QObject
{
    Q_OBJECT

public:
    explicit TripleValueModel(QObject* parent = nullptr);

    // Геттеры
    int valueA() const { return m_valueA; }
    int valueB() const { return m_valueB; }
    int valueC() const { return m_valueC; }
    int minValue() const { return m_minValue; }
    int maxValue() const { return m_maxValue; }

    // Сеттеры с разной логикой
    void setValueA(int value);  // Разрешающее поведение
    void setValueB(int value);  // Запрещающее поведение
    void setValueC(int value);  // Разрешающее поведение

    // Сохранение/загрузка
    void saveData();
    void loadData();

signals:
    void valuesChanged(int a, int b, int c);

private:
    bool isValidValue(int value) const;
    void ensureConsistency();
    void emitIfChanged(int oldA, int oldB, int oldC);

    int m_valueA;
    int m_valueB;
    int m_valueC;
    const int m_minValue = 0;
    const int m_maxValue = 100;

    static const QString DATA_FILE;
};

// Главное окно
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onModelValuesChanged(int a, int b, int c);

private:
    void setupUI();
    void setupConnections();
    void applyLimits();

    // Обработчики для значения A
    void onSpinAChanged(int value);
    void onSliderAChanged(int value);
    void onTextAFinished();

    // Обработчики для значения B
    void onSpinBChanged(int value);
    void onSliderBChanged(int value);
    void onTextBFinished();

    // Обработчики для значения C
    void onSpinCChanged(int value);
    void onSliderCChanged(int value);
    void onTextCFinished();

    // Обновление интерфейса
    void updateInterface(int a, int b, int c);

    TripleValueModel* m_model;

    // UI элементы
    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;

    // Labels
    QHBoxLayout* m_labelsLayout;
    QLabel* m_labelA;
    QLabel* m_labelLeq1;
    QLabel* m_labelB;
    QLabel* m_labelLeq2;
    QLabel* m_labelC;

    // SpinBoxes
    QHBoxLayout* m_spinLayout;
    QSpinBox* m_spinBoxA;
    QSpinBox* m_spinBoxB;
    QSpinBox* m_spinBoxC;

    // Text fields
    QHBoxLayout* m_textLayout;
    QLineEdit* m_textFieldA;
    QLineEdit* m_textFieldB;
    QLineEdit* m_textFieldC;

    // Sliders
    QHBoxLayout* m_sliderLayout;
    QSlider* m_sliderA;
    QSlider* m_sliderB;
    QSlider* m_sliderC;

    int m_updateCounter = 0;
};

#endif // TRIPLEVALUESMVC_H
