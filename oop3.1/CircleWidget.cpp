#include "CircleWidget.h"
#include <sstream>

// Реализация класса Circle
Circle::Circle(int x, int y) : x(x), y(y), selected(false) {
}

void Circle::draw(QPainter& painter) const {
    if (selected) {
        painter.setPen(QColor(85, 107, 47));
    }
    else {
        painter.setPen(QColor(0, 255, 0));
    }

    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(x - RADIUS, y - RADIUS, RADIUS * 2, RADIUS * 2);
}

bool Circle::contains(int x, int y) const {
    int dx = x - this->x;
    int dy = y - this->y;
    return (dx * dx + dy * dy) <= (RADIUS * RADIUS);
}

// Реализация CircleStorage
void CircleStorage::addCircle(std::shared_ptr<Circle> circle) {
    circles.push_back(circle);
}

void CircleStorage::clearSelection() {
    for (auto& circle : circles) {
        circle->setSelected(false);
    }
}

void CircleStorage::removeSelected() {
    auto it = std::remove_if(circles.begin(), circles.end(),
        [](const std::shared_ptr<Circle>& circle) {
            return circle->isSelected();
        });
    circles.erase(it, circles.end());
}

// Реализация CircleWidget
CircleWidget::CircleWidget(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("background-color: white;");
}

void CircleWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (const auto& circle : storage.getCircles()) {
        circle->draw(painter);
    }
}

void CircleWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int x = event->pos().x();
        int y = event->pos().y();

        bool clickedOnCircle = false;
        std::vector<std::shared_ptr<Circle>> circlesUnderCursor;

        for (const auto& circle : storage.getCircles()) {
            if (circle->contains(x, y)) {
                circlesUnderCursor.push_back(circle);
            }
        }

        if (!circlesUnderCursor.empty()) { // ВЫДЕЛЕНИЕ
            if (event->modifiers() & Qt::ControlModifier) {
                for (auto& circle : circlesUnderCursor) {
                    circle->setSelected(!circle->isSelected());
                }
            }
            else {
                storage.clearSelection();
                for (auto& circle : circlesUnderCursor) {
                    circle->setSelected(true);
                }
            }
            clickedOnCircle = true;

            QString info = QString("Highlighted circles: %1").arg(circlesUnderCursor.size()); 
            if (circlesUnderCursor.size() == 1) {
                auto circle = circlesUnderCursor.front();
                info += QString(" (x: %1, y: %2)").arg(circle->getX()).arg(circle->getY());
            }
            showMessage(info);
        }

        if (!clickedOnCircle) {
            storage.clearSelection();
            auto newCircle = std::make_shared<Circle>(x, y);
            storage.addCircle(newCircle);

            QString message = QString("A circle has been created! Center coordinates: x: %1, y: %2")
                .arg(x).arg(y);
            showMessage(message);
        }

        update();
    }
}

void CircleWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete) {
        QString deletedInfo = "Circles with coordinates have been removed:\n";
        bool anyDeleted = false;

        for (const auto& circle : storage.getCircles()) {
            if (circle->isSelected()) {
                deletedInfo += QString("x: %1, y: %2\n").arg(circle->getX()).arg(circle->getY());
                anyDeleted = true;
            }
        }

        if (anyDeleted) {
            storage.removeSelected();
            showMessage(deletedInfo);
            update();
        }
    }
}

void CircleWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    QString message = QString("The form size has been changed: %1x%2")
        .arg(event->size().width())
        .arg(event->size().height());
    showMessage(message);
    update();
}

void CircleWidget::showMessage(const QString& message) {
    QWidget* parent = parentWidget();
    while (parent && !qobject_cast<QMainWindow*>(parent)) {
        parent = parent->parentWidget();
    }

    if (QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parent)) {
        mainWindow->statusBar()->showMessage(message, 3000);
    }
}

// Реализация MainWindow
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("CircleApp");
    resize(800, 600);

    CircleWidget* widget = new CircleWidget(this);
    setCentralWidget(widget);

    statusBar()->showMessage("Done. Click to create a circle, Ctrl+click to select multiple objects, Del to delete");

    QLabel* infoLabel = new QLabel("Instruction: LMB - create/select, Ctrl+LMB - add/remove selection, Del - delete selected", this);
    statusBar()->addPermanentWidget(infoLabel);
}

// Точка входа
int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
