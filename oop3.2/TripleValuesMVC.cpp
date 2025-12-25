#include "TripleValuesMVC.h"
#include <QApplication>
#include <QDebug>

// TripleValueModel implementation
TripleValueModel::TripleValueModel(QObject* parent)
    : QObject(parent)
    , m_valueA(0)
    , m_valueB(0)
    , m_valueC(0)
{
}

void TripleValueModel::ensureConsistency()
{
    // Простая логика: A <= B <= C
    if (m_valueA > m_valueB) {
        m_valueB = m_valueA;
    }
    if (m_valueB > m_valueC) {
        m_valueC = m_valueB;
    }
    if (m_valueC < m_valueA) {
        m_valueA = m_valueC;
    }
}

void TripleValueModel::setValueA(int value)
{
    if (value < m_minValue || value > m_maxValue) return;
    
    int oldA = m_valueA;
    int oldB = m_valueB;
    int oldC = m_valueC;
    
    m_valueA = value;
    ensureConsistency();
    
    if (oldA != m_valueA || oldB != m_valueB || oldC != m_valueC) {
        emit valuesChanged(m_valueA, m_valueB, m_valueC);
    }
}

void TripleValueModel::setValueB(int value)
{
    if (value < m_minValue || value > m_maxValue) return;
    if (value < m_valueA || value > m_valueC) return; // Запрещающее поведение
    
    int oldA = m_valueA;
    int oldB = m_valueB;
    int oldC = m_valueC;
    
    m_valueB = value;
    
    if (oldA != m_valueA || oldB != m_valueB || oldC != m_valueC) {
        emit valuesChanged(m_valueA, m_valueB, m_valueC);
    }
}

void TripleValueModel::setValueC(int value)
{
    if (value < m_minValue || value > m_maxValue) return;
    
    int oldA = m_valueA;
    int oldB = m_valueB;
    int oldC = m_valueC;
    
    m_valueC = value;
    ensureConsistency();
    
    if (oldA != m_valueA || oldB != m_valueB || oldC != m_valueC) {
        emit valuesChanged(m_valueA, m_valueB, m_valueC);
    }
}

// MainWindow implementation
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new TripleValueModel(this))
{
    setWindowTitle("Triple Values - Stage 1");
    setFixedSize(400, 200);

    setWindowTitle("Triple Values - Stage 2");
    setFixedSize(400, 300);
    
    setupUI();
    
    connect(m_model, &TripleValueModel::valuesChanged,
            this, &MainWindow::onModelValuesChanged);
    
    // Инициализация
    onModelValuesChanged(0, 0, 0);
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    m_spinBoxA = new QSpinBox();
    m_spinBoxB = new QSpinBox();
    m_spinBoxC = new QSpinBox();
    
    m_spinBoxA->setRange(0, 100);
    m_spinBoxB->setRange(0, 100);
    m_spinBoxC->setRange(0, 100);
    
    connect(m_spinBoxA, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onSpinAChanged);
    connect(m_spinBoxB, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onSpinBChanged);
    connect(m_spinBoxC, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onSpinCChanged);
    
    mainLayout->addWidget(m_spinBoxA);
    mainLayout->addWidget(m_spinBoxB);
    mainLayout->addWidget(m_spinBoxC);
    mainLayout->addStretch();
}

void MainWindow::onModelValuesChanged(int a, int b, int c)
{
    qDebug() << "Values changed: A =" << a << "B =" << b << "C =" << c;
    
    m_spinBoxA->setValue(a);
    m_spinBoxB->setValue(b);
    m_spinBoxC->setValue(c);
}

void MainWindow::onSpinAChanged(int value)
{
    m_model->setValueA(value);
}

void MainWindow::onSpinBChanged(int value)
{
    m_model->setValueB(value);
}

void MainWindow::onSpinCChanged(int value)
{
    m_model->setValueC(value);
}

// Main function
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
