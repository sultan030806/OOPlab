#include "TripleValuesMVC.h"
#include <QApplication>
#include <QDebug>
#include <algorithm>

const QString TripleValueModel::DATA_FILE = "data.txt";

// TripleValueModel implementation
TripleValueModel::TripleValueModel(QObject* parent)
    : QObject(parent)
    , m_valueA(0)
    , m_valueB(0)
    , m_valueC(0)
{
    loadData();
    // Единичное уведомление при запуске
    emit valuesChanged(m_valueA, m_valueB, m_valueC);
}

bool TripleValueModel::isValidValue(int value) const
{
    return (value >= m_minValue && value <= m_maxValue);
}

void TripleValueModel::ensureConsistency()
{
    // Разрешающее поведение для A и C
    // B должно быть между A и C
    if (m_valueA > m_valueB) {
        m_valueB = m_valueA;
    }
    if (m_valueB > m_valueC) {
        m_valueC = m_valueB;
    }
    // Дополнительная проверка на случай, если C < A
    if (m_valueC < m_valueA) {
        m_valueA = m_valueC;
    }
}

void TripleValueModel::emitIfChanged(int oldA, int oldB, int oldC)
{
    if (oldA != m_valueA || oldB != m_valueB || oldC != m_valueC) {
        emit valuesChanged(m_valueA, m_valueB, m_valueC);
    }
}

void TripleValueModel::setValueA(int value)
{
    if (!isValidValue(value)) return;

    int oldA = m_valueA;
    int oldB = m_valueB;
    int oldC = m_valueC;

    // Сохраняем введенное пользователем значение A
    m_valueA = value;

    // Разрешающее поведение: корректируем B и C чтобы сохранить условие A <= B <= C
    if (m_valueA > m_valueB) {
        m_valueB = m_valueA;
    }
    if (m_valueB > m_valueC) {
        m_valueC = m_valueB;
    }

    emitIfChanged(oldA, oldB, oldC);
}

void TripleValueModel::setValueB(int value)
{
    if (!isValidValue(value)) return;

    // Запрещающее поведение: отклоняем недопустимые значения
    if (value < m_valueA || value > m_valueC) {
        return; // Откатываем изменение
    }

    int oldA = m_valueA;
    int oldB = m_valueB;
    int oldC = m_valueC;

    m_valueB = value;

    emitIfChanged(oldA, oldB, oldC);
}

void TripleValueModel::setValueC(int value)
{
    if (!isValidValue(value)) return;

    int oldA = m_valueA;
    int oldB = m_valueB;
    int oldC = m_valueC;

    // Сохраняем введенное пользователем значение C
    m_valueC = value;

    // Разрешающее поведение: корректируем A и B чтобы сохранить условие A <= B <= C
    if (m_valueC < m_valueB) {
        m_valueB = m_valueC;
    }
    if (m_valueB < m_valueA) {
        m_valueA = m_valueB;
    }

    emitIfChanged(oldA, oldB, oldC);
}

void TripleValueModel::saveData()
{
    QFile file(DATA_FILE);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // Сохраняем только A и C (пользовательские значения)
        out << m_valueA << " " << m_valueC;
        file.close();
        qDebug() << "Data saved to" << DATA_FILE << "- A:" << m_valueA << "C:" << m_valueC;
    }
}

void TripleValueModel::loadData()
{
    QFile file(DATA_FILE);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);

        if (parts.size() == 2) {
            bool ok1, ok2;
            int a = parts[0].toInt(&ok1);
            int c = parts[1].toInt(&ok2);

            if (ok1 && ok2 && isValidValue(a) && isValidValue(c)) {
                // Загружаем сохраненные значения A и C
                m_valueA = a;
                m_valueC = c;

                // Вычисляем B как значение между A и C
                // Можно выбрать среднее, минимальное из C или максимальное из A
                // По логике задания B должно быть между A и C, выберем A (или можно среднее)
                m_valueB = std::max(m_valueA, std::min(m_valueC, (m_valueA + m_valueC) / 2));

                // Гарантируем выполнение условий
                ensureConsistency();

                qDebug() << "Data loaded from" << DATA_FILE << "- A:" << m_valueA
                    << "B:" << m_valueB << "C:" << m_valueC;
            }
        }
        else if (parts.size() == 3) {
            // Для обратной совместимости с предыдущей версией
            bool ok1, ok2, ok3;
            int a = parts[0].toInt(&ok1);
            int b = parts[1].toInt(&ok2);
            int c = parts[2].toInt(&ok3);

            if (ok1 && ok2 && ok3 &&
                isValidValue(a) && isValidValue(b) && isValidValue(c)) {
                m_valueA = a;
                m_valueB = b;
                m_valueC = c;
                ensureConsistency();
                qDebug() << "Legacy data loaded from" << DATA_FILE;
            }
        }
        file.close();
    }
    else {
        // Если файла нет, инициализируем значениями по умолчанию
        m_valueA = 0;
        m_valueB = 0;
        m_valueC = 0;
        qDebug() << "No data file found, using default values";
    }
}

