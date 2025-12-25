#ifndef TRIPLEVALUESMVC_H
#define TRIPLEVALUESMVC_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QSpinBox>

// Простейшая модель
class TripleValueModel : public QObject
{
    Q_OBJECT

public:
    explicit TripleValueModel(QObject* parent = nullptr);

    int valueA() const { return m_valueA; }
    int valueB() const { return m_valueB; }
    int valueC() const { return m_valueC; }

    void setValueA(int value);
    void setValueB(int value);
    void setValueC(int value);

signals:
    void valuesChanged(int a, int b, int c);

private:
    void ensureConsistency();

    int m_valueA;
    int m_valueB;
    int m_valueC;
    const int m_minValue = 0;
    const int m_maxValue = 100;
};

// Главное окно с минимальным UI
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void onModelValuesChanged(int a, int b, int c);
    void onSpinAChanged(int value);
    void onSpinBChanged(int value);
    void onSpinCChanged(int value);

private:
    private:
    void setupUI();
    void setupConnections();
    void updateInterface(int a, int b, int c);
    
    // Новые UI элементы
    QSlider* m_sliderA;
    QSlider* m_sliderB;
    QSlider* m_sliderC;
    QLabel* m_labelA;
    QLabel* m_labelB;
    QLabel* m_labelC;
    
    // Новые обработчики
    void onSliderAChanged(int value);
    void onSliderBChanged(int value);
    void onSliderCChanged(int value);
    
    TripleValueModel* m_model;
    QSpinBox* m_spinBoxA;
    QSpinBox* m_spinBoxB;
    QSpinBox* m_spinBoxC;
};

#endif // TRIPLEVALUESMVC_H
