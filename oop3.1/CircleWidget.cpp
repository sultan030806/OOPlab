#include "CircleWidget.h"
#include <QApplication>
#include <QMainWindow>

Circle::Circle(int x, int y) : x(x), y(y) {
}

void Circle::draw(QPainter& painter) const {
    painter.setPen(Qt::green);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(x - RADIUS, y - RADIUS, RADIUS * 2, RADIUS * 2);
}

bool Circle::contains(int x, int y) const {
    int dx = x - this->x;
    int dy = y - this->y;
    return (dx * dx + dy * dy) <= (RADIUS * RADIUS);
}

CircleWidget::CircleWidget(QWidget* parent) : QWidget(parent) {
    setStyleSheet("background-color: white;");
}

void CircleWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    for (const auto& circle : circles) {
        circle.draw(painter);
    }
}

void CircleWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int x = event->pos().x();
        int y = event->pos().y();
        
        // Всегда создаем новый круг
        circles.emplace_back(x, y);
        update();
    }
}

// Простое main для тестирования
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow window;
    window.setWindowTitle("CircleApp");
    window.resize(800, 600);
    
    CircleWidget* widget = new CircleWidget(&window);
    window.setCentralWidget(widget);
    
    window.show();
    return app.exec();
}