// MainWindow implementation
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new TripleValueModel(this))
{
    setWindowTitle("Triple Values Controller - MVC C++");
    setFixedSize(500, 300);

    setupUI();
    setupConnections();
    applyLimits();

    // Подключаем сигнал от модели
    connect(m_model, &TripleValueModel::valuesChanged,
        this, &MainWindow::onModelValuesChanged);

    // Инициализируем интерфейс начальными значениями
    onModelValuesChanged(m_model->valueA(), m_model->valueB(), m_model->valueC());
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);

    // Labels
    m_labelsLayout = new QHBoxLayout();
    m_labelA = new QLabel("A");
    m_labelLeq1 = new QLabel("<=");
    m_labelB = new QLabel("B");
    m_labelLeq2 = new QLabel("<=");
    m_labelC = new QLabel("C");

    QFont labelFont("Times New Roman", 30, QFont::Bold);
    m_labelA->setFont(labelFont);
    m_labelLeq1->setFont(labelFont);
    m_labelB->setFont(labelFont);
    m_labelLeq2->setFont(labelFont);
    m_labelC->setFont(labelFont);

    m_labelsLayout->addWidget(m_labelA, 0, Qt::AlignCenter | Qt::AlignTop);
    m_labelsLayout->addWidget(m_labelLeq1, 0, Qt::AlignCenter | Qt::AlignTop);
    m_labelsLayout->addWidget(m_labelB, 0, Qt::AlignCenter | Qt::AlignTop);
    m_labelsLayout->addWidget(m_labelLeq2, 0, Qt::AlignCenter | Qt::AlignTop);
    m_labelsLayout->addWidget(m_labelC, 0, Qt::AlignCenter | Qt::AlignTop);

    // SpinBoxes
    m_spinLayout = new QHBoxLayout();
    m_spinBoxA = new QSpinBox();
    m_spinBoxB = new QSpinBox();
    m_spinBoxC = new QSpinBox();

    m_spinLayout->addWidget(m_spinBoxA, 0, Qt::AlignCenter);
    m_spinLayout->addWidget(m_spinBoxB, 0, Qt::AlignCenter);
    m_spinLayout->addWidget(m_spinBoxC, 0, Qt::AlignCenter);

    // Text fields
    m_textLayout = new QHBoxLayout();
    m_textFieldA = new QLineEdit();
    m_textFieldB = new QLineEdit();
    m_textFieldC = new QLineEdit();

    m_textLayout->addWidget(m_textFieldA, 0, Qt::AlignCenter);
    m_textLayout->addWidget(m_textFieldB, 0, Qt::AlignCenter);
    m_textLayout->addWidget(m_textFieldC, 0, Qt::AlignCenter);

    // Sliders
    m_sliderLayout = new QHBoxLayout();
    m_sliderA = new QSlider(Qt::Horizontal);
    m_sliderB = new QSlider(Qt::Horizontal);
    m_sliderC = new QSlider(Qt::Horizontal);

    m_sliderLayout->addWidget(m_sliderA, 0, Qt::AlignCenter);
    m_sliderLayout->addWidget(m_sliderB, 0, Qt::AlignCenter);
    m_sliderLayout->addWidget(m_sliderC, 0, Qt::AlignCenter);

    // Добавляем все в главный layout
    m_mainLayout->addLayout(m_labelsLayout);
    m_mainLayout->addLayout(m_spinLayout);
    m_mainLayout->addLayout(m_textLayout);
    m_mainLayout->addLayout(m_sliderLayout);
    m_mainLayout->addStretch();
}

void MainWindow::setupConnections()
{
    // Value A
    connect(m_spinBoxA, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &MainWindow::onSpinAChanged);
    connect(m_sliderA, &QSlider::valueChanged,
        this, &MainWindow::onSliderAChanged);
    connect(m_textFieldA, &QLineEdit::editingFinished,
        this, &MainWindow::onTextAFinished);

    // Value B
    connect(m_spinBoxB, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &MainWindow::onSpinBChanged);
    connect(m_sliderB, &QSlider::valueChanged,
        this, &MainWindow::onSliderBChanged);
    connect(m_textFieldB, &QLineEdit::editingFinished,
        this, &MainWindow::onTextBFinished);

    // Value C
    connect(m_spinBoxC, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &MainWindow::onSpinCChanged);
    connect(m_sliderC, &QSlider::valueChanged,
        this, &MainWindow::onSliderCChanged);
    connect(m_textFieldC, &QLineEdit::editingFinished,
        this, &MainWindow::onTextCFinished);
}

void MainWindow::applyLimits()
{
    int minVal = m_model->minValue();
    int maxVal = m_model->maxValue();

    m_spinBoxA->setRange(minVal, maxVal);
    m_spinBoxB->setRange(minVal, maxVal);
    m_spinBoxC->setRange(minVal, maxVal);

    m_sliderA->setRange(minVal, maxVal);
    m_sliderB->setRange(minVal, maxVal);
    m_sliderC->setRange(minVal, maxVal);
}

void MainWindow::onModelValuesChanged(int a, int b, int c)
{
    m_updateCounter++;
    qDebug() << "Update #" << m_updateCounter << ". New values: A =" << a << "B =" << b << "C =" << c;

    updateInterface(a, b, c);
}

void MainWindow::updateInterface(int a, int b, int c)
{
    // Блокируем сигналы во избежание рекурсивных вызовов
    QSignalBlocker blockerA(m_spinBoxA);
    QSignalBlocker blockerB(m_spinBoxB);
    QSignalBlocker blockerC(m_spinBoxC);

    QSignalBlocker sliderBlockerA(m_sliderA);
    QSignalBlocker sliderBlockerB(m_sliderB);
    QSignalBlocker sliderBlockerC(m_sliderC);

    m_spinBoxA->setValue(a);
    m_spinBoxB->setValue(b);
    m_spinBoxC->setValue(c);

    m_textFieldA->setText(QString::number(a));
    m_textFieldB->setText(QString::number(b));
    m_textFieldC->setText(QString::number(c));

    m_sliderA->setValue(a);
    m_sliderB->setValue(b);
    m_sliderC->setValue(c);
}

// Value A handlers
void MainWindow::onSpinAChanged(int value)
{
    m_model->setValueA(value);
    // Синхронизация в случае отката
    if (m_spinBoxA->value() != m_model->valueA()) {
        QSignalBlocker blocker(m_spinBoxA);
        m_spinBoxA->setValue(m_model->valueA());
    }
}

void MainWindow::onSliderAChanged(int value)
{
    m_model->setValueA(value);
    // Синхронизация в случае отката
    if (m_sliderA->value() != m_model->valueA()) {
        QSignalBlocker blocker(m_sliderA);
        m_sliderA->setValue(m_model->valueA());
    }
}

void MainWindow::onTextAFinished()
{
    QString text = m_textFieldA->text().trimmed();
    bool ok;
    int value;

    if (text.isEmpty()) {
        value = 0;
    }
    else {
        value = text.toInt(&ok);
        if (!ok || !(value >= 0 && value <= 100)) {
            value = m_model->valueA();
        }
    }

    m_model->setValueA(value);
    m_textFieldA->setText(QString::number(m_model->valueA()));
}

// Value B handlers
void MainWindow::onSpinBChanged(int value)
{
    m_model->setValueB(value);
    // Синхронизация в случае отката
    if (m_spinBoxB->value() != m_model->valueB()) {
        QSignalBlocker blocker(m_spinBoxB);
        m_spinBoxB->setValue(m_model->valueB());
    }
}

void MainWindow::onSliderBChanged(int value)
{
    m_model->setValueB(value);
    // Синхронизация в случае отката
    if (m_sliderB->value() != m_model->valueB()) {
        QSignalBlocker blocker(m_sliderB);
        m_sliderB->setValue(m_model->valueB());
    }
}

void MainWindow::onTextBFinished()
{
    QString text = m_textFieldB->text().trimmed();
    bool ok;
    int value;

    if (text.isEmpty()) {
        value = 0;
    }
    else {
        value = text.toInt(&ok);
        if (!ok || !(value >= 0 && value <= 100)) {
            value = m_model->valueB();
        }
    }

    m_model->setValueB(value);
    m_textFieldB->setText(QString::number(m_model->valueB()));
}

// Value C handlers
void MainWindow::onSpinCChanged(int value)
{
    m_model->setValueC(value);
    // Синхронизация в случае отката
    if (m_spinBoxC->value() != m_model->valueC()) {
        QSignalBlocker blocker(m_spinBoxC);
        m_spinBoxC->setValue(m_model->valueC());
    }
}

void MainWindow::onSliderCChanged(int value)
{
    m_model->setValueC(value);
    // Синхронизация в случае отката
    if (m_sliderC->value() != m_model->valueC()) {
        QSignalBlocker blocker(m_sliderC);
        m_sliderC->setValue(m_model->valueC());
    }
}

void MainWindow::onTextCFinished()
{
    QString text = m_textFieldC->text().trimmed();
    bool ok;
    int value;

    if (text.isEmpty()) {
        value = 0;
    }
    else {
        value = text.toInt(&ok);
        if (!ok || !(value >= 0 && value <= 100)) {
            value = m_model->valueC();
        }
    }

    m_model->setValueC(value);
    m_textFieldC->setText(QString::number(m_model->valueC()));
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_model->saveData();
    QMainWindow::closeEvent(event);
}

// Main function
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
